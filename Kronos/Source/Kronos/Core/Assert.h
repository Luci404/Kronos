#pragma once

#include <Kronos/Core/Log.h>

#define KRONOS_CORE_ASSERT(x, ...) { if(!(x)) { KRONOS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }