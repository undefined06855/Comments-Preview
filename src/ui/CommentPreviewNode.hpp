#pragma once
#include "../CommentData.hpp"
#include <alphalaneous.alphas-ui-pack/include/nodes/RenderNode.hpp>

class CommentPreviewNode : public cocos2d::CCNode {
public:
    static CommentPreviewNode* create(CommentData data, float availableWidth);
    bool init(CommentData data, float availableWidth);

    int m_index;
    CommentData m_data;
};
