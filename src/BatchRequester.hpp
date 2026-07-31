#pragma once

class BatchRequester : public cocos2d::CCObject {
public:
    static BatchRequester& get();
    void init();

    std::vector<int> m_levelIDQueue;

    void queueID(int id);
    arc::Future<geode::Result<>> fetchComments(std::vector<int> ids);

    virtual void update(float dt) override;
};
