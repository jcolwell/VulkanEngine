//======================================================================================
// Filename: BUILD_OPTIONS.h
// Description:
//======================================================================================

#include "BUILD_OPTIONS.h"
//======================================================================================
// Includes
//======================================================================================
#include "GraphicsCommon.h"

#include "Common.h"
//======================================================================================
// Functions
//======================================================================================

#if BUILD_ENABLE_VULKAN_RUNTIME_DEBUG

void vkErrorCheck(VkResult result)
{
	std::string msg;

	if (result < 0) {
		switch (result) {
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			msg = "VK_ERROR_OUT_OF_HOST_MEMORY";
			break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			msg = "VK_ERROR_OUT_OF_DEVICE_MEMORY";
			break;
		case VK_ERROR_INITIALIZATION_FAILED:
			msg = "VK_ERROR_INITIALIZATION_FAILED";
			break;
		case VK_ERROR_DEVICE_LOST:
			msg = "VK_ERROR_DEVICE_LOST";
			break;
		case VK_ERROR_MEMORY_MAP_FAILED:
			msg = "VK_ERROR_MEMORY_MAP_FAILED";
			break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			msg = "VK_ERROR_LAYER_NOT_PRESENT";
			break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			msg = "VK_ERROR_EXTENSION_NOT_PRESENT";
			break;
		case VK_ERROR_FEATURE_NOT_PRESENT:
			msg = "VK_ERROR_FEATURE_NOT_PRESENT";
			break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			msg = "VK_ERROR_INCOMPATIBLE_DRIVER";
			break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			msg = "VK_ERROR_TOO_MANY_OBJECTS";
			break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			msg = "VK_ERROR_FORMAT_NOT_SUPPORTED";
			break;
		case VK_ERROR_SURFACE_LOST_KHR:
			msg = "VK_ERROR_SURFACE_LOST_KHR";
			break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			msg = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
			break;
		case VK_SUBOPTIMAL_KHR:
			msg = "VK_SUBOPTIMAL_KHR";
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			msg = "VK_ERROR_OUT_OF_DATE_KHR";
			break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			msg = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
			break;
		case VK_ERROR_VALIDATION_FAILED_EXT:
			msg = "VK_ERROR_VALIDATION_FAILED_EXT";
			break;
		default:
			msg = "Unknown Vulkan Error";
			break;
		}

		LOG(msg.c_str());
	}
}

uint32_t FindMemoryTypeIndex(	const VkPhysicalDeviceMemoryProperties* gpuMemoryProperties,
								const VkMemoryRequirements* memoryRequirements,
								const VkMemoryPropertyFlags requriedProperty)
{
	for (uint32_t i = 0; i < gpuMemoryProperties->memoryTypeCount; ++i)
	{
		if (memoryRequirements->memoryTypeBits & (1 << i))
		{
			if ((gpuMemoryProperties->memoryTypes[i].propertyFlags & requriedProperty) == requriedProperty )
			{
				return i;
			}
		}
	}
	ASSERT(false, [Gaphics] "Couldn't find proper memory type");
	return UINT32_MAX;
}

#else

void vkErrorCheck(VkResult) {}





#endif //BUILD_ENABLED_VULKAN_RUNTIME_DEBUG
