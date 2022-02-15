#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include <dwmapi.h>
#include <cmath>
//#include <vector>
//#include <chrono>

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Our state
bool show_demo_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

HWND m_TargetWindow = nullptr;

const wchar_t* m_TargetWindowName = L"Calculator";
int m_ToggleMenuKey = VK_NUMPAD5;
int m_CloseOverlayKey = VK_NUMPAD2;
ImFont* m_Font = nullptr;

bool UpdateOverlayPosition(HWND target, HWND hwnd)
{
    ImVec4 m_Rect;

    bool isWindow = IsWindow(target);

    if (isWindow)
    {
        RECT rect;
        GetClientRect(target, &rect);

        m_Rect.z = rect.right - rect.left;
        m_Rect.w = rect.bottom - rect.top;

        POINT point = { 0, 0 };
        ClientToScreen(target, &point);

        m_Rect.x = point.x;
        m_Rect.y = point.y;
    }
    else
    {
        m_Rect.x = 0;
        m_Rect.y = 0;
        m_Rect.z = 1000;
        m_Rect.w = 1000;
    }

    MoveWindow(hwnd, m_Rect.x, m_Rect.y, m_Rect.z, m_Rect.w, true);

    return isWindow;
}

void DrawDemoWindow()
{
    static float f = 0.0f;
    static int counter = 0;

    ImGui::SetNextWindowSize(ImVec2(0, 0));

    ImGui::Begin("Hello, world!", &show_demo_window);       // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    static int currentFov = 120;
    static bool fovChanger = true;

    ImGui::Text("fov: %d", currentFov);
    if (ImGui::Checkbox("fovChanger", &fovChanger))
    {
        if (fovChanger)
        {
            currentFov = 120;
        }
        else
        {
            currentFov = 90;
        }
    }

    ImGui::End();
}

void DrawBackground()
{
    int w = g_d3dpp.BackBufferWidth;
    int h = g_d3dpp.BackBufferHeight;

    ImU32 redColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1, 0, 0, 1));
    ImU32 greenColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 1, 0, 1));
    ImU32 blueColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 1, 0.5f));

    ImDrawList* d = ImGui::GetBackgroundDrawList();

    int t = 2;

    d->AddLine(ImVec2(0, 0), ImVec2(w, h), redColor, t);
    d->AddLine(ImVec2(w, 0), ImVec2(0, h), redColor, t);

    ImVec2 center = ImVec2(w * 0.5f, h * 0.5f);

    d->AddRect(ImVec2(0, 0), ImVec2(100, 100), redColor, 0, 0, t);

    d->AddRectFilled(ImVec2(0, 100), ImVec2(100, 200), redColor);

    d->AddCircle(center, 100, greenColor, 0, t);

    d->AddCircleFilled(center, 75, blueColor);

    char* text = "Hello, World!";

    d->AddText(center, greenColor, text);

    int fontSize = 20;

    ImVec2 textSize = m_Font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text);
    ImVec2 pos = ImVec2(center.x - textSize.x * 0.5f, center.y - textSize.y * 0.5f);
    d->AddText(m_Font, fontSize, pos, greenColor, text);
}

void DrawCircleColoredOvertime()
{
    float time = ImGui::GetFrameCount() * 0.05;

    float r = 0.5 + 0.5 * cos(time + 0);
    float g = 0.5 + 0.5 * cos(time + 2);
    float b = 0.5 + 0.5 * cos(time + 4);

    ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, 1));

    auto draw = ImGui::GetBackgroundDrawList();
    draw->AddCircleFilled(ImVec2(200, 200), 200, color);
}

/*
struct Vector2Int
{
    int x, y;
};

Vector2Int grid;

Vector2Int player = { 0, 0 };

Vector2Int direction = { 1, 0 };

Vector2Int apple = { 5, 5 };

int cellSize = 10;
int frameSpeed = 10;

int tail = 1;

std::vector<Vector2Int> trail;

ImVec2 vMin;
ImVec2 vMax;

ImVec2 snakeWindowSize = ImVec2(300, 300);

ImVec2 GetCoord(int x, int y)
{
    auto a = ImVec2(x * cellSize, y * cellSize);
    a.x += vMin.x;
    a.y += vMin.y;
    return a;
}

void InitSnake()
{
    vMin = ImGui::GetWindowContentRegionMin();
    vMax = ImGui::GetWindowContentRegionMax();

    vMin.x += ImGui::GetWindowPos().x;
    vMin.y += ImGui::GetWindowPos().y;
    vMax.x += ImGui::GetWindowPos().x;
    vMax.y += ImGui::GetWindowPos().y;

    grid.x = (vMax.x - vMin.x) / cellSize;
    grid.y = (vMax.y - vMin.y) / cellSize;
}

void SnakeInput()
{
    if (GetAsyncKeyState(VK_UP) & 1)
    {
        direction.y = -1;
        direction.x = 0;
    }
    else if (GetAsyncKeyState(VK_DOWN) & 1)
    {
        direction.y = 1;
        direction.x = 0;
    }
    else if (GetAsyncKeyState(VK_RIGHT) & 1)
    {
        direction.x = 1;
        direction.y = 0;
    }
    else if (GetAsyncKeyState(VK_LEFT) & 1)
    {
        direction.x = -1;
        direction.y = 0;
    }

    if (GetAsyncKeyState(VK_SPACE) & 1)
    {
        tail += 1;
    }
}

void Frame()
{
    player.x += direction.x;
    player.y += direction.y;

    if (player.x < 0)
    {
        player.x = grid.x - 1;
    }
    else if (player.x > grid.x - 1)
    {
        player.x = 0;
    }

    if (player.y < 0)
    {
        player.y = grid.y - 1;
    }
    else if (player.y > grid.y - 1)
    {
        player.y = 0;
    }

    for (size_t i = 0; i < trail.size(); i++)
    {
        Vector2Int t = trail[i];

        if (player.x == t.x && player.y == t.y)
        {
            tail = 1;
        }
    }

    trail.push_back(player);

    while (trail.size() > tail)
    {
        trail.erase(trail.begin());
    }

    if (player.x == apple.x && player.y == apple.y)
    {
        tail++;

        apple.x = rand() % grid.x;
        apple.y = rand() % grid.y;
    }
}

void DrawGrid(ImDrawList* draw, ImU32 color)
{
    for (size_t y = 0; y < grid.y; y++)
    {
        for (size_t x = 0; x < grid.x; x++)
        {
            draw->AddRect(GetCoord(x, y), GetCoord(x + 1, y + 1), color);
        }
    }
}

void DrawSnake(ImDrawList* draw, ImU32 color)
{
    // draw tail
    for (size_t i = 0; i < trail.size(); i++)
    {
        Vector2Int t = trail[i];
        draw->AddRectFilled(GetCoord(t.x, t.y), GetCoord(t.x + 1, t.y + 1), color);
    }

    // draw head
    //draw->AddRectFilled(GetCoord(player.x, player.y), GetCoord(player.x + 1, player.y + 1), color);
}

void DrawSnakeGame()
{
    ImU32 white = ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1));

    ImU32 red = ImGui::ColorConvertFloat4ToU32(ImVec4(1, 0, 0, 1));
    ImU32 green = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 1, 0, 1));
    ImU32 blue = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 1, 1));

    ImU32 yellow = ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 0, 1));

    ImGui::SetNextWindowSize(snakeWindowSize);

    ImGui::Begin("snake");

    InitSnake();

    SnakeInput();

    if (ImGui::GetFrameCount() % frameSpeed == 0)
    {
        Frame();
    }

    auto draw = ImGui::GetForegroundDrawList();

    //draw->AddRect(vMin, vMax, red);

    //DrawGrid(draw, white);

    DrawSnake(draw, green);

    // draw apple
    draw->AddRectFilled(GetCoord(apple.x, apple.y), GetCoord(apple.x + 1, apple.y + 1), yellow);

    ImGui::End();
}

*/

// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);

    DWORD exStyle = 0;
    exStyle |= WS_EX_TOPMOST;
    exStyle |= WS_EX_TRANSPARENT;
    exStyle |= WS_EX_LAYERED;
    exStyle |= WS_EX_NOACTIVATE;

    HWND hwnd = ::CreateWindowEx(exStyle , wc.lpszClassName, _T("Dear ImGui DirectX9 Example"), WS_POPUP, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margin);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOW);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    //(void)io;
    io.IniFilename = nullptr;

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    m_Font = io.Fonts->AddFontDefault();
    IM_ASSERT(m_Font != NULL);

    // Main loop
    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                done = true;
            }
        }

        if (done)
        {
            break;
        }

        HWND target = FindWindow(0, m_TargetWindowName);

        // detect when target window changes - becames closed/opened (used to draw in future)
        if (m_TargetWindow != target)
        {
            m_TargetWindow = target;
        }

        UpdateOverlayPosition(target, hwnd);

        // toggle menu visibility
        if (GetAsyncKeyState(m_ToggleMenuKey) & 1)
        {
            show_demo_window = !show_demo_window;

            long style = GetWindowLong(hwnd, GWL_EXSTYLE);

            if (show_demo_window)
            {
                // remove layered style
                style &= ~WS_EX_LAYERED;
                SetWindowLong(hwnd, GWL_EXSTYLE, style);

                // activate window
                //SetForegroundWindow(target);
                //SetActiveWindow(target);
            }
            else
            {
                // add layered style (mouse click will passthrough)
                style |= WS_EX_LAYERED;
                SetWindowLong(hwnd, GWL_EXSTYLE, style);
            }
        }

        if (GetAsyncKeyState(m_CloseOverlayKey) & 1)
        {
            break;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
        {
            DrawDemoWindow();
        }

        DrawBackground();
        
        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
        if (SUCCEEDED(g_pd3dDevice->BeginScene()))
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        {
            ResetDevice();
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions
bool CreateDeviceD3D(HWND hWnd)
{
    g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (g_pD3D == NULL)
    {
        return false;
    }

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8; // manually changed to this type
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate

    HRESULT hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice);

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = NULL;
    }

    if (g_pD3D)
    {
        g_pD3D->Release();
        g_pD3D = NULL;
    }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
