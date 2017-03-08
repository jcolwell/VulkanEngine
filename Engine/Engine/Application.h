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
#include "Common.h"
#include <string>

class Renderer;
class Window;
//======================================================================================

//======================================================================================
// Class Application
//======================================================================================
class Application
{
public:
	Application();
	~Application();

	void Initialize( const std::string& appName, u32 windowWidth, u32 windowHeight );
	bool Run();
	void Terminate();
	
private:
	NONCOPYABLE(Application)

private:
	bool mIsRunning = true;
	Renderer* mRenderer;

	Window* mWindow;
};
//======================================================================================
#endif //ENGINE_APPLICATION_H__