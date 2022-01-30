#include "Core/Memory.h"
#include "Core/Layer.h"
#include "Core/Application.h"

#include "LoggingModule/LoggingModule.h"
#include "InputSystemModule/InputSystemModule.h"
#include "IntegrantModule/IntegrantModule.h"
#include "LevelStreamingModule/LevelStreamingModule.h"
#include "GameplayFrameworkModule/GameplayFrameworkModule.h"

class PlatformLayer final : public Layer
{
public:
    PlatformLayer()
    {
        m_ModuleStack.PushModule(new LoggingModule());
        Log::Trace("Initializing platform layer...");
        m_ModuleStack.PushModule(new InputSystemModule());
    }
};

class CoreLayer final : public Layer
{
public:
    CoreLayer()
    {
        Log::Trace("Initializing core layer...");
        m_ModuleStack.PushModule(new IntegrantModule());
    }
};

class SimulationLayer final : public Layer
{
public:
    SimulationLayer()
    {
        Log::Trace("Initializing simulation layer...");
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
    ExampleGameModule() 
    {
        Log::Trace("Initializing game module...");
        m_ExampleGameGameInstance = Kronos::CreateRef<ExampleGameGameInstance>();
        m_ExampleGameGameInstance->BeginPlay();
    }

private:
    Kronos::Ref<ExampleGameGameInstance> m_ExampleGameGameInstance; 
};

class ApplicationLayer final : public Layer
{
public:
    ApplicationLayer()
    {
        Log::Trace("Initializing application layer...");
        m_ModuleStack.PushModule(new ExampleGameModule()); // TODO: This sould be a dynamic module.
    }
};

int main(int argc, char* argv[])
{
    Application application;
    application.m_LayerStack.PushLayer(new PlatformLayer());
    application.m_LayerStack.PushLayer(new CoreLayer());
    application.m_LayerStack.PushLayer(new SimulationLayer());
    application.m_LayerStack.PushLayer(new ApplicationLayer());

    application.Initialize();
    while (true) { application.Tick(); }
    application.Terminate();
}