#include "Application.h"

#include "MainMenuScene.h"

void main()
{
	Application app;
	app.Initialize();
	app.SetScene(new MainMenuScene);
	while (app.IsRunning())
		app.Update();
	app.Shutdown();
}