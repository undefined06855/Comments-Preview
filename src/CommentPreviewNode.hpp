#pragma once
#include "CommentData.hpp"

class CommentPreviewNode : public cocos2d::CCNode {
public:
    static CommentPreviewNode* create(const CommentData& data, float availableWidth);
    bool init(const CommentData& data, float availableWidth);
};
