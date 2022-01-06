#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <thread>
#include <dwmapi.h>
#include "DeviceContext.h"
#include "DrawingContext.h"

#pragma comment(lib, "dwmapi.lib")

namespace Overlay
{
    struct Rect
    {
        int x, y, width, height;
    };

    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
}

class OverlayApp
{
	const wchar_t* m_TargetName = nullptr;
    const wchar_t* m_WindowTitle = L"Overlay by Cucumber";

	DeviceContext m_DeviceContext;
    DrawingContext m_DrawingContext;
    HWND m_Window = 0;
    HWND m_TargetFindow = 0;

    Overlay::Rect m_Rect;
    bool m_IsValid = false;

    bool m_IsCreated = false;

    int m_FrameCount = 0;
    float m_DeltaTime = 0;

    std::chrono::high_resolution_clock m_Timer;
    std::chrono::steady_clock::time_point m_LastTime;
    std::chrono::steady_clock::time_point m_Now;

    std::vector<std::function<void(OverlayApp*)>> m_Actions;

public:
    OverlayApp(const wchar_t* targetName)
	{
		m_TargetName = targetName;
	}

    DrawingContext* Draw()
    {
        return &m_DrawingContext;
    }

    int Width()
    {
        return m_Rect.width;
    }

    int Height()
    {
        return m_Rect.height;
    }

    int FrameCount()
    {
        return m_FrameCount;
    }

    float DeltaTime()
    {
        return m_DeltaTime;
    }

    bool IsTargetValid()
    {
        return m_IsValid;
    }

    void AddCallback(std::function<void(OverlayApp*)> entry)
    {
        m_Actions.push_back(entry);
    }

    void OnResize()
    {
        if (m_IsCreated)
        {
            m_DeviceContext.OnResize(m_Rect.width, m_Rect.height);
        }
    }

    void Render()
    {
        //if (m_TargetFindow != GetForegroundWindow()) return;

        m_FrameCount += 1;

        CalculateDelta();

        // example usage
        //AddCallback(std::bind(&OnResize, this));
        //AddCallback(OnResize);

        if (m_DeviceContext.BeginRender())
        {
            // loop over draw functions
            for (auto& action : m_Actions)
            {
                action(this);
            }

            m_DeviceContext.EndRender();
        }
    }

	void Create()
	{
        if (GetTargetPosition(m_Rect))
        {
            // start values cached
        }
        else
        {
            ExpandRect();
        }

        COLORREF transparentColor = RGB(0, 0, 0);

        RegisterWindowClass(Overlay::WndProc);

        DWORD exStyle = 0;
        exStyle |= WS_EX_TOPMOST;
        exStyle |= WS_EX_TRANSPARENT;
        exStyle |= WS_EX_LAYERED;

        m_Window = CreateWindowEx(exStyle, m_WindowTitle, m_WindowTitle, WS_POPUP, m_Rect.x, m_Rect.y, m_Rect.width, m_Rect.height, nullptr, nullptr, NULL, (LPVOID)this);
        
        //SetLayeredWindowAttributes(hWnd, transparentColor, 0, LWA_COLORKEY);
        SetLayeredWindowAttributes(m_Window, transparentColor, 255, LWA_ALPHA);

        MARGINS margin = { -1 };
        DwmExtendFrameIntoClientArea(m_Window, &margin);

        ShowWindow(m_Window, SW_SHOW);

        // todo: target settopmost

        CreateContext();
	}

    void Run()
    {
        MSG msg;
        
        using namespace std::chrono_literals;
        auto delay = 33ms;

        while (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            UpdatePosition();

            if (msg.message == WM_PAINT)
            {
                this->Render();
                std::this_thread::sleep_for(delay);
            }
        }
    }

    void Close()
    {
        PostMessage(m_Window, WM_CLOSE, 0, 0);
    }

private:

    void CreateContext()
    {
        m_DeviceContext = DeviceContext(m_Window, m_Rect.width, m_Rect.height);
        m_DeviceContext.Create();

        m_DrawingContext = DrawingContext(&m_DeviceContext);

        m_IsCreated = true;
    }

    void RegisterWindowClass(WNDPROC wndProc)
    {
        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(wcex);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = wndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = NULL;
        wcex.hIcon = 0;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        //wcex.hbrBackground = CreateSolidBrush(transparentColor);
        wcex.hbrBackground = 0;
        wcex.lpszMenuName = m_WindowTitle;
        wcex.lpszClassName = m_WindowTitle;
        wcex.hIconSm = 0;

        RegisterClassEx(&wcex);
    }

    void CalculateDelta()
    {
        using s = std::chrono::duration<float>;

        m_LastTime = m_Now;
        m_Now = m_Timer.now();

        m_DeltaTime = std::chrono::duration_cast<s>(m_Now - m_LastTime).count();
    }

    bool GetTargetPosition(Overlay::Rect& out)
    {
        m_TargetFindow = FindWindow(0, m_TargetName);

        if (!IsWindow(m_TargetFindow))
        {
            ExpandRect();
            return false;
        }

        RECT rect;
        GetClientRect(m_TargetFindow, &rect);

        out.width = rect.right - rect.left;
        out.height = rect.bottom - rect.top;

        POINT point = { 0, 0 };
        ClientToScreen(m_TargetFindow, &point);

        out.x = point.x;
        out.y = point.y;

        return true;
    }

    void ExpandRect()
    {
        m_Rect.x = 0;
        m_Rect.y = 0;
        m_Rect.width = GetSystemMetrics(SM_CXSCREEN) - 100;
        m_Rect.height = GetSystemMetrics(SM_CYSCREEN) - 100;
    }

    void UpdatePosition()
    {
        m_IsValid = GetTargetPosition(m_Rect);

        MoveWindow(m_Window, m_Rect.x, m_Rect.y, m_Rect.width, m_Rect.height, true);
    }
};
