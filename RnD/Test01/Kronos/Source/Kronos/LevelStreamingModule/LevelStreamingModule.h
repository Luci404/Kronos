#pragma once

#include "Kronos/Core/Memory.h"
#include "Kronos/Core/Module.h"

namespace Kronos
{
    class LevelStreamingModule final : public StaticModule
    {
    };

    class Level
    {
    };

    class LevelStreamingManager
    {
    public:
        static void LoadLevel(Kronos::Ref<Level> level) {}
        static void UnloadLevel(Kronos::Ref<Level> level) {}
    };
}