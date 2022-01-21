#pragma once

#include "Module.h"

#include <vector>

class Layer
{
public:
    // Inheriting layers should add modules in the constructor.

    virtual void Initialize()
    {
        for (StaticModule *module : m_ModuleStack)
        {
            module->Initialize();
        }
    };

    virtual void Tick()
    {
        for (StaticModule *module : m_ModuleStack)
        {
            module->Tick();
        }
    };

    virtual void Terminate()
    {
        for (StaticModule *module : m_ModuleStack)
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
    Layer* PushLayer(Layer* layer)
    { 
        m_Layers.push_back(layer);
        return layer;
    }

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