//======================================================================================
// Filename: Window.cpp
// Description:
//======================================================================================

//======================================================================================
// INCLUDE
//======================================================================================
#include "Window.h"

#include "Common.h"
#include "GraphicsCommon.h"
#include "Renderer.h"

#include <array>

//======================================================================================
// WINDOW CLASS
//======================================================================================
Window::Window(Renderer* renderer, uint32_t windowWidth, uint32_t windowHeight, std::string name)
	: mRenderer( renderer )
	, mSurfaceWidth( windowWidth)
	, mSurfaceHeight( windowHeight )
	, mWindowName( name )
{
	InitOSWindow();
	InitSurface();
	InitSwapchain();
	InitSwapchainImages();
	InitDepthStencilImage();
	InitRenderPass();
	InitFrameBuffers();
	InitSynchronizations();
}

//--------------------------------------------------------------------------------------

Window::~Window() 
{
	TerminateSynchronizations();
	TerminateFrameBuffers();
	TerminateRenderPass();
	TerminateDepthStencilImage();
	TerminateSwapchainImages();
	TerminateSwapchain();
	TerminateSurface();
	TerminateOSWindow();
}

//--------------------------------------------------------------------------------------

void Window::Terminate() 
{
	mIsRunning = false;
}

//--------------------------------------------------------------------------------------

bool Window::Update() 
{
	UpdateOSWindow();
	return mIsRunning;
}

//--------------------------------------------------------------------------------------

void Window::BeginRender()
{
	vkErrorCheck( vkAcquireNextImageKHR(
						mRenderer->GetVulkanDevice(), 
						mSwapchain, 
						U64_MAX,
						VK_NULL_HANDLE,
						mSwapchainImageAvailable, 
						&mActiveSwapchainImageID) );
	vkErrorCheck( vkWaitForFences(	mRenderer->GetVulkanDevice(), 1, 
									&mSwapchainImageAvailable, VK_TRUE, UINT64_MAX));
	vkErrorCheck( vkResetFences(mRenderer->GetVulkanDevice(), 
								1, &mSwapchainImageAvailable));
	vkErrorCheck( vkQueueWaitIdle(mRenderer->GetVulkanQueue()) );
}

//--------------------------------------------------------------------------------------

void Window::EndRender(std::vector<VkSemaphore> waitSemaphores)
{
	VkResult presentResult = VkResult::VK_RESULT_MAX_ENUM;

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount	= static_cast<uint32_t>(waitSemaphores.size());
	presentInfo.pWaitSemaphores		= waitSemaphores.data();
	presentInfo.swapchainCount		= 1;
	presentInfo.pSwapchains			= &mSwapchain;
	presentInfo.pImageIndices		= &mActiveSwapchainImageID;
	presentInfo.pResults			= &presentResult;

	vkErrorCheck(vkQueuePresentKHR(mRenderer->GetVulkanQueue(), &presentInfo));
	vkErrorCheck(presentResult);
}

//--------------------------------------------------------------------------------------

void Window::InitSurface() 
{
	InitOSSurface();

	auto gpu = mRenderer->GetVulkanPhysicalDevice();

	VkBool32 WSISupported = false;
	vkErrorCheck( vkGetPhysicalDeviceSurfaceSupportKHR( gpu,
										  mRenderer->GetVulkanGraphicsQueueFamily(),
										  mSurface, &WSISupported) );

	if (!WSISupported)
	{
		ASSERT(false, "[Window] WSI Not Supported");
		std::exit(-1);
	}

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( gpu, mSurface, &mSurfaceCapabilities);
	if (mSurfaceCapabilities.currentExtent.width < UINT32_MAX)
	{
		mSurfaceWidth = mSurfaceCapabilities.currentExtent.width;
		mSurfaceHeight = mSurfaceCapabilities.currentExtent.height;
	}

	{
		VkBool32 format_count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, mSurface, &format_count, nullptr);
		if (format_count == 0)
		{
			ASSERT(false, "[Window] Surface formats missing!");
			std::exit(-1);
		}
		std::vector<VkSurfaceFormatKHR> formats(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, mSurface, &format_count, formats.data() );
		if (formats[0].format == VK_FORMAT_UNDEFINED)
		{
			mSurfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
			mSurfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		}
		else
		{
			mSurfaceFormat = formats[0];
		}

	}
}

//--------------------------------------------------------------------------------------

void Window::TerminateSurface() 
{
	vkDestroySurfaceKHR(mRenderer->GetVulkanInstance(), mSurface, nullptr);
}

