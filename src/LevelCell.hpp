#include <Geode/modify/LevelCell.hpp>
#include "CommentData.hpp"

class $modify(HookedLevelCell, LevelCell) {
    struct Fields {
        Fields();
        int attempts;
    };

    void loadFromLevel(GJGameLevel* level);

    void attemptAddComments();
    void addComments(const CommentData& data);
};
