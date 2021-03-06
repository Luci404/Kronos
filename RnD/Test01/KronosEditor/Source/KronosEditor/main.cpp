#include <Kronos/Core/Memory.h>
#include <Kronos/Core/Layer.h>
#include <Kronos/Core/Application.h>

#include <Kronos/LoggingModule/LoggingModule.h>
#include <Kronos/WindowModule/WindowModule.h>
#include <Kronos/InputSystemModule/InputSystemModule.h>
#include <Kronos/IntegrantModule/IntegrantModule.h>
#include <Kronos/LevelStreamingModule/LevelStreamingModule.h>
#include <Kronos/RendererModule/RendererModule.h>
#include <Kronos/GameplayFrameworkModule/GameplayFrameworkModule.h>

namespace KronosEditor
{
    class PlatformLayer final : public Kronos::Layer
    {
    public:
        PlatformLayer()
        {
            Kronos::Log::Trace("Initializing platform layer...");
            m_ModuleStack.PushModule(new Kronos::LoggingModule());
            m_ModuleStack.PushModule(new Kronos::WindowModule());
            m_ModuleStack.PushModule(new Kronos::InputSystemModule());
        }
    };

    class CoreLayer final : public Kronos::Layer
    {
    public:
        CoreLayer()
        {
            Kronos::Log::Trace("Initializing core layer...");
            m_ModuleStack.PushModule(new Kronos::IntegrantModule());
        }
    };

    class SimulationLayer final : public Kronos::Layer
    {
    public:
        SimulationLayer()
        {
            Kronos::Log::Trace("Initializing simulation layer...");
            m_ModuleStack.PushModule(new Kronos::LevelStreamingModule());
            m_ModuleStack.PushModule(new Kronos::RendererModule());
            m_ModuleStack.PushModule(new Kronos::GameplayFrameworkModule());
        }
    };

    class ExampleGameGameInstance : public Kronos::GameInstance
    {
    public:
        ExampleGameGameInstance()
        {
            m_GameWindow = Kronos::CreateRef<Kronos::Window>();
            //m_SceneRenderer = Kronos::CreateRef<Kronos::SceneRenderer>(m_GameWindow);
        }

        void Tick() override
        {
            Kronos::GameInstance::Tick();
            //m_SceneRenderer->Render();
            m_GameWindow->PollEvents();
        }

    private:
        Kronos::Ref<Kronos::Window> m_GameWindow;
        //Kronos::Ref<Kronos::SceneRenderer> m_SceneRenderer;
    };

    class ExampleGameModule : public Kronos::StaticModule
    {
    public:
        ExampleGameModule()
        {
            Kronos::Log::Trace("Initializing game module...");
            m_ExampleGameGameInstance = Kronos::CreateRef<ExampleGameGameInstance>();
            m_ExampleGameGameInstance->BeginPlay();
        }

        void Tick() override
        {
            StaticModule::Tick();
            m_ExampleGameGameInstance->Tick();
        }

    private:
        Kronos::Ref<ExampleGameGameInstance> m_ExampleGameGameInstance;
    };

    class ApplicationLayer final : public Kronos::Layer
    {
    public:
        ApplicationLayer()
        {
            Kronos::Log::Trace("Initializing application layer...");
            m_ModuleStack.PushModule(new ExampleGameModule()); // TODO: This sould be a dynamic module.
        }
    };
}

int main(int argc, char* argv[])
{
    Kronos::Application application;
    application.m_LayerStack.PushLayer(new KronosEditor::PlatformLayer());
    application.m_LayerStack.PushLayer(new KronosEditor::CoreLayer());
    application.m_LayerStack.PushLayer(new KronosEditor::SimulationLayer());
    application.m_LayerStack.PushLayer(new KronosEditor::ApplicationLayer());

    application.Initialize();
    while (true)
    {
        application.Tick();
    }
    application.Terminate();
}

// To ask
// Should surface creation be a resposibility of the renderer or the window class.
// The renderer would need to call low-level platform specific stuff...
// Where should I draw the line?..

// High priority
// TODO: Renderer module api blockout

// Medium priority
// TODO: Hot-reloading (DynamicModule).

// Low priority
// TODO: Graph tool for visualization of module dependencies and nodes graphs (shader builders etc.), this tool could, and properly should, be build on ImGui.