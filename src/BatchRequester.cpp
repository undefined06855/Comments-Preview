#include "BatchRequester.hpp"
#include "CommentData.hpp"
#include "CachedCommentsUpdatedEvent.hpp"
#include "utils.hpp"

BatchRequester& BatchRequester::get() {
    static BatchRequester instance;
    return instance;
}

BatchRequester::BatchRequester()
    : m_timerStarted(false)
    , m_timer(0.f) {}

void BatchRequester::init() {
    cocos2d::CCScheduler::get()->scheduleUpdateForTarget(this, 0, false);
}

void BatchRequester::queueID(int id) {
    comments::log::debug("Queueing {}", id);
    m_levelIDQueue.push_back(id);

    if (!m_timerStarted) {
        comments::log::debug("...and starting timer");
        m_timerStarted = true;
        m_timer = 0.f;
    }
}

arc::Future<geode::Result<>> BatchRequester::fetchComments(std::vector<int> ids) {
    if (ids.size() == 0) co_return geode::Err("no ids");

    std::string levelIDs = "";
    levelIDs.reserve(ids.size() * 10);
    for (auto id : ids) { levelIDs += fmt::format("{},", id); }


    levelIDs.erase(levelIDs.size() - 1);

    geode::utils::web::WebRequest req;
    req.param("levelIDs", levelIDs);
    req.param("modVersion", geode::Mod::get()->getVersion().toVString());
    req.param("platform", GEODE_PLATFORM_NAME);
    req.param("geodeVersion", geode::Loader::get()->getVersion().toVString());
    req.param("gdVersion", geode::Loader::get()->getGameVersion());
    req.timeout(asp::Duration::fromSecs(10));

    auto res = co_await req.get(fmt::format("{}/comments", geode::Mod::get()->getSettingValue<std::string>("endpoint")));

    GEODE_CO_UNWRAP_INTO(auto json, res.json());

    if (json.contains("error")) {
        auto error = UNWRAP_N_CAST(json.get("error"), std::string);
        co_return geode::Err(error);
    }

    geode::Loader::get()->queueInMainThread([this, json] {
        for (auto [key, level] : json.get("levels").unwrapOr(matjson::Value())) {
            auto id = geode::utils::numFromString<int>(key);

            if (id.isErr()) {
                comments::log::warn("invalid id {}", key);
                continue;
            }

            int unwrappedID = id.unwrap();
            m_cache.emplace(unwrappedID, CommentDataWithTimeout{
                .data = CommentData(unwrappedID, level),
                .expiryTime = asp::Instant::now().saturatingAdd(asp::Duration::fromMinutes(10))
            });

            CachedCommentsUpdatedEvent(unwrappedID).send();
        }
    });

    co_return geode::Ok();
}

geode::Result<CommentData> BatchRequester::getCommentData(int id) {
    if (!m_cache.contains(id)) return geode::Err("not in cache");

    auto& data = m_cache.at(id);
    if (data.expiryTime.until().isZero()) {
        m_cache.erase(id);
        return geode::Err("cached key expired!");
    }

    return geode::Ok(data.data);
}

void BatchRequester::update(float dt) {
    if (m_levelIDQueue.empty()) return;

    if (m_timerStarted) {
        m_timer += dt;
        if (m_timer < .5f) return;

        comments::log::info("Sending request with {} ids", m_levelIDQueue.size());

        m_timerStarted = false;

        auto queueCopy = m_levelIDQueue;
        geode::async::spawn([this, queueCopy = std::move(queueCopy)] -> arc::Future<> {
            auto res = co_await this->fetchComments(queueCopy);
            if (res.isErr()) {
                comments::log::warn("failed to fetch {} comments: {}", queueCopy.size(), res.unwrapErr());
            }
        });

        m_levelIDQueue.clear();
    }
}

$on_mod(Loaded) {
    BatchRequester::get().init();
}
