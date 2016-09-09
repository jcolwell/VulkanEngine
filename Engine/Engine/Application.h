#ifndef ENGINE_APPLICATION_H__
#define ENGINE_APPLICATION_H__
//======================================================================================
// Filename: Application.h
// Description:
//
//======================================================================================

//======================================================================================
// Includes
//======================================================================================
// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

class Renderer;
//======================================================================================

//======================================================================================
// Class Application
//======================================================================================
class Application
{
public:
	Application();
	~Application();

	void Initialize();
	void Run();
	void Terminate();

private:

private:
	bool mIsRunning = true;

	SDL_Window* mWindow = nullptr;
	Renderer* mRenderer;
};
//======================================================================================
#endif //ENGINE_APPLICATION_H__