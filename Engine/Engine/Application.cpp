//======================================================================================
// Filename: Application.cpp
// Description:
//
//======================================================================================

//======================================================================================
// Includes
//======================================================================================
#include "Application.h"
#include "Common.h"
#include "Renderer.h"
#include "Window.h"
//======================================================================================

constexpr double kPi				= 3.14159265358979323846;
constexpr double kCircleRad			= kPi * 2;
constexpr double kCircleThird		= kCircleRad / 3.0;
constexpr double kCircleThird1		= 0;
constexpr double kCircleThird2		= kCircleThird;
constexpr double kCircleThird3		= kCircleThird * 2;

namespace
{
	VkCommandPool		_commandPool	= VK_NULL_HANDLE;
	VkCommandBuffer		_commandBuffer	= VK_NULL_HANDLE;

	VkSemaphore			_renderCompleteSempahore = VK_NULL_HANDLE;

	struct Color
	{
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 0.0f;
	};

	Color _color = {};
	double _colorRotator = 0.0f;
}



Application::Application()
{
}

//--------------------------------------------------------------------------------------

Application::~Application()
{
}

//--------------------------------------------------------------------------------------

void Application::Initialize(const std::string& appName, u32 windowWidth, u32 windowHeight)
{

	mRenderer = new Renderer();
	mRenderer->InitializeWindow( appName, windowWidth, windowHeight );
	mWindow = mRenderer->GetWindow();

	VkCommandPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
									| VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolCreateInfo.queueFamilyIndex = mRenderer->GetVulkanGraphicsQueueFamily();
	vkCreateCommandPool(mRenderer->GetVulkanDevice(), &poolCreateInfo, nullptr, &_commandPool);

	VkCommandBufferAllocateInfo cmdBufferAllocateInfo = {};
	cmdBufferAllocateInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocateInfo.commandPool			= _commandPool;
	cmdBufferAllocateInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferAllocateInfo.commandBufferCount	= 1;
	vkAllocateCommandBuffers(mRenderer->GetVulkanDevice(), &cmdBufferAllocateInfo, &_commandBuffer);

	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vkCreateSemaphore(mRenderer->GetVulkanDevice(), &semaphoreCreateInfo, nullptr, &_renderCompleteSempahore);

}

//--------------------------------------------------------------------------------------

void Application::Terminate()
{
	vkQueueWaitIdle(mRenderer->GetVulkanQueue());
	vkDestroySemaphore(mRenderer->GetVulkanDevice(), _renderCompleteSempahore, nullptr);
	vkDestroyCommandPool(mRenderer->GetVulkanDevice(), _commandPool, nullptr);
	SAVE_DELETE(mRenderer);
}

//--------------------------------------------------------------------------------------

bool Application::Run()
{
	mIsRunning = mRenderer->Run();
	if (mIsRunning)
	{
		//Begin render
		mWindow->BeginRender();
		
		//Record command buffer
		VkCommandBufferBeginInfo bufferBeginInfo = {};
		bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		bufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		//Begin CommandBuffer
		vkBeginCommandBuffer(_commandBuffer, &bufferBeginInfo);
		
		VkRect2D renderArea = {};
		renderArea.offset.x = 0;
		renderArea.offset.y = 0;
		renderArea.extent	= mWindow->GetVulkanSurfaceSize();

		const uint32_t kClearValueSize = 2;
		VkClearValue clearValue[kClearValueSize];
		clearValue[0].depthStencil.depth = 0.0f;
		clearValue[0].depthStencil.stencil = 0;

		//Cycle colors
		{
			_colorRotator += 0.001;
			_color.r = static_cast<float>( std::sin( _colorRotator + kCircleThird1 ) * 0.5 + 0.5 );
			_color.g = static_cast<float>( std::sin( _colorRotator + kCircleThird2 ) * 0.5 + 0.5 );
			_color.b = static_cast<float>( std::sin( _colorRotator + kCircleThird3 ) * 0.5 + 0.5 );
		}

		//@TODO: Check unlying color type and init proper
		clearValue[1].color.float32[0] = _color.r; //r
		clearValue[1].color.float32[1] = _color.g; //g
		clearValue[1].color.float32[2] = _color.b; //b
		clearValue[1].color.float32[3] = _color.a; //a

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass		= mWindow->GetVulkanRenderPass();
		renderPassInfo.framebuffer		= mWindow->GetVulkanActiveFrameBuffer();
		renderPassInfo.renderArea		= renderArea;
		renderPassInfo.clearValueCount	= kClearValueSize;
		renderPassInfo.pClearValues		= clearValue;

		//Begin RenderPass
		vkCmdBeginRenderPass(_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		//EndRenderPass
		vkCmdEndRenderPass(_commandBuffer);
		
		//End CommandBuffer
		vkEndCommandBuffer(_commandBuffer);

		//Submit command buffer
		VkSubmitInfo submitInfo = {};
		submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount	= 0;
		submitInfo.pWaitSemaphores		= VK_NULL_HANDLE;
		submitInfo.pWaitDstStageMask	= nullptr;
		submitInfo.commandBufferCount	= 1;
		submitInfo.pCommandBuffers		= &_commandBuffer;
		submitInfo.signalSemaphoreCount	= 1;
		submitInfo.pSignalSemaphores	= &_renderCompleteSempahore;

		vkQueueSubmit(mRenderer->GetVulkanQueue(), 1, &submitInfo, VK_NULL_HANDLE);

		//End Runder
		mWindow->EndRender({_renderCompleteSempahore});

	}
	vkQueueWaitIdle(mRenderer->GetVulkanQueue());
	return mIsRunning;
}


//--------------------------------------------------------------------------------------


//======================================================================================