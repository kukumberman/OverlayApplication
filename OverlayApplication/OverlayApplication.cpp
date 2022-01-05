//#include "framework.h"
//#include "resource.h"
#include <iostream>
#include <d3d9.h>
#include <d3dx9.h>
#include <dwmapi.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

#include "DrawingContext.h"
#include "Overlay.h"

int width = 800;
int height = 600;
int frameCount = 0;
int messageCount = 0;

IDirect3D9Ex* d3dObject;
IDirect3DDevice9Ex* d3dDevice;
D3DPRESENT_PARAMETERS d3dparams;
ID3DXFont* d3dFont;

COLORREF transparentColor = RGB(0, 0, 0);

LPCWSTR m_TargetWindowName = L"Calculator";
WCHAR szTitle[] = L"Overlay";
HWND hWnd;
HWND gameWindow;

void CreateDevice()
{
    if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &d3dObject)))
    {
        exit(1);
    }

    ZeroMemory(&d3dparams, sizeof(d3dparams));

    d3dparams.BackBufferWidth = width;
    d3dparams.BackBufferHeight = height;
    d3dparams.Windowed = TRUE;
    d3dparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dparams.hDeviceWindow = hWnd;
    d3dparams.MultiSampleQuality = D3DMULTISAMPLE_NONE;
    d3dparams.BackBufferFormat = D3DFMT_A8R8G8B8;
    d3dparams.EnableAutoDepthStencil = TRUE;
    d3dparams.AutoDepthStencilFormat = D3DFMT_D16;
    d3dparams.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    HRESULT res = d3dObject->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dparams, nullptr, &d3dDevice);

    if (FAILED(res))
    {
        exit(1);
    }

    //D3DXCreateFont(d3dDevice, 25, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Comic Sans", &d3dFont);
    D3DXCreateFont(d3dDevice, 50, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Comic Sans", &d3dFont);
}

void DrawLabelOutline(const wchar_t* text, int x, int y)
{
    int offset = 1;

    D3DCOLOR color = D3DCOLOR_RGBA(1, 1, 1, 255);

    RECT top = { x, y + offset, 0, 0 };
    RECT bottom = { x, y - offset, 0, 0 };
    RECT left = { x - offset, y, 0, 0 };
    RECT right = { x + offset, y, 0, 0 };

    d3dFont->DrawTextW(nullptr, text, wcslen(text), &top, DT_NOCLIP, color);
    d3dFont->DrawTextW(nullptr, text, wcslen(text), &bottom, DT_NOCLIP, color);
    d3dFont->DrawTextW(nullptr, text, wcslen(text), &left, DT_NOCLIP, color);
    d3dFont->DrawTextW(nullptr, text, wcslen(text), &right, DT_NOCLIP, color);
}

void DrawLabel(const wchar_t* text, int x, int y, D3DCOLOR color)
{
    RECT rect;
    d3dFont->DrawTextW(nullptr, text, wcslen(text), &rect, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));

    rect.left = x - (rect.right - rect.left) * 0.5f;
    rect.top = y - (rect.bottom - rect.top) * 0.5f;

    //DrawLabelOutline(text, rect.left, rect.top);

    d3dFont->DrawTextW(nullptr, text, wcslen(text), &rect, DT_NOCLIP, color);
}

void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color, bool antialias)
{
    ID3DXLine* line;
    D3DXCreateLine(d3dDevice, &line);

    D3DXVECTOR2 vertex[2];
    vertex[0] = D3DXVECTOR2(x1, y1);
    vertex[1] = D3DXVECTOR2(x2, y2);

    line->SetWidth(thickness);
    line->SetAntialias(antialias);
    line->Begin();
    line->Draw(vertex, 2, color);
    line->End();
    line->Release();
}

void DrawRect(int x, int y, int width, int height, int thickness, D3DCOLOR color, bool antialias)
{
    ID3DXLine* line;
    D3DXCreateLine(d3dDevice, &line);

    D3DXVECTOR2 rect[5];
    rect[0] = D3DXVECTOR2(x, y);
    rect[1] = D3DXVECTOR2(x + width, y);
    rect[2] = D3DXVECTOR2(x + width, y + height);
    rect[3] = D3DXVECTOR2(x, y + height);
    rect[4] = D3DXVECTOR2(x, y);

    line->SetWidth(thickness);
    line->SetAntialias(antialias);
    line->Begin();
    line->Draw(rect, 5, color);
    line->End();
    line->Release();
}

void DrawCircle(int x, int y, int radius, int thickness, D3DCOLOR color, bool antialias)
{
    ID3DXLine* line;
    D3DXCreateLine(d3dDevice, &line);

    const int count = 30;
    float step = 360.f / count;
    const int vertexCount = count + 1;

    D3DXVECTOR2 circle[vertexCount];

    for (int i = 0; i < vertexCount; i++)
    {
        float angle = i * step;
        float rad = angle * D3DX_PI / 180;
        float _x = x + sin(rad) * radius;
        float _y = y + cos(rad) * radius;

        circle[i] = D3DXVECTOR2(_x, _y);
    }

    line->SetWidth(thickness);
    line->SetAntialias(antialias);
    line->Begin();
    line->Draw(circle, vertexCount, color);
    line->End();
    line->Release();
}

void RenderOverlay()
{
    wchar_t buffer[100];
    
    /*RECT rect;
    GetWindowRect(gameWindow, &rect);

    swprintf_s(buffer, L"(%d, %d, %d, %d)", rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top);
    DrawLabel(buffer, 50, 100, D3DCOLOR_XRGB(0, 0, 255));

    GetClientRect(gameWindow, &rect);
    swprintf_s(buffer, L"(%d, %d, %d, %d)", rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top);
    DrawLabel(buffer, 50, 200, D3DCOLOR_XRGB(0, 0, 255));*/

    DrawLine(0, 0, width, height, 2, D3DCOLOR_XRGB(255, 0, 0), false);
    DrawLine(width, 0, 0, height, 2, D3DCOLOR_XRGB(255, 0, 0), true);
   
    DrawRect(50, 50, 100, 100, 2, D3DCOLOR_XRGB(0, 0, 255), false);
    DrawRect(150, 150, 100, 100, 2, D3DCOLOR_XRGB(0, 0, 255), true);

    DrawLabel(L"Centered", width * 0.5f, height * 0.5f, D3DCOLOR_RGBA(255, 0, 0, 255));
    DrawLabel(L"Centered", width * 0.5f, height * 0.75f, D3DCOLOR_RGBA(255, 0, 0, 128));

    wsprintf(buffer, L"%d %d", frameCount++, messageCount);
    DrawLabel(buffer, width * 0.5f, height * 0.25f, D3DCOLOR_XRGB(0, 0, 0));

    DrawCircle(width * 0.5f, height * 0.5f, 100, 2, D3DCOLOR_XRGB(255, 255, 255), true);
    DrawCircle(width * 0.5f, height * 0.5f, 50, 2, D3DCOLOR_XRGB(1, 1, 1), false);
    DrawCircle(width * 0.5f, height * 0.5f, 60, 2, D3DCOLOR_XRGB(0, 0, 0), true);
}

int Render()
{
    if (d3dDevice == nullptr) return 1;

    d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    d3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

    d3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1, 0);
    d3dDevice->BeginScene();

    if (gameWindow == GetForegroundWindow())
    {
        RenderOverlay();
    }

    d3dDevice->EndScene();
    d3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        Render();
        //PAINTSTRUCT ps;
        //HDC hdc = BeginPaint(hWnd, &ps);
        //// TODO: Добавьте сюда любой код прорисовки, использующий HDC...
        //EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void CreateOverlay(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(transparentColor);
    wcex.lpszMenuName = szTitle;
    wcex.lpszClassName = szTitle;
    wcex.hIconSm = 0;

    RegisterClassEx(&wcex);

    DWORD exStyle = 0;
    exStyle |= WS_EX_TOPMOST;
    exStyle |= WS_EX_TRANSPARENT;
    exStyle |= WS_EX_LAYERED;

    hWnd = CreateWindowEx(exStyle, szTitle, szTitle, WS_POPUP, 0, 0, width, height, nullptr, nullptr, hInstance, nullptr);

    //SetLayeredWindowAttributes(hWnd, transparentColor, 0, LWA_COLORKEY);
    SetLayeredWindowAttributes(hWnd, transparentColor, 255, LWA_ALPHA);

    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(hWnd, &margin);

    ShowWindow(hWnd, nCmdShow);

    // todo: settopmost
}

void UpdateWindow()
{
    // coords mismatch
    /*RECT wRect;
    GetWindowRect(gameWindow, &wRect);

    width = wRect.right - wRect.left;
    height = wRect.bottom - wRect.top;

    MoveWindow(hWnd, wRect.left, wRect.top, width, height, true);*/

    RECT rect;
    GetClientRect(gameWindow, &rect);

    width = rect.right - rect.left;
    height = rect.bottom - rect.top;

    POINT point = { 0, 0 };
    ClientToScreen(gameWindow, &point);

    MoveWindow(hWnd, point.x, point.y, width, height, true);
}

void Foo(OverlayApp* overlay)
{
    if (overlay->IsTargetValid())
    {
        overlay->Draw()->Circle(100, 100, 50);

        overlay->Draw()->Line(0, 0, overlay->GetWidth(), overlay->GetHeight());
        overlay->Draw()->Line(overlay->GetWidth(), 0, 0, overlay->GetHeight());
    }
    else
    {
        overlay->Draw()->Circle(overlay->GetWidth() * 0.5f, overlay->GetHeight() * 0.5f, 100);
    }
}

void DankOverlay()
{
    OverlayApp overlay(L"Calculator");

    overlay.Create();

    overlay.AddCallback(Foo);
    overlay.Run();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    DankOverlay();

    // end of the refactored code!!!

    return 0;

    gameWindow = FindWindow(0, m_TargetWindowName);

    while (gameWindow == NULL)
    {
        gameWindow = FindWindow(0, m_TargetWindowName);
        std::cout << "No window found!" << std::endl;
        Sleep(100);
    }

    std::cout << "Running overlay" << std::endl;

    UpdateWindow();

    CreateOverlay(hInstance, nCmdShow);

    CreateDevice();

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        UpdateWindow();
    }

    fclose(f);
    FreeConsole();

    return (int) msg.wParam;
}
