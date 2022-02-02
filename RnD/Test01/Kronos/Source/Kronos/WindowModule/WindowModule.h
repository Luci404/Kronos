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

            glViewport(0, 0, 800, 600);
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
        case WM_DESTROY: { PostQuitMessage(0); break; }
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
            HDC deviceContext = GetDC(m_WindowHandle);
            SwapBuffers(deviceContext);

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
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