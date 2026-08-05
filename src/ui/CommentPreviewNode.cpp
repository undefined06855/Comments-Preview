#include "CommentPreviewNode.hpp"

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

    float labelPadLeft = 15.f;
    float labelPadRight = 20.f;
    float iconXPos = 8.0625;

    this->setAnchorPoint({ 0.f, .5f });
    this->setContentSize({ availableWidth, 16.f });

    auto icon = cue::PlayerIcon::create(IconType::Cube);
    icon->setID("icon");
    icon->setScale(.5f);
    icon->setZOrder(2);
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

    auto likesLabel = cocos2d::CCLabelBMFont::create("...", "bigFont.fnt");
    likesLabel->setID("likes-label");
    likesLabel->setScale(.15f);
    likesWrapper->addChild(likesLabel);

    // so that the rendernode has the correct size with constrain set to true
    m_labelWrapper = cocos2d::CCNode::create();
    m_labelWrapper->setID("label-wrapper");
    m_labelWrapper->setContentSize({ this->getContentWidth() - iconXPos, this->getContentHeight() });

    auto label = cocos2d::CCLabelBMFont::create("...", "chatFont.fnt");
    label->setID("label");
    label->setScale(.6f);
    label->setAnchorPoint({ 0.f, .5f });
    label->setPosition({ labelPadLeft, this->getContentHeight() / 2.f });
    m_labelWrapper->addChild(label);

    auto renderNode = alpha::ui::RenderNode::create(m_labelWrapper, true);
    renderNode->setID("render-node");
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

                label->setString(comment.content.c_str());
                icon->updateIcons(comment.player);
                likesLabel->setString(fmt::to_string(comment.likes).c_str());
                auto sprite = comment.likes < 0 ? "GJ_dislikesIcon_001.png" : "GJ_likesIcon_001.png";
                likesSprite->setDisplayFrame(cocos2d::CCSpriteFrameCache::get()->spriteFrameByName(sprite));
                likesWrapper->updateLayout();
            }),

            geode::cocos::CallFuncExt::create([=, this] {
                label->runAction(cocos2d::CCFadeIn::create(1.f));
                icon->runAction(cocos2d::CCFadeIn::create(1.f));
                likesWrapper->runAction(cocos2d::CCFadeIn::create(1.f));
            }), cocos2d::CCDelayTime::create(1.5f),

            geode::cocos::CallFuncExt::create([=, this] {
                float movementLeft = -(label->getScaledContentWidth() - m_labelWrapper->getContentWidth()) - labelPadLeft - labelPadRight;
                if (movementLeft > 0) return;

                // this lets us use a variable length and delay
                // because the ccdelaytime's delay gets evaluated once in init and then not again
                // there's certainly better solutions to this but this is the simplest i could think of without making
                // a custom ccactioninterval

                float length = label->getContentWidth() / 150.f;
                label->runAction(cocos2d::CCSequence::create(
                    geode::cocos::CallFuncExt::create([this] { this->pauseSchedulerAndActions(); }),
                    cocos2d::CCMoveBy::create(length, { movementLeft, 0.f }),
                    geode::cocos::CallFuncExt::create([this] { this->resumeSchedulerAndActions(); }),
                    nullptr
                ));
            }),

            cocos2d::CCDelayTime::create(.5f),

            geode::cocos::CallFuncExt::create([=, this] {
                label->runAction(cocos2d::CCFadeOut::create(1.f));
                icon->runAction(cocos2d::CCFadeOut::create(1.f));
                likesWrapper->runAction(cocos2d::CCFadeOut::create(1.f));
            }), cocos2d::CCDelayTime::create(1.f),

            geode::cocos::CallFuncExt::create([=, this] {
                label->setPositionX(labelPadLeft);
            }),

            nullptr
        )
    ));

    return true;
}
