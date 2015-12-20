#pragma once

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
#include "Emitter.h"
#include "Enemy.h"
#include "Entity.h"
#include "GameState.h"
#include <SDL_mixer.h>


class Application {

public:

	SDL_Window* displayWindow;
	ShaderProgram* program;
	ShaderProgram* programText;
	Matrix projectionMatrix;
	Matrix viewMatrix;
	Matrix modelMatrix;
	SDL_Joystick* playerOneController;

	float TimeSinceLastHit;

	Mix_Music* FireSound;
	Mix_Chunk* BloodSpatterSound;
	Mix_Chunk* ZombieAlert;
	Mix_Chunk* ZombieDeath;
	Mix_Chunk* OutOfMana;
	Mix_Chunk* ZombieHit;
	Mix_Chunk* PauseSound;
	Mix_Chunk* LightSound;


	std::vector<Enemy> enemies;

	GLuint font;
	MapGenerator* map;

	Emitter* FireEmitter;
	Emitter* BloodEmitter;
	
	GameState* GameManager;

	std::vector<Entity> lights;

	float LastFrameTick;

	std::string CurrentScene;
	Player* player;
	bool quit;
	bool startingGame;

	void Initialize();
	void SetUpGame();
	void Loop();
	void GameInput();
	void DrawPlayer();
	void DrawEnemy();
	void DeathScreen();
	void CheckCollision();
	void MainMenu();

	GLuint LoadTexture(const char*);
	void DrawText(int, std::string, float, float);


};