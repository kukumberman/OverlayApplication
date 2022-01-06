#include "Overlay.h"

LRESULT CALLBACK Overlay::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    OverlayApp* overlay = nullptr;

    if (IsWindow(hWnd))
    {
        overlay = (OverlayApp*)GetProp(hWnd, L"overlay");
    }

    //std::cout << "message: " << message << std::endl;

    switch (message)
    {
    case WM_CREATE:
    {
        // http://rsdn.org/forum/winapi/2679609.hot
        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
        overlay = (OverlayApp*)(lpcs->lpCreateParams);
        SetProp(hWnd, L"overlay", (HANDLE)overlay);

        /*pThis->m_hWnd = hWnd;
        pThis->OnCreate();*/
    }
    break;
    case WM_SIZE:
    {
        std::cout << "Size changed" << std::endl;
        if (overlay != nullptr)
        {
            std::cout << "Should resize spawnchain" << std::endl;
            overlay->OnResize();
        }
    }
    break;
    case WM_PAINT:
    {
        //std::cout << "Paint" << std::endl;
        //if (overlay != nullptr)
        //{
        //    //std::cout << "overlay is ok" << std::endl;
        //    overlay->Render();
        //}
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);;
    }

    return 0;
}