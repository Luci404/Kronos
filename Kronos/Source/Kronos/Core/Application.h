#include <Kronos/Core/Memory.h>
#include <Kronos/Core/Window.h>

namespace Kronos
{
    class Application
    {
    public:
        Application(const std::string &name = "Kronos Application")
        {
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