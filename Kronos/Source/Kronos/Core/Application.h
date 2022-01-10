#include "Kronos/Core/Window.h"
#include "Kronos/Core/Assert.h"
#include "Kronos/Core/Log.h"

namespace Kronos
{
    class Application
    {
    public:
        Application(const std::string &name = "Kronos Application")
        {
            Log::Initialize();

            KRONOS_CORE_TRACE("Trace")
            KRONOS_CORE_INFO("Info")
            KRONOS_CORE_WARN("Warn")
            KRONOS_CORE_ERROR("Error")
            KRONOS_CORE_CRITICAL("Critical")
            
            m_Window = CreateScope<Window>();

            while (true)
            {
                m_Window->Update();
            }
        }

        virtual ~Application()
        {
        }

    private:
        Scope<Window> m_Window;
    };
}