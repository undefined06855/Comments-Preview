#pragma once
#include <cue/PlayerIcon.hpp>

struct SingleComment {
    SingleComment(const matjson::Value& data);

    std::string content;
    std::string playerName;
    int likes;
    geode::Ref<cue::PlayerIcon> player;
};

struct CommentData {
    CommentData(int id, const matjson::Value& data);

    std::vector<SingleComment> comments;
    int levelID;
};
