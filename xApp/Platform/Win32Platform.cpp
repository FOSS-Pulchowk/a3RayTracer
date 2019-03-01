#include "Common/Core.h"
#include "Platform.h"
#include "GL/LoadOpenGL.h"

#include "GL/GLDebug.h"
#include "GlRenderer.h"
#include "Utility/AssetManager.h"

#include "Math/Math.h"
#include "Math/Color.h"
#include "Utility/UIContext.h"
#include "Utility/Algorithm.h"

#include "Graphics/Rasterizer3D.h"

#include "HardwarePlatform.h"

#include <Windows.h>
#include <windowsx.h> // for mouse macros
// for windows dialogue windows/boxes
#include <objbase.h>
#include <shobjidl_core.h>

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

#ifdef MessageBox
#undef MessageBox
#endif

//
// Globals
//
namespace a3 {
	const a3_platform Platform;
}

static const HANDLE s_GenericHeapHandle = GetProcessHeap();
static const HANDLE s_PersistentHeapHandle = HeapCreate(0, a3MegaBytes(512), 0);

// TODO(Zero):
// Make keyboard input system
// Input system should be moved from platforn layer and needs to be made proper
struct win32_user_data
{
	HWND windowHandle;
	a3::input_info inputSystem;
};

win32_user_data* Win32GetUserData()
{
	static win32_user_data s_Win32UserData;
	return &s_Win32UserData;
}

#if defined(A3DEBUG) || defined(A3INTERNAL)
static u64 s_TotalHeapAllocated;
static u64 s_TotalHeapFreed;
static u64 s_PersistantHeapAllocated;
static u64 s_PersistantHeapFreed;
#define a3Main() main()

#define a3InternalAllocationSize(x) ((x) + sizeof(u64))
#define a3InternalGetActualPtr(p) ((void*)((u8*)(p) - sizeof(u64)))

#define a3InternalHeapAllocation(x) x;\
if(ptr) { \
    if(size > a3MegaBytes(1)) \
    a3LogWarn("Large Heap Allocation of {u} bytes at {s}:{i}", size, file, line); \
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
    a3LogWarn("Large Heap Re Allocation of {u} bytes at {s}:{i}", size, file, line); \
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

#define a3InternalPersistantHeapAllocation(x) x;\
if(ptr) { \
    if(size > a3MegaBytes(1)) \
    a3LogWarn("Large Heap Allocation of {u} bytes at {s}:{i}", size, file, size); \
    s_PersistantHeapAllocated += size;\
    u64* loc = (u64*)ptr; \
    *loc = size; \
    ptr = ((u8*)loc + sizeof(u64)); \
} \
else \
a3LogWarn("Nullptr returned by heap allocation");
#define a3InternalPersistantHeapReAllocation(x) x;\
if(ptr) { \
    if(size > a3MegaBytes(1)) \
    a3LogWarn("Large Heap Re Allocation of {u} bytes at {s}:{i}", size, file, size); \
    s_PersistantHeapAllocated += (size - *(u64*)ptr);\
    u64* loc = (u64*)ptr; \
    *loc = size; \
    ptr = ((u8*)loc + sizeof(u64)); \
}
#define a3InternalPersistantHeapFree(x) u64 freed = *(u64*)(a3InternalGetActualPtr(ptr)); \
x;\
if(result) { \
    s_PersistantHeapFreed += freed; \
}
#else
#define a3Main() CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, i32)
#define a3InternalAllocationSize(x) (x)
#define a3InternalGetActualPtr(p) (p)
#define a3InternalHeapAllocation(x) x
#define a3InternalHeapReAllocation(x) x
#define a3InternalHeapFree(x) x
#define a3InternalPersistantHeapAllocation(x) x
#define a3InternalPersistantHeapReAllocation(x) x
#define a3InternalPersistantHeapFree(x) x
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
				VirtualFree(buffer, 0, MEM_RELEASE);
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
		VirtualFree(fileReadInfo.Buffer, 0, MEM_RELEASE);
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

#if defined(A3DEBUG) || defined(A3INTERNAL)
#define A3_DEFINE_ALLOCATION(name) name(u64 size, s8 file, i32 line)
#define A3_DEFINE_REALLOCATION(name) name(void* usrPtr, u64 size, s8 file, i32 line)
#else
#define A3_DEFINE_ALLOCATION(name) name(u64 size)
#define A3_DEFINE_REALLOCATION(name) name(void* usrPtr, u64 size)
#endif

void* a3_platform::A3_DEFINE_ALLOCATION(Malloc) const
{
	a3InternalHeapAllocation(
		void* ptr = HeapAlloc(s_GenericHeapHandle, 0, a3InternalAllocationSize(size))
	);
	return ptr;
}

void* a3_platform::A3_DEFINE_ALLOCATION(Calloc) const
{
	a3InternalHeapAllocation(
		void* ptr = HeapAlloc(s_GenericHeapHandle, HEAP_ZERO_MEMORY, a3InternalAllocationSize(size))
	);
	return ptr;
}

void* a3_platform::A3_DEFINE_REALLOCATION(Realloc) const
{
	if (usrPtr)
	{
		a3InternalHeapReAllocation(
			void* ptr = HeapReAlloc(s_GenericHeapHandle, 0, a3InternalGetActualPtr(usrPtr), a3InternalAllocationSize(size))
		);
		return ptr;
	}
	// NOTE(Zero):
	// If the usrPtr is null, Malloc is called
	// Following the Standard Library
	return a3Malloc(size, void);
}

void* a3_platform::A3_DEFINE_REALLOCATION(Recalloc) const
{
	if (usrPtr)
	{
		a3InternalHeapReAllocation(
			void* ptr = HeapReAlloc(s_GenericHeapHandle, HEAP_ZERO_MEMORY, a3InternalGetActualPtr(usrPtr), a3InternalAllocationSize(size))
		);
		return ptr;
	}
	// NOTE(Zero):
	// If the usrPtr is null, Malloc is called
	// Following the Standard Library
	return a3Calloc(size, void);
}

b32 a3_platform::Free(void* ptr) const
{
	if (ptr)
	{
		a3InternalHeapFree(b32 result = (b32)HeapFree(s_GenericHeapHandle, 0, a3InternalGetActualPtr(ptr)));
		return result;
	}
	return false;
}

void * a3_platform::A3_DEFINE_ALLOCATION(AllocMemory) const
{
	a3InternalPersistantHeapAllocation(
		void* ptr = HeapAlloc(s_PersistentHeapHandle, HEAP_ZERO_MEMORY, a3InternalAllocationSize(size))
	);
	return ptr;
}

void * a3_platform::A3_DEFINE_REALLOCATION(ResizeMemory) const
{
	if (usrPtr)
	{
		a3InternalPersistantHeapReAllocation(
			void* ptr = HeapReAlloc(s_PersistentHeapHandle, HEAP_ZERO_MEMORY, a3InternalGetActualPtr(usrPtr), a3InternalAllocationSize(size))
		);
		return ptr;
	}
	// NOTE(Zero):
	// If the usrPtr is null, Malloc is called
	// Following the Standard Library
	return a3Allocate(size, void);
}

b32 a3_platform::Release(void * ptr) const
{
	if (ptr)
	{
		a3InternalPersistantHeapFree(b32 result = (b32)HeapFree(s_PersistentHeapHandle, 0, a3InternalGetActualPtr(ptr)));
		return result;
	}
	return false;
}

utf8 * a3_platform::LoadFromDialogue(s8 title, a3::file_type type) const
{
	IFileOpenDialog *pOpenDialog = A3NULL;
	HRESULT hr;

	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, (void**)&pOpenDialog);
	if (SUCCEEDED(hr))
	{
		utf16* wTitle = A3NULL;
		i32 size = MultiByteToWideChar(CP_UTF8, 0, title, -1, wTitle, 0);
		wTitle = a3New utf16[size];
		MultiByteToWideChar(CP_UTF8, 0, title, -1, wTitle, size);
		pOpenDialog->SetTitle(wTitle);
		a3Delete[] wTitle;

		if (type == a3::FileTypePNG)
		{
			COMDLG_FILTERSPEC filterTypes = {
				L"Portable Network Graphics",
				L"*.png"
			};
			pOpenDialog->SetFileTypes(1, &filterTypes);
			pOpenDialog->SetFileTypeIndex(1);
		}

		hr = pOpenDialog->Show(Win32GetUserData()->windowHandle);
		utf8* resultPath = A3NULL;
		if (SUCCEEDED(hr))
		{
			IShellItem *pShellItem;
			hr = pOpenDialog->GetResult(&pShellItem);
			if (SUCCEEDED(hr))
			{
				utf16* wFilePath;
				hr = pShellItem->GetDisplayName(SIGDN_FILESYSPATH, &wFilePath);
				if (SUCCEEDED(hr))
				{
					// NOTE(Zero):
					// MAX_PATH is used here because when I query the length information and use it
					// it didn't work, and I don't know why, could be because the WideChar string given
					// might not be null terminated, I could not find information on this in the documentation
					resultPath = a3New utf8[MAX_PATH];
					i32 v = WideCharToMultiByte(CP_UTF8, 0, wFilePath, -1, resultPath, MAX_PATH, 0, 0);
					i32 err = GetLastError();
					CoTaskMemFree(wFilePath);
				}
				pShellItem->Release();
			}
		}
		pOpenDialog->Release();
		return resultPath;
	}
	return A3NULL;
}

utf8 * a3_platform::SaveFromDialogue(s8 title, a3::file_type type) const
{
	IFileSaveDialog *pSaveDialog = A3NULL;
	HRESULT hr;

	hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, (void**)&pSaveDialog);
	if (SUCCEEDED(hr))
	{
		utf16* wTitle = A3NULL;
		i32 size = MultiByteToWideChar(CP_UTF8, 0, title, -1, wTitle, 0);
		wTitle = a3New utf16[size];
		MultiByteToWideChar(CP_UTF8, 0, title, -1, wTitle, size);
		pSaveDialog->SetTitle(wTitle);
		a3Delete[] wTitle;

		if (type == a3::FileTypePNG)
		{
			COMDLG_FILTERSPEC filterTypes = {
				L"Portable Network Graphics",
				L"*.png"
			};
			pSaveDialog->SetFileTypes(1, &filterTypes);
			pSaveDialog->SetFileTypeIndex(1);
		}

		hr = pSaveDialog->Show(Win32GetUserData()->windowHandle);
		utf8* resultPath = A3NULL;
		if (SUCCEEDED(hr))
		{
			IShellItem *pShellItem;
			hr = pSaveDialog->GetResult(&pShellItem);
			if (SUCCEEDED(hr))
			{
				utf16* wFilePath;
				hr = pShellItem->GetDisplayName(SIGDN_FILESYSPATH, &wFilePath);
				if (SUCCEEDED(hr))
				{
					// NOTE(Zero):
					// MAX_PATH is used here because when I query the length information and use it
					// it didn't work, and I don't know why, could be because the WideChar string given
					// might not be null terminated, I could not find information on this in the documentation
					resultPath = a3Calloc(MAX_PATH, utf8);
					i32 v = WideCharToMultiByte(CP_UTF8, 0, wFilePath, -1, resultPath, MAX_PATH, 0, 0);
					if (v == MAX_PATH)
					{
						resultPath = a3Recalloc(resultPath, MAX_PATH + 3, utf8);
					}
					*(u32*)&resultPath[v] = type;
					CoTaskMemFree(wFilePath);
				}
				pShellItem->Release();
			}
		}
		pSaveDialog->Release();
		return resultPath;
	}
	return A3NULL;
}

void a3_platform::FreeDialogueData(utf8 * data) const
{
	a3Delete[] data;
}

a3::message_box_result a3_platform::MessageBox(s8 title, s8 caption, a3::message_box_type type, a3::message_box_icon icon) const
{
	u32 flags = 0;
	switch (type)
	{
	case a3::message_box_type::MessageBoxTypeAbortRetryIgnore: flags |= MB_ABORTRETRYIGNORE; break;
	case a3::message_box_type::MessageBoxTypeCancelTryContinue: flags |= MB_CANCELTRYCONTINUE; break;
	case a3::message_box_type::MessageBoxTypeHelp: flags |= MB_HELP; break;
	case a3::message_box_type::MessageBoxTypeOk: flags |= MB_OK; break;
	case a3::message_box_type::MessageBoxTypeOkCancel: flags |= MB_OKCANCEL; break;
	case a3::message_box_type::MessageBoxTypeRetryCancel: flags |= MB_RETRYCANCEL; break;
	case a3::message_box_type::MessageBoxTypeYesNo: flags |= MB_YESNO; break;
	case a3::message_box_type::MessageBoxTypeYesNoCancel: flags |= MB_YESNOCANCEL; break;
	}
	switch (icon)
	{
	case a3::message_box_icon::MessageBoxIconNone: flags |= 0; break;
	case a3::message_box_icon::MessageBoxIconExclamation: flags |= MB_ICONEXCLAMATION; break;
	case a3::message_box_icon::MessageBoxIconWarning: flags |= MB_ICONWARNING; break;
	case a3::message_box_icon::MessageBoxIconInformation: flags |= MB_ICONINFORMATION; break;
	case a3::message_box_icon::MessageBoxIconAsterisk: flags |= MB_ICONASTERISK; break;
	case a3::message_box_icon::MessageBoxIconQuestion: flags |= MB_ICONQUESTION; break;
	case a3::message_box_icon::MessageBoxIconStop: flags |= MB_ICONSTOP; break;
	case a3::message_box_icon::MessageBoxIconError: flags |= MB_ICONERROR; break;
	case a3::message_box_icon::MessageBoxIconHand: flags |= MB_ICONHAND; break;
	}
	i32 result = MessageBoxA(Win32GetUserData()->windowHandle, caption, title, flags);
	switch (result)
	{
	case IDABORT: return a3::message_box_result::MessageBoxResultAbort;
	case IDCANCEL: return a3::message_box_result::MessageBoxResultCancel;
	case IDCONTINUE: return a3::message_box_result::MessageBoxResultContinue;
	case IDIGNORE: return a3::message_box_result::MessageBoxResultIgnore;
	case IDNO: return a3::message_box_result::MessageBoxResultNo;
	case IDOK: return a3::message_box_result::MessageBoxResultOk;
	case IDRETRY: return a3::message_box_result::MessageBoxResultRetry;
	case IDTRYAGAIN: return a3::message_box_result::MessageBoxResultTryAgain;
	case IDYES: return a3::message_box_result::MessageBoxResultYes;
	default: return a3::message_box_result::MessageBoxResultError;
	}
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
u64 a3_platform::GetPersistantHeapAllocated() const
{
	return s_PersistantHeapAllocated;
}
u64 a3_platform::GetPersistantHeapFreed() const
{
	return s_PersistantHeapFreed;
}
#endif

void* operator A3_DEFINE_ALLOCATION(new)
{
#if defined(A3DEBUG) || defined(A3INTERNAL)
	return a3::Platform.Malloc(size, file, line);
#else
	return a3::Platform.Malloc(size);
#endif
}

void* operator A3_DEFINE_ALLOCATION(new[])
{
#if defined(A3DEBUG) || defined(A3INTERNAL)
	return a3::Platform.Malloc(size, file, line);
#else
	return a3::Platform.Malloc(size);
#endif
}

void * operator new(u64 size, void * where)
{
	return where;
}

void * operator new[](u64 size, void * where)
{
	return where;
}

void operator delete(void* ptr)
{
	a3::Platform.Free(ptr);
}

void operator delete[](void* ptr)
{
	a3::Platform.Free(ptr);
}

void operator delete[](void*, void*)
{
}

void operator delete(void*, void*)
{
}

// TODO(Zero):
// Do not force window resolution
// Make the window size fixed
// Make application independent to window resolution
#define A3_WINDOW_WIDTH 1280
#define A3_WINDOW_HEIGHT 720

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

a3::key Win32MapKey(WPARAM keyCode)
{
	switch (keyCode)
	{
	case VK_UP: return a3::KeyUp;
	case VK_DOWN: return a3::KeyDown;
	case VK_LEFT: return a3::KeyLeft;
	case VK_RIGHT: return a3::KeyRight;
	}
	return a3::KeyUnknown;
}

void Win32ProcessKeyState(a3::key_state* state, LPARAM lParam)
{
	state->Repeats = a3ConsumeBits(lParam, 0, 15);
	state->Extended = a3GetBit(lParam, a3Bit(24));
	state->Down = (a3GetBit(lParam, a3Bit(31)) == 0);
	state->Up = (a3GetBit(lParam, a3Bit(31)) == 1);
	state->Pressed = ((a3GetBit(lParam, a3Bit(30)) == 0) && state->Up);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	win32_user_data& userData = *(win32_user_data*)Win32GetUserData();

	auto l_StoreInputData = [&userData](LPARAM lParam)
	{
		i32 mx = GET_X_LPARAM(lParam);
		i32 my = GET_Y_LPARAM(lParam);
		i32 ww = userData.inputSystem.WindowWidth;
		i32 wh = userData.inputSystem.WindowHeight;
		userData.inputSystem.MouseX = (f32)mx / (f32)ww;
		// NOTE(Zero): Y coordinate for mouse position is inversed
		userData.inputSystem.MouseY = (f32)(wh - my) / (f32)wh;
	};

	switch (msg)
	{
	case WM_SYSCHAR:
		return 0;

	case WM_ACTIVATEAPP:
		return 0;

	case WM_CREATE:
	{
		GLLoad(hWnd);
		userData.windowHandle = hWnd;
		return 0;
	}

	case WM_MOUSEMOVE:
	{
		l_StoreInputData(lParam);
		break;
	}

	case WM_LBUTTONDOWN:
	{
		l_StoreInputData(lParam);
		userData.inputSystem.Buttons[a3::ButtonLeft].Down = true;
		userData.inputSystem.Buttons[a3::ButtonLeft].Up = false;
		break;
	}

	case WM_LBUTTONUP:
	{
		l_StoreInputData(lParam);
		userData.inputSystem.Buttons[a3::ButtonLeft].Down = false;
		userData.inputSystem.Buttons[a3::ButtonLeft].Up = true;
		break;
	}

	case WM_RBUTTONDOWN:
	{
		l_StoreInputData(lParam);
		userData.inputSystem.Buttons[a3::ButtonRight].Down = true;
		userData.inputSystem.Buttons[a3::ButtonRight].Up = false;
		break;
	}

	case WM_RBUTTONUP:
	{
		l_StoreInputData(lParam);
		userData.inputSystem.Buttons[a3::ButtonRight].Down = false;
		userData.inputSystem.Buttons[a3::ButtonRight].Up = true;
		break;
	}

	case WM_MBUTTONDOWN:
	{
		l_StoreInputData(lParam);
		userData.inputSystem.Buttons[a3::ButtonMiddle].Down = true;
		userData.inputSystem.Buttons[a3::ButtonMiddle].Up = false;
		break;
	}

	case WM_MBUTTONUP:
	{
		l_StoreInputData(lParam);
		userData.inputSystem.Buttons[a3::ButtonMiddle].Down = false;
		userData.inputSystem.Buttons[a3::ButtonMiddle].Up = true;
		break;
	}

	case WM_KEYUP:
	case WM_KEYDOWN:
	{
		Win32ProcessKeyState(&userData.inputSystem.Keys[Win32MapKey(wParam)], lParam);
	} break;

	case WM_SIZE:
	{
		// TODO(Zero): This needs to handle more things then it is currently
		if (wParam == SIZE_MINIMIZED)
		{
			a3Log("Window minimized");
		}
		a3Log("Window resized to {i} X {i}", LOWORD(lParam), HIWORD(lParam));
		userData.inputSystem.WindowWidth = LOWORD(lParam);
		userData.inputSystem.WindowHeight = HIWORD(lParam);
		HDC dc = GetDC(hWnd);
		SwapBuffers(dc);
		ReleaseDC(hWnd, dc);
		return 0;
	}

	case WM_SETCURSOR:
		SetCursor(LoadCursorA(A3NULL, IDC_ARROW));
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProcA(hWnd, msg, wParam, lParam);
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


#define A3_WINDOW_CLASS_NAME "a3WindowClass"

struct transform
{
	static const v3 WorldUp;
	static const v3 WorldRight;
	static const v3 WorldForward;

	v3 position;
	v3 scale;
	quat orientation;

	transform()
	{
		position = {};
		scale = { 1.0f, 1.0f, 1.0f };
		orientation = quat();
	}

	transform& SetPosition(f32 x, f32 y, f32 z)
	{
		position.x = x;
		position.y = y;
		position.z = z;
		return *this;
	}

	transform& SetPosition(const v3& position)
	{
		this->position = position;
		return *this;
	}

	transform& SetScale(f32 x, f32 y, f32 z)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
		return *this;
	}

	transform& SetScale(const v3& scale)
	{
		this->scale = scale;
		return *this;
	}

	transform& FactorScale(f32 factor)
	{
		scale *= factor;
		return *this;
	}

	transform& FactorScale(f32 xf, f32 yf, f32 zf)
	{
		scale.x *= xf;
		scale.y *= yf;
		scale.z *= zf;
		return *this;
	}

	transform& FactorScale(const v3& factor)
	{
		scale = scale * factor;
		return *this;
	}

	transform& SetOrientation(f32 yaw, f32 pitch, f32 roll)
	{
		orientation = EulerAnglesToQuat(v3{ yaw, pitch, roll });
		return *this;
	}

	transform& SetOrientation(const v3& eulerAngles)
	{
		orientation = EulerAnglesToQuat(eulerAngles);
		return *this;
	}

	v3 GetForward()
	{
		return GetAxisFromQuat(quat::Normalize(orientation));
	}

	v3 GetRight()
	{
		return Cross(Normalize(GetForward()), transform::WorldUp);
	}

	v3 GetUp()
	{
		return Cross(Normalize(GetForward()), transform::WorldRight);
	}

	transform& MoveTowards(f32 distance, v3 direction)
	{
		v3 udir = Normalize(direction);
		position += udir * distance;
		return *this;
	}

	transform& MoveForward(f32 distance)
	{
		return MoveTowards(distance, GetForward());
	}

	transform& MoveRight(f32 distance)
	{
		return MoveTowards(distance, GetRight());
	}

	transform& MoveUp(f32 distance)
	{
		return MoveTowards(distance, GetUp());
	}

	transform& LookAt(const v3& target)
	{
		v3 dir = Normalize(target - position);
		f32 angle = ArcCosf(a3ToRadians(-dir.z));
		orientation = AngleAxisToQuat(angle, dir);
		return *this;
	}

	transform& LookAt(f32 x, f32 y, f32 z)
	{
		return LookAt(v3{ x,y,z });
	}

	transform& RotateOrientation(f32 angle, const v3& axis)
	{
		orientation *= AngleAxisToQuat(angle, Normalize(axis));
		return *this;
	}

	transform& RotateOrientationUp(f32 angle)
	{
		return RotateOrientation(angle, GetUp());
	}

	transform& RotateOrientationRight(f32 angle)
	{
		return RotateOrientation(angle, GetRight());
	}

	transform& RotateOrientationForward(f32 angle)
	{
		return RotateOrientation(angle, GetForward());
	}

	m4x4 CalculateModelM4X4()
	{
		return m4x4::ScaleR(scale)*QuatToMat4x4R(orientation)*m4x4::TranslationR(position);
	}
};

const v3 transform::WorldUp = v3{ 0,1,0 };
const v3 transform::WorldRight = v3{ 1,0,0 };
const v3 transform::WorldForward = v3{ 0,0,-1 };

i32 a3Main()
{
	HMODULE hInstance = GetModuleHandleA(0);

	WNDCLASSEXA wndClassExA = {};
	wndClassExA.cbSize = sizeof(wndClassExA);
	wndClassExA.style = CS_HREDRAW | CS_VREDRAW;
	wndClassExA.lpfnWndProc = WndProc;
	wndClassExA.hInstance = hInstance;
	wndClassExA.lpszClassName = A3_WINDOW_CLASS_NAME;
	//TODO(Zero): Put icons
	//wndClassExW.hIcon = 
	//wndClassExW.hIconSm =
	wndClassExA.hCursor = LoadCursorA(hInstance, IDC_ARROW);
	RegisterClassExA(&wndClassExA);

	DWORD wndStyles = WS_OVERLAPPEDWINDOW;
	i32 width = A3_WINDOW_WIDTH;
	i32 height = A3_WINDOW_HEIGHT;
	RECT wrc = {};
	wrc.right = width;
	wrc.bottom = height;
	AdjustWindowRectEx(&wrc, wndStyles, FALSE, 0);
	width = wrc.right - wrc.left;
	height = wrc.bottom - wrc.top;
	HWND hWnd = CreateWindowExA(0, A3_WINDOW_CLASS_NAME, "a3 Ray Tracer", wndStyles, CW_USEDEFAULT, CW_USEDEFAULT, width, height, A3NULL, A3NULL, hInstance, 0);
	if (!hWnd)
	{
		a3LogError("Window could not be created!");
		return 1;
	}
	a3Log("Window of resolution {i} X {i} created.", A3_WINDOW_WIDTH, A3_WINDOW_HEIGHT);

	HRESULT hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (FAILED(hr))
	{
		a3LogError("Could not initialize COM objects!");
		return -1;
	}

	// NOTE(Zero):
	// Seeding for random generator is done here
	{
		u32 seeds[16];
		LARGE_INTEGER li;
		for (i32 i = 0; i < 16; ++i)
		{
			QueryPerformanceCounter(&li);
			seeds[i] = li.LowPart;
		}
		a3::InitializeGenerator(seeds);
	}

	a3::basic2d_renderer renderer = a3::Renderer.Create2DRenderer(a3::shaders::GLBasic2DVertex, a3::shaders::GLBasic2DFragment);
	renderer.SetRegion(0.0f, 1280.0f, 0.0f, 720.0f);

	//a3::image* temp = a3::Asset.LoadImageFromFile(19, "Resources/BigSmile.png");
	a3::image img = a3::CreateImageBuffer(640, 480);
	a3::FillImageBuffer(&img, a3::color::Black);

	a3::swapchain sc;
	sc.SetFrameBuffer(&img);
	//sc.SetCamera(m4x4::LookR(v3{ 0.0f, 0.0f, 0.0f }, v3{ 0.0f, 0.0f, 1.0f }));
	sc.SetViewport(0, 0, 640, 480);
	sc.SetMesh(a3::Asset.LoadMeshFromFile(21, "Resources/Axis.obj"));

	//a3::DrawLine(&img, v2{ 0.0f, 0.0f }, v2{ 1280.0f, 720.0f }, a3::color::Blue);
	//a3::FillTriangle(&img, v2{ 50.0f, 50.0f }, v2{ 100.0f, 100.0f }, v2{ 30.0f, 100.0f }, a3::color::Green);
	//a3::FillTriangle(&img, v2{ 100.0f, 200.0f }, v2{ 200.0f, 300.0f }, v2{ 20.0f, 350.0f }, a3::color::White);
	//a3::FillTriangle(&img, v2{ 50.0f, 100.0f }, v2{ 200.0f, 100.0f }, v2{ 100.0f, 300.0f }, a3::color::Blue);
	//a3::FillTriangle(&img, v2{ 0.0f, 0.0f }, v2{ 1280.0f, 500.0f }, v2{ 500.0f, 700.0f }, a3::color::White);
	//a3::FillTriangle(&img, v2{ 0.0f, 0.0f }, v2{ 1300.0f, 500.0f }, v2{ 800.0f, 900.0f }, a3::color::White);
	//a3::FillTriangle(&img, v2{ 1235.0f, 492.0f }, v2{ 1280.0f, 492.0f }, v2{ 1280.0f, 510.0f }, a3::color::Red);
	//a3::FillTriangle(&img, v2{ 1000.0f, 100.0f }, v2{ 1500.0f, 200.0f }, v2{ 1600.0f, 500.0f }, a3::color::Red);
	//a3::FillTriangle(&img, v2{ 500.0f, 500.0f }, v2{ 600.0f, 600.0f }, v2{ 400.0f, 600.0f }, a3::color::White);
	//a3::FillTriangle(&img, v2{ -100.0f, 150.0f }, v2{ 150.0f, 50.0f }, v2{ 150.0f, 300.0f }, a3::color::White);

	a3::image fontbg = a3::CreateImageBuffer(500, 500);
	a3::FillImageBuffer(&fontbg, a3::color::Black, 0.5f);

	a3::image_texture* bigsmile = a3::Asset.LoadTexture2DFromFile(11, "Resources/BigSmile.png", a3::FilterLinear, a3::WrapClampToEdge);
	a3::image_texture* hugesmile = a3::Asset.LoadTexture2DFromFile(12, "Resources/HugeSmile.png", a3::FilterLinear, a3::WrapClampToEdge);
	a3::image_texture* fontback = a3::Asset.LoadTexture2DFromPixels(14, fontbg.Pixels, fontbg.Width, fontbg.Height, fontbg.Channels, a3::FilterLinear, a3::WrapClampToEdge);

	a3Log("Window displayed.");

	a3::input_info oldInput = {};
	LARGE_INTEGER performanceFrequency;
	a3Assert(QueryPerformanceFrequency(&performanceFrequency));
	LARGE_INTEGER performanceCounter;
	QueryPerformanceCounter(&performanceCounter);

	b32 renderDebugInformation = true;

	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	f32 deltaTime = 0.0f;
	win32_user_data* userData = Win32GetUserData();
	HDC windowDeviceContext = GetDC(userData->windowHandle);

	a3::font_renderer fontRenderer = a3::Renderer.CreateFontRenderer(a3::shaders::GLFontVertex, a3::shaders::GLFontFragment);
	fontRenderer.SetRegion(0.0f, 1280.0f, 0.0f, 720.0f);
	a3::Asset.LoadFontTextureAtlasFromFile(a3::asset_id::DebugFont, "Resources/HackRegular.ttf", 50.0f);
	fontRenderer.SetFont(a3::Asset.Get<a3::font_texture>(a3::asset_id::DebugFont));
	a3::ui_context ui(1280.0f, 720.0f);

	//v3 cameraPosition = {};
	//v3 cameraForward = v3{ 0,0,-1 };
	//cameraForward = Normalize(cameraForward);

	transform camera;

	f32 angle = a3ToRadians(5.0f);
	f32 speed = 5.0f;

	b32 shouldRun = true;
	while (shouldRun)
	{
		MSG sMsg;
		while (PeekMessageA(&sMsg, A3NULL, 0, 0, PM_REMOVE))
		{
			if (sMsg.message == WM_QUIT)
			{
				shouldRun = false;
				a3Log("Quitting");
				break;
			}
			else
			{
				TranslateMessage(&sMsg);
				DispatchMessageA(&sMsg);
			}
		}


		if (userData->inputSystem.Keys[a3::KeyUp].Down)
		{
			camera.MoveForward(speed * deltaTime);
		}

		if (userData->inputSystem.Keys[a3::KeyDown].Down)
		{
			camera.MoveForward(-speed * deltaTime);
		}

		if (userData->inputSystem.Keys[a3::KeyRight].Down)
		{
			camera.RotateOrientationRight(angle * deltaTime);
		}

		if (userData->inputSystem.Keys[a3::KeyLeft].Down)
		{
			camera.RotateOrientationRight(-angle * deltaTime);
		}

		if (userData->inputSystem.Buttons[a3::ButtonLeft].Down)
		{
		}

		if (userData->inputSystem.Buttons[a3::ButtonRight].Down)
		{
		}

		a3::FillImageBuffer(&img, a3::color::Black);
		sc.SetCamera(camera.CalculateModelM4X4());
		//sc.SetCamera(m4x4::Identity());
		m4x4 model = m4x4::TranslationR(v3{ 0, 0, -5 });
		sc.Render(model);

		//angle += 2.0f;

		v3 cc = a3::color::NotQuiteBlack;
		a3GL(glClearColor(cc.r, cc.g, cc.b, 1.0f));
		a3GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		a3GL(glDisable(GL_DEPTH_TEST));
		a3GL(glEnable(GL_BLEND));
		a3GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		// TODO(Zero):
		// Only change the viewport if the window has been resized
		// Should we setup callbacks for window resizing?
		a3GL(glViewport(0, 0, userData->inputSystem.WindowWidth, userData->inputSystem.WindowHeight));

		ui.UpdateIO(userData->inputSystem);
		if (userData->inputSystem.Buttons[a3::ButtonRight].Up && oldInput.Buttons[a3::ButtonRight].Down)
			renderDebugInformation = !renderDebugInformation;

		oldInput = userData->inputSystem;

		LARGE_INTEGER currentPerformanceCounter;
		a3Assert(QueryPerformanceCounter(&currentPerformanceCounter));
		deltaTime = (f32)(currentPerformanceCounter.QuadPart - performanceCounter.QuadPart) / (f32)performanceFrequency.QuadPart;
		performanceCounter = currentPerformanceCounter;

		a3::image_texture* raw = a3::Asset.LoadTexture2DFromPixels(13, img.Pixels, img.Width, img.Height, img.Channels, a3::FilterLinear, a3::WrapClampToEdge);

		renderer.BeginFrame();
		renderer.Push(v3{ 300.0f, 200.0f, 0.0f }, 50.0f, a3::color::White, bigsmile);
		renderer.Push(v3{ 300.0f, 300.0f, 0.0f }, 50.0f, a3::color::White, hugesmile);
		renderer.Push(v3{ 100.0f, 100.0f, 0.0f }, v2{ 640, 480 }, a3::color::White, raw);
		if (renderDebugInformation)
		{
#if defined(A3DEBUG) || defined(A3INTERNAL)
			renderer.Push(v3{ 0.0f, 630.0f, 0.0f }, v2{ 270.0f, 100.0f }, a3::color::White, fontback);
#endif
		}
		renderer.EndFrame();

		v2 dim{ 120.0f, 25.0f };
		ui.BeginFrame(v2{ 1120.0f, 550.0f });

		if (ui.Button(1, dim, "Load Object"))
		{
		}
		if (ui.Button(2, dim, "Ray Trace"))
		{
		}
		if (ui.Button(3, dim, "Preview Image"))
		{
		}
		if (ui.Button(4, dim, "Button 4"))
		{
		}
		static b32 checked = false;
		if (ui.Checkbox(5, dim, checked, "Checkbox"))
		{
			checked = !checked;
		}
		ui.EndFrame();

		if (renderDebugInformation)
		{
#if defined(A3DEBUG) || defined(A3INTERNAL)
			utf8 buffer[256];
			_snprintf_s(buffer, 256, 256, "FPS: %d", (i32)(1.0f / deltaTime));
			fontRenderer.Render(buffer, v2{ 0.0f, 700.0f }, 20.0f, a3::color::GreenYellow);

			_snprintf_s(buffer, 256, 256, "Total Heap Allocations: %.2fMB", (f32)a3::Platform.GetTotalHeapAllocated() / (f32)a3MegaBytes(1));
			fontRenderer.Render(buffer, v2{ 0.0f, 685.0f }, 15.0f, a3::color::GreenYellow);
			_snprintf_s(buffer, 256, 256, "Total Heap Freed: %.2fMB", (f32)a3::Platform.GetTotalHeapFreed() / (f32)a3MegaBytes(1));
			fontRenderer.Render(buffer, v2{ 0.0f, 670.0f }, 15.0f, a3::color::GreenYellow);
			_snprintf_s(buffer, 256, 256, "Total App Memory Used: %.2fMB", (f32)a3::Platform.GetPersistantHeapAllocated() / (f32)a3MegaBytes(1));
			fontRenderer.Render(buffer, v2{ 0.0f, 655.0f }, 15.0f, a3::color::GreenYellow);
			_snprintf_s(buffer, 256, 256, "Total App Memory Freed: %.2fMB", (f32)a3::Platform.GetPersistantHeapFreed() / (f32)a3MegaBytes(1));
			fontRenderer.Render(buffer, v2{ 0.0f, 640.0f }, 15.0f, a3::color::GreenYellow);
#endif
		}

		SwapBuffers(windowDeviceContext);
	}

	return 0;
}