#include "Common/Core.h"
#include "Math/Math.h"
#include "gl/glad.h"
#include "gl/glExtensions.h"
#include "gl/glDebug.h"
#include <Windows.h>
#include <windowsx.h> // for mouse macros

#define XWNDCLASSNAME L"xWindowClass"
#define XWIDTH 800
#define XHEIGHT 600

struct file_read_info
{
	void* Buffer;
	i64 Size;
};

file_read_info ReadEntireFile(s8 fileName)
{
	file_read_info result = {};
	HANDLE hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return result;
	}
	LARGE_INTEGER fileSize;
	xAssert(GetFileSizeEx(hFile, &fileSize));
	result.Size = fileSize.QuadPart;
	// TODO(Zero): Memory Manager
	void* buffer = VirtualAlloc(0, result.Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	i64 totalBytesRead = 0;
	DWORD bytesRead = 0;
	u8* readingPtr = (u8*)buffer;
	while(totalBytesRead < result.Size)
	{
		if(!ReadFile(hFile, readingPtr, (DWORD)result.Size, &bytesRead, NULL))
		{
			if(GetLastError())
			{
				VirtualFree(buffer, result.Size, MEM_RELEASE);
				CloseHandle(hFile);
				result.Size = 0;
				return result;
			}
			else
			{
				break;
			}
		}
		else
		{
			totalBytesRead += bytesRead;
			readingPtr += bytesRead;
			bytesRead = 0;
		}
	}
	CloseHandle(hFile);
	result.Buffer = buffer;
	return result;
}

void FreeFileContent(file_read_info fileReadInfo)
{
	if(fileReadInfo.Buffer)
	{
		VirtualFree(fileReadInfo.Buffer, fileReadInfo.Size, MEM_RELEASE);
	}
}

