#include "stdafx.h"

#include "draw.h"

#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "game.h"

float DPIScale::scaleX = 1.0f;
float DPIScale::scaleY = 1.0f;

ID2D1Factory			*g_pFactory;
ID2D1HwndRenderTarget	*g_pRenderTarget;
ID2D1SolidColorBrush	*g_pBrush;
//ID2D1StrokeStyle		*g_pStrokeStyle;

// 棋盘距边缘距离/min(窗口长, 窗口宽)
const float BORDER_GAP = 0.1f;
// 点击判定半径/单格边长（稍大于棋子半径）
const float CLICK_RANGE = 0.5f;
// 棋子半径/单格边长
const float PIECE_SIZE = 0.4f;

// 棋盘边缘坐标（DIP）
float g_boardLeft, g_boardTop, g_boardRight, g_boardBottom;
// 单格边长（DIP）
float g_gridGap;
// 左键按下时的行列坐标
int g_lastPointX, g_lastPointY;

extern HWND hwnd;

// Recalculate drawing layout when the size of the window changes.
void CalculateLayout()
{
	if (g_pRenderTarget != NULL)
	{
		D2D1_SIZE_F size = g_pRenderTarget->GetSize();
		const float x = size.width / 2;
		const float y = size.height / 2;
		const float halfSideLength = (0.5f - BORDER_GAP) * 2 * min(x, y);

		g_boardLeft = x - halfSideLength;
		g_boardTop = y - halfSideLength;
		g_boardRight = x + halfSideLength;
		g_boardBottom = y + halfSideLength;

		g_gridGap = halfSideLength * 2 / (Board::BOARD_SIZE - 1);
	}
}

bool cmpDis(const D2D1_POINT_2F a, const D2D1_POINT_2F b, float r)
{
	float x = a.x - b.x;
	float y = a.y - b.y;
	return x * x + y * y < r * r;
}

HRESULT CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (g_pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(hwnd, &rc);

		hr = g_pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				hwnd,
				// rc.left, rc.top 都等于0
				D2D1::SizeU(
					rc.right,
					rc.bottom
				)
			),
			&g_pRenderTarget
		);
		
		if (SUCCEEDED(hr))
		{
			//float dashes[] = { 1.0f, 2.0f, 2.0f, 3.0f, 2.0f, 2.0f };
			//hr = g_pFactory->CreateStrokeStyle(
			//	D2D1::StrokeStyleProperties(
			//		D2D1_CAP_STYLE_ROUND,
			//		D2D1_CAP_STYLE_ROUND,
			//		D2D1_CAP_STYLE_ROUND,
			//		D2D1_LINE_JOIN_MITER,
			//		10.f,
			//	),
			//	NULL,
			//	0,
			//	&g_pStrokeStyle
			//);

			hr = g_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &g_pBrush);

			if (SUCCEEDED(hr))
			{
				CalculateLayout();
			}
		}
	}
	return hr;
}

// 将DIPs转为行列坐标
void DipsToRc(D2D1_POINT_2F curPoint, int &x, int &y)
{
	x = int((curPoint.x - g_boardLeft) / g_gridGap + 0.5f);
	y = int((curPoint.y - g_boardTop) / g_gridGap + 0.5f);
}

void DiscardGraphicsResources()
{
	SafeRelease(g_pRenderTarget);
	SafeRelease(g_pBrush);
	//SafeRelease(g_pStrokeStyle);
}

D2D1_POINT_2F RcToDips(int x, int y)
{
	return D2D1::Point2F(g_boardLeft + g_gridGap * x, g_boardTop + g_gridGap * y);
}

void OnLButtonDown(int pixelX, int pixelY, DWORD flags)
{
	D2D1_POINT_2F lastPoint = DPIScale::PixelsToDips(pixelX, pixelY);
	DipsToRc(lastPoint, g_lastPointX, g_lastPointY);
	if (!cmpDis(RcToDips(g_lastPointX, g_lastPointY), lastPoint, g_gridGap * CLICK_RANGE))
	{
		g_lastPointX = g_lastPointY = -1;
	}
}

void OnLButtonUp(int pixelX, int pixelY, DWORD flags)
{
	D2D1_POINT_2F curPoint = DPIScale::PixelsToDips(pixelX, pixelY);
	int x, y;
	DipsToRc(curPoint, x, y);
	if (cmpDis(RcToDips(g_lastPointX, g_lastPointY), curPoint, g_gridGap * CLICK_RANGE))
	{
		Board::Piece res = g_mainBoard.setPiece(x, y);

		InvalidateRect(hwnd, NULL, FALSE);

		if (res == Board::white || res == Board::black)
		{
			if (res == Board::white)
				MessageBox(hwnd, _T("White wins!"), _T("Game over"), MB_OK);
			if (res == Board::black)
				MessageBox(hwnd, _T("Black wins!"), _T("Game over"), MB_OK);

			g_mainBoard.clear();

			InvalidateRect(hwnd, NULL, FALSE);
		}
	}
}

void OnPaint()
{
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);

		g_pRenderTarget->BeginDraw();

		g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
		PaintBoard();
		PaintPieces();

		hr = g_pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}

		EndPaint(hwnd, &ps);
	}
}

void PaintBoard()
{
	g_pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
	D2D1_POINT_2F start, end;
	for (int i = 0; i < Board::BOARD_SIZE; ++i)
	{
		start.x = g_boardLeft;
		start.y = g_boardTop + i * g_gridGap;
		end.x = g_boardRight;
		end.y = start.y;

		g_pRenderTarget->DrawLine(
			start,
			end,
			g_pBrush
		);

		start.x = g_boardLeft + i * g_gridGap;
		start.y = g_boardTop;
		end.x = start.x;
		end.y = g_boardBottom;

		g_pRenderTarget->DrawLine(
			start,
			end,
			g_pBrush
		);
	}
}

void PaintPieces()
{
	D2D1_ELLIPSE ellipse;
	ellipse.radiusX = ellipse.radiusY = g_gridGap * PIECE_SIZE;

	g_pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
	for (int i = 0; i < Board::BOARD_SIZE; ++i)
	{
		for (int j = 0; j < Board::BOARD_SIZE; ++j)
		{
			if (g_mainBoard.getPiece(i, j) == Board::black)
			{
				ellipse.point = RcToDips(i, j);
				g_pRenderTarget->FillEllipse(ellipse, g_pBrush);
			}
		}
	}

	g_pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
	for (int i = 0; i < Board::BOARD_SIZE; ++i)
	{
		for (int j = 0; j < Board::BOARD_SIZE; ++j)
		{
			if (g_mainBoard.getPiece(i, j) == Board::white)
			{
				ellipse.point = RcToDips(i, j);
				g_pRenderTarget->FillEllipse(ellipse, g_pBrush);
			}
		}
	}
}

void Resize()
{
	if (g_pRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		g_pRenderTarget->Resize(size);
		CalculateLayout();
		InvalidateRect(hwnd, NULL, FALSE);
	}
}