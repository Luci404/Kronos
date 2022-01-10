#include <stdint.h>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Kronos
{
    class Window
    {
    public:
        Window(const std::string &title = "untitled", uint32_t width = 1600, uint32_t height = 900, uint32_t xPosition = 0, uint32_t yPosition = 0, bool maximized = false, bool vSync = true);

        ~Window();

        virtual void Update();

    private:
        std::string m_Title;
        uint32_t m_Width;
        uint32_t m_Height;
        uint32_t m_XPosition;
        uint32_t m_YPosition;
        bool m_Maximized;
        bool m_VSync;
        
        GLFWwindow* m_GLFWWindow;
    };
}