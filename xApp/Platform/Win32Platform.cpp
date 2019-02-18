#include "Common/Core.h"
#include "Platform.h"
#include "GL/LoadOpenGL.h"

#include "GL/GLDebug.h"
#include "GlRenderer.h"
#include "Platform/AssetManager.h"

#include "Math/Math.h"
#include "Math/Color.h"
#include "UIContext.h"

#include "GLResources.h"
#include "Utility/Resource.h"

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
#define a3Main() CALLBACK wWinMain(HINSTANCE, HINSTANCE, LPWSTR, i32)
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

void operator delete(void* ptr)
{
	a3::Platform.Free(ptr);
}

void operator delete[](void* ptr)
{
	a3::Platform.Free(ptr);
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
		// TODO(Zero): Y coordinate for mouse position is inversed
		userData.inputSystem.MouseY = (f32)(wh - my) / (f32)wh;
	};
    
	switch (msg)
	{
        case WM_CREATE:
        {
            GLLoad(hWnd);
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
            userData.inputSystem.Buttons[a3::ButtonLeft] = a3::ButtonDown;
            break;
        }
        
        case WM_LBUTTONUP:
        {
            l_StoreInputData(lParam);
            userData.inputSystem.Buttons[a3::ButtonLeft] = a3::ButtonUp;
            break;
        }
        
        case WM_RBUTTONDOWN:
        {
            l_StoreInputData(lParam);
            userData.inputSystem.Buttons[a3::ButtonRight] = a3::ButtonDown;
            break;
        }
        
        case WM_RBUTTONUP:
        {
            l_StoreInputData(lParam);
            userData.inputSystem.Buttons[a3::ButtonRight] = a3::ButtonUp;
            break;
        }
        
        case WM_MBUTTONDOWN:
        {
            l_StoreInputData(lParam);
            userData.inputSystem.Buttons[a3::ButtonMiddle] = a3::ButtonDown;
            break;
        }
        
        case WM_MBUTTONUP:
        {
            l_StoreInputData(lParam);
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
            a3Log("Window resized to {i} X {i}", LOWORD(lParam), HIWORD(lParam));
            userData.inputSystem.WindowWidth = LOWORD(lParam);
            userData.inputSystem.WindowHeight = HIWORD(lParam);
            HDC hDC = GetDC(hWnd);
            SwapBuffers(hDC);
            ReleaseDC(hWnd, hDC);
            return 0;
        }
        
        case WM_SETCURSOR:
		SetCursor(LoadCursorW(A3NULL, IDC_ARROW));
		return 0;
        
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
    
	HRESULT hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    
	if (FAILED(hr))
	{
		a3LogError("Could not initialize COM objects!");
		return -1;
	}
	
	a3::font_renderer fontRenderer = a3::Renderer.CreateFontRenderer(a3::shaders::GLFontVertex, a3::shaders::GLFontFragment);
	fontRenderer.SetRegion(0.0f, 800.0f, 0.0f, 600.0f);
	a3::Asset.LoadFontTextureAtlasFromFile(a3::asset_id::DebugFont, "Resources/HackRegular.ttf", 50.0f);
	fontRenderer.SetFont(a3::Asset.Get<a3::font_texture>(a3::asset_id::DebugFont));
    
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	a3Log("Window displayed.");
    
	win32_user_data* userData = Win32GetUserData();
	userData->windowHandle = hWnd;
    
	a3::input_info oldInput = {};
	LARGE_INTEGER performanceFrequency;
	a3Assert(QueryPerformanceFrequency(&performanceFrequency));
	LARGE_INTEGER performanceCounter;
	QueryPerformanceCounter(&performanceCounter);
    
	b32 renderDebugInformation = true;
    
	f32 deltaTime = 0.0f;

	utf8* path = a3::Platform.SaveFromDialogue("save as", a3::FileTypePNG);

	a3::Platform.MessageBox("Path", path, a3::MessageBoxTypeOk, a3::MessageBoxIconNone);

	a3::Platform.FreeDialogueData(path);

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
        
                
		LARGE_INTEGER currentPerformanceCounter;
		a3Assert(QueryPerformanceCounter(&currentPerformanceCounter));
		deltaTime = (f32)(currentPerformanceCounter.QuadPart - performanceCounter.QuadPart) / (f32)performanceFrequency.QuadPart;
		performanceCounter = currentPerformanceCounter;
        
		if (renderDebugInformation)
		{
#if defined(A3DEBUG) || defined(A3INTERNAL)
			utf8 buffer[256];
			_snprintf_s(buffer, 256, 256, "FPS: %d", (i32)(1.0f / deltaTime));
			fontRenderer.Render(buffer, v2{ 0.0f, 580.0f }, 20.0f, a3::color::GreenYellow);
			
			_snprintf_s(buffer, 256, 256, "Total Heap Allocations: %.2fKB", (f32)a3::Platform.GetTotalHeapAllocated() / (1024.0f));
			fontRenderer.Render(buffer, v2{ 0.0f, 565.0f }, 15.0f, a3::color::GreenYellow);
			_snprintf_s(buffer, 256, 256, "Total Heap Freed: %.2fKB", (f32)a3::Platform.GetTotalHeapFreed() / (1024.0f));
			fontRenderer.Render(buffer, v2{ 0.0f, 550.0f }, 15.0f, a3::color::GreenYellow);
			_snprintf_s(buffer, 256, 256, "Total App Memory Used: %.2fKB", (f32)a3::Platform.GetPersistantHeapAllocated() / (1024.0f));
			fontRenderer.Render(buffer, v2{ 0.0f, 535.0f }, 15.0f, a3::color::GreenYellow);
			_snprintf_s(buffer, 256, 256, "Total App Memory Freed: %.2fKB", (f32)a3::Platform.GetPersistantHeapFreed() / (1024.0f));
			fontRenderer.Render(buffer, v2{ 0.0f, 520.0f }, 15.0f, a3::color::GreenYellow);
#endif
		}
        
		SwapBuffers(hDC);
	}
    
	return 0;
}