#pragma once

#include "Kronos/Core/Layer.h"

#include "Kronos/LoggingModule/LoggingModule.h"

class Application
{
public:
    Application()
        : m_LayerStack()
    {
    }

    void Initialize()
    {
        Log::Trace("Initializing application...");

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

public: // TODO: Private
    LayerStack m_LayerStack;
};