#ifndef ENGINE_RENDERER_H__
#define	ENGINE_RENDERER_H__

//======================================================================================
// Filename: Renderer.h
// Description:
//======================================================================================

//======================================================================================
// Includes
//======================================================================================
#include "Platform.h"

#include <vector>
#include <string>

class Window;
//======================================================================================

//======================================================================================
// Class Application
//======================================================================================

class Renderer
{
public:
	Renderer();
    ~Renderer();

	void InitializeWindow( const std::string& appName, int width, int height);
	
	bool Run();

	Window* GetWindow()						{ return mWindow; }

	const VkInstance						GetVulkanInstance() const							{ return mInstance; }
	const VkPhysicalDevice					GetVulkanPhysicalDevice() const						{ return mPhysicalDevice; }
	const VkDevice							GetVulkanDevice() const								{ return mDevice; }
	const VkQueue							GetVulkanQueue() const								{ return mQueue; }
	const uint32_t							GetVulkanGraphicsQueueFamily() const				{ return mGraphicsFamilyIndex; }
	const VkPhysicalDeviceProperties&		GetVulkanPhysicalDeviceProperties() const			{ return mPhysicalDeviceProperties; }
	const VkPhysicalDeviceMemoryProperties& GetVulkanPhysicalDeviceMemoryProperties() const		{ return mPhysicalDeviceMemoryProperties; }

private:
	NONCOPYABLE(Renderer);

	void SetupLayersAndExtensions();

	void InitVulkanInstance();
	void TerminateVulkanInstance();

	void InitVulkanDevice();
	void TerminateVulkanPhysicalDevice();

	void InitVulkanCommandPool();
	void TerminateVulkanCommandPool();

	void SetupDebug();
	void InitDebug();
	void TerminateDebug();

	void FindPhysicalDevice();
	uint32_t FindGraphicsFamilyIndex();


private:
	Window* mWindow = nullptr;

	int mSurfaceWidth = 0;
	int mSurfaceHeight = 0;

	VkInstance mInstance = VK_NULL_HANDLE;
	
	VkDevice mDevice = VK_NULL_HANDLE;
	VkPhysicalDevice  mPhysicalDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties mPhysicalDeviceProperties = {};
	VkPhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties = {};

	VkCommandPool mCmdPool = VK_NULL_HANDLE;
	VkCommandBuffer mCmdBuffer = VK_NULL_HANDLE;

	std::vector<const char*> mInstanceLayers;
	std::vector<const char*> mInstanceExtensions;
	
	std::vector<const char*> mDeviceExtensions;



	VkDebugReportCallbackEXT mDebugReport = VK_NULL_HANDLE;
	VkDebugReportCallbackCreateInfoEXT mDebugCallbackCreateInfo = {};

	uint32_t mGraphicsFamilyIndex = 0;
	VkQueue mQueue;
};

//======================================================================================
#endif //ENGINE_RENDERER_H__