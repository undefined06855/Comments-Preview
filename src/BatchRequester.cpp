#include "BatchRequester.hpp"
#include "CommentData.hpp"
#include "GJGameLevel.hpp"
#include "CachedCommentsUpdatedEvent.hpp"
#include "utils.hpp"

BatchRequester& BatchRequester::get() {
    static BatchRequester instance;
    return instance;
}

void BatchRequester::init() {
    cocos2d::CCScheduler::get()->scheduleUpdateForTarget(this, 0, false);
}

void BatchRequester::queueID(int id) {
    m_levelIDQueue.push_back(id);
}

arc::Future<geode::Result<>> BatchRequester::fetchComments(std::vector<int> ids) {
    std::string levelIDs = "";
    levelIDs.reserve(ids.size() * 10);
    for (auto id : ids) { levelIDs += fmt::format("{},", ids); }

    geode::utils::web::WebRequest req;
    req.param("levelIDs", levelIDs);
    req.timeout(std::chrono::seconds(10));

    auto res = co_await req.get("https://top-comments.undefined0.dev/v1/comments");

    GEODE_CO_UNWRAP_INTO(auto json, res.json());

    // creating CommentData is not thread safe
    geode::Loader::get()->queueInMainThread([json] {
        for (auto level : UNWRAP_N_CAST(json.get("levels"), std::vector<matjson::Value>)) {
            auto data = CommentData(level);
            auto gjlevel = geode::cast::modify_cast<HookedGJGameLevel*>(GameLevelManager::get()->getSavedLevel(data.levelID));
            gjlevel->m_fields->commentData = data;
            CachedCommentsUpdatedEvent(data.levelID).send();
        }
    });

    co_return geode::Ok();
}

void BatchRequester::update(float dt) {
    if (m_levelIDQueue.empty()) return;

    auto queueCopy = m_levelIDQueue;
    geode::async::spawn([this, queueCopy = std::move(queueCopy)] -> arc::Future<> {
        auto res = co_await this->fetchComments(queueCopy);
        if (res.isErr()) {
            geode::log::warn("Failed to fetch {} comments: {}", queueCopy.size(), res.unwrapErr());
        }
    });
}

$on_mod(Loaded) {
    BatchRequester::get().init();
}
