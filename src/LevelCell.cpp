#include "LevelCell.hpp"
#include "BatchRequester.hpp"
#include "CachedCommentsUpdatedEvent.hpp"
#include "CommentPreviewNode.hpp"

HookedLevelCell::Fields::Fields()
    : attempts(0) {}

void HookedLevelCell::loadFromLevel(GJGameLevel* level) {
    LevelCell::loadFromLevel(level);
    this->attemptAddComments();
}

void HookedLevelCell::attemptAddComments() {
    auto fields = m_fields.self();

    auto data = BatchRequester::get().getCommentData(m_level->m_levelID);
    if (data.isErr() && fields->attempts < 3) {
        geode::log::debug("{}", data.unwrapErr());
        BatchRequester::get().queueID(m_level->m_levelID);
        fields->attempts += 1;

        this->addEventListener(CachedCommentsUpdatedEvent(m_level->m_levelID), [this, fields] {
            this->attemptAddComments();
            return geode::ListenerResult::Propagate;
        });

        return;
    }

    this->addComments(data.unwrap());
}

void HookedLevelCell::addComments(const CommentData& data) {
    if (data.comments.empty()) return;

    auto nameLabel = m_mainLayer->getChildByID("level-name");
    if (!nameLabel) return;

    auto pos = nameLabel->getPosition();
    pos.x += nameLabel->getScaledContentWidth() + 10.f;

    auto availableWidth = this->getContentWidth() - pos.x - 10.f;

    auto node = CommentPreviewNode::create(data, availableWidth);
    node->setID("comment-preview"_spr);
    node->setPosition(pos);
    m_mainLayer->addChild(node);
}
