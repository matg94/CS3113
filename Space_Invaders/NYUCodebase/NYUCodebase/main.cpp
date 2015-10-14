#include "Application.h"

int main(int argc, char *argv[])
{
	Application app;
	app.SetUp();
	while (!app.done) {
		if (app.startScreen) {
			app.Input();
			app.StartScreen();
		}
		else if (app.player->endScreen) {
			app.Input();
			app.EndScreen();
		} else {
		app.Input();
		app.Update();
		app.Render();
		}
	}
	SDL_JoystickClose(app.playerOneController);
	SDL_Quit();
	return 0;
}
