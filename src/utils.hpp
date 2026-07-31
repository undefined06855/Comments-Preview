#pragma once

#define UNWRAP_INTO_MEMBER(output, thing) \
    if (thing.isOk()) { \
        auto cast = thing.unwrap().as<decltype(output)>(); \
        if (cast.isOk()) { \
            output = cast.unwrap(); \
        } else { \
            geode::log::warn("Could not unwrap casted: {}", thing.unwrapErr()); \
        } \
    } else { \
        geode::log::warn("Could not unwrap: {}", thing.unwrapErr()); \
    }

#define UNWRAP_INTO_MEMBER_AS(output, thing, asType) \
    if (thing.isOk()) { \
        auto cast = thing.unwrap().as<asType>(); \
        if (cast.isOk()) { \
            output = static_cast<decltype(output)>(cast.unwrap()); \
        } else { \
            geode::log::warn("Could not unwrap casted: {}", thing.unwrapErr()); \
        } \
    } else { \
        geode::log::warn("Could not unwrap: {}", thing.unwrapErr()); \
    }

#define UNWRAP_N_CAST(thing, type) \
    ([&] { \
        type member; \
        UNWRAP_INTO_MEMBER(member, thing) \
        return member; \
    }())
