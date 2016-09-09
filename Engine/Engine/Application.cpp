//======================================================================================
// Filename: Application.cpp
// Description:
//
//======================================================================================

//======================================================================================
// Includes
//======================================================================================
#include "Application.h"
#include "Renderer.h"
#include <stdexcept>
//======================================================================================

Application::Application()
{
}

//--------------------------------------------------------------------------------------

Application::~Application()
{
}

//--------------------------------------------------------------------------------------

void Application::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) throw std::runtime_error("Could not init SDL");

	mWindow = SDL_CreateWindow( "VulkanEngine", 
								SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
								1280, 720, SDL_WINDOW_OPENGL );
	if (mWindow == nullptr) throw std::runtime_error("Failed to create SDL Window.");

	mRenderer = new Renderer();
	mRenderer->Initialize( mWindow );

}

//--------------------------------------------------------------------------------------

void Application::Terminate()
{
	mRenderer->Terminate();
	mRenderer = nullptr;

	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

//--------------------------------------------------------------------------------------

void Application::Run()
{
	SDL_Event event;
	while ( mIsRunning )
	{
		switch (event.type)
		{

		case SDL_QUIT:
			mIsRunning = false;
			break;

		default:
			// Do nothing.
			break;
		}

		SDL_Delay(10);

	}
}


//--------------------------------------------------------------------------------------


//======================================================================================