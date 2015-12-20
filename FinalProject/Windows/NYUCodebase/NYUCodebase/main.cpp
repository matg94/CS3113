#include "Application.h"

SDL_Window* displayWindow;

int main(int argc, char *argv[])
{
	Application app;
	app.Initialize();

	while (!app.quit) {

		app.GameInput();
		if (app.GameManager->MainGame) {
			app.Loop();
		}
		else if (app.GameManager->EndMenu) {
			app.DeathScreen();
		}
		else if (app.GameManager->MainMenu) {
			app.MainMenu();
		}

	}

	SDL_Quit();
	return 0;
}
