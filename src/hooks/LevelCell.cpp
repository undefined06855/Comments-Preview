#include "LevelCell.hpp"
#include "../BatchRequester.hpp"
#include "../CachedCommentsUpdatedEvent.hpp"
#include "../ui/CommentPreviewNode.hpp"
#include <Geode/ui/PopupManager.hpp>

HookedLevelCell::Fields::Fields()
    : attempts(0) {}

void HookedLevelCell::loadFromLevel(GJGameLevel* level) {
    LevelCell::loadFromLevel(level);
    this->attemptAddComments();
}

void HookedLevelCell::attemptAddComments() {
    auto fields = m_fields.self();

    auto data = BatchRequester::get().getCommentData(m_level->m_levelID);
    if (data.isErr()) {
        if (fields->attempts > 3) return;

        BatchRequester::get().queueID(m_level->m_levelID);
        fields->attempts += 1;

        this->addEventListener("comments-event"_spr, CachedCommentsUpdatedEvent(m_level->m_levelID), [this, fields] {
            this->removeEventListener("comments-event"_spr);
            this->attemptAddComments();
            return geode::ListenerResult::Propagate;
        });

        return;
    }

    this->addComments(std::move(data.unwrap()));
}

void HookedLevelCell::addComments(CommentData data) {
    if (this->getContentSize() == cocos2d::CCSize{ 0.f, 0.f }) {
        this->setContentSize(m_backgroundLayer->getContentSize());
    }

    if (auto node = m_mainLayer->getChildByType<CommentPreviewNode>()) {
        node->removeFromParent();
    }

    if (data.comments.empty()) return;

    // level tags disablement
    bool isMini = this->getContentHeight() < 70.f;
    if (isMini && geode::Loader::get()->isModLoaded("kampwski.level_tags")) {
        if (!geode::Mod::get()->getSavedValue<bool>("shown-level-tags-warning", false)) {
            geode::Mod::get()->setSavedValue("shown-level-tags-warning", true);

            geode::PopupManager::get().alert(
                "Comments Preview",
                fmt::format(
                    "<cj>Comments Preview</c> is <cr>not</c> compatible with <co>Level Tags</c> while using the "
                    "<ca>compact view</c>.\n\nThe mod will <co>not activate</c>, since there will be <cy>visual "
                    "issues</c>.\n{}",
                    geode::Loader::get()->isModLoaded("cvolton.compact_lists")
                        ? "Either <cc>toggle off</c> compact list view, or <cr>disable</c> Level Tags."
                        : "Please <cr>disable</c> level tags."
                ),
                "ok"
            ).showQueue();

        }
        
        return;
    }

    auto nameLabel = m_mainLayer->getChildByID("level-name");
    if (!nameLabel) return;

    auto pos = nameLabel->getPosition();

    if (isMini) {
        // mini
        pos.y -= 12.f;

        static const std::array<const char*, 6> nodesToMove = {
            "ncs-icon", "song-name", "coin-icon-1", "coin-icon-2", "coin-icon-3", "zilko.artist_logos/artist-logo"
        };

        for (auto nodeID : nodesToMove) {
            auto node = m_mainLayer->getChildByID(nodeID);
            if (!node) continue;
            node->setPositionY(node->getPositionY() - 5.f);
            node->setScale(node->getScale() * .9f);
        }
    } else {
        // normal size
        pos.x += nameLabel->getScaledContentWidth() + 10.f;
        pos.y -= 2.5f;

        if (m_mainLayer->getChildByID("completed-icon")) {
            pos.x += 30.f;
        } else if (m_mainLayer->getChildByID("percentage-label")) {
            pos.x += 41.f;
        }
    }

    auto availableWidth = this->getContentWidth() - pos.x;

    if (isMini) availableWidth -= 16.f;

    auto node = CommentPreviewNode::create(std::move(data), availableWidth);
    node->setID("comment-preview"_spr);
    node->setPosition(pos);
    m_mainLayer->addChild(node);

    if (isMini) node->setScale(.7f);
}
