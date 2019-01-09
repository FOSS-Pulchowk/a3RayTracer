#include "Common/Core.h"
#include "Platform.h"
#include "Math/Math.h"
#include "Graphics/Image.h"
#include "gl/glad.h"
#include "gl/glExtensions.h"
#include "gl/glDebug.h"
#include <Windows.h>
#include <windowsx.h> // for mouse macros

// undefining annoying windows macros
// NOTE(Zero): Add every window macros that fucks up our code
#ifdef LoadImage
#undef LoadImage
#endif
#ifdef CreateWindow
#undef CreateWindow
#endif

#define XWNDCLASSNAME L"xWindowClass"
#define XWIDTH 800
#define XHEIGHT 600

// NOTE(Zero): 
// The value of this global variable is only valid on application startup
static HMODULE g_OpenGLLibrary;

#define KiloBytes(n) (sizeof(u8) * (n) * 1024u)
#define MegaBytes(n) (KiloBytes(n) * 1024u)
#define GigaBytes(n) (MegaBytes(n) * 1024u)
memory_arena NewMemoryBlock(u32 size)
{
	memory_arena arena = {};
	arena.memory = (u8*)VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	xAssert(arena.memory);
	arena.current = arena.memory;
	arena.size = size;
	arena.allocated = 0;
	return arena;
}

enum button
{
	ButtonLeft,
	ButtonRight,
	ButtonMiddle,

	ButtonCount
};

enum input_state
{
	ButtonUp,
	ButtonDown
};

struct input_system
{
	i32 MouseX;
	i32 MouseY;
	input_state Buttons[ButtonCount];
};

struct win32_user_data
{
	input_system inputSystem;
};

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

void* OpenGLFunctionLoader(s8 name)
{
	void* result = 0;
	result = wglGetProcAddress(name);
	if(result) return result;
	xAssert(g_OpenGLLibrary);
	result = GetProcAddress(g_OpenGLLibrary, name);
	xAssert(result);
	return result;
}

void LoadOpenGL(HWND hWnd)
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
	gl_version glVersion;
	g_OpenGLLibrary = LoadLibraryW(L"opengl32.dll");
	xAssert(GladLoadGLLoader(OpenGLFunctionLoader, &glVersion));
	xLog("OpenGL Context created. Loaded OpenGL Version: {i}.{i}", glVersion.major, glVersion.minor);
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
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	win32_user_data& userData = *(win32_user_data*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
	switch(msg)
	{
		case WM_CREATE:
		{
			LoadOpenGL(hWnd);
			return 0;
		}

		// TODO(Zero): Inverse mouse Y co-ordinate?
		case WM_MOUSEMOVE:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			break;
		}

		case WM_LBUTTONDOWN:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[ButtonLeft] = ButtonDown;
			break;
		}

		case WM_LBUTTONUP:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[ButtonLeft] = ButtonUp;
			break;
		}

		case WM_RBUTTONDOWN:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[ButtonRight] = ButtonDown;
			break;
		}

		case WM_RBUTTONUP:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[ButtonRight] = ButtonUp;
			break;
		}

		case WM_MBUTTONDOWN:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[ButtonMiddle] = ButtonDown;
			break;
		}

		case WM_MBUTTONUP:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[ButtonMiddle] = ButtonUp;
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

struct vertex2d
{
	v3 position;
	v3 color;
	v2 texCoords;
};

struct entity
{
	v3 position;
	v2 dimension;
	union
	{
		v3 acolor[4];
		struct
		{
			union { v3 color; };
			union { v3 color; };
			union { v3 color; };
			union { v3 color; };
		};
	};
	b32 isMoving;
	v2 moveFinalPosition;
	f32 moveFrameTime;
};

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

	#define XGL_MAX_VERTEX2D 50
	#define xOffsetOf(s, m) (&(((s*)0)->m))

	xGL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertex2d) * 4 * XGL_MAX_VERTEX2D, null, GL_STATIC_DRAW));
	xGL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex2d), (void*)(xOffsetOf(vertex2d, vertex2d::position))));
	xGL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex2d), (void*)(xOffsetOf(vertex2d, vertex2d::color))));
	xGL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex2d), (void*)(xOffsetOf(vertex2d, vertex2d::texCoords))));

	xGL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iab));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * 6 * XGL_MAX_VERTEX2D, null, GL_STATIC_DRAW);
	xGL(glEnableVertexAttribArray(0));
	xGL(glEnableVertexAttribArray(1));
	xGL(glEnableVertexAttribArray(2));
	xGL(glBindVertexArray(0));

	file_read_info vSource = ReadEntireFile("Platform/sample.vert");
	file_read_info fSource = ReadEntireFile("Platform/sample.frag");
	u32 sProgram = LoadOpenGLShaderFromSource((s8)vSource.Buffer, (s8)fSource.Buffer);
	FreeFileContent(vSource);
	FreeFileContent(fSource);

	m4x4 projection = m4x4::OrthographicR(0.0f, 800.0f, 0.0f, 600.0, -1.0f, 1.0f);
	xGL(glUseProgram(sProgram));
	xGL(u32 projLoc = glGetUniformLocation(sProgram, "Projection"));
	xGL(glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.elements));
	xGL(glUseProgram(0));

	f32 value = 0.0f;

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	xLog("Window displayed.");

	win32_user_data userData = {};
	SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)&userData);
	input_system oldInput = {};
	LARGE_INTEGER performanceFrequency;
	xAssert(QueryPerformanceFrequency(&performanceFrequency));
	LARGE_INTEGER performanceCounter;
	QueryPerformanceCounter(&performanceCounter);

	#define X_NUMBER_OF_ENTITIES 1
	entity rect = {};
	rect.position = { 100.0f, 100.0f, 0.0f };
	rect.dimension = { 100.0f, 100.0f };
	rect.color = { 1.0f, 0.0f, 0.f };
	rect.acolor[0] = { 1.0f, 0.0f, 0.0f };
	rect.acolor[1] = { 0.0f, 1.0f, 0.0f };
	rect.acolor[2] = { 0.0f, 0.0f, 1.0f };
	rect.acolor[3] = { 1.0f, 1.0f, 1.0f };
	rect.isMoving = false;
	rect.moveFinalPosition = v2{ 0.0f, 0.0f };

	memory_arena memory = NewMemoryBlock(GigaBytes(1));

	x::image* testImage = x::LoadImage(memory, "Resources/BigSmile.png");
	xAssert(testImage);

	u32 texID;
	xGL(glGenTextures(1, &texID));
	xGL(glBindTexture(GL_TEXTURE_2D, texID));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	xGL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, testImage->Width, testImage->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, testImage->Pixels));
	xGL(glBindTexture(GL_TEXTURE_2D, 0));

	f32 deltaTime = 0.0f;

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

		input_system& input = userData.inputSystem;
		if(oldInput.Buttons[ButtonLeft] && input.Buttons[ButtonLeft] == ButtonUp)
		{
			//rect.position.x = (f32)input.MouseX - rect.dimension.x / 2;
			//rect.position.y = (f32)input.MouseY - rect.dimension.y / 2;
			rect.isMoving = true;
			rect.moveFinalPosition.x = input.MouseX - rect.dimension.x / 2;
			rect.moveFinalPosition.y = input.MouseY - rect.dimension.y / 2;
			rect.moveFrameTime = 0.0f;
		}
		oldInput = input;

		if(rect.isMoving)
		{
			rect.position.xy += (rect.moveFrameTime * deltaTime * Normalize(rect.moveFinalPosition - rect.position.xy) * 450.0f);
			rect.moveFrameTime += deltaTime;
			f32 distance = fabsf(sqrtf(Distance2(rect.position.xy, rect.moveFinalPosition)));
			if(distance < 0.01f)
			{
				rect.isMoving = false;
				rect.moveFrameTime = 0.0f;
				rect.moveFinalPosition = v2{ 0.0f, 0.0f };
			}
		}

		xGL(glClearColor(0.25f, 0.5f, 1.0f, 1.0f));
		xGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		xGL(glDisable(GL_DEPTH_TEST));
		xGL(glEnable(GL_BLEND));
		xGL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		xGL(glBindVertexArray(vao));
		xGL(glUseProgram(sProgram));
		xGL(u32 loc = glGetUniformLocation(sProgram, "Texture"));
		xGL(glUniform1i(loc, 0));
		xGL(glActiveTexture(GL_TEXTURE0));
		xGL(glBindTexture(GL_TEXTURE_2D, texID));

		f32 angle = sinf(value);

		xGL(vertex2d* v = (vertex2d*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		xGL(u32* indices = (u32*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
		for(i32 i = 0; i < X_NUMBER_OF_ENTITIES; ++i)
		{
			v2 d = rect.dimension;
			v[i * 4 + 0].position = v3{ 0.0f, 0.0f, 0.0f } * m4x4::TranslationR(rect.position);
			v[i * 4 + 1].position = v3{ 0.0f, d.y, 0.0f } * m4x4::TranslationR(rect.position);
			v[i * 4 + 2].position = v3{ d.x, d.y, 0.0f } * m4x4::TranslationR(rect.position);
			v[i * 4 + 3].position = v3{ d.x, 0.0f, 0.0f } * m4x4::TranslationR(rect.position);
			v[i * 4 + 0].color = rect.acolor[0];
			v[i * 4 + 1].color = rect.acolor[1];
			v[i * 4 + 2].color = rect.acolor[2];
			v[i * 4 + 3].color = rect.acolor[3];
			v[i * 4 + 0].texCoords = v2{ 0.0f, 0.0f };
			v[i * 4 + 1].texCoords = v2{ 0.0f, 1.0f };
			v[i * 4 + 2].texCoords = v2{ 1.0f, 1.0f };
			v[i * 4 + 3].texCoords = v2{ 1.0f, 0.0f };

			indices[i * 6 + 0] = i * 4 + 0;
			indices[i * 6 + 1] = i * 4 + 1;
			indices[i * 6 + 2] = i * 4 + 2;
			indices[i * 6 + 3] = i * 4 + 0;
			indices[i * 6 + 4] = i * 4 + 2;
			indices[i * 6 + 5] = i * 4 + 3;
		}
		xGL(glUnmapBuffer(GL_ARRAY_BUFFER));
		xGL(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER));
		xGL(glDrawElements(GL_TRIANGLES, X_NUMBER_OF_ENTITIES * 6, GL_UNSIGNED_INT, null));

		SwapBuffers(hDC);
		value += 0.01f;

		LARGE_INTEGER currentPerformanceCounter;
		xAssert(QueryPerformanceCounter(&currentPerformanceCounter));
		deltaTime = (f32)(currentPerformanceCounter.QuadPart - performanceCounter.QuadPart) / (f32)performanceFrequency.QuadPart;
		performanceCounter = currentPerformanceCounter;
		//xLogTrace("FPS: {f} Time spent last frame: {f}ms", 1000.0f / dt, dt);
	}

	return 0;
}