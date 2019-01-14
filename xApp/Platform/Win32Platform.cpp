#include "Common/Core.h"
#include "Platform.h"
#include "GL/LoadOpenGL.h"

#include "GL/GLDebug.h"

#include "GlShader.h"
#include "GlRenderer.h"

#include "Math/Math.h"
#include "Graphics/Image.h"

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
#define xInternalAllocationSize(x) ((x) + sizeof(u64))
#define xInternalGetActualPtr(p) ((void*)((u8*)(p) - sizeof(u64)))
#define xInternalHeapAllocation(x) x;\
			 if(ptr) { \
				if(size > a3MegaBytes(1)) \
					a3LogWarn("Large Heap Allocation of {u} bytes", size); \
				s_TotalHeapAllocated += size;\
				u64* loc = (u64*)ptr; \
				*loc = size; \
				ptr = ((u8*)loc + sizeof(u64)); \
			}
#define xInternalHeapReAllocation(x) x;\
			i32 err = GetLastError(); \
			 if(ptr) { \
				if(size > a3MegaBytes(1)) \
					a3LogWarn("Large Heap Re Allocation of {u} bytes", size); \
				s_TotalHeapAllocated += (size - *(u64*)ptr);\
				u64* loc = (u64*)ptr; \
				*loc = size; \
				ptr = ((u8*)loc + sizeof(u64)); \
			}
#define xInternalHeapFree(x) u64 freed = *(u64*)ptr; \
			x;\
			if(result) { \
				s_TotalHeapFreed += freed; \
			}
#else
#define xInternalAllocationSize(x) (x)
#define xInternalGetActualPtr(p) (p)
#define xInternalHeapAllocation(x) x;
#define xInternalHeapReAllocation(x) x;
#define xInternalHeapFree(x) x;
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
	i64 totalBytesRead = 0;
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
	DWORD numOfBytesToWrite = file.Size;
	DWORD numOfBytesWritten = 0;
	while (numOfBytesWritten < numOfBytesToWrite)
	{
		void* buffer = ((u8*)file.Buffer + numOfBytesWritten);
		if (!WriteFile(hFile, buffer, numOfBytesToWrite, &numOfBytesWritten, NULL))
		{
			CloseHandle(hFile);
			return false;
		}
	}
	CloseHandle(hFile);
	return true;
}

b32 a3_platform::ReplaceFileContent(s8 fileName, const a3::file_content & file) const
{
	HANDLE hFile = CreateFileA(fileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	DWORD numOfBytesToWrite = file.Size;
	DWORD numOfBytesWritten = 0;
	while (numOfBytesWritten < numOfBytesToWrite)
	{
		void* buffer = ((u8*)file.Buffer + numOfBytesWritten);
		if (!WriteFile(hFile, buffer, numOfBytesToWrite, &numOfBytesWritten, NULL))
		{
			CloseHandle(hFile);
			return false;
		}
	}
	CloseHandle(hFile);
	return true;
}

void* a3_platform::Malloc(u64 size) const
{
	xInternalHeapAllocation(
		void* ptr = HeapAlloc(s_HeapHandle, 0, xInternalAllocationSize(size))
	);
	if (!ptr) a3LogWarn("Nullptr returned by heap allocation");
	return ptr;
}

void* a3_platform::Calloc(u64 size) const
{
	xInternalHeapAllocation(
		void* ptr = HeapAlloc(s_HeapHandle, HEAP_ZERO_MEMORY, xInternalAllocationSize(size))
	);
	if (!ptr) a3LogWarn("Nullptr returned by heap allocation");
	return ptr;
}

void* a3_platform::Realloc(void* usrPtr, u64 size) const
{
	if (usrPtr)
	{
		xInternalHeapReAllocation(
			void* ptr = HeapReAlloc(s_HeapHandle, 0, xInternalGetActualPtr(usrPtr), xInternalAllocationSize(size))
		);
		return ptr;
	}
	// NOTE(Zero):
	// If the usrPtr is null, Malloc is called
	// Following the Standard Library
	return a3::Platform.Malloc(size);
}

b32 a3_platform::Free(void* ptr) const
{
	if (ptr)
	{
		xInternalHeapFree(b32 result = (b32)HeapFree(s_HeapHandle, 0, xInternalGetActualPtr(ptr)));
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

#define XWIDTH 800
#define XHEIGHT 600

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

struct win32_user_data
{
	a3_input_system inputSystem;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	win32_user_data& userData = *(win32_user_data*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
	switch(msg)
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
			userData.inputSystem.MouseY = XHEIGHT - my;
			break;
		}

		case WM_LBUTTONDOWN:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[a3::ButtonLeft] = a3::ButtonDown;
			break;
		}

		case WM_LBUTTONUP:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[a3::ButtonLeft] = a3::ButtonUp;
			break;
		}

		case WM_RBUTTONDOWN:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[a3::ButtonRight] = a3::ButtonDown;
			break;
		}

		case WM_RBUTTONUP:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[a3::ButtonRight] = a3::ButtonUp;
			break;
		}

		case WM_MBUTTONDOWN:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[a3::ButtonMiddle] = a3::ButtonDown;
			break;
		}

		case WM_MBUTTONUP:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[a3::ButtonMiddle] = a3::ButtonUp;
			break;
		}

		case WM_SIZE:
		{
			// TODO(Zero): This needs to handle more things then it is currently
			if(wParam == SIZE_MINIMIZED)
			{
				a3Log("Window minimized");
			}
			else if(wParam == SIZE_RESTORED)
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

#define XWNDCLASSNAME L"xWindowClass"
#if defined(A3DEBUG) || defined(A3INTERNAL)
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
		a3LogError("Window could not be created!");
		return 1;
	}
	a3Log("Window of resolution {i} X {i} created.", XWIDTH, XHEIGHT);
	HDC hDC = GetDC(hWnd);
	a3GL(glViewport(0, 0, XWIDTH, XHEIGHT));

	a3::file_content vSource = a3::Platform.LoadFileContent("Platform/sample.vert");
	a3::file_content fSource = a3::Platform.LoadFileContent("Platform/sample.frag");
	u32 sProgram = GLCreateShaderProgramFromFile((s8)vSource.Buffer, (s8)fSource.Buffer);
	a3::Platform.FreeFileContent(vSource);
	a3::Platform.FreeFileContent(fSource);

	a3::file_content vText = a3::Platform.LoadFileContent("Platform/font.vert");
	a3::file_content fText = a3::Platform.LoadFileContent("Platform/font.frag");
	u32 fProgram = GLCreateShaderProgramFromFile((s8)vText.Buffer, (s8)fText.Buffer);
	a3::Platform.FreeFileContent(vText);
	a3::Platform.FreeFileContent(fSource);

	m4x4 projection = m4x4::OrthographicR(0.0f, 800.0f, 0.0f, 600.0, -1.0f, 1.0f);
	a3GL(glUseProgram(sProgram));
	a3GL(u32 projLoc = glGetUniformLocation(sProgram, "u_Projection"));
	a3GL(glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.elements));
	a3GL(glUseProgram(0));

	a3GL(glUseProgram(fProgram));
	a3GL(u32 aprojLoc = glGetUniformLocation(fProgram, "u_Projection"));
	a3GL(glUniformMatrix4fv(aprojLoc, 1, GL_FALSE, projection.elements));
	a3GL(glUseProgram(0));

	a3::renderer2d renderer2d = a3::CreateBatchRenderer2D(sProgram);
	a3::renderer_font fontRenderer = a3::CreateFontRenderer(fProgram);

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

	memory_arena memory = NewMemoryBlock(a3GigaBytes(1));

	a3::image* testImage = a3::LoadPNGImage(memory, "Resources/BigSmile.png");
	i32 err = GetLastError();
	a3Assert(testImage);

	a3::image* zeroImage = a3::LoadPNGImage(memory, "Resources/Zero.png");
	a3Assert(zeroImage);

	a3::fonts* testFont = a3::LoadTTFont(memory, "Resources/HackRegular.ttf", 100.0f);
	//x::ttfont* testFont = x::LoadTTFont(memory, "c:/windows/fonts/arialbd.ttf");
	a3Assert(testFont);

	u32 texID, zeroID, fontTexID;

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

	a3GL(glGenTextures(1, &fontTexID));
	a3GL(glBindTexture(GL_TEXTURE_2D, fontTexID));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	a3GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	// NOTE(Zero): Should we pack bytes and use single channel for the fonts or should we use all 4 channels for the fonts
#if 1
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	u8 loc = 3;
	a3GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, testFont[loc].width, testFont[loc].height, 0, GL_RED, GL_UNSIGNED_BYTE, testFont[loc].pixels));
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
#else
	xGL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, testFont->Width, testFont->Height, 0, GL_RED, GL_UNSIGNED_BYTE, testFont->Pixels));
#endif
	a3GL(glBindTexture(GL_TEXTURE_2D, 0));

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
				a3Log("Quitting");
			}
			else
			{
				TranslateMessage(&sMsg);
				DispatchMessageW(&sMsg);
			}
		}

		a3_input_system& input = userData.inputSystem;
		if(oldInput.Buttons[a3::ButtonLeft] && input.Buttons[a3::ButtonLeft] == a3::ButtonUp)
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
			f32 distance = a3FAbsf(a3Sqrtf(Distance2(rect.position.xy, rect.moveFinalPosition)));
			if(distance < 0.01f)
			{
				rect.isMoving = false;
				rect.moveFrameTime = 0.0f;
				rect.moveFinalPosition = v2{ 0.0f, 0.0f };
			}
		}

		a3GL(glClearColor(0.25f, 0.5f, 1.0f, 1.0f));
		a3GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		a3GL(glDisable(GL_DEPTH_TEST));
		a3GL(glEnable(GL_BLEND));
		a3GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		a3GL(glBindVertexArray(renderer2d.VertexBufferObject));
		a3GL(glBindBuffer(GL_ARRAY_BUFFER, renderer2d.VertexArrayBuffer));
		a3GL(glUseProgram(renderer2d.ShaderProgram));
		a3GL(u32 loc = glGetUniformLocation(renderer2d.ShaderProgram, "u_Texture"));
		a3GL(glUniform1i(loc, 0));
		a3GL(glActiveTexture(GL_TEXTURE0));
		a3GL(glBindTexture(GL_TEXTURE_2D, texID));
		//xGL(glBindTexture(GL_TEXTURE_2D, zeroID));

		f32 angle = a3Sinf(value);
		
		a3GL(x_v2d* v = (x_v2d*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		a3GL(u32* indices = (u32*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
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
		a3GL(glUnmapBuffer(GL_ARRAY_BUFFER));
		a3GL(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER));
		a3GL(glDrawElements(GL_TRIANGLES, X_NUMBER_OF_ENTITIES * 6, GL_UNSIGNED_INT, null));

		a3GL(glBindVertexArray(fontRenderer.VertexBufferObject));
		a3GL(glActiveTexture(GL_TEXTURE1));
		a3GL(glBindTexture(GL_TEXTURE_2D, fontTexID));
		//xGL(glBindTexture(GL_TEXTURE_2D, fontTexID));
		a3GL(glUseProgram(fontRenderer.ShaderProgram));
		a3GL(u32 pos = glGetUniformLocation(fontRenderer.ShaderProgram, "u_Texture"));
		a3GL(glUniform1i(pos, 1));
		
		v3 fontColor = { 1,0,0 };
		a3GL(pos = glGetUniformLocation(fontRenderer.ShaderProgram, "u_Color"));
		a3GL(glUniform3fv(pos, 1, fontColor.values));

		a3GL(pos = glGetUniformLocation(fontRenderer.ShaderProgram, "u_Projection"));
		a3GL(glUniformMatrix4fv(pos, 1, GL_FALSE, projection.elements));

		a3GL(glBindBuffer(GL_ARRAY_BUFFER, fontRenderer.VertexArrayBuffer));
		a3GL(x_vfont* fontVertices = (x_vfont*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		
		f32 x = 50;
		f32 y = 50;
		f32 w = 100.0f;
		f32 h = 100.0f;

		f32 tsx = 0.0f;
		f32 tsy = 0.0f;
		f32 tex = 1.0f;
		f32 tey = 1.0f;

		//fontVertices[0].positionTexCoords = { x, y, tsx, tsy };
		//fontVertices[1].positionTexCoords = { x, y + h, tsx, tey };
		//fontVertices[2].positionTexCoords = { x + w, y + h, tex, tey };
		//fontVertices[3].positionTexCoords = { x, y, tsx, tsy };
		//fontVertices[4].positionTexCoords = { x + w, y + h, tex, tey };
		//fontVertices[5].positionTexCoords = { x + w, y, tex, tsy };
		
		fontVertices[0].positionTexCoords = { x, y, 0.0f, 0.0f };
		fontVertices[1].positionTexCoords = { x, y + h, 0.0f, 1.0f };
		fontVertices[2].positionTexCoords = { x + w, y + h, 1.0f, 1.f };
		fontVertices[3].positionTexCoords = { x, y, 0.0f, 0.0f };
		fontVertices[4].positionTexCoords = { x + w, y + h, 1.0f, 1.0f };
		fontVertices[5].positionTexCoords = { x + w, y, 1.0f, 0.0f };

		a3GL(glUnmapBuffer(GL_ARRAY_BUFFER));

		a3GL(glDrawArrays(GL_TRIANGLES, 0, 6));
		
		SwapBuffers(hDC);
		value += 0.01f;

		LARGE_INTEGER currentPerformanceCounter;
		a3Assert(QueryPerformanceCounter(&currentPerformanceCounter));
		deltaTime = (f32)(currentPerformanceCounter.QuadPart - performanceCounter.QuadPart) / (f32)performanceFrequency.QuadPart;
		performanceCounter = currentPerformanceCounter;
		//xLogTrace("FPS: {f} Time spent last frame: {f}ms", 1000.0f / dt, dt);
	}

	return 0;
}