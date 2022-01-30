#pragma once

#include "Kronos/Core/Memory.h"
#include "Kronos/Core/Module.h"

#include "Kronos/InputSystemModule/InputSystemModule.h"
#include "Kronos/LevelStreamingModule/LevelStreamingModule.h"

namespace Kronos
{
    class GameplayFrameworkModule final : public StaticModule
    {
    public:
        virtual void Initialize() {}
        virtual void Terminate() {}
    };

    class PlayerPawn
    {
    public:
        virtual void StartGame()
        {
            InputManager::BindKey(EInputKey::KEY_A, EInputKeyState::Pressed, this, &PlayerPawn::OnAPressed);
        }

    private:
        virtual void OnAPressed()
        {
        }
    };
    class PlayerController
    {
    };

    class GameMode
    {
    public:
        virtual void StartGame()
        {
        }
    };

    class GameState
    {
    };
    class PlayerState
    {
    };
    class HUD
    {
    };

    class GameInstance
    {
    public:
        virtual void BeginPlay()
        {
            Log::Trace("Starting game...");
            m_EntryLevel = Kronos::CreateRef<Level>();
            LevelStreamingManager::LoadLevel(m_EntryLevel);
        }

        virtual void Tick() {}

    private:
        Kronos::Ref<Level> m_EntryLevel;
        Kronos::Ref<GameMode> m_GameMode;
    };
}