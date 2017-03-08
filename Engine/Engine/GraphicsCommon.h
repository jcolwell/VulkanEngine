#ifndef INCLUDE_GRAPHICS_COMMON_H__
#define INCLUDE_GRAPHICS_COMMON_H__
//======================================================================================
// Filename: BUILD_OPTIONS.h
// Description:
//======================================================================================

//======================================================================================
// Includes
//======================================================================================
#include "Common.h"

#include <assert.h>
#include <vulkan\vulkan.hpp>

//======================================================================================
// Functions
//======================================================================================

void vkErrorCheck(VkResult result);

uint32_t FindMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties* gpuMemoryProperties,
								const VkMemoryRequirements* memoryRequirements,
								const VkMemoryPropertyFlags memoryProperties);

#endif // INCLUDE_GRAPHICS_COMMON_H__