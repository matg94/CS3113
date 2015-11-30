#include "Application.h"

SDL_Window* displayWindow;

int main(int argc, char *argv[])
{
	Application app;
	app.Initialize();
	app.SetUpGame();

	while (!app.quit) {

		app.Loop();
		app.GameInput();

	}

	SDL_Quit();
	return 0;
}
