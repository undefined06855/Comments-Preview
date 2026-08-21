#include "CommentPreviewNode.hpp"
#include "../utils.hpp"

alpha::ui::RenderNode* CommentPreviewNode::createAndWrap(CommentData data, float availableWidth, float margin) {
    auto node = CommentPreviewNode::create(std::move(data), availableWidth);
    node->setID("comment-preview");

    // for extra margin around the commentpreviewnode, else overhanging elements would get cut off
    auto wrap = cocos2d::CCNode::create();
    wrap->setID("comment-preview-wrap");
    wrap->setAnchorPoint({ .5f, .5f });
    wrap->setContentSize(node->getContentSize() + cocos2d::CCSize{ margin, margin });
    wrap->addChildAtPosition(node, geode::Anchor::Center);

    auto ret = RetainingRenderNode::create(wrap, true);
    ret->setID("comment-preview-render-node");
    node->m_renderNode = ret;

    return ret;
}

CommentPreviewNode* CommentPreviewNode::create(CommentData data, float availableWidth) {
    auto ret = new CommentPreviewNode;
    if (ret->init(std::move(data), availableWidth)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool CommentPreviewNode::init(CommentData data, float availableWidth) {
    if (!CCNode::init()) return false;

    m_index = -1;
    m_data = std::move(data);
    m_renderNode = nullptr;

    float labelPadLeft = 15.f;
    float labelPadRight = 20.f;
    float iconXPos = 8.0625;

    this->setAnchorPoint({ .5f, .5f });
    this->setContentSize({ availableWidth, 16.f });

    auto authorLabel = geode::Label::create("...", "bigFont.fnt");
    authorLabel->setID("author-label");
    authorLabel->setScale(.15f);
    authorLabel->setZOrder(2);
    this->addChildAtPosition(authorLabel, geode::Anchor::Left, { iconXPos, 8.5f });

    auto icon = cue::PlayerIcon::create(IconType::Cube);
    icon->setID("icon");
    icon->setScale(.5f);
    icon->setZOrder(1);
    this->addChildAtPosition(icon, geode::Anchor::Left, { iconXPos, 0.f });

    auto likesWrapper = cocos2d::CCNodeRGBA::create();
    likesWrapper->setID("likes-wrapper");
    likesWrapper->setZOrder(5);
    likesWrapper->setAnchorPoint({ .5f, .5f });
    likesWrapper->setContentSize({ icon->getContentWidth(), 10.f });
    likesWrapper->setCascadeOpacityEnabled(true);
    likesWrapper->setLayout(
        geode::RowLayout::create()
            ->setAutoGrowAxis(0.f)
            ->setAutoScale(false)
            ->setGap(1.5f)
    );
    this->addChildAtPosition(likesWrapper, geode::Anchor::BottomLeft, { iconXPos, 1.f });

    auto likesSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_likesIcon_001.png");
    likesSprite->setID("likes-sprite");
    likesSprite->setScale(.25f);
    likesWrapper->addChild(likesSprite);

    auto likesLabel = geode::Label::create("...", "bigFont.fnt");
    likesLabel->setID("likes-label");
    likesLabel->setScale(.15f);
    likesWrapper->addChild(likesLabel);

    // so that the rendernode has the correct size with constrain set to true
    auto labelWrapper = cocos2d::CCNode::create();
    labelWrapper->setID("label-wrapper");
    labelWrapper->setContentSize({ this->getContentWidth() - iconXPos, this->getContentHeight() });

    auto bodyLabel = geode::Label::create("...", "chatFont.fnt");
    bodyLabel->setID("comment-body");
    bodyLabel->setScale(.6f);
    bodyLabel->setAnchorPoint({ 0.f, .5f });
    bodyLabel->setPosition({ labelPadLeft, this->getContentHeight() / 2.f });
    labelWrapper->addChild(bodyLabel);

    auto renderNode = RetainingRenderNode::create(labelWrapper, true);
    renderNode->setID("label-render-node");
    renderNode->setAnchorPoint({ 0.f, .5f });
    this->addChildAtPosition(renderNode, geode::Anchor::Left, { iconXPos, 0.f });

    // this gets picked up in the shader
    // not sure why uniforms dont work, they just dont?
    uint16_t width = renderNode->getContentWidth();
    uint16_t height = renderNode->getContentHeight();
    renderNode->setColor({
        static_cast<GLubyte>(width & 0xff),
        static_cast<GLubyte>((width >> 8) & 0xff),
        static_cast<GLubyte>((height >> 8) & 0xff)
    });
    renderNode->setOpacity(static_cast<GLubyte>(height & 0xff));

    renderNode->setShaderProgram(cocos2d::CCShaderCache::sharedShaderCache()->programForKey("fade_shader"_spr));

    this->runAction(cocos2d::CCRepeatForever::create(
        cocos2d::CCSequence::create(
            geode::cocos::CallFuncExt::create([=, this] {
                m_index++;
                if (m_index == m_data.comments.size()) m_index = 0;

                auto& comment = m_data.comments.at(m_index);

                authorLabel->setString(comment.playerName.c_str());
                bodyLabel->setString(comment.content.c_str());

                icon->updateIcons(comment.player);

                likesLabel->setString(fmt::to_string(comment.likes).c_str());
                auto sprite = comment.likes < 0 ? "GJ_dislikesIcon_001.png" : "GJ_likesIcon_001.png";
                likesSprite->setDisplayFrame(cocos2d::CCSpriteFrameCache::get()->spriteFrameByName(sprite));
                likesWrapper->updateLayout();
            }),

            geode::cocos::CallFuncExt::create([=, this] {
                if (m_renderNode) m_renderNode->runAction(cocos2d::CCFadeIn::create(1.f));
            }), cocos2d::CCDelayTime::create(1.5f),

            geode::cocos::CallFuncExt::create([=, this] {
                float movementLeft = -(bodyLabel->getScaledContentWidth() - labelWrapper->getContentWidth()) - labelPadLeft - labelPadRight;
                if (movementLeft > 0) return;

                // this lets us use a variable length and delay
                // because the ccdelaytime's delay gets evaluated once in init and then not again
                // there's certainly better solutions to this but this is the simplest i could think of without making
                // a custom ccactioninterval

                float length = bodyLabel->getContentWidth() / 150.f;
                bodyLabel->runAction(cocos2d::CCSequence::create(
                    geode::cocos::CallFuncExt::create([this] { this->pauseSchedulerAndActions(); }),
                    cocos2d::CCMoveBy::create(length, { movementLeft, 0.f }),
                    geode::cocos::CallFuncExt::create([this] { this->resumeSchedulerAndActions(); }),
                    nullptr
                ));
            }),

            cocos2d::CCDelayTime::create(.5f),

            geode::cocos::CallFuncExt::create([=, this] {
                if (m_renderNode) m_renderNode->runAction(cocos2d::CCFadeOut::create(1.f));
            }), cocos2d::CCDelayTime::create(1.f),

            geode::cocos::CallFuncExt::create([=, this] {
                bodyLabel->setPositionX(labelPadLeft);
            }),

            nullptr
        )
    ));

    return true;
}
