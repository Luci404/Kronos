#include <vector>
#include <iostream>

/*
LayerStack: A list of layers with some utility functions for layer management.

Layer: A structure reposible for the execution of modules, furthermore, layers can have extra logic and data for specialized layer behavior.

ModuleStack: A list of modules with some utility functions for module management.

Module: Resposible for the initialization, termination and ticking of a feature or api. For some systems like the physics engine,
the module would be responsible for initialization of PhysX at preperation for it's use in other modules on higher layers.
Other modules might not do other than reading a config file.
*/

#pragma region Layers

// Platform layer
class LoggingModule : public StaticModule {};
class ThreadingModule : public StaticModule {};
class FileIOModule : public StaticModule {};

class PlatformLayer : public Layer
{
public:
    PlatformLayer()
    {
        m_ModuleStack.PushModule(new LoggingModule());
        m_ModuleStack.PushModule(new ThreadingModule());
        m_ModuleStack.PushModule(new FileIOModule());
    }
};

// Core layer
class JobSystem : public StaticModule {};
class EngineConfig : public StaticModule {};
class Serializer : public StaticModule {};

class CoreLayer : public Layer
{
public:
    CoreLayer()
    {
        m_ModuleStack.PushModule(new JobSystem());
        m_ModuleStack.PushModule(new EngineConfig());
        m_ModuleStack.PushModule(new Serializer());
    }
};

// Management layer
class SceneGraph : public StaticModule {};
class Networking : public StaticModule {};

class ManagementLayer : public Layer
{
public:
    ManagementLayer()
    {
        m_ModuleStack.PushModule(new SceneGraph());
        m_ModuleStack.PushModule(new Networking());
    }
};

// Simulation layer
class Physics : public StaticModule {};
class EventSystem : public StaticModule {};

class SimulationLayer : public Layer
{
public:
    SimulationLayer()
    {
        m_ModuleStack.PushModule(new EventSystem());
        m_ModuleStack.PushModule(new Physics());
    }
};

// Application layer
#define CR_HOST
#define CR_WINDOWS
#include "cr.h"

class DynamicModule : public StaticModule 
{
public:
    virtual void Initialize() override
    {
        cr_plugin_open(ctx, "C:/Dev/Kronos/ArchitectureDemos/build/Debug/module.dll");
    }
    
    virtual void Tick() override
    {
        cr_plugin_update(ctx);
    }
    
    virtual void Terminate() override
    {
        cr_plugin_close(ctx);
    }

private:
    cr_plugin ctx;
};

class ApplicationLayer : public Layer
{
public:
    ApplicationLayer()
    {
        //m_ModuleStack.PushModule(new DynamicModule());
    }
};

#pragma endregion Layers

#pragma region Application

class Application
{
public:
    Application()
        : m_LayerStack()
    {
        m_LayerStack.PushLayer(new PlatformLayer());
        m_LayerStack.PushLayer(new CoreLayer());
        m_LayerStack.PushLayer(new ManagementLayer());
        m_LayerStack.PushLayer(new SimulationLayer());
        m_LayerStack.PushLayer(new ApplicationLayer());

        Initialize();

        while (true)
        {
            Tick();
        }

        Terminate();
    }

    void Initialize()
    {
        for (Layer *layer : m_LayerStack)
        {
            layer->Initialize();
        }
    }

    void Tick()
    {
        for (Layer *layer : m_LayerStack)
        {
            layer->Tick();
        }
    }

    void Terminate()
    {
        for (Layer *layer : m_LayerStack)
        {
            layer->Terminate();
        }
    }

private:
    LayerStack m_LayerStack;
};

#pragma endregion Application

#include <iostream>

int main(int argc, char* argv[])
{
    Application application;

    application.Initialize();

    while (true)
    {
        application.Tick();
    }

    application.Terminate();

    return 0;
}