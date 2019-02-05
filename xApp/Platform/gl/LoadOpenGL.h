#pragma once
#include "Common/Core.h"
#include "Platform/Platform.h"
#include "Glad.h"
#include "GLExtensions.h"

#ifdef A3OSWINDOWS

#include <Windows.h>
#include <wingdi.h>

// NOTE(Zero): 
// The value of this global variable is only valid on application startup
static HMODULE g_OpenGLLibrary;

inline void* GLFunctionLoader(s8 name)
{
	void* result = 0;
	result = wglGetProcAddress(name);
	if (result) return result;
	a3Assert(g_OpenGLLibrary);
	result = GetProcAddress(g_OpenGLLibrary, name);
	a3Assert(result);
	return result;
}

inline void GLLoad(HWND hWnd)
{
	a3Log("Initializing OpenGL context");
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
	a3Assert(pfi != 0);
	PIXELFORMATDESCRIPTOR suggestedPFI;
	DescribePixelFormat(hDummyDC, pfi, sizeof(suggestedPFI), &suggestedPFI);
	SetPixelFormat(hDummyDC, pfi, &suggestedPFI);
	HGLRC dummyGLContext = wglCreateContext(hDummyDC);
	a3Assert(wglMakeCurrent(hDummyDC, dummyGLContext));
	typedef HGLRC WINAPI tag_wglCreateContextAttribsARB(HDC hdc, HGLRC hShareContext, const i32 *attribList);
	tag_wglCreateContextAttribsARB *wglCreateContextAttribsARB;
	typedef BOOL WINAPI tag_wglChoosePixelFormatARB(HDC hdc, const i32 *piAttribIList, const f32 *pfAttribFList, u32 nMaxFormats, i32 *piFormats, u32 *nNumFormats);
	tag_wglChoosePixelFormatARB *wglChoosePixelFormatARB;
	wglCreateContextAttribsARB = (tag_wglCreateContextAttribsARB*)wglGetProcAddress("wglCreateContextAttribsARB");
	wglChoosePixelFormatARB = (tag_wglChoosePixelFormatARB*)wglGetProcAddress("wglChoosePixelFormatARB");
	gl_version glVersion;
	g_OpenGLLibrary = LoadLibraryW(L"opengl32.dll");
	a3Assert(GladLoadGLLoader(GLFunctionLoader, &glVersion));
	a3Log("OpenGL Context created. Loaded OpenGL Version: {i}.{i}", glVersion.major, glVersion.minor);
	FreeLibrary(g_OpenGLLibrary);
	g_OpenGLLibrary = 0;
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
	a3Assert(numFormats);
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
}


#endif