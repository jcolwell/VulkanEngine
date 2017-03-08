//======================================================================================
// Filename: Renderer.cpp
// Description:
//======================================================================================

#include "BUILD_OPTIONS.h"
//======================================================================================
// Includes
//======================================================================================
#include "GraphicsCommon.h"
#include "Renderer.h"
#include "Window.h"

#include <memory>
#include <sstream>
//======================================================================================

Renderer::Renderer() 
{
	SetupLayersAndExtensions();
	SetupDebug();
	InitVulkanInstance();
	InitDebug();
	InitVulkanDevice();
	InitVulkanCommandPool();
}

//--------------------------------------------------------------------------------------

Renderer::~Renderer() 
{
	SAVE_DELETE(mWindow);
	
	TerminateVulkanCommandPool();
	TerminateVulkanPhysicalDevice();
	TerminateDebug();
	TerminateVulkanInstance();
}

//--------------------------------------------------------------------------------------

void Renderer::InitializeWindow(const std::string& appName, int width, int height)
{
	VERIFY(mWindow == nullptr, "[Renderer] Failed to Initialize because a Window already exists!");
	mWindow = new Window( this, width, height, appName);
}

//--------------------------------------------------------------------------------------

bool Renderer::Run()
{
	if (nullptr != mWindow) 
	{
		return mWindow->Update();
	}
	return true;
}

//--------------------------------------------------------------------------------------

void Renderer::SetupLayersAndExtensions()
{
	mInstanceExtensions.push_back( VK_KHR_SURFACE_EXTENSION_NAME   );
	mInstanceExtensions.push_back( PLATFORM_SURFACE_EXTENSION_NAME );
	
	mDeviceExtensions.push_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME );
}

//--------------------------------------------------------------------------------------

void Renderer::InitVulkanInstance()
{

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 39);
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.pApplicationName = "Vulkan Test";

	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = static_cast<uint32_t>( mInstanceLayers.size() );
	instanceInfo.ppEnabledLayerNames = mInstanceLayers.data();
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(mInstanceExtensions.size());
	instanceInfo.ppEnabledExtensionNames = mInstanceExtensions.data();
	instanceInfo.pNext = &mDebugCallbackCreateInfo;

	vkErrorCheck( vkCreateInstance( &instanceInfo, nullptr, &mInstance ) );
}

//--------------------------------------------------------------------------------------

void Renderer::TerminateVulkanInstance()
{
	vkDestroyInstance(mInstance, nullptr);
	mInstance = nullptr;
}

//--------------------------------------------------------------------------------------

void Renderer::InitVulkanDevice()
{
	FindPhysicalDevice();

	//Find Compatibile extensions and layers
	//{
	//	uint32_t layerCount = 0;
	//	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	//	std::vector< VkLayerProperties> layerPropertiesList(layerCount);
	//	vkEnumerateInstanceLayerProperties(&layerCount, layerPropertiesList.data());
	//
	//	uint32_t extensionCount = 0;
	//	vkEnumerateDeviceLayerProperties(mPhysicalDevice, &extensionCount, nullptr);
	//	std::vector< VkLayerProperties > extensionPropertiesList(extensionCount);
	//	vkEnumerateDeviceLayerProperties(mPhysicalDevice, &extensionCount, extensionPropertiesList.data());
	//}

	float queuePriorities[] = { 1.0f };
	mGraphicsFamilyIndex = FindGraphicsFamilyIndex();

	VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.queueFamilyIndex = mGraphicsFamilyIndex;
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	deviceInfo.enabledExtensionCount = static_cast<uint32_t>( mDeviceExtensions.size() );
	deviceInfo.ppEnabledExtensionNames = mDeviceExtensions.data();

	vkErrorCheck( vkCreateDevice(mPhysicalDevice, &deviceInfo, nullptr, &mDevice) );
	
	vkGetDeviceQueue(mDevice, mGraphicsFamilyIndex, 0, &mQueue);
}

//--------------------------------------------------------------------------------------

void Renderer::TerminateVulkanPhysicalDevice()
{
	vkDestroyDevice(mDevice, nullptr);
	mDevice = nullptr;
	mPhysicalDevice = nullptr;
	mPhysicalDeviceProperties = {};
}

//--------------------------------------------------------------------------------------

void Renderer::InitVulkanCommandPool()
{

	VkCommandPoolCreateInfo vkCreateCmdPoolInfo {};
	vkCreateCmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	vkCreateCmdPoolInfo.queueFamilyIndex = mGraphicsFamilyIndex;
	vkCreateCmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	vkErrorCheck( vkCreateCommandPool( mDevice, &vkCreateCmdPoolInfo, nullptr, &mCmdPool ));

	VkCommandBufferAllocateInfo bufferAllocateInfo = {};
	bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	bufferAllocateInfo.commandPool = mCmdPool;
	bufferAllocateInfo.commandBufferCount = 1;
	bufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	vkErrorCheck( vkAllocateCommandBuffers( mDevice, &bufferAllocateInfo, &mCmdBuffer ) );
}

//--------------------------------------------------------------------------------------

void Renderer::TerminateVulkanCommandPool()
{
	vkDestroyCommandPool(mDevice, mCmdPool, nullptr);
}

//--------------------------------------------------------------------------------------

void Renderer::FindPhysicalDevice()
{
	uint32_t gpuCount = 0;
	vkEnumeratePhysicalDevices(mInstance, &gpuCount, nullptr);
	std::vector<VkPhysicalDevice> gpuList(gpuCount);
	vkEnumeratePhysicalDevices(mInstance, &gpuCount, gpuList.data());

	mPhysicalDevice = gpuList[0];
	vkGetPhysicalDeviceProperties(mPhysicalDevice, &mPhysicalDeviceProperties);
	vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mPhysicalDeviceMemoryProperties);
}

//--------------------------------------------------------------------------------------

uint32_t Renderer::FindGraphicsFamilyIndex()
{
	uint32_t graphicsFamilyIndex = 0;

	uint32_t familyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &familyCount, nullptr);
	std::vector< VkQueueFamilyProperties > familyPropertiesList(familyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &familyCount, familyPropertiesList.data());

	bool isFound = false;
	for (uint32_t i = 0; i < familyCount; ++i)
	{
		if (familyPropertiesList[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			isFound = true;
			graphicsFamilyIndex = i;
			break;
		}
	}

	if (!isFound)
	{
		LOG( "Queue Family supporting Graphics not found" );
	}

	return graphicsFamilyIndex;
}

//--------------------------------------------------------------------------------------

#if BUILD_ENABLE_VULKAN_DEBUG

//--------------------------------------------------------------------------------------

VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanDebugCallBack(
	VkDebugReportFlagsEXT		flags,
	VkDebugReportObjectTypeEXT	objType,
	uint64_t					srcObj,
	size_t						locaiton,
	int32_t						msgCode,
	const char*					layerPrefix,
	const char*					msg,
	void*						userData
	)
{
	std::ostringstream stream;
	stream << "VKDBG: ";
	if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
	{
		stream << "INFO: ";
	}
	if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
	{
		stream << "WARNING: ";
	}
	if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
	{
		stream << "PERFORMANCE: ";
	}
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		stream << "ERROR: ";
	}
	if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
	{
		stream << "DEBUG: ";
	}
	stream << "@[" << layerPrefix << "]: " << msg << std::endl;
	LOG( stream.str().c_str() );

#ifdef _WIN32
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		MessageBox( nullptr, stream.str().c_str(), "Vulkan Error!", 0);
	}
#endif // _Win32

	return false;
}

//--------------------------------------------------------------------------------------

void Renderer::SetupDebug()
{
	mDebugCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	mDebugCallbackCreateInfo.pfnCallback = VulkanDebugCallBack;
	mDebugCallbackCreateInfo.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT
		| VK_DEBUG_REPORT_WARNING_BIT_EXT
		| VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
		| VK_DEBUG_REPORT_ERROR_BIT_EXT
		| VK_DEBUG_REPORT_DEBUG_BIT_EXT
		| 0;

	mInstanceLayers.push_back("VK_LAYER_LUNARG_core_validation");
	mInstanceExtensions.push_back("VK_EXT_debug_report");
}

//--------------------------------------------------------------------------------------

PFN_vkCreateDebugReportCallbackEXT fvkCreateDebugReportCallbackEXT = nullptr;
PFN_vkDestroyDebugReportCallbackEXT fvkDestroyDebugReportCallbackEXT = nullptr;

void Renderer::InitDebug()
{
	fvkCreateDebugReportCallbackEXT 
		= (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(mInstance, "vkCreateDebugReportCallbackEXT");
	fvkDestroyDebugReportCallbackEXT 
		= (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(mInstance, "vkDestroyDebugReportCallbackEXT");
	if (nullptr == fvkCreateDebugReportCallbackEXT || nullptr == fvkDestroyDebugReportCallbackEXT)
	{
		ASSERT(false, "Unable to setup Debug Callback!");
	}
	
	fvkCreateDebugReportCallbackEXT(mInstance, &mDebugCallbackCreateInfo, nullptr, &mDebugReport);
}

//--------------------------------------------------------------------------------------

void Renderer::TerminateDebug()
{
	fvkDestroyDebugReportCallbackEXT(mInstance, mDebugReport, nullptr);
	mDebugReport = {};
}

//--------------------------------------------------------------------------------------

#else 

//--------------------------------------------------------------------------------------

void Renderer::SetupDebug() {}

//--------------------------------------------------------------------------------------

void Renderer::InitDebug() {}

//--------------------------------------------------------------------------------------

void Renderer::TerminateDebug() {}

#endif //BUILD_ENABLE_VULKAN_DEBUG

//======================================================================================