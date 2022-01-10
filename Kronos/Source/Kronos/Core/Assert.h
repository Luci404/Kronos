#pragma once

#include <filesystem>

#include "Kronos/Core/Log.h"

#define KRONOS_DEBUGBREAK() __debugbreak()

#define KRONOS_EXPAND_MACRO(x) x
#define KRONOS_STRINGIFY_MACRO(x) #x

#define KRONOS_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { KRONOS##type##ERROR(msg, __VA_ARGS__); KRONOS_DEBUGBREAK(); } }
#define KRONOS_INTERNAL_ASSERT_WITH_MSG(type, check, ...) KRONOS_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define KRONOS_INTERNAL_ASSERT_NO_MSG(type, check) KRONOS_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", KRONOS_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define KRONOS_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define KRONOS_INTERNAL_ASSERT_GET_MACRO(...) KRONOS_EXPAND_MACRO( KRONOS_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, KRONOS_INTERNAL_ASSERT_WITH_MSG, KRONOS_INTERNAL_ASSERT_NO_MSG) )

#define KRONOS_ASSERT(...) KRONOS_EXPAND_MACRO( KRONOS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define KRONOS_CORE_ASSERT(...) KRONOS_EXPAND_MACRO( KRONOS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )