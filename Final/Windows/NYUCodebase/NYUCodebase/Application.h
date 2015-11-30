#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
#include <string>
#include "Player.h"
#include "Matrix.h"
#include "ShaderProgram.h"
#include "MapGenerator.h"
#include <vector>
#include "Enemy.h"

#pragma once


class Application {

public:

	SDL_Window* displayWindow;
	ShaderProgram* program;
	Matrix projectionMatrix;
	Matrix viewMatrix;
	Matrix modelMatrix;
	SDL_Joystick* playerOneController;
	float matrix_X;
	float matrix_Y;

	std::vector<Enemy> enemies;

	GLuint font;
	MapGenerator* map;

	float LastFrameTick;

	std::string CurrentScene;
	Player player;
	bool quit;

	void Initialize();
	void SetUpGame();
	void Loop();
	void GameInput();
	void DrawPlayer();
	void DrawEnemy();

	GLuint LoadTexture(const char*);
	void DrawText(int, std::string, float, float);


};