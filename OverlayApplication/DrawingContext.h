#pragma once
#include "DeviceContext.h"

class DrawingContext
{
	DeviceContext* m_DeviceContext;
	ID3DXFont* m_Font;
	D3DCOLOR m_Color = D3DCOLOR_XRGB(255, 255, 255);
	int m_Thickness = 2;
	bool m_Antialias = true;

public:
	DrawingContext(DeviceContext* ctx)
	{
		m_DeviceContext = ctx;
		m_Font = m_DeviceContext->MakeFont(L"Arial", 50);
	}

	DrawingContext()
	{

	}

	void SetColor(D3DCOLOR color)
	{
		m_Color = color;
	}

	void Text(const wchar_t* text, int x, int y)
	{
		DeviceContext::DrawLabelW(m_Font, text, x, y, m_Color);
	}

	void Text(int x, int y, const wchar_t* format, ...)
	{
		wchar_t buffer[100];

		va_list args;
		va_start(args, format);
		vswprintf_s(buffer, 100, format, args);
		va_end(args);

		Text(buffer, x, y);
	}

	void Line(int x1, int y1, int x2, int y2)
	{
		m_DeviceContext->DrawLine(x1, y1, x2, y2, m_Thickness, m_Color, m_Antialias);
	}

	void Rect(int x, int y, int width, int height)
	{
		m_DeviceContext->DrawRect(x, y, width, height, m_Thickness, m_Color, m_Antialias);
	}

	void Circle(int x, int y, int radius)
	{
		m_DeviceContext->DrawCircle(x, y, radius, m_Thickness, m_Color, m_Antialias);
	}
};
