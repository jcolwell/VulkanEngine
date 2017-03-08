#ifndef ENGINE_GRAPHICS_WINDOW_H__
#define ENGINE_GRAPHICS_WINDOW_H__
//======================================================================================
// Filename: Window.h
// Description:
//======================================================================================

//======================================================================================
// INCLUDE
//======================================================================================
#include "Common.h"
#include "Platform.h"

#include <vector>

class Renderer;
//======================================================================================
// WINDOW CLASS
//======================================================================================

class Window
{
public:
	Window( Renderer* renderer, uint32_t windowWidth, uint32_t windowHeight, std::string name);
	~Window();

	void Terminate();
	bool Update();

	void BeginRender();
	void EndRender( std::vector<VkSemaphore> waitSemaphores );

	VkRenderPass	GetVulkanRenderPass() const				{ return mRenderPass; }
	VkFramebuffer	GetVulkanActiveFrameBuffer() const		{ return mFrameBuffers[mActiveSwapchainImageID]; }
	VkExtent2D		GetVulkanSurfaceSize() const			{ return{ mSurfaceWidth, mSurfaceHeight }; }

private:
	NONCOPYABLE(Window);

	void InitOSWindow();
	void TerminateOSWindow();

	void UpdateOSWindow();
	void InitOSSurface();
	
	void InitSurface();
	void TerminateSurface();

	void InitSwapchain();
	void TerminateSwapchain();

	void InitSwapchainImages();
	void TerminateSwapchainImages();

	void InitDepthStencilImage();
	void TerminateDepthStencilImage();

	void InitRenderPass();
	void TerminateRenderPass();

	void InitFrameBuffers();
	void TerminateFrameBuffers();

	void InitSynchronizations();
	void TerminateSynchronizations();

private:
	
	Renderer* mRenderer = nullptr;

	VkSurfaceKHR    mSurface = VK_NULL_HANDLE;
	VkSwapchainKHR  mSwapchain = VK_NULL_HANDLE;
	VkRenderPass	mRenderPass = VK_NULL_HANDLE;

	uint32_t mSurfaceWidth = 1280;
	uint32_t mSurfaceHeight = 720;
	std::string mWindowName;
	uint32_t mSwapchainImageCount = 2;
	uint32_t mActiveSwapchainImageID = UINT32_MAX;

	VkFence mSwapchainImageAvailable = VK_NULL_HANDLE;

	std::vector<VkImage> mSwapchainImages;
	std::vector<VkImageView> mSwapchainImageViews;
	std::vector<VkFramebuffer> mFrameBuffers;

	VkImage	mDepthStencilImg = VK_NULL_HANDLE;
	VkDeviceMemory mDepthStencilImgMemory = VK_NULL_HANDLE;
	VkImageView mDepthStencilImgView = VK_NULL_HANDLE;

	VkSurfaceFormatKHR mSurfaceFormat = {};
	VkSurfaceCapabilitiesKHR mSurfaceCapabilities = {};

	VkFormat mDepthStencilFormat = VK_FORMAT_UNDEFINED;
	bool mStencilAvailable = false;

	bool mIsRunning = true;

#if VK_USE_PLATFORM_WIN32_KHR
	HINSTANCE mWin32Instance = nullptr;
	HWND mWin32Window = nullptr;
	std::string mWin32ClassName;
	static uint64_t mWin32ClassIDCounter;
#endif

};


//======================================================================================
#endif // !ENGINE_GRAPHICS_WINDOW_H__
