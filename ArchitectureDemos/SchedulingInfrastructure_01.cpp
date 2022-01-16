#include <vector>

// Scheduling infrastructure
class Module
{
public:
    virtual void Initialize() = 0;
    virtual void Tick() = 0;
    virtual void Terminate() = 0;
};

class ModuleStack
{
public
    Module *PushModule(Module *module) { return module; }

    std::vector<Module *>::iterator begin() { return m_Modules.begin(); }
    std::vector<Module *>::iterator end() { return m_Modules.end(); }
    std::vector<Module *>::reverse_iterator rbegin() { return m_Modules.rbegin(); }
    std::vector<Module *>::reverse_iterator rend() { return m_Modules.rend(); }

    std::vector<Module *>::const_iterator begin() const { return m_Modules.begin(); }
    std::vector<Module *>::const_iterator end() const { return m_Modules.end(); }
    std::vector<Module *>::const_reverse_iterator rbegin() const { return m_Modules.rbegin(); }
    std::vector<Module *>::const_reverse_iterator rend() const { return m_Modules.rend(); }

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
    Layer *PushLayer(Layer *layer) { return layer; }

    std::vector<Layer *>::iterator begin() { return m_Layers.begin(); }
    std::vector<Layer *>::iterator end() { return m_Layers.end(); }
    std::vector<Layer *>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
    std::vector<Layer *>::reverse_iterator rend() { return m_Layers.rend(); }

    std::vector<Layer *>::const_iterator begin() const { return m_Layers.begin(); }
    std::vector<Layer *>::const_iterator end() const { return m_Layers.end(); }
    std::vector<Layer *>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
    std::vector<Layer *>::const_reverse_iterator rend() const { return m_Layers.rend(); }

private:
    std::vector<Layer *> m_Layers;
};

#pragma region Layers

// Platform layer
class PlatformLayer : public Layer
{
    PlatformLayer() {}
};

// Core layer
class CoreLayer : public Layer
{
    CoreLayer() {}
};

// Management layer
class ManagementLayer : public Layer
{
    ManagementLayer() {}
};

// Simulation layer
class SimulationLayer : public Layer
{
    SimulationLayer() {}
};
#pragma endregion Layers


// Application (simplified)
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
