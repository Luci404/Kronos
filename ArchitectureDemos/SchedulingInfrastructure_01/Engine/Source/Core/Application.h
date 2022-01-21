#pragma once

#include "Layer.h"

#include "LoggingModule.h"
#include "RendererModule.h"

class PlatformLayer : public Layer 
{
public:
    PlatformLayer()
    {
        m_LayerStack.PushLayer(new LoggingModule());
    }
};

class CoreLayer : public Layer {};
class ManagementLayer : public Layer {};
class SimulationLayer : public Layer {};
class PresentationLayer : public Layer 
{
public:
    PresentationLayer()
    {
        m_LayerStack.PushLayer(new RendererModule());
    }
};

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
        m_LayerStack.PushLayer(new PresentationLayer());
        
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