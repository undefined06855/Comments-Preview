#pragma once
#include <cue/PlayerIcon.hpp>

struct SingleComment {
    SingleComment(matjson::Value data);

    std::string content;
    geode::Ref<cue::PlayerIcon> player;
};

struct CommentData {
    CommentData(matjson::Value data);

    std::vector<SingleComment> comments;
    int levelID;
};
