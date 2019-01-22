#include "Common/Core.h"
#include "Platform.h"
#include "GL/LoadOpenGL.h"

#include "GL/GLDebug.h"
#include "GlRenderer.h"
#include "Platform/AssetManager.h"

#include "Math/Math.h"
#include "Graphics/Image.h"

#include <Windows.h>
#include <windowsx.h> // for mouse macros

// Needed only for Debug and internal build
#if defined(A3DEBUG) || defined(A3INTERNAL)
// TODO(Zero): Implement our own and remove this
#include <stdio.h> // For _snprintf_s
#endif

// undefining annoying windows macros
// NOTE(Zero): Add every window macros that fucks up our code
#ifdef LoadImage
#undef LoadImage
#endif
#ifdef CreateWindow
#undef CreateWindow
#endif

//
// Globals
//
namespace a3 {
	const a3_platform Platform;
}

static const HANDLE s_HeapHandle = GetProcessHeap();

#if defined(A3DEBUG) || defined(A3INTERNAL)
static u64 s_TotalHeapAllocated;
static u64 s_TotalHeapFreed;
#define a3Main() main()
#define a3InternalAllocationSize(x) ((x) + sizeof(u64))
#define a3InternalGetActualPtr(p) ((void*)((u8*)(p) - sizeof(u64)))
#define a3InternalHeapAllocation(x) x;\
			 if(ptr) { \
				if(size > a3MegaBytes(1)) \
					a3LogWarn("Large Heap Allocation of {u} bytes", size); \
				s_TotalHeapAllocated += size;\
				u64* loc = (u64*)ptr; \
				*loc = size; \
				ptr = ((u8*)loc + sizeof(u64)); \
			} \
			else \
				 a3LogWarn("Nullptr returned by heap allocation");
#define a3InternalHeapReAllocation(x) x;\
			 if(ptr) { \
				if(size > a3MegaBytes(1)) \
					a3LogWarn("Large Heap Re Allocation of {u} bytes", size); \
				s_TotalHeapAllocated += (size - *(u64*)ptr);\
				u64* loc = (u64*)ptr; \
				*loc = size; \
				ptr = ((u8*)loc + sizeof(u64)); \
			}
#define a3InternalHeapFree(x) u64 freed = *(u64*)(a3InternalGetActualPtr(ptr)); \
			x;\
			if(result) { \
				s_TotalHeapFreed += freed; \
			}
#else
#define a3Main() CALLBACK wWinMain(HINSTANCE, HINSTANCE, LPWSTR, i32)
#define a3InternalAllocationSize(x) (x)
#define a3InternalGetActualPtr(p) (p)
#define a3InternalHeapAllocation(x) x;
#define a3InternalHeapReAllocation(x) x;
#define a3InternalHeapFree(x) x;
#endif

