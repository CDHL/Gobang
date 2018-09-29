#pragma once

#include <Windows.h>
#include <d2d1.h>

extern ID2D1Factory				*g_pFactory;
extern ID2D1HwndRenderTarget	*g_pRenderTarget;
extern ID2D1SolidColorBrush		*g_pBrush;

template<class T> inline void SafeRelease(T *&ppT)
{
	if (ppT)
	{
		ppT->Release();
		ppT = NULL;
	}
}

class DPIScale
{
	static float scaleX;
	static float scaleY;

public:
	static void Initialize(ID2D1Factory *pFactory)
	{
		FLOAT dpiX, dpiY;
		pFactory->GetDesktopDpi(&dpiX, &dpiY);
		scaleX = dpiX / 96.0f;
		scaleY = dpiY / 96.0f;
	}

	template <typename T>
	static D2D1_POINT_2F PixelsToDips(T x, T y)
	{
		return D2D1::Point2F(static_cast<float>(x) / scaleX, static_cast<float>(y) / scaleY);
	}
};

void CalculateLayout();

bool cmpDis(const D2D1_POINT_2F a, const D2D1_POINT_2F b, float r);

HRESULT CreateGraphicsResources();

void DiscardGraphicsResources();

D2D1_POINT_2F RcToDips(int x, int y);

void OnLButtonDown(int pixelX, int pixelY, DWORD flags);

void OnLButtonUp(int pixelX, int pixelY, DWORD flags);

void OnPaint();

void PaintBoard();

void PaintPieces();

void Resize();
