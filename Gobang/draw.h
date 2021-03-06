#pragma once

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

// 像素转为DIPs（设备无关像素）
	template <typename T>
	static D2D1_POINT_2F PixelsToDips(T x, T y)
	{
		return D2D1::Point2F(static_cast<float>(x) / scaleX, static_cast<float>(y) / scaleY);
	}
};

// 计算布局
void CalculateLayout();

// 两点距离是否小于r
bool cmpDis(const D2D1_POINT_2F a, const D2D1_POINT_2F b, float r);

// 创建Direct2D资源
HRESULT CreateGraphicsResources();

// 放弃Direct2D资源
void DiscardGraphicsResources();

// 转换行列坐标至DIPs（设备无关像素）
D2D1_POINT_2F RcToDips(int x, int y);

// 左键按下
void OnLButtonDown(int pixelX, int pixelY, DWORD flags);

// 左键松开
void OnLButtonUp(int pixelX, int pixelY, DWORD flags);

// 绘制窗口
void OnPaint();

// 绘制棋盘
void PaintBoard();

// 绘制棋子
void PaintPieces();

// 调整窗口大小
void Resize();
