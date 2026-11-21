#pragma once
#include "../CommentData.hpp"
#include <alphalaneous.alphas-ui-pack/nodes/RenderNode.hpp>

class CommentPreviewNode : public cocos2d::CCNode {
public:
    static alpha::ui::RenderNode* createAndWrap(CommentData data, float availableWidth, float margin);
    static CommentPreviewNode* create(CommentData data, float availableWidth);
    bool init(CommentData data, float availableWidth);

    int m_index;
    CommentData m_data;
    alpha::ui::RenderNode* m_renderNode; // don't ref this else it will be a circular retain
};