u32 CompileShader(GLenum type, s8 source)
{
	xGL(u32 shader = glCreateShader(type));
	xGL(glShaderSource(shader, 1, &source, 0));
	xGL(glCompileShader(shader));
	i32 result;
	xGL(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
	if(result != GL_TRUE)
	{
		i32 len;
		utf8 errMsg[1024];
		xGL(glGetShaderInfoLog(shader, 1024, &len, errMsg));
		xGL(xLogWarn("Shader source could not be compiled!\n"));
		xLogWarn("Shader Compilation Error: '%s'\n", errMsg);
		xGL(glDeleteShader(shader));
		return 0;
	}
	return shader;
}

u32 LoadOpenGLShaderFromSource(s8 vSource, s8 fSource)
{
	u32 vShader = CompileShader(GL_VERTEX_SHADER, vSource);
	u32 fShader = CompileShader(GL_FRAGMENT_SHADER, fSource);
	
	xGL(u32 program = glCreateProgram());
	xGL(glAttachShader(program, vShader));
	xGL(glAttachShader(program, fShader));
	xGL(glLinkProgram(program));
	xGL(glDeleteShader(vShader));
	xGL(glDeleteShader(fShader));
	i32 result;
	xGL(glGetProgramiv(program, GL_LINK_STATUS, &result));
	if(result != GL_TRUE)
	{
		i32 len;
		utf8 errMsg[1024];
		xGL(glGetProgramInfoLog(program, 1024, &len, errMsg));
		xGL(xLogWarn("Shaders could not be linked!\n"));
		xGL(glDeleteProgram(program));
		return 0;
	}
	xGL(glUseProgram(0));
	return program;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
		{
			xLog("Initializing OpenGL context");
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
			xLog("OpenGL Context created. Loaded OpenGL Version: {i}.{i}", GLVersion.major, GLVersion.minor);
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
			return 0;
		}

		// TODO(Zero): Inverse mouse Y co-ordinate?

		case WM_MOUSEMOVE:
		{
			i32 x = GET_X_LPARAM(lParam);
			i32 y = GET_Y_LPARAM(lParam);
			xLogTrace("Mouse moved at x:{i} y:{i}", x, y);
			break;
		}

		case WM_LBUTTONDOWN:
		{
			i32 x = GET_X_LPARAM(lParam);
			i32 y = GET_Y_LPARAM(lParam);
			xLogTrace("Left mouse button down at x: {i} y: {i}", x, y);
			break;
		}

		case WM_LBUTTONUP:
		{
			i32 x = GET_X_LPARAM(lParam);
			i32 y = GET_Y_LPARAM(lParam);
			xLogTrace("Left mouse button up at x: {i} y: {i}", x, y);
			break;
		}
		
		case WM_RBUTTONDOWN:
		{
			i32 x = GET_X_LPARAM(lParam);
			i32 y = GET_Y_LPARAM(lParam);
			xLogTrace("Right mouse button down at x: {i} y: {i}", x, y);
			break;
		}

		case WM_RBUTTONUP:
		{
			i32 x = GET_X_LPARAM(lParam);
			i32 y = GET_Y_LPARAM(lParam);
			xLogTrace("Right mouse button up at x: {i} y: {i}", x, y);
			break;
		}

		case WM_SIZE:
		{
			// TODO(Zero): This needs to handle more things then it is currently
			if(wParam == SIZE_MINIMIZED)
			{
				xLog("Window minimized");
			}
			else if(wParam == SIZE_RESTORED)
			{
				xLog("Window restored");
				xLog("Window resized to {i} X {i}", LOWORD(lParam), HIWORD(lParam));
			}
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

#if defined(XDEBUG) || defined(XINTERNAL)
int main()
{
	HINSTANCE hInstance = GetModuleHandleW(0);
#else
i32 CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, i32)
{
#endif
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
		xLogError("Window could not be created!");
		return 1;
	}

	xLog("Window of resolution {i} X {i} created.", XWIDTH, XHEIGHT);
	HDC hDC = GetDC(hWnd);
	xGL(glViewport(0, 0, XWIDTH, XHEIGHT));

	u32 vao;
	xGL(glGenVertexArrays(1, &vao));
	u32 vab, iab;
	xGL(glGenBuffers(1, &vab));
	xGL(glGenBuffers(1, &iab));
	xGL(glBindVertexArray(vao));
	xGL(glBindBuffer(GL_ARRAY_BUFFER, vab));
	f32 vertices[] = {
		200.0f, 100.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		200.0f, 500.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		600.0f, 500.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		600.0f, 100.0f, 0.0f, 1.0f, 1.0f, 1.0f
	};
	xGL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
	xGL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(f32) * 6, null));
	xGL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(f32) * 6, (void*)(sizeof(f32) * 3)));
	xGL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iab));
	u32 indices[] = {
		0, 1, 2,
		0, 2, 3
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	xGL(glEnableVertexAttribArray(0));
	xGL(glEnableVertexAttribArray(1));
	xGL(glBindVertexArray(0));

	file_read_info vSource = ReadEntireFile("Platform/sample.vert");
	file_read_info fSource = ReadEntireFile("Platform/sample.frag");
	u32 sProgram = LoadOpenGLShaderFromSource((s8)vSource.Buffer, (s8)fSource.Buffer);
	FreeFileContent(vSource);
	FreeFileContent(fSource);

	m4x4 projection = m4x4::OrthographicR(0.0f, 800.0f, 0.0f, 600.0, -1.0f, 1.0f);
	m4x4 modalview;
	xGL(glUseProgram(sProgram));
	xGL(u32 projLoc = glGetUniformLocation(sProgram, "Projection"));
	xGL(glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.elements));
	xGL(u32 mvLoc = glGetUniformLocation(sProgram, "ModalView"));
	xGL(glUniformMatrix4fv(mvLoc, 1, GL_FALSE, modalview.elements));
	xGL(glUseProgram(0));

	f32 value = 0.0f;

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	xLog("Window displayed.");
	b32 shouldRun = true;
	while(shouldRun)
	{
		MSG sMsg;
		while(PeekMessageW(&sMsg, null, 0, 0, PM_REMOVE))
		{
			if(sMsg.message == WM_QUIT)
			{
				shouldRun = false;
				xLog("Quitting");
			}
			else
			{
				TranslateMessage(&sMsg);
				DispatchMessageW(&sMsg);
			}
		}
		xGL(glClearColor(0.25f, 0.5f, 1.0f, 1.0f));
		xGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		xGL(glBindVertexArray(vao));
		xGL(glUseProgram(sProgram));
		f32 angle = sinf(value);
		modalview = m4x4::TranslationR({ -400.0f, -300.0f, 0.0f }) *
			m4x4::RotationR(angle, { 0.0f, 0.0f, -1.0f }) *
			m4x4::TranslationR({ 400.0f, 300.0f, 0.0f });
		xGL(glUniformMatrix4fv(mvLoc, 1, GL_FALSE, modalview.elements));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, null);
		SwapBuffers(hDC);
		value += 0.01f;
	}

	return 0;
}