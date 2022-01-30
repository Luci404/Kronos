#pragma once

#include "Kronos/Core/Module.h"
#include "Kronos/Core/Assert.h"
#include "Kronos/LoggingModule/LoggingModule.h"

#include <windows.h>
#include <d2d1.h>
#include <stdio.h>

#define KRONOS_WNDCLASSNAME L"SampleWindowClass"

namespace Kronos
{
    void PaintWindow(HWND hwnd)
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        Rectangle(hdc, 5, 5, 100, 100);
        EndPaint(hwnd, &ps);
    }

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_PAINT:
        {
            PaintWindow(hwnd);
            break;
        }
        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        default:
        {
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
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