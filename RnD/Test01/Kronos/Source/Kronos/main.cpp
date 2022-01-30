#include "Kronos/Core/Memory.h"
#include "Kronos/Core/Layer.h"
#include "Kronos/Core/Application.h"

#include "Kronos/LoggingModule/LoggingModule.h"
#include "Kronos/WindowModule/WindowModule.h"
#include "Kronos/InputSystemModule/InputSystemModule.h"
#include "Kronos/IntegrantModule/IntegrantModule.h"
#include "Kronos/LevelStreamingModule/LevelStreamingModule.h"
#include "Kronos/GameplayFrameworkModule/GameplayFrameworkModule.h"

class PlatformLayer final : public Layer
{
public:
    PlatformLayer()
    {
        m_ModuleStack.PushModule(new LoggingModule());
        Log::Trace("Initializing platform layer...");
        m_ModuleStack.PushModule(new Kronos::WindowModule());
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

        m_GameWindow = Kronos::CreateRef<Kronos::Window>();
    }
private:

    Kronos::Ref<Kronos::Window> m_GameWindow;
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
    std::cout << "test" << std::endl;
    application.m_LayerStack.PushLayer(new SimulationLayer());
    std::cout << "test" << std::endl;
    application.m_LayerStack.PushLayer(new ApplicationLayer());
    std::cout << "test" << std::endl;

    application.Initialize();
    while (true) { application.Tick(); }
    application.Terminate();
}