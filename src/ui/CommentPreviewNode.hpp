#pragma once
#include "../CommentData.hpp"

class CommentPreviewNode : public cocos2d::CCNode {
public:
    static CommentPreviewNode* create(CommentData data, float availableWidth);
    bool init(CommentData data, float availableWidth);

    geode::Ref<cocos2d::CCNode> m_labelWrapper;
    int m_index;
    CommentData m_data;
};