//--------------------------------------------------------------------------------------

void Window::InitSwapchain()
{
	
	if (mSwapchainImageCount < mSurfaceCapabilities.minImageCount + 1)
	{
		mSwapchainImageCount = mSurfaceCapabilities.minImageCount + 1;
	}
	if (mSurfaceCapabilities.maxImageCount > 0 
				&& mSwapchainImageCount > mSurfaceCapabilities.maxImageCount)
	{
		mSwapchainImageCount = mSurfaceCapabilities.maxImageCount;
	}

	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	{
		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(  mRenderer->GetVulkanPhysicalDevice(),
													mSurface, &presentModeCount, nullptr);
		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		vkErrorCheck( vkGetPhysicalDeviceSurfacePresentModesKHR(  mRenderer->GetVulkanPhysicalDevice(),
													mSurface, &presentModeCount, presentModes.data()) );
		//Use Mailbox Present Mode if it exists
		for (auto m : presentModes)
		{
			if (m == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				presentMode = m;
				break;
			}
		}
	}
	
	VkSwapchainCreateInfoKHR swapchainInfo = {};
	swapchainInfo.sType						= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainInfo.surface					= mSurface;
	swapchainInfo.minImageCount				= mSwapchainImageCount;
	swapchainInfo.imageFormat				= mSurfaceFormat.format;
	swapchainInfo.imageExtent.width			= mSurfaceWidth;
	swapchainInfo.imageExtent.height		= mSurfaceHeight;
	swapchainInfo.imageColorSpace			= mSurfaceFormat.colorSpace;
	swapchainInfo.imageArrayLayers			= 1;
	swapchainInfo.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainInfo.imageSharingMode			= VK_SHARING_MODE_EXCLUSIVE;
	swapchainInfo.queueFamilyIndexCount		= 0;
	swapchainInfo.pQueueFamilyIndices		= nullptr;
	swapchainInfo.preTransform				= VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchainInfo.compositeAlpha			= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainInfo.presentMode				= presentMode;
	swapchainInfo.clipped					= VK_TRUE;
	swapchainInfo.oldSwapchain				= VK_NULL_HANDLE; //Useful for resizing

	vkErrorCheck( vkCreateSwapchainKHR( mRenderer->GetVulkanDevice(), 
										&swapchainInfo, nullptr, &mSwapchain) );
	vkErrorCheck( vkGetSwapchainImagesKHR( mRenderer->GetVulkanDevice(), 
										   mSwapchain, &mSwapchainImageCount, nullptr) );
}

//--------------------------------------------------------------------------------------

void Window::TerminateSwapchain()

{
	vkDestroySwapchainKHR(mRenderer->GetVulkanDevice(), mSwapchain, nullptr);
}

//--------------------------------------------------------------------------------------

void Window::InitSwapchainImages() 
{
	mSwapchainImages.resize(mSwapchainImageCount);
	mSwapchainImageViews.resize(mSwapchainImageCount);

	vkErrorCheck(vkGetSwapchainImagesKHR( mRenderer->GetVulkanDevice(), mSwapchain,
											&mSwapchainImageCount, mSwapchainImages.data() ));
	for (uint32_t i = 0; i < mSwapchainImageCount; ++i)
	{
		VkImageViewCreateInfo imgViewInfo = {};
		imgViewInfo.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imgViewInfo.image							= mSwapchainImages[i];
		imgViewInfo.viewType						= VK_IMAGE_VIEW_TYPE_2D;
		imgViewInfo.format							= mSurfaceFormat.format;
		imgViewInfo.components.r					= VK_COMPONENT_SWIZZLE_IDENTITY;
		imgViewInfo.components.g					= VK_COMPONENT_SWIZZLE_IDENTITY;
		imgViewInfo.components.b					= VK_COMPONENT_SWIZZLE_IDENTITY;
		imgViewInfo.components.a					= VK_COMPONENT_SWIZZLE_IDENTITY;
		imgViewInfo.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		imgViewInfo.subresourceRange.baseMipLevel	= 0;
		imgViewInfo.subresourceRange.levelCount		= 1;
		imgViewInfo.subresourceRange.baseArrayLayer = 0;
		imgViewInfo.subresourceRange.layerCount		= 1;
		
		vkErrorCheck(vkCreateImageView( mRenderer->GetVulkanDevice(), 
										&imgViewInfo, nullptr, &mSwapchainImageViews[i] ));
	}
}

//--------------------------------------------------------------------------------------

