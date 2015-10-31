#include "Application.h"

int main(int argc, char *argv[])
{
	Application app;
	app.SetUp();
	while (!app.done) {
		if (app.screen) {
			app.Input();
			app.StartScreen();
		}
		else {
			app.Update();
			app.Input();
			app.Render();
		}
	}
	SDL_Quit();
	return 0;
}
