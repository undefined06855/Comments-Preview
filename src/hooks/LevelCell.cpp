#include "LevelCell.hpp"
#include "../BatchRequester.hpp"
#include "../CachedCommentsUpdatedEvent.hpp"
#include "../ui/CommentPreviewNode.hpp"

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

        geode::log::debug("{}", data.unwrapErr());
        BatchRequester::get().queueID(m_level->m_levelID);
        fields->attempts += 1;

        this->addEventListener(CachedCommentsUpdatedEvent(m_level->m_levelID), [this, fields] {
            this->attemptAddComments();
            return geode::ListenerResult::Propagate;
        });

        return;
    }

    this->addComments(std::move(data.unwrap()));
}

void HookedLevelCell::addComments(CommentData data) {
    // wait one frame else we have zero content size for some reason
    this->runAction(geode::cocos::CallFuncExt::create([this, data = std::move(data)] {
        geode::log::info("adding {} comments to {}", data.comments.size(), data.levelID);
        if (data.comments.empty()) return;

        auto nameLabel = m_mainLayer->getChildByType<cocos2d::CCLabelBMFont*>();

        auto pos = nameLabel->getPosition();
        pos.x += nameLabel->getScaledContentWidth() + 10.f;
        pos.y -= 2.5f;

        auto availableWidth = this->getContentWidth() - pos.x;

        auto node = CommentPreviewNode::create(std::move(data), availableWidth);
        node->setID("comment-preview"_spr);
        node->setPosition(pos);
        m_mainLayer->addChild(node);
    }));
}