void Window::TerminateSwapchainImages()
{
	for (auto view : mSwapchainImageViews)
	{
		vkDestroyImageView(mRenderer->GetVulkanDevice(), view, nullptr);
	}
}

//--------------------------------------------------------------------------------------

void Window::InitDepthStencilImage()
{
	{
		std::vector<VkFormat> formats
		{ 
			VK_FORMAT_D32_SFLOAT_S8_UINT	,
			VK_FORMAT_D24_UNORM_S8_UINT		, 
			VK_FORMAT_D16_UNORM_S8_UINT		,
			VK_FORMAT_D32_SFLOAT			,
			VK_FORMAT_D16_UNORM 
		};

		for (auto f : formats)
		{
			VkFormatProperties formatProperties = {};
			vkGetPhysicalDeviceFormatProperties(mRenderer->GetVulkanPhysicalDevice(),
												f, &formatProperties);

			if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				mDepthStencilFormat = f;
				break;
			}
		}

		if (mDepthStencilFormat == VK_FORMAT_UNDEFINED)
		{
			ASSERT(false, "[Window] Depth stencil format not selected");
			std::exit(-1);
		}

		if (	 (mDepthStencilFormat == VK_FORMAT_D32_SFLOAT_S8_UINT) || 
				 (mDepthStencilFormat == VK_FORMAT_D24_UNORM_S8_UINT)  || 
			     (mDepthStencilFormat == VK_FORMAT_D16_UNORM_S8_UINT)  || 
				 (mDepthStencilFormat == VK_FORMAT_S8_UINT))
		{
			mStencilAvailable = true;
		}
	}

	VkImageCreateInfo imgCreateInfo = {};
	imgCreateInfo.sType					= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imgCreateInfo.flags					= 0;
	imgCreateInfo.imageType				= VK_IMAGE_TYPE_2D;
	imgCreateInfo.format				= mDepthStencilFormat;
	imgCreateInfo.extent.width			= mSurfaceWidth;
	imgCreateInfo.extent.height			= mSurfaceHeight;
	imgCreateInfo.extent.depth			= 1;
	imgCreateInfo.mipLevels				= 1;
	imgCreateInfo.arrayLayers			= 1;
	imgCreateInfo.samples				= VK_SAMPLE_COUNT_1_BIT;
	imgCreateInfo.tiling				= VK_IMAGE_TILING_OPTIMAL;
	imgCreateInfo.usage					= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imgCreateInfo.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
	imgCreateInfo.queueFamilyIndexCount	= VK_QUEUE_FAMILY_IGNORED;
	imgCreateInfo.pQueueFamilyIndices	= nullptr;
	imgCreateInfo.initialLayout			= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	vkCreateImage( mRenderer->GetVulkanDevice(), &imgCreateInfo, nullptr, &mDepthStencilImg);
 
	VkMemoryRequirements imgMemoryRequirements = {};
	vkGetImageMemoryRequirements(mRenderer->GetVulkanDevice(), mDepthStencilImg, &imgMemoryRequirements);

	
	uint32_t memoryIndex = FindMemoryTypeIndex(	&mRenderer->GetVulkanPhysicalDeviceMemoryProperties(),
												&imgMemoryRequirements, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);
	
	VkMemoryAllocateInfo memoryAllocationInfo = {};
	memoryAllocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocationInfo.allocationSize = imgMemoryRequirements.size;
	memoryAllocationInfo.memoryTypeIndex = memoryIndex;

	vkAllocateMemory(	mRenderer->GetVulkanDevice(),
						&memoryAllocationInfo, nullptr, &mDepthStencilImgMemory );
	
	vkBindImageMemory(	mRenderer->GetVulkanDevice(),
						mDepthStencilImg, mDepthStencilImgMemory, 0);

	VkImageViewCreateInfo imgViewCreateInfo = {};
	imgViewCreateInfo.sType			= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imgViewCreateInfo.image			= mDepthStencilImg;
	imgViewCreateInfo.viewType		= VK_IMAGE_VIEW_TYPE_2D;
	imgViewCreateInfo.format		= mDepthStencilFormat;
	imgViewCreateInfo.components.r	= VK_COMPONENT_SWIZZLE_IDENTITY;
	imgViewCreateInfo.components.b	= VK_COMPONENT_SWIZZLE_IDENTITY;
	imgViewCreateInfo.components.g	= VK_COMPONENT_SWIZZLE_IDENTITY;
	imgViewCreateInfo.components.a	= VK_COMPONENT_SWIZZLE_IDENTITY;
	imgViewCreateInfo.subresourceRange.aspectMask	= VK_IMAGE_ASPECT_STENCIL_BIT
														| (mStencilAvailable ? VK_IMAGE_ASPECT_STENCIL_BIT : 0);
	imgViewCreateInfo.subresourceRange.baseArrayLayer	= 0;
	imgViewCreateInfo.subresourceRange.levelCount		= 1;
	imgViewCreateInfo.subresourceRange.baseArrayLayer	= 0;
	imgViewCreateInfo.subresourceRange.layerCount		= 1;

	vkErrorCheck( vkCreateImageView(	mRenderer->GetVulkanDevice(), 
										&imgViewCreateInfo, nullptr, &mDepthStencilImgView));
}

