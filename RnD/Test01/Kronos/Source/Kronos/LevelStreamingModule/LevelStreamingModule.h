#pragma once

#include "../Core/Memory.h"
#include "../Core/Module.h"

class LevelStreamingModule final : public StaticModule {};

class Level {};

class LevelStreamingManager
{
public:
    static void LoadLevel(Kronos::Ref<Level> level) {}
    static void UnloadLevel(Kronos::Ref<Level> level) {}
};
