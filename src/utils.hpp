#pragma once
#include <alphalaneous.alphas-ui-pack/nodes/RenderNode.hpp>

#define UNWRAP_INTO_MEMBER(output, thing) \
    if (thing.isOk()) { \
        auto cast = thing.unwrap().as<decltype(output)>(); \
        if (cast.isOk()) { \
            output = cast.unwrap(); \
        } else { \
            comments::log::warn("Could not unwrap casted: {} ({}@{})", thing.unwrapErr(), __FILE__, __LINE__); \
        } \
    } else { \
        comments::log::warn("Could not unwrap: {} ({}@{})", thing.unwrapErr(), __FILE__, __LINE__); \
    }

#define UNWRAP_INTO_MEMBER_AS(output, thing, asType) \
    if (thing.isOk()) { \
        auto cast = thing.unwrap().as<asType>(); \
        if (cast.isOk()) { \
            output = static_cast<decltype(output)>(cast.unwrap()); \
        } else { \
            comments::log::warn("Could not unwrap casted: {} ({}@{})", thing.unwrapErr(), __FILE__, __LINE__); \
        } \
    } else { \
        comments::log::warn("Could not unwrap: {} ({}@{})", thing.unwrapErr(), __FILE__, __LINE__); \
    }

#define UNWRAP_N_CAST(thing, type) \
    ([&] { \
        type member; \
        UNWRAP_INTO_MEMBER(member, thing) \
        return member; \
    }())

namespace comments::log {

template <typename... Args>
inline void trace(geode::format::FmtStr<Args...> str, Args&&... args) {
    if (geode::Mod::get()->getSettingValue<bool>("logging")) geode::log::trace(str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void debug(geode::format::FmtStr<Args...> str, Args&&... args) {
    if (geode::Mod::get()->getSettingValue<bool>("logging")) geode::log::debug(str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void info(geode::format::FmtStr<Args...> str, Args&&... args) {
    if (geode::Mod::get()->getSettingValue<bool>("logging")) geode::log::info(str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void warn(geode::format::FmtStr<Args...> str, Args&&... args) {
    if (geode::Mod::get()->getSettingValue<bool>("logging")) geode::log::warn(str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void error(geode::format::FmtStr<Args...> str, Args&&... args) {
    if (geode::Mod::get()->getSettingValue<bool>("logging")) geode::log::error(str, std::forward<Args>(args)...);
}

}
