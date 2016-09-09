#ifndef ENGINE_RENDERER_H__
#define	ENGINE_RENDERER_H__

//======================================================================================
// Filename: Renderer.h
// Description:
//
//======================================================================================

#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif


//======================================================================================
// Includes
//======================================================================================
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <vector>

struct SDL_Window;
//======================================================================================

//======================================================================================
// Class Application
//======================================================================================
class Renderer
{
public:
	Renderer();
    ~Renderer();

	void Initialize( SDL_Window* window );
	void Terminate();
	
	void Update();
	void Render();

private:
	vk::SurfaceKHR CreateVulkanSurface(const vk::Instance& instance, SDL_Window* window);
	std::vector<const char*> GetAvailableWSIExtensions();

private:
	std::vector<const char*> mExtensions;
	std::vector<const char*> mLayers;

	vk::Instance mInstance;
	vk::SurfaceKHR mSurface;

};
//======================================================================================
#endif //ENGINE_RENDERER_H__