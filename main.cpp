#include "Application.h"

void main()
{
	Application app;
	app.Initialize();
	while (app.IsRunning())
		app.Update();
	app.Shutdown();
}