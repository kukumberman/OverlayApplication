#pragma once

#include <iostream>
#include <vector>
#include <functional>
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

    int GetWidth()
    {
        return m_Rect.width;
    }

    int GetHeight()
    {
        return m_Rect.height;
    }

    bool IsTargetValid()
    {
        return m_IsValid;
    }

    void AddCallback(std::function<void(OverlayApp*)> entry)
    {
        m_Actions.push_back(entry);
    }

    void Render()
    {
        // AddCallback(std::bind(&Foobar, this));

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
            m_Rect.x = 0;
            m_Rect.y = 0;
            m_Rect.width = 500;
            m_Rect.height = 500;
        }

        COLORREF transparentColor = RGB(0, 0, 0);

        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(wcex);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = Overlay::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = NULL;
        wcex.hIcon = 0;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = CreateSolidBrush(transparentColor);
        wcex.lpszMenuName = m_WindowTitle;
        wcex.lpszClassName = m_WindowTitle;
        wcex.hIconSm = 0;

        RegisterClassEx(&wcex);

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

        // todo: settopmost

        m_DeviceContext = DeviceContext(m_Window, m_Rect.width, m_Rect.height);
        m_DeviceContext.Create();

        m_DrawingContext = DrawingContext(&m_DeviceContext);
	}

    bool GetTargetPosition(Overlay::Rect& out)
    {
        m_TargetFindow = FindWindow(0, m_TargetName);

        if (!IsWindow(m_TargetFindow))
        {
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

    void Run()
    {
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            m_IsValid = GetTargetPosition(m_Rect);

            if (m_IsValid)
            {
                MoveWindow(m_Window, m_Rect.x, m_Rect.y, m_Rect.width, m_Rect.height, true);
            }
            else
            {
                // todo: expand to fullscreen

                MoveWindow(m_Window, 50, 50, 100, 100, true);
            }
        }
    }
};