const a3::file_content a3_platform::LoadFileContent(s8 fileName) const
{
	a3::file_content result = {};
	HANDLE hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return result;
	}
	LARGE_INTEGER fileSize;
	a3Assert(GetFileSizeEx(hFile, &fileSize));
	result.Size = fileSize.QuadPart;
	void* buffer = VirtualAlloc(0, result.Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	u64 totalBytesRead = 0;
	DWORD bytesRead = 0;
	u8* readingPtr = (u8*)buffer;
	while (totalBytesRead < result.Size)
	{
		if (!ReadFile(hFile, readingPtr, (DWORD)result.Size, &bytesRead, NULL))
		{
			if (GetLastError())
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

void a3_platform::FreeFileContent(a3::file_content fileReadInfo) const
{
	if (fileReadInfo.Buffer)
	{
		VirtualFree(fileReadInfo.Buffer, fileReadInfo.Size, MEM_RELEASE);
	}
}

b32 a3_platform::WriteFileContent(s8 fileName, const a3::file_content & file) const
{
	HANDLE hFile = CreateFileA(fileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	a3Assert(file.Size < (u64)max_u32);
	DWORD numOfBytesToWrite = (u32)file.Size;
	DWORD numOfBytesWritten = 0;
	if (!WriteFile(hFile, file.Buffer, numOfBytesToWrite, &numOfBytesWritten, NULL))
	{
		CloseHandle(hFile);
		return false;
	}
	// NOTE(Zero): Synchronous way writes all data at once
	b32 result = (numOfBytesToWrite == numOfBytesWritten);
	CloseHandle(hFile);
	return result;
}

b32 a3_platform::ReplaceFileContent(s8 fileName, const a3::file_content & file) const
{
	HANDLE hFile = CreateFileA(fileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	a3Assert(file.Size < (u64)max_u32);
	DWORD numOfBytesToWrite = (u32)file.Size;
	DWORD numOfBytesWritten = 0;
	if (!WriteFile(hFile, file.Buffer, numOfBytesToWrite, &numOfBytesWritten, NULL))
	{
		CloseHandle(hFile);
		return false;
	}
	// NOTE(Zero): Synchronous way writes all data at once
	b32 result = (numOfBytesToWrite == numOfBytesWritten);
	CloseHandle(hFile);
	return result;
}

void* a3_platform::Malloc(u64 size) const
{
	a3InternalHeapAllocation(
		void* ptr = HeapAlloc(s_HeapHandle, 0, a3InternalAllocationSize(size))
	);
	return ptr;
}

void* a3_platform::Calloc(u64 size) const
{
	a3InternalHeapAllocation(
		void* ptr = HeapAlloc(s_HeapHandle, HEAP_ZERO_MEMORY, a3InternalAllocationSize(size))
	);
	return ptr;
}

void* a3_platform::Realloc(void* usrPtr, u64 size) const
{
	if (usrPtr)
	{
		a3InternalHeapReAllocation(
			void* ptr = HeapReAlloc(s_HeapHandle, 0, a3InternalGetActualPtr(usrPtr), a3InternalAllocationSize(size))
		);
		return ptr;
	}
	// NOTE(Zero):
	// If the usrPtr is null, Malloc is called
	// Following the Standard Library
	return a3::Platform.Malloc(size);
}

void* a3_platform::Recalloc(void* usrPtr, u64 size) const
{
	if (usrPtr)
	{
		a3InternalHeapReAllocation(
			void* ptr = HeapReAlloc(s_HeapHandle, HEAP_ZERO_MEMORY, a3InternalGetActualPtr(usrPtr), a3InternalAllocationSize(size))
		);
		return ptr;
	}
	// NOTE(Zero):
	// If the usrPtr is null, Malloc is called
	// Following the Standard Library
	return a3::Platform.Calloc(size);
}

b32 a3_platform::Free(void* ptr) const
{
	if (ptr)
	{
		a3InternalHeapFree(b32 result = (b32)HeapFree(s_HeapHandle, 0, a3InternalGetActualPtr(ptr)));
		return result;
	}
	a3LogWarn("Attempt to free null pointer");
	return false;
}

#if defined(A3DEBUG) || defined(A3INTERNAL)
u64 a3_platform::GetTotalHeapAllocated() const
{
	return s_TotalHeapAllocated;
}
u64 a3_platform::GetTotalHeapFreed() const
{
	return s_TotalHeapFreed;
}
#endif

void* operator new(u64 size)
{
	return a3::Platform.Malloc(size);
}

void operator delete(void* ptr)
{
	a3::Platform.Free(ptr);
}

// TODO(Zero):
// Do not force window resolution
// Make the window size fixed
// Make application independent to window resolution
#define A3_WINDOW_WIDTH 800
#define A3_WINDOW_HEIGHT 600

memory_arena NewMemoryBlock(u32 size)
{
	memory_arena arena = {};
	arena.Memory = (u8*)VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	a3Assert(arena.Memory);
	arena.Current = arena.Memory;
	arena.Capacity = size;
	arena.Consumed = 0;
	return arena;
}

// TODO(Zero):
// Make keyboard input system
// Input system should be moved from platforn layer and needs to be made proper
struct win32_user_data
{
	a3_input_system inputSystem;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	win32_user_data& userData = *(win32_user_data*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
	switch (msg)
	{
	case WM_CREATE:
	{
		GLLoad(hWnd);
		return 0;
	}

	// TODO(Zero): Inverse mouse Y co-ordinate?
	case WM_MOUSEMOVE:
	{
		i32 mx = GET_X_LPARAM(lParam);
		i32 my = GET_Y_LPARAM(lParam);
		userData.inputSystem.MouseX = mx;
		userData.inputSystem.MouseY = A3_WINDOW_HEIGHT - my;
		break;
	}

	case WM_LBUTTONDOWN:
	{
		i32 mx = GET_X_LPARAM(lParam);
		i32 my = GET_Y_LPARAM(lParam);
		userData.inputSystem.MouseX = mx;
		userData.inputSystem.MouseY = A3_WINDOW_HEIGHT - my;
		userData.inputSystem.Buttons[a3::ButtonLeft] = a3::ButtonDown;
		break;
	}

	case WM_LBUTTONUP:
	{
		i32 mx = GET_X_LPARAM(lParam);
		i32 my = GET_Y_LPARAM(lParam);
		userData.inputSystem.MouseX = mx;
		userData.inputSystem.MouseY = A3_WINDOW_HEIGHT - my;
		userData.inputSystem.Buttons[a3::ButtonLeft] = a3::ButtonUp;
		break;
	}

	case WM_RBUTTONDOWN:
	{
		i32 mx = GET_X_LPARAM(lParam);
		i32 my = GET_Y_LPARAM(lParam);
		userData.inputSystem.MouseX = mx;
		userData.inputSystem.MouseY = A3_WINDOW_HEIGHT - my;
		userData.inputSystem.Buttons[a3::ButtonRight] = a3::ButtonDown;
		break;
	}

	case WM_RBUTTONUP:
	{
		i32 mx = GET_X_LPARAM(lParam);
		i32 my = GET_Y_LPARAM(lParam);
		userData.inputSystem.MouseX = mx;
		userData.inputSystem.MouseY = A3_WINDOW_HEIGHT - my;
		userData.inputSystem.Buttons[a3::ButtonRight] = a3::ButtonUp;
		break;
	}

	case WM_MBUTTONDOWN:
	{
		i32 mx = GET_X_LPARAM(lParam);
		i32 my = GET_Y_LPARAM(lParam);
		userData.inputSystem.MouseX = mx;
		userData.inputSystem.MouseY = A3_WINDOW_HEIGHT - my;
		userData.inputSystem.Buttons[a3::ButtonMiddle] = a3::ButtonDown;
		break;
	}

	case WM_MBUTTONUP:
	{
		i32 mx = GET_X_LPARAM(lParam);
		i32 my = GET_Y_LPARAM(lParam);
		userData.inputSystem.MouseX = mx;
		userData.inputSystem.MouseY = A3_WINDOW_HEIGHT - my;
		userData.inputSystem.Buttons[a3::ButtonMiddle] = a3::ButtonUp;
		break;
	}

	case WM_SIZE:
	{
		// TODO(Zero): This needs to handle more things then it is currently
		if (wParam == SIZE_MINIMIZED)
		{
			a3Log("Window minimized");
		}
		else if (wParam == SIZE_RESTORED)
		{
			a3Log("Window restored");
			a3Log("Window resized to {i} X {i}", LOWORD(lParam), HIWORD(lParam));
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


// TODO(Zero):
// Make entity system proper
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

#define A3_WINDOW_CLASS_NAME L"a3WindowClass"

i32 a3Main()
{
	HMODULE hInstance = GetModuleHandleW(0);

	WNDCLASSEXW wndClassExW = {};
	wndClassExW.cbSize = sizeof(wndClassExW);
	wndClassExW.style = CS_HREDRAW | CS_VREDRAW;
	wndClassExW.lpfnWndProc = WndProc;
	wndClassExW.hInstance = hInstance;
	wndClassExW.lpszClassName = A3_WINDOW_CLASS_NAME;
	//TODO(Zero): Put icons
	//wndClassExW.hIcon = 
	//wndClassExW.hIconSm =
	wndClassExW.hCursor = LoadCursorW(hInstance, IDC_ARROW);
	RegisterClassExW(&wndClassExW);

	DWORD wndStyles = WS_OVERLAPPEDWINDOW;
	i32 width = A3_WINDOW_WIDTH;
	i32 height = A3_WINDOW_HEIGHT;
	RECT wrc = {};
	wrc.right = width;
	wrc.bottom = height;
	AdjustWindowRectEx(&wrc, wndStyles, FALSE, 0);
	width = wrc.right - wrc.left;
	height = wrc.bottom - wrc.top;
	HWND hWnd = CreateWindowExW(0, A3_WINDOW_CLASS_NAME, L"a3 Ray Tracer", wndStyles, CW_USEDEFAULT, CW_USEDEFAULT, width, height, A3NULL, A3NULL, hInstance, 0);

	if (!hWnd)
	{
		a3LogError("Window could not be created!");
		return 1;
	}
	a3Log("Window of resolution {i} X {i} created.", A3_WINDOW_WIDTH, A3_WINDOW_HEIGHT);
	HDC hDC = GetDC(hWnd);
	a3GL(glViewport(0, 0, A3_WINDOW_WIDTH, A3_WINDOW_HEIGHT));

	a3::file_content r2dVS = a3::Platform.LoadFileContent("Platform/GLSL/Basic2DVertexShader.glsl");
	a3::file_content r2dFS = a3::Platform.LoadFileContent("Platform/GLSL/Basic2DFragmentShader.glsl");
	a3Assert(r2dVS.Buffer);
	a3Assert(r2dFS.Buffer);
	a3::file_content fVS = a3::Platform.LoadFileContent("Platform/GLSL/FontVertexShader.glsl");
	a3::file_content fFS = a3::Platform.LoadFileContent("Platform/GLSL/FontFragmentShader.glsl");
	a3Assert(fVS.Buffer);
	a3Assert(fFS.Buffer);
	a3::file_content uiVS = a3::Platform.LoadFileContent("Platform/GLSL/UIVertexShader.glsl");
	a3::file_content uiFS = a3::Platform.LoadFileContent("Platform/GLSL/UIFragmentShader.glsl");
	a3Assert(uiVS.Buffer);
	a3Assert(uiFS.Buffer);

	a3::basic2drenderer renderer2d = a3::Renderer.Create2DRenderer((s8)r2dVS.Buffer, (s8)r2dFS.Buffer);
	renderer2d.SetRegion(0.0f, 800.0f, 0.0f, 600.0f);

	a3::Asset.LoadFontFromFile(2, "Resources/HackRegular.ttf", 50.0f);
	a3::font_renderer fontRenderer = a3::Renderer.CreateFontRenderer((s8)fVS.Buffer, (s8)fFS.Buffer);
	fontRenderer.SetRegion(0.0f, 800.0f, 0.0f, 600.0f);
	fontRenderer.SetFont(a3::Asset.Get<a3::font>(2));

	a3::ui_renderer uiRenderer = a3::Renderer.CreateUIRenderer((s8)uiVS.Buffer, (s8)uiFS.Buffer);
	uiRenderer.SetRegion(0.0f, 800.0f, 0.0f, 600.0f);

	a3::Platform.FreeFileContent(r2dVS);
	a3::Platform.FreeFileContent(r2dFS);
	a3::Platform.FreeFileContent(fFS);
	a3::Platform.FreeFileContent(fFS);
	a3::Platform.FreeFileContent(uiFS);
	a3::Platform.FreeFileContent(uiFS);

	f32 value = 0.0f;

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	a3Log("Window displayed.");

	win32_user_data userData = {};
	SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)&userData);
	a3_input_system oldInput = {};
	LARGE_INTEGER performanceFrequency;
	a3Assert(QueryPerformanceFrequency(&performanceFrequency));
	LARGE_INTEGER performanceCounter;
	QueryPerformanceCounter(&performanceCounter);

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

	//memory_arena memory = NewMemoryBlock(a3GigaBytes(1));
	a3::Asset.LoadImageFromFile(0, "Resources/BigSmile.png");
	a3::Asset.LoadImageFromFile(1, "Resources/Zero.png");

	a3::image* testImage = a3::Asset.Get<a3::image>(0);
	a3::image* zeroImage = a3::Asset.Get<a3::image>(1);

	u32 texID, zeroID;

	// TODO(Zero):
	// This should be moved to GL platform and should not be in Win32
	a3GL(glGenTextures(1, &texID));
	a3GL(glBindTexture(GL_TEXTURE_2D, texID));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	a3GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, testImage->Width, testImage->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, testImage->Pixels));
	a3GL(glBindTexture(GL_TEXTURE_2D, 0));

	a3GL(glGenTextures(1, &zeroID));
	a3GL(glBindTexture(GL_TEXTURE_2D, zeroID));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	a3GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, zeroImage->Width, zeroImage->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, zeroImage->Pixels));
	a3GL(glBindTexture(GL_TEXTURE_2D, 0));	

	// NOTE(Zero):
	// Width = ?
	// Height = ?;
	i32 a = 255;
	const utf8 ticked[] =
	{
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 0, 126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

	u32 tick;
	a3GL(glGenTextures(1, &tick));
	a3GL(glBindTexture(GL_TEXTURE_2D, tick));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	a3GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 50, 50, 0, GL_RED, GL_BYTE, ticked));
	a3GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
	a3GL(glBindTexture(GL_TEXTURE_2D, 0));

	a3::WritePNGImage("test_img.png", 18, 8, 1, 1, (uutf8*)ticked);

	v3 redcolor[] =
	{
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f}
	};

	f32 deltaTime = 0.0f;

	b32 shouldRun = true;
	while (shouldRun)
	{
		MSG sMsg;
		while (PeekMessageW(&sMsg, A3NULL, 0, 0, PM_REMOVE))
		{
			if (sMsg.message == WM_QUIT)
			{
				shouldRun = false;
				a3Log("Quitting");
			}
			else
			{
				TranslateMessage(&sMsg);
				DispatchMessageW(&sMsg);
			}
		}

		a3_input_system& input = userData.inputSystem;
		if (oldInput.Buttons[a3::ButtonLeft] && input.Buttons[a3::ButtonLeft] == a3::ButtonUp)
		{
			//rect.position.x = (f32)input.MouseX - rect.dimension.x / 2;
			//rect.position.y = (f32)input.MouseY - rect.dimension.y / 2;
			rect.isMoving = true;
			rect.moveFinalPosition.x = input.MouseX - rect.dimension.x / 2;
			rect.moveFinalPosition.y = input.MouseY - rect.dimension.y / 2;
			rect.moveFrameTime = 0.0f;
		}
		oldInput = input;

		if (rect.isMoving)
		{
			rect.position.xy += (rect.moveFrameTime * deltaTime * Normalize(rect.moveFinalPosition - rect.position.xy) * 450.0f);
			rect.moveFrameTime += deltaTime;
			f32 distance = FAbsf(Sqrtf(Distance2(rect.position.xy, rect.moveFinalPosition)));
			if (distance < 0.01f)
			{
				rect.isMoving = false;
				rect.moveFrameTime = 0.0f;
				rect.moveFinalPosition = v2{ 0.0f, 0.0f };
			}
		}

		a3GL(glClearColor(0.2f, 0.2f, 0.2f, 1.0f));
		a3GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		a3GL(glDisable(GL_DEPTH_TEST));
		a3GL(glEnable(GL_BLEND));
		a3GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		v2 dimension = { 100.0f, 100.0f };
		renderer2d.Render(rect.position, dimension, rect.acolor, texID);

		uiRenderer.Push(rect.position.xy + v2{ 100.0f, 100.0f }, dimension, redcolor, { 0.0f, 0.0f }, tick);
		uiRenderer.Flush(tick);

		LARGE_INTEGER currentPerformanceCounter;
		a3Assert(QueryPerformanceCounter(&currentPerformanceCounter));
		deltaTime = (f32)(currentPerformanceCounter.QuadPart - performanceCounter.QuadPart) / (f32)performanceFrequency.QuadPart;
		performanceCounter = currentPerformanceCounter;

#if defined(A3DEBUG) || defined(A3INTERNAL)
		utf8 buffer[256];
		_snprintf_s(buffer, 256, 256, "FPS: %d", (i32)(1.0f / deltaTime));
		fontRenderer.Render(buffer, { 0.0f, 580.0f }, 0.6f, { 0.8f, 0.9f, 0.2f });
		_snprintf_s(buffer, 256, 256, "Total Heap Allocations: %.2fKB", (f32)a3::Platform.GetTotalHeapAllocated() / (1024.0f));
		fontRenderer.Render(buffer, { 0.0f, 560.0f }, 0.4f, { 0.8f, 0.9f, 0.2f });
		_snprintf_s(buffer, 256, 256, "Total Heap Freed: %.2fKB", (f32)a3::Platform.GetTotalHeapFreed() / (1024.0f));
		fontRenderer.Render(buffer, { 0.0f, 540.0f }, 0.4f, { 0.8f, 0.9f, 0.2f });
		//_snprintf_s(buffer, 256, 256, "Total Application Memory: %.2fMB", (f32)memory.Capacity / (1024.0f * 1024.0f));
		//fontRenderer.Render(buffer, { 0.0f, 520.0f }, 0.4f, { 0.8f, 0.9f, 0.2f }, fontTexture, *font);
		//_snprintf_s(buffer, 256, 256, "Used Application Memory: %.2fMB", (f32)memory.Consumed / (1024.0f * 1024.0f));
		//fontRenderer.Render(buffer, { 0.0f, 500.0f }, 0.4f, { 0.8f, 0.9f, 0.2f }, fontTexture, *font);
#endif

		SwapBuffers(hDC);
		value += 0.01f;

	}

	return 0;
}