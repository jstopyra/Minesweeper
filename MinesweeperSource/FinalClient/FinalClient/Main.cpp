#include <SDL.h>

#include "Application.h"
#include "WinSock2.h"

int main(int argc, char* args[])
{
	//Application is the back bones of the program, This is what renders on the screen and takes input from the player.
	Application* app = nullptr;
	app->GetApplication();

	//Start up SDL and create window
	if (!app->GetApplication()->Init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		while (app->GetApplication()->IsRunning())
		{
			app->GetApplication()->Update();
		}
	}

	//Free resources and close SDL
	app->GetApplication()->Close();

	return 0;
}