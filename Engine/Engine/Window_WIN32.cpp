//======================================================================================
// Filename: Window_WIN32.cpp
// Description:
//======================================================================================

#include "BUILD_OPTIONS.h"
#include "Platform.h"
//======================================================================================
// INCLUDE
//======================================================================================
#include "Window.h"

#include "Common.h"
#include "Renderer.h"

#if VK_USE_PLATFORM_WIN32_KHR
//======================================================================================
// WINDOW_WIN32 CLASS
//======================================================================================
LRESULT CALLBACK WindowsEventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window* window = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

	switch (uMsg)
	{
		case WM_CLOSE:
			window->Terminate();
			return 0;
		case WM_SIZE:
			//Window has been resized by hand
			break;
		default:
			break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//--------------------------------------------------------------------------------------

uint64_t Window::mWin32ClassIDCounter = 0;

//--------------------------------------------------------------------------------------

void Window::InitOSWindow() 
{
	WNDCLASSEX winClass{};
	ASSERT(mSurfaceWidth >= 0, "[Window] Window width can't be less than 0!");
	ASSERT(mSurfaceHeight >= 0, "[Window] Window height can't be less than 0!");

	mWin32Instance = GetModuleHandle(nullptr);
	mWin32ClassName = mWindowName + "_" + std::to_string(mWin32ClassIDCounter);
	++mWin32ClassIDCounter;

	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = WindowsEventHandler;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = mWin32Instance; // hInstance
	winClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winClass.lpszMenuName = NULL;
	winClass.lpszClassName = mWin32ClassName.c_str();
	winClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	// Register window class:
	if (!RegisterClassEx(&winClass)) 
	{
		// It didn't work, so try to give a useful error:
		ASSERT(false, "[Window] Failed to create a window!\n");
		fflush(stdout);
		std::exit(-1);
	}

	DWORD ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	// Create window with the registered class:
	RECT wr = { 0, 0, LONG(mSurfaceWidth), LONG(mSurfaceHeight) };
	AdjustWindowRectEx(&wr, style, FALSE, ex_style);
	mWin32Window = CreateWindowEx(0,
		mWin32ClassName.c_str(),		// class name
		mWindowName.c_str(),			// app name
		style,							// window style
		CW_USEDEFAULT, CW_USEDEFAULT,	// x/y coords
		wr.right - wr.left,				// width
		wr.bottom - wr.top,				// height
		NULL,							// handle to parent
		NULL,							// handle to menu
		mWin32Instance,					// hInstance
		NULL);							// no extra parameters
	if (!mWin32Window) 
	{
		ASSERT(false, "[Window] Failed to create a window!\n");
		fflush(stdout);
		std::exit(-1);
	}
	SetWindowLongPtr(mWin32Window, GWLP_USERDATA, (LONG_PTR)this);

	ShowWindow(mWin32Window, SW_SHOW);
	SetForegroundWindow(mWin32Window);
	SetFocus(mWin32Window);
}

//--------------------------------------------------------------------------------------

void Window::TerminateOSWindow() 
{
	DestroyWindow(mWin32Window);
	UnregisterClass(mWin32ClassName.c_str(), mWin32Instance);
}

//--------------------------------------------------------------------------------------

void Window::UpdateOSWindow() 
{
	MSG msg;
	if (PeekMessage(&msg, mWin32Window, 0, 0, PM_REMOVE)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

//--------------------------------------------------------------------------------------

void Window::InitOSSurface()
{
	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hinstance = mWin32Instance;
	createInfo.hwnd = mWin32Window;

	vkCreateWin32SurfaceKHR(mRenderer->GetVulkanInstance(), &createInfo, nullptr, &mSurface);
}

//--------------------------------------------------------------------------------------

//======================================================================================
#endif //!VK_USE_PLATFORM_WIN32_KHR