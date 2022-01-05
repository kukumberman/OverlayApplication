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

	void Text(LPCWSTR text, int x, int y)
	{
		DeviceContext::DrawLabelW(m_Font, text, x, y, m_Color);
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
