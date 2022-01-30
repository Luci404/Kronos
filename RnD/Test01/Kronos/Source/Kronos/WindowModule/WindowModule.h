#pragma once

#include "Kronos/Core/Module.h"
#include "Kronos/Core/Assert.h"
#include "Kronos/LoggingModule/LoggingModule.h"

#include <windows.h>
#include <d2d1.h>
#include <stdio.h>

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
        Log::Trace("Close");
        DestroyWindow(hwnd);
        break;
    }
    case WM_DESTROY:
    {
        Log::Trace("Destroy");
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

#define KRONOS_WNDCLASSNAME L"SampleWindowClass"

namespace Kronos
{
    class Window
    {
    public:
        Window(const std::string& title = "untitled", uint32_t width = 1600, uint32_t height = 900, uint32_t xPosition = 0, uint32_t yPosition = 0, bool maximized = false, bool vSync = true)
        {
            Log::Trace("Creating window...");

            WNDCLASSEXW wc = {};
            wc.cbSize = sizeof(wc);
            wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
            wc.lpfnWndProc = (WNDPROC)WndProc;
            wc.hInstance = GetModuleHandleW(NULL);
            wc.hCursor = LoadCursor(NULL, IDC_ARROW);
            wc.lpszClassName = KRONOS_WNDCLASSNAME;
            KRONOS_CORE_ASSERT(RegisterClassExW(&wc), "Failed to register class.");

            DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION | WS_MAXIMIZEBOX | WS_THICKFRAME;
            DWORD exStyle = WS_EX_APPWINDOW;
            HWND hwnd = CreateWindowExW(exStyle, KRONOS_WNDCLASSNAME, L"untitled", style, 160, 90, 1600, 900, NULL, NULL, GetModuleHandleW(NULL), NULL);
            KRONOS_CORE_ASSERT(hwnd != NULL, "Failed to create window.");

            ShowWindow(hwnd, SW_SHOWNORMAL);
            UpdateWindow(hwnd);

            MSG Msg;
            while (GetMessage(&Msg, NULL, 0, 0) > 0)
            {
                TranslateMessage(&Msg);
                DispatchMessage(&Msg);
            }
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
    };

    class WindowModule : public StaticModule
    {
    };
}
