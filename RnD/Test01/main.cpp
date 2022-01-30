#include "Core/Memory.h"
#include "Core/Layer.h"
#include "Core/Application.h"

#include "InputSystemModule/InputSystemModule.h"
#include "LevelStreamingModule/LevelStreamingModule.h"
#include "GameplayFrameworkModule/GameplayFrameworkModule.h"

class PlatformLayer final : public Layer
{
public:
    virtual void Initialize() override
    {
        m_ModuleStack.PushModule(new InputSystemModule());
    }
};

class SimulationLayer final : public Layer
{
public:
    virtual void Initialize() override
    {
        m_ModuleStack.PushModule(new LevelStreamingModule());
        m_ModuleStack.PushModule(new GameplayFrameworkModule());
    }
};

class ExampleGameGameInstance : public GameInstance
{
public:
    ExampleGameGameInstance()
    {
        //GameModeClass = ExampleGameGameMode.ClassStuff;
        //GameStateClass = ExampleGameGameState.ClassStuff;
    }
};

class ExampleGameModule : public StaticModule
{
public:
    virtual void Initialize() override
    {
        m_ExampleGameGameInstance->StartGame();
    }

    virtual void Terminate() override
    {
        //m_ExampleGameGameInstance->StopGame();
    }

private:
    Kronos::Ref<ExampleGameGameInstance> m_ExampleGameGameInstance; 
};

class ApplicationLayer final : public Layer
{
public:
    virtual void Initialize()
    {
        // TODO: This sould be a dynamic module.
        m_ModuleStack.PushModule(new ExampleGameModule());
    }
};

int main(int argc, char* argv[])
{
    Application application;
    application.m_LayerStack.PushLayer(new SimulationLayer());
    application.m_LayerStack.PushLayer(new ApplicationLayer());

    application.Initialize();
    while (true) { application.Tick(); }
    application.Terminate();
}