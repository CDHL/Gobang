#include "draw.h"
#include "game.h"

#include <d2d1.h>
#pragma comment(lib, "d2d1")

ID2D1Factory			*g_pFactory;
ID2D1HwndRenderTarget	*g_pRenderTarget;
ID2D1SolidColorBrush	*g_pBrush;
ID2D1StrokeStyle		*g_pStrokeStyle;

const float BORDER_GAP = 0.1f;

float g_boardLeft, g_boardTop, g_boardRight, g_boardBottom;
float g_gridGap;

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

		g_gridGap = halfSideLength * 2 / (BOARD_SIZE - 1);
	}
}

HRESULT CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (g_pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(hwnd, &rc);

		D2D_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = g_pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hwnd, size),
			&g_pRenderTarget
		);
		
		if (SUCCEEDED(hr))
		{
			//float dashes[] = { 1.0f, 2.0f, 2.0f, 3.0f, 2.0f, 2.0f };
			hr = g_pFactory->CreateStrokeStyle(
				D2D1::StrokeStyleProperties(
					D2D1_CAP_STYLE_ROUND,
					D2D1_CAP_STYLE_ROUND,
					D2D1_CAP_STYLE_ROUND,
					D2D1_LINE_JOIN_MITER,
					10.f,
					D2D1_DASH_STYLE_DASH_DOT_DOT
				),
				NULL,
				0,
				&g_pStrokeStyle
			);

			if (SUCCEEDED(hr))
			{
				hr = g_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &g_pBrush);

				if (SUCCEEDED(hr))
				{
					CalculateLayout();
				}
			}
		}
	}
	return hr;
}

void DiscardGraphicsResources()
{
	SafeRelease(g_pRenderTarget);
	SafeRelease(g_pBrush);
	SafeRelease(g_pStrokeStyle);
}

D2D1_POINT_2F rcToDips(int x, int y)
{
	return D2D1::Point2F(g_boardLeft + g_gridGap * x, g_boardTop + g_gridGap * y);
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
	for (int i = 0; i < BOARD_SIZE; ++i)
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