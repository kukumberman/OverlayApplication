#pragma once
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

class DeviceContext
{
	IDirect3D9Ex* m_D3DObject = nullptr;
	IDirect3DDevice9Ex* m_D3DDevice = nullptr;
	D3DPRESENT_PARAMETERS m_D3DParams;

	HWND m_DeviceWindow = NULL;
	int m_Width = 0;
	int m_Height = 0;

public:
	DeviceContext()
	{

	}

	DeviceContext(HWND deviceWindow, int width, int height)
	{
		m_DeviceWindow = deviceWindow;
		m_Width = width;
		m_Height = height;
	}

	bool Create()
	{
		if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &m_D3DObject)))
		{
			exit(1);
		}

		ZeroMemory(&m_D3DParams, sizeof(m_D3DParams));

		m_D3DParams.BackBufferWidth = m_Width;
		m_D3DParams.BackBufferHeight = m_Height;
		m_D3DParams.Windowed = TRUE;
		m_D3DParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_D3DParams.hDeviceWindow = m_DeviceWindow;
		m_D3DParams.MultiSampleQuality = D3DMULTISAMPLE_NONE;
		m_D3DParams.BackBufferFormat = D3DFMT_A8R8G8B8;
		m_D3DParams.EnableAutoDepthStencil = TRUE;
		m_D3DParams.AutoDepthStencilFormat = D3DFMT_D16;
		m_D3DParams.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

		HRESULT res = m_D3DObject->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_DeviceWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_D3DParams, nullptr, &m_D3DDevice);

		return SUCCEEDED(res);
	}

	void Destroy()
	{
		if (m_D3DDevice)
		{
			m_D3DDevice->Release();
			m_D3DDevice = nullptr;
		}

		if (m_D3DObject)
		{
			m_D3DObject->Release();
			m_D3DObject = nullptr;
		}
	}

	void OnResize(int width, int height)
	{
		// https://www.unknowncheats.me/forum/c-/128592-directx-scaling-issues.html
		//todo
		m_D3DParams.BackBufferWidth = width;
		m_D3DParams.BackBufferHeight = height;
	}

	bool BeginRender()
	{
		if (m_D3DDevice == nullptr)
		{
			return false;
		}

		m_D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_D3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_D3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_D3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		m_D3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		m_D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_D3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		m_D3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

		m_D3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1, 0);
		m_D3DDevice->BeginScene();

		return true;
	}

	void EndRender()
	{
		m_D3DDevice->EndScene();
		m_D3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
	}

	ID3DXFont* MakeFont(LPCWSTR fontFamily, int height)
	{
		ID3DXFont* d3dFont;
		D3DXCreateFont(m_D3DDevice, height, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontFamily, &d3dFont);
		return d3dFont;
	}

	void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color, bool antialias)
	{
		DrawLine(m_D3DDevice, x1, y1, x2, y2, thickness, color, antialias);
	}

	void DrawRect(int x, int y, int width, int height, int thickness, D3DCOLOR color, bool antialias)
	{
		DrawRect(m_D3DDevice, x, y, width, height, thickness, color, antialias);
	}

	void DrawCircle(int x, int y, int radius, int thickness, D3DCOLOR color, bool antialias)
	{
		DrawCircle(m_D3DDevice, x, y, radius, thickness, color, antialias);
	}

	static void DrawLabelW(ID3DXFont* d3dFont, const wchar_t* text, int x, int y, D3DCOLOR color)
	{
		RECT rect;
		d3dFont->DrawTextW(nullptr, text, wcslen(text), &rect, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));

		rect.left = x - (rect.right - rect.left) * 0.5f;
		rect.top = y - (rect.bottom - rect.top) * 0.5f;

		d3dFont->DrawTextW(nullptr, text, wcslen(text), &rect, DT_NOCLIP, color);
	}

	static void DrawLabelA(ID3DXFont* d3dFont, const char* text, int x, int y, D3DCOLOR color)
	{
		RECT rect;
		d3dFont->DrawTextA(nullptr, text, strlen(text), &rect, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));

		rect.left = x - (rect.right - rect.left) * 0.5f;
		rect.top = y - (rect.bottom - rect.top) * 0.5f;

		d3dFont->DrawTextA(nullptr, text, strlen(text), &rect, DT_NOCLIP, color);
	}

	static void DrawLine(IDirect3DDevice9Ex* d3dDevice, int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color, bool antialias)
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

	static void DrawRect(IDirect3DDevice9Ex* d3dDevice, int x, int y, int width, int height, int thickness, D3DCOLOR color, bool antialias)
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

	static void DrawCircle(IDirect3DDevice9Ex* d3dDevice, int x, int y, int radius, int thickness, D3DCOLOR color, bool antialias)
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
};
