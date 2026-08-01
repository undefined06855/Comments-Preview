#include "CommentPreviewNode.hpp"

CommentPreviewNode* CommentPreviewNode::create(const CommentData& data, float availableWidth) {
    auto ret = new CommentPreviewNode;
    if (ret->init(data, availableWidth)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool CommentPreviewNode::init(const CommentData& data, float availableWidth) {
    if (!CCNode::init()) return false;

    this->setContentSize({ availableWidth, 16.f });

    return true;
}
