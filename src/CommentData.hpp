#pragma once
#include <cue/PlayerIcon.hpp>

struct SingleComment {
    SingleComment() = default;
    SingleComment(const matjson::Value& data);

    std::string content;
    std::string playerName;
    int likes;
    cue::Icons player;
};

struct CommentData {
    CommentData() = default;
    CommentData(int id, const matjson::Value& data);

    std::vector<SingleComment> comments;
    int levelID;
};
