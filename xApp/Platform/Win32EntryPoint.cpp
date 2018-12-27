#include "Common/Core.h"
#include <Windows.h>
#include "Common/glad/glad.h"

#define XWNDCLASSNAME L"xWindowClass"
#define XWIDTH 800
#define XHEIGHT 600

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
		{
			PIXELFORMATDESCRIPTOR pixelFormatDescriptor = {};
			pixelFormatDescriptor.nSize = sizeof(pixelFormatDescriptor);
			pixelFormatDescriptor.nVersion = 1;
			pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
			pixelFormatDescriptor.cColorBits = 32;
			pixelFormatDescriptor.cDepthBits = 24;
			pixelFormatDescriptor.cStencilBits = 8;

			HDC hDC = GetDC(hWnd);
			i32 pixelFormatIndex = ChoosePixelFormat(hDC, &pixelFormatDescriptor);
			xAssert(pixelFormatIndex != 0);
			PIXELFORMATDESCRIPTOR suggestedPixelFormatDescriptor = {};
			DescribePixelFormat(hDC, pixelFormatIndex, sizeof(suggestedPixelFormatDescriptor), &suggestedPixelFormatDescriptor);
			SetPixelFormat(hDC, pixelFormatIndex, &suggestedPixelFormatDescriptor);
			HGLRC glContext = wglCreateContext(hDC);
			xAssert(wglMakeCurrent(hDC, glContext));
			ReleaseDC(hWnd, hDC);

			xAssert(gladLoadGL());
			xLog(L"OpenGL Version: %d.%d\n", GLVersion.major, GLVersion.minor);
			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);
			return 0;
		}

		case WM_SIZE:
		{
			HDC hDC = GetDC(hWnd);
			SwapBuffers(hDC);
			ReleaseDC(hWnd, hDC);
			return 0;
		}

		case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

i32 CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, i32)
{
	WNDCLASSEXW wndClassExW = {};
	wndClassExW.cbSize = sizeof(wndClassExW);
	wndClassExW.style = CS_HREDRAW | CS_VREDRAW;
	wndClassExW.lpfnWndProc = WndProc;
	wndClassExW.hInstance = hInstance;
	wndClassExW.lpszClassName = XWNDCLASSNAME;
	//TODO(Zero): Put icons
	//wndClassExW.hIcon = 
	//wndClassExW.hIconSm =
	wndClassExW.hCursor = LoadCursorW(hInstance, IDC_ARROW);

	RegisterClassExW(&wndClassExW);

	DWORD wndStyles = WS_OVERLAPPEDWINDOW;
	i32 width = XWIDTH;
	i32 height = XHEIGHT;
	RECT wrc = {};
	wrc.right = width;
	wrc.bottom = height;
	AdjustWindowRectEx(&wrc, wndStyles, FALSE, 0);
	width = wrc.right - wrc.left;
	height = wrc.bottom - wrc.top;
	HWND hWnd = CreateWindowExW(0, XWNDCLASSNAME, L"x Application", wndStyles, CW_USEDEFAULT, CW_USEDEFAULT, width, height, null, null, hInstance, 0);

	if(!hWnd)
	{
		xLogError(L"Window could not be created!");
		return 1;
	}
	HDC hDC = GetDC(hWnd);
	glViewport(0, 0, XWIDTH, XHEIGHT);

	b32 shouldRun = true;
	while(shouldRun)
	{
		MSG sMsg;
		while(PeekMessageW(&sMsg, null, 0, 0, PM_REMOVE))
		{
			if(sMsg.message == WM_QUIT)
			{
				shouldRun = false;
			}
			else
			{
				TranslateMessage(&sMsg);
				DispatchMessageW(&sMsg);
			}
		}
		glClearColor(0.25f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		SwapBuffers(hDC);
	}

	return 0;
}