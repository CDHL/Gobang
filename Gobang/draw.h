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

void CalculateLayout();

HRESULT CreateGraphicsResources();

void DiscardGraphicsResources();

D2D1_POINT_2F rcToDips(int x, int y);

void OnPaint();

void PaintBoard();

void PaintPieces();

void Resize();