//--------------------------------------------------------------------------------------

void Window::TerminateDepthStencilImage()
{
	vkDestroyImageView(mRenderer->GetVulkanDevice(), mDepthStencilImgView, nullptr);
	vkFreeMemory(mRenderer->GetVulkanDevice(), mDepthStencilImgMemory, nullptr);
	vkDestroyImage(mRenderer->GetVulkanDevice(), mDepthStencilImg, nullptr);
}

//--------------------------------------------------------------------------------------

void Window::InitRenderPass()
{
	std::array<VkAttachmentDescription, 2> attachments = {};
	attachments[0].format			= mDepthStencilFormat;
	attachments[0].samples			= VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout	= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachments[0].finalLayout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	
	attachments[1].flags			= 0;
	attachments[1].format			= mSurfaceFormat.format;
	attachments[1].samples			= VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference subpass_0_depthStencilAttachment = {};
	subpass_0_depthStencilAttachment.attachment = 0;
	subpass_0_depthStencilAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::array<VkAttachmentReference, 1> subpass_0_colorAttachment = {};
	subpass_0_colorAttachment[0].attachment = 1;
	subpass_0_colorAttachment[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	std::array<VkSubpassDescription, 1> subpasses = {};
	subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses[0].inputAttachmentCount = 0;
	subpasses[0].pInputAttachments = nullptr;
	subpasses[0].colorAttachmentCount = static_cast<uint32_t>(subpass_0_colorAttachment.size());
	subpasses[0].pColorAttachments = subpass_0_colorAttachment.data();
	subpasses[0].pDepthStencilAttachment = &subpass_0_depthStencilAttachment;	

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
	renderPassInfo.pSubpasses = subpasses.data();

	vkErrorCheck(vkCreateRenderPass(mRenderer->GetVulkanDevice(), &renderPassInfo, nullptr, &mRenderPass));
}

//--------------------------------------------------------------------------------------

void Window::TerminateRenderPass()
{
	vkDestroyRenderPass(mRenderer->GetVulkanDevice(), mRenderPass, nullptr);
}

//--------------------------------------------------------------------------------------

void Window::InitFrameBuffers()
{
	mFrameBuffers.resize(mSwapchainImageCount);
	for (uint32_t i = 0; i < mSwapchainImageCount; ++i)
	{
		std::array<VkImageView, 2> attachments = {};
		attachments[0] = mDepthStencilImgView;
		attachments[1] = mSwapchainImageViews[i];

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType				= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass		= mRenderPass;
		framebufferCreateInfo.attachmentCount	= static_cast<uint32_t>( attachments.size() );
		framebufferCreateInfo.pAttachments		= attachments.data();
		framebufferCreateInfo.width				= mSurfaceWidth;
		framebufferCreateInfo.height			= mSurfaceHeight;
		framebufferCreateInfo.layers			= 1;

		vkErrorCheck( vkCreateFramebuffer(	mRenderer->GetVulkanDevice(), 
											&framebufferCreateInfo, nullptr, &mFrameBuffers[i]) );

	}

}

//--------------------------------------------------------------------------------------

void Window::TerminateFrameBuffers()
{
	for (auto& fb : mFrameBuffers)
	{
		vkDestroyFramebuffer(mRenderer->GetVulkanDevice(), fb, nullptr);
	}
}

//--------------------------------------------------------------------------------------

void Window::InitSynchronizations()
{
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	vkCreateFence(mRenderer->GetVulkanDevice(), &fenceCreateInfo, nullptr, &mSwapchainImageAvailable);
}

//--------------------------------------------------------------------------------------

void Window::TerminateSynchronizations()
{
	vkDestroyFence(mRenderer->GetVulkanDevice(), mSwapchainImageAvailable, nullptr);
}

//--------------------------------------------------------------------------------------