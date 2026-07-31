#include "CommentData.hpp"
#include "utils.hpp"

SingleComment::SingleComment(matjson::Value data) {
    UNWRAP_INTO_MEMBER(content, data.get("content"))

    auto iconData = cue::Icons();
    UNWRAP_INTO_MEMBER(iconData.color1, data.get("iconMainColor"));
    UNWRAP_INTO_MEMBER(iconData.color2, data.get("iconSecondaryColor"));
    UNWRAP_INTO_MEMBER(iconData.glowColor, data.get("iconGlowColor"));
    UNWRAP_INTO_MEMBER(iconData.id, data.get("iconFrame"));
    UNWRAP_INTO_MEMBER_AS(iconData.type, data.get("iconType"), int);
    player = cue::PlayerIcon::create(iconData);
}

CommentData::CommentData(matjson::Value data) {
    UNWRAP_INTO_MEMBER(levelID, data.get("levelID"))

    for (auto commentData : UNWRAP_N_CAST(data.get("comments"), std::vector<matjson::Value>)) {
        comments.push_back(SingleComment(commentData));
    }
}
