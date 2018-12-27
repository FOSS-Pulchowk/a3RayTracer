#include "Common/Core.h"
#include <Windows.h>
#include "Common/gl/glad.h"
#include "Common/gl/glExtensions.h"

#define XWNDCLASSNAME L"xWindowClass"
#define XWIDTH 800
#define XHEIGHT 600

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
		{
			WNDCLASSEXW wndClassExW = {};
			wndClassExW.cbSize = sizeof(wndClassExW);
			wndClassExW.style = CS_HREDRAW | CS_VREDRAW;
			wndClassExW.lpfnWndProc = DefWindowProcW;
			wndClassExW.hInstance = GetModuleHandleW(0);
			wndClassExW.lpszClassName = L"xDummyWindow";
			RegisterClassExW(&wndClassExW);
			HWND hDummyWnd = CreateWindowExW(0, L"xDummyWindow", L"xDummyWindow", 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandleW(0), 0);
			HDC hDummyDC = GetDC(hDummyWnd);
			PIXELFORMATDESCRIPTOR dummyPDF = {};
			dummyPDF.nSize = sizeof(dummyPDF);
			dummyPDF.nVersion = 1;
			dummyPDF.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			dummyPDF.iPixelType = PFD_TYPE_RGBA;
			dummyPDF.cColorBits = 32;
			dummyPDF.cDepthBits = 24;
			dummyPDF.cStencilBits = 8;
			i32 pfi = ChoosePixelFormat(hDummyDC, &dummyPDF);
			xAssert(pfi != 0);
			PIXELFORMATDESCRIPTOR suggestedPFI;
			DescribePixelFormat(hDummyDC, pfi, sizeof(suggestedPFI), &suggestedPFI);
			SetPixelFormat(hDummyDC, pfi, &suggestedPFI);
			HGLRC dummyGLContext = wglCreateContext(hDummyDC);
			xAssert(wglMakeCurrent(hDummyDC, dummyGLContext));
			typedef HGLRC WINAPI tag_wglCreateContextAttribsARB(HDC hdc, HGLRC hShareContext, const i32 *attribList);
			tag_wglCreateContextAttribsARB *wglCreateContextAttribsARB;
			typedef BOOL WINAPI tag_wglChoosePixelFormatARB(HDC hdc, const i32 *piAttribIList, const f32 *pfAttribFList, u32 nMaxFormats, i32 *piFormats, u32 *nNumFormats);
			tag_wglChoosePixelFormatARB *wglChoosePixelFormatARB;
			wglCreateContextAttribsARB = (tag_wglCreateContextAttribsARB*)wglGetProcAddress("wglCreateContextAttribsARB");
			wglChoosePixelFormatARB = (tag_wglChoosePixelFormatARB*)wglGetProcAddress("wglChoosePixelFormatARB");
			xAssert(gladLoadGL());
			xLog(L"OpenGL Version: %d.%d\n", GLVersion.major, GLVersion.minor);
			wglMakeCurrent(hDummyDC, 0);
			wglDeleteContext(dummyGLContext);
			ReleaseDC(hDummyWnd, hDummyDC);
			DestroyWindow(hDummyWnd);

			i32 attribList[] =
			{
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
				WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
				WGL_COLOR_BITS_ARB, 32,
				WGL_DEPTH_BITS_ARB, 24,
				WGL_STENCIL_BITS_ARB, 8,
				0
			};

			i32 pixelFormat;
			u32 numFormats;
			HDC hDC = GetDC(hWnd);
			wglChoosePixelFormatARB(hDC, attribList, 0, 1, &pixelFormat, &numFormats);
			xAssert(numFormats);
			PIXELFORMATDESCRIPTOR pfd;
			DescribePixelFormat(hDC, pixelFormat, sizeof(pfd), &pfd);
			SetPixelFormat(hDC, pixelFormat, &pfd);

			int glVersionAttribs[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
				WGL_CONTEXT_MINOR_VERSION_ARB, 2,
				WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0,
			};
			HGLRC glContext = wglCreateContextAttribsARB(hDC, 0, glVersionAttribs);
			wglMakeCurrent(hDC, glContext);
			ReleaseDC(hWnd, hDC);

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
		glClearColor(0.25f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		SwapBuffers(hDC);
	}

	return 0;
}