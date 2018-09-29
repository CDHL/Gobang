#include "draw.h"

#include <Windows.h>
#include <windowsx.h>

HWND hwnd;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLime, int nCmdShow)
{
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Gobang Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.

	hwnd = CreateWindowEx(
		0,						// Optional window styles
		CLASS_NAME,				// Window class
		L"Test Title",			// Window text
		WS_OVERLAPPEDWINDOW,	// Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,		// Parent window
		NULL,		// Menu
		hInstance,	// Instance handle
		NULL		// Additional application data
	);

	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	// Run the message loop.
	
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pFactory)))
		{
			return -1;
		}
		DPIScale::Initialize(g_pFactory);
		return 0;

	//case WM_CLOSE:
	//	if (MessageBox(hwnd, L"真的要退出吗？", L"My Application", MB_OKCANCEL) == IDOK)
	//	{
	//		DestroyWindow(hwnd);
	//	}
	//	return 0;

	case WM_DESTROY:
		DiscardGraphicsResources();
		SafeRelease(g_pFactory);
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		OnPaint();
		return 0;

	case WM_SIZE:
		Resize();
		return 0;

	case WM_LBUTTONDOWN:
		OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;

	case WM_LBUTTONUP:
		OnLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;

	//case WM_MOUSEMOVE:
	//	OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
	//	return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
