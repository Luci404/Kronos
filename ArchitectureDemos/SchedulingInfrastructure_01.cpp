#include <vector>

/*
LayerStack: A list of layers with some utility functions for layer management.

Layer: A structure reposible for the execution of modules, furthermore, layers can have extra logic and data for specialized layer behavior.

ModuleStack: A list of modules with some utility functions for module management.

Module: Resposible for the initialization, termination and ticking of a feature or api. For some systems like the physics engine,
the module would be responsible for initialization of PhysX at preperation for it's use in other modules on higher layers.
Other modules might not do other than reading a config file.
*/

#pragma region SchedulingInfrastructure

class Module
{
public:
    virtual void Initialize(){};
    virtual void Tick(){};
    virtual void Terminate(){};
};

class ModuleStack
{
public:
    Module* PushModule(Module* module) { return module; }

    std::vector<Module*>::iterator begin() { return m_Modules.begin(); }
    std::vector<Module*>::iterator end() { return m_Modules.end(); }
    std::vector<Module*>::reverse_iterator rbegin() { return m_Modules.rbegin(); }
    std::vector<Module*>::reverse_iterator rend() { return m_Modules.rend(); }

    std::vector<Module*>::const_iterator begin() const { return m_Modules.begin(); }
    std::vector<Module*>::const_iterator end() const { return m_Modules.end(); }
    std::vector<Module*>::const_reverse_iterator rbegin() const { return m_Modules.rbegin(); }
    std::vector<Module*>::const_reverse_iterator rend() const { return m_Modules.rend(); }

private:
    std::vector<Module *> m_Modules;
};

class Layer
{
public:
    // Inheriting layers should add modules in the constructor.

    virtual void Initialize()
    {
        for (Module *module : m_ModuleStack)
        {
            module->Initialize();
        }
    };

    virtual void Tick()
    {
        for (Module *module : m_ModuleStack)
        {
            module->Tick();
        }
    };

    virtual void Terminate()
    {
        for (Module *module : m_ModuleStack)
        {
            module->Terminate();
        }
    };

protected:
    ModuleStack m_ModuleStack;
};

class LayerStack
{
public:
    Layer* PushLayer(Layer* layer) { return layer; }

    std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
    std::vector<Layer*>::iterator end() { return m_Layers.end(); }
    std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
    std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

    std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
    std::vector<Layer*>::const_iterator end() const { return m_Layers.end(); }
    std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
    std::vector<Layer*>::const_reverse_iterator rend() const { return m_Layers.rend(); }

private:
    std::vector<Layer*> m_Layers;
};

#pragma endregion SchedulingInfrastructure

#pragma region Layers

// Platform layer
class LoggingModule : public Module {};
class ThreadingModule : public Module {};
class FileIOModule : public Module {};

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
class JobSystem : public Module {};
class EngineConfig : public Module {};
class Serializer : public Module {};

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
class SceneGraph : public Module {};
class Networking : public Module {};

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
class Physics : public Module {};
class EventSystem : public Module {};

class SimulationLayer : public Layer
{
public:
    SimulationLayer()
    {
        m_ModuleStack.PushModule(new Physics());
        m_ModuleStack.PushModule(new EventSystem());
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