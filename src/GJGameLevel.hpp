#pragma once
#include "CommentData.hpp"

class $modify(HookedGJGameLevel, GJGameLevel) {
    struct Fields {
        CommentData commentData;
    };
};
