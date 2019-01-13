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
namespace x {
	const x_platform Platform;
}

static const HANDLE s_HeapHandle = GetProcessHeap();

#if defined(XDEBUG) || defined(XINTERNAL)
static u64 s_TotalHeapAllocated;
static u64 s_TotalHeapFreed;
#define xInternalAllocationSize(x) ((x) + sizeof(u64))
#define xInternalFreePtr(p) ((void*)((u8*)(p) - sizeof(u64)))
#define xInternalHeapAllocation(x) x;\
			 if(ptr) { \
				if(size > xMegaBytes(1)) \
					xLogWarn("Large Heap Allocation {u}", size); \
				s_TotalHeapAllocated += size;\
				u64* loc = (u64*)ptr; \
				*loc = size; \
				ptr = (u8*)loc + sizeof(u64); \
			}
#define xInternalHeapFree(x) x;\
			if(result) { \
				s_TotalHeapFreed += *(u64*)ptr; \
			}
#else
#define xInternalAllocationSize(x) (x)
#define xInternalFreePtr(p) (p)
#define xInternalHeapAllocation(x) x;
#define xInternalHeapFree(x) x;
#endif

const x::file_content x_platform::LoadFileContent(s8 fileName) const
{
	x::file_content result = {};
	HANDLE hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return result;
	}
	LARGE_INTEGER fileSize;
	xAssert(GetFileSizeEx(hFile, &fileSize));
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

void x_platform::FreeFileContent(x::file_content fileReadInfo) const
{
	if (fileReadInfo.Buffer)
	{
		VirtualFree(fileReadInfo.Buffer, fileReadInfo.Size, MEM_RELEASE);
	}
}

void* x_platform::Malloc(u64 size) const
{
	xInternalHeapAllocation(
		void* ptr = HeapAlloc(s_HeapHandle, 0, xInternalAllocationSize(size))
	);
	return ptr;
}

void* x_platform::Calloc(u64 size) const
{
	xInternalHeapAllocation(
		void* ptr = HeapAlloc(s_HeapHandle, 0, xInternalAllocationSize(size))
	);
	return ptr;
}

void* x_platform::Realloc(void* usrPtr, u64 size) const
{
	xInternalHeapAllocation(
		void* ptr = HeapReAlloc(s_HeapHandle, 0, usrPtr, xInternalAllocationSize(size))
	);
	return ptr;
}

b32 x_platform::Free(void* ptr) const
{
	xInternalHeapFree(b32 result = (b32)HeapFree(s_HeapHandle, 0, xInternalFreePtr(ptr)));
	return result;
}

#if defined(XDEBUG) || defined(XINTERNAL)
u64 x_platform::GetTotalHeapAllocated() const
{
	return s_TotalHeapAllocated;
}
u64 x_platform::GetTotalHeapFreed() const
{
	return s_TotalHeapFreed;
}
#endif

void* operator new(u64 size)
{
	return x::Platform.Malloc(size);
}

void operator delete(void* ptr)
{
	x::Platform.Free(ptr);
}

#define XWIDTH 800
#define XHEIGHT 600

memory_arena NewMemoryBlock(u32 size)
{
	memory_arena arena = {};
	arena.Memory = (u8*)VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	xAssert(arena.Memory);
	arena.Current = arena.Memory;
	arena.Capacity = size;
	arena.Consumed = 0;
	return arena;
}

struct win32_user_data
{
	x_input_system inputSystem;
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
			userData.inputSystem.Buttons[x::ButtonLeft] = x::ButtonDown;
			break;
		}

		case WM_LBUTTONUP:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[x::ButtonLeft] = x::ButtonUp;
			break;
		}

		case WM_RBUTTONDOWN:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[x::ButtonRight] = x::ButtonDown;
			break;
		}

		case WM_RBUTTONUP:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[x::ButtonRight] = x::ButtonUp;
			break;
		}

		case WM_MBUTTONDOWN:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[x::ButtonMiddle] = x::ButtonDown;
			break;
		}

		case WM_MBUTTONUP:
		{
			i32 mx = GET_X_LPARAM(lParam);
			i32 my = GET_Y_LPARAM(lParam);
			userData.inputSystem.MouseX = mx;
			userData.inputSystem.MouseY = XHEIGHT - my;
			userData.inputSystem.Buttons[x::ButtonMiddle] = x::ButtonUp;
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
#if 0
	u32 vao;
	u32 vab, iab;
	xGL(glGenVertexArrays(1, &vao));
	xGL(glGenBuffers(1, &vab));
	xGL(glGenBuffers(1, &iab));

	xGL(glBindVertexArray(vao));
	xGL(glBindBuffer(GL_ARRAY_BUFFER, vab));

	#define XGL_MAX_VERTEX2D 50

	xGL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertex2d) * 4 * XGL_MAX_VERTEX2D, null, GL_STATIC_DRAW));
	xGL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex2d), (void*)(xOffsetOf(vertex2d, vertex2d::position))));
	xGL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex2d), (void*)(xOffsetOf(vertex2d, vertex2d::color))));
	xGL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex2d), (void*)(xOffsetOf(vertex2d, vertex2d::texCoords))));

	xGL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iab));
	xGL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * 6 * XGL_MAX_VERTEX2D, null, GL_STATIC_DRAW));
	xGL(glEnableVertexAttribArray(0));
	xGL(glEnableVertexAttribArray(1));
	xGL(glEnableVertexAttribArray(2));
	xGL(glBindVertexArray(0));


	u32 tvao;
	u32 tvab;
	xGL(glGenVertexArrays(1, &tvao));
	xGL(glGenBuffers(1, &tvab));
	xGL(glBindVertexArray(tvao));
	xGL(glBindBuffer(GL_ARRAY_BUFFER, tvab));
	xGL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertexFont) * 6, null, GL_STATIC_DRAW));
	xGL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertexFont), (void*)(xOffsetOf(vertexFont, vertexFont::position))));
	xGL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertexFont), (void*)(xOffsetOf(vertexFont, vertexFont::color))));
	xGL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertexFont), (void*)(xOffsetOf(vertexFont, vertexFont::texCoords))));
	xGL(glEnableVertexAttribArray(0));
	xGL(glEnableVertexAttribArray(1));
	xGL(glEnableVertexAttribArray(2));
	xGL(glBindVertexArray(0));
#endif

	x::file_content vSource = x::Platform.LoadFileContent("Platform/sample.vert");
	x::file_content fSource = x::Platform.LoadFileContent("Platform/sample.frag");
	u32 sProgram = GLCreateShaderProgramFromFile((s8)vSource.Buffer, (s8)fSource.Buffer);
	x::Platform.FreeFileContent(vSource);
	x::Platform.FreeFileContent(fSource);

	x::file_content vText = x::Platform.LoadFileContent("Platform/font.vert");
	x::file_content fText = x::Platform.LoadFileContent("Platform/font.frag");
	u32 fProgram = GLCreateShaderProgramFromFile((s8)vText.Buffer, (s8)fText.Buffer);
	x::Platform.FreeFileContent(vText);
	x::Platform.FreeFileContent(fSource);

	m4x4 projection = m4x4::OrthographicR(0.0f, 800.0f, 0.0f, 600.0, -1.0f, 1.0f);
	xGL(glUseProgram(sProgram));
	xGL(u32 projLoc = glGetUniformLocation(sProgram, "u_Projection"));
	xGL(glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.elements));
	xGL(glUseProgram(0));

	xGL(glUseProgram(fProgram));
	xGL(u32 aprojLoc = glGetUniformLocation(fProgram, "u_Projection"));
	xGL(glUniformMatrix4fv(aprojLoc, 1, GL_FALSE, projection.elements));
	xGL(glUseProgram(0));

	x::renderer2d renderer2d = x::CreateBatchRenderer2D(sProgram);
	x::renderer_font fontRenderer = x::CreateFontRenderer(fProgram);

	f32 value = 0.0f;

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	xLog("Window displayed.");

	win32_user_data userData = {};
	SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)&userData);
	x_input_system oldInput = {};
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

	memory_arena memory = NewMemoryBlock(xGigaBytes(1));

	x::image* testImage = x::LoadPNGImage(memory, "Resources/BigSmile.png");
	xAssert(testImage);

	x::image* zeroImage = x::LoadPNGImage(memory, "Resources/Zero.png");
	xAssert(zeroImage);

	x::ttf* testFont = x::LoadTTFont(memory, "Resources/HackRegular.ttf", 100);
	//x::ttfont* testFont = x::LoadTTFont(memory, "c:/windows/fonts/arialbd.ttf");
	xAssert(testFont);

	u32 texID, zeroID, fontTexID;

	xGL(glGenTextures(1, &texID));
	xGL(glBindTexture(GL_TEXTURE_2D, texID));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	xGL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, testImage->Width, testImage->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, testImage->Pixels));
	xGL(glBindTexture(GL_TEXTURE_2D, 0));
	
	xGL(glGenTextures(1, &zeroID));
	xGL(glBindTexture(GL_TEXTURE_2D, zeroID));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	xGL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, zeroImage->Width, zeroImage->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, zeroImage->Pixels));
	xGL(glBindTexture(GL_TEXTURE_2D, 0));

	xGL(glGenTextures(1, &fontTexID));
	xGL(glBindTexture(GL_TEXTURE_2D, fontTexID));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	xGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	// NOTE(Zero): Should we pack bytes and use single channel for the fonts or should we use all 4 channels for the fonts
#if 1
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	xGL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, testFont->Width, testFont->Height, 0, GL_RED, GL_UNSIGNED_BYTE, testFont->Pixels));
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
#else
	xGL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, testFont->Width, testFont->Height, 0, GL_RED, GL_UNSIGNED_BYTE, testFont->Pixels));
#endif
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

		x_input_system& input = userData.inputSystem;
		if(oldInput.Buttons[x::ButtonLeft] && input.Buttons[x::ButtonLeft] == x::ButtonUp)
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

		xGL(glBindVertexArray(renderer2d.VertexBufferObject));
		xGL(glBindBuffer(GL_ARRAY_BUFFER, renderer2d.VertexArrayBuffer));
		xGL(glUseProgram(renderer2d.ShaderProgram));
		xGL(u32 loc = glGetUniformLocation(renderer2d.ShaderProgram, "u_Texture"));
		xGL(glUniform1i(loc, 0));
		xGL(glActiveTexture(GL_TEXTURE0));
		xGL(glBindTexture(GL_TEXTURE_2D, texID));
		//xGL(glBindTexture(GL_TEXTURE_2D, zeroID));

		f32 angle = sinf(value);
		
		xGL(x_v2d* v = (x_v2d*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
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

		xGL(glBindVertexArray(fontRenderer.VertexBufferObject));
		xGL(glActiveTexture(GL_TEXTURE1));
		xGL(glBindTexture(GL_TEXTURE_2D, fontTexID));
		//xGL(glBindTexture(GL_TEXTURE_2D, fontTexID));
		xGL(glUseProgram(fontRenderer.ShaderProgram));
		xGL(u32 pos = glGetUniformLocation(fontRenderer.ShaderProgram, "u_Texture"));
		xGL(glUniform1i(pos, 1));
		
		v3 fontColor = { 1,0,0 };
		xGL(pos = glGetUniformLocation(fontRenderer.ShaderProgram, "u_Color"));
		xGL(glUniform3fv(pos, 1, fontColor.values));

		xGL(pos = glGetUniformLocation(fontRenderer.ShaderProgram, "u_Projection"));
		xGL(glUniformMatrix4fv(pos, 1, GL_FALSE, projection.elements));

		xGL(glBindBuffer(GL_ARRAY_BUFFER, fontRenderer.VertexArrayBuffer));
		xGL(x_vfont* fontVertices = (x_vfont*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		
		f32 x = 50;
		f32 y = 50;
		f32 w = 100.0f;
		f32 h = 100.0f;

		f32 tsx = (f32)testFont->Glyphs['A'].Position / (f32)testFont->Width;
		f32 tsy = 0.0f;
		f32 tex = (f32)(testFont->Glyphs['A'].Position + testFont->Glyphs['A'].Width) / (f32)testFont->Width;
		f32 tey = 1.0f; // (f32)testFont->Glyphs['A'].Height / (f32)testFont->Height;

		fontVertices[0].positionTexCoords = { x, y, tsx, tsy };
		fontVertices[1].positionTexCoords = { x, y + h, tsx, tey };
		fontVertices[2].positionTexCoords = { x + w, y + h, tex, tey };
		fontVertices[3].positionTexCoords = { x, y, tsx, tsy };
		fontVertices[4].positionTexCoords = { x + w, y + h, tex, tey };
		fontVertices[5].positionTexCoords = { x + w, y, tex, tsy };

		xGL(glUnmapBuffer(GL_ARRAY_BUFFER));

		xGL(glDrawArrays(GL_TRIANGLES, 0, 6));
		
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