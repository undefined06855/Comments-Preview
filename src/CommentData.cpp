#include "CommentData.hpp"
#include "utils.hpp"

SingleComment::SingleComment(const matjson::Value& data) {
    UNWRAP_INTO_MEMBER(content, data.get("comment"))
    UNWRAP_INTO_MEMBER(playerName, data.get("player_name"))
    UNWRAP_INTO_MEMBER(likes, data.get("likes"))

    player = cue::Icons();
    UNWRAP_INTO_MEMBER(player.color1, data.get("icon_main_color"));
    UNWRAP_INTO_MEMBER(player.color2, data.get("icon_secondary_color"));
    UNWRAP_INTO_MEMBER(player.glowColor, data.get("icon_glow_color"));
    UNWRAP_INTO_MEMBER(player.id, data.get("icon_frame"));
    UNWRAP_INTO_MEMBER_AS(player.type, data.get("icon_type"), int);
}

CommentData::CommentData(int id, const matjson::Value& data) {
    levelID = id;

    for (auto commentData : data) {
        comments.push_back(SingleComment(commentData));
    }
}
