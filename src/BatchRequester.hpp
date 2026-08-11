#pragma once
#include "CommentData.hpp"

struct CommentDataWithTimeout {
    CommentData data;
    asp::time::Instant expiryTime;
};

class BatchRequester : public cocos2d::CCObject {
    BatchRequester();
public:
    static BatchRequester& get();
    void init();

    std::vector<int> m_levelIDQueue;
    std::unordered_map<int, CommentDataWithTimeout> m_cache;

    bool m_timerStarted;
    float m_timer;

    void queueID(int id);
    arc::Future<geode::Result<>> fetchComments(std::vector<int> ids);

    geode::Result<CommentData> getCommentData(int id);

    virtual void update(float dt) override;
};
