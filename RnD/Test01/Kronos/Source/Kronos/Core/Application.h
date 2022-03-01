#pragma once

#include "Kronos/Core/Layer.h"

#include "Kronos/LoggingModule/LoggingModule.h"

namespace Kronos
{
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
            for (std::vector<Layer *>::reverse_iterator it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
            {
                (*it)->Terminate();
            }
        }

    public: // TODO: Private
        LayerStack m_LayerStack;
    };
}