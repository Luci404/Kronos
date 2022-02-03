#pragma once

#include "Kronos/Core/Module.h"
#include "Kronos/Core/Assert.h"
#include "Kronos/LoggingModule/LoggingModule.h"

#include <glad/glad.h>

#include <windows.h>
#include <gl/GL.h>
#include <d2d1.h>
#include <stdio.h>

#define KRONOS_WNDCLASSNAME L"SampleWindowClass"

namespace Kronos
{

    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;

    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n\0";

    unsigned int VBO, VAO;
    unsigned int shaderProgram;

    HDC ourWindowHandleToDeviceContext;
    HGLRC ourOpenGLRenderingContext;

    LRESULT CALLBACK WndProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CREATE:
        {
            PIXELFORMATDESCRIPTOR pixelFormatDescriptor =
            {
                sizeof(PIXELFORMATDESCRIPTOR),
                1,
                PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
                PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
                32,                   // Colordepth of the framebuffer.
                0, 0, 0, 0, 0, 0,
                0,
                0,
                0,
                0, 0, 0, 0,
                24,                   // Number of bits for the depthbuffer
                8,                    // Number of bits for the stencilbuffer
                0,                    // Number of Aux buffers in the framebuffer.
                PFD_MAIN_PLANE,
                0,
                0, 0, 0
            };

            // Create, and make context current.
            ourWindowHandleToDeviceContext = GetDC(windowHandle);

            int letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pixelFormatDescriptor);
            SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pixelFormatDescriptor);

            ourOpenGLRenderingContext = wglCreateContext(ourWindowHandleToDeviceContext);
            wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRenderingContext);
            
            // Initialize glad
            KRONOS_CORE_ASSERT(gladLoadGL(), "Failed to initialize OpenGL context.");

            // TODO: MOVE
             // build and compile our shader program
    // ------------------------------------
    // vertex shader
            unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
            glCompileShader(vertexShader);
            // check for shader compile errors
            int success;
            char infoLog[512];
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
            // fragment shader
            unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
            glCompileShader(fragmentShader);
            // check for shader compile errors
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
            // link shaders
            shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);
            // check for linking errors
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            }
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            // set up vertex data (and buffer(s)) and configure vertex attributes
            // ------------------------------------------------------------------
            float vertices[] = {
                -0.5f, -0.5f, 0.0f, // left  
                 0.5f, -0.5f, 0.0f, // right 
                 0.0f,  0.5f, 0.0f  // top   
            };

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
            // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
            glBindVertexArray(0);
        }
        break;
        case WM_CLOSE:
        {
            wglMakeCurrent(ourWindowHandleToDeviceContext, NULL);
            wglDeleteContext(ourOpenGLRenderingContext);
            PostQuitMessage(0);

            DestroyWindow(windowHandle);
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        case WM_SIZE:
        {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            glViewport(0, 0, width, height);

            break;
        }
        default: return DefWindowProc(windowHandle, msg, wParam, lParam);
        }
        return 0;
    }

    class Window
    {
    public:
        Window(const std::string &title = "untitled", uint32_t width = 1600, uint32_t height = 900, uint32_t xPosition = 0, uint32_t yPosition = 0, bool maximized = false, bool vSync = true)
        {
            Log::Trace("Creating window...");

            WNDCLASSEXW wc = {};
            wc.cbSize = sizeof(wc);
            wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
            wc.lpfnWndProc = (WNDPROC)WndProc;
            wc.hInstance = GetModuleHandleW(NULL);
            wc.hCursor = LoadCursor(NULL, IDC_ARROW);
            wc.lpszClassName = KRONOS_WNDCLASSNAME;
            wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
            KRONOS_CORE_ASSERT(RegisterClassExW(&wc), "Failed to register class.");

            DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION | WS_MAXIMIZEBOX | WS_THICKFRAME;
            DWORD exStyle = WS_EX_APPWINDOW;
            m_WindowHandle = CreateWindowExW(exStyle, KRONOS_WNDCLASSNAME, L"untitled", style, 160, 90, 1600, 900, NULL, NULL, GetModuleHandleW(NULL), NULL);
            KRONOS_CORE_ASSERT(m_WindowHandle != NULL, "Failed to create window.");

            ShowWindow(m_WindowHandle, SW_SHOWNORMAL);
            UpdateWindow(m_WindowHandle);
        }

        void PollEvents()
        {
            MSG msg;
            HWND handle;

            while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }

        void Render()
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // draw our first triangle
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
            glDrawArrays(GL_TRIANGLES, 0, 3);

            HDC deviceContext = GetDC(m_WindowHandle);
            SwapBuffers(deviceContext);
        }

        HWND GetHWND_TMP() const
        {
            return m_WindowHandle;
        }

        virtual ~Window()
        {
            Log::Trace("Destroying window...");
        }

        virtual void Update() {}

    private:
        std::string m_Title;
        uint32_t m_Width;
        uint32_t m_Height;
        uint32_t m_XPosition;
        uint32_t m_YPosition;
        bool m_Maximized;
        bool m_VSync;

        HWND m_WindowHandle;
    };

    class WindowModule : public StaticModule
    {
    };
}