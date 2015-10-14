#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <vector>
#include "Entity.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

class Entity;

class Application {
public:
	SDL_Window* displayWindow;
	SDL_Event event;
	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	Matrix laserMatrix;
	Matrix enemyMatrix;
	Matrix textMatrix;
	ShaderProgram* program;
	float LastFrameTick;
	float elapsed;
	Entity* player;
	GLuint font;
	SDL_Joystick* playerOneController;
	Entity* lasers[30] = {};
	Entity* enemies[50] = {};
	bool startScreen;
	int currentLaserIndex;
	int PlayerLaserTexture;
	int EnemyLaserTexture;
	int ExplosionSheet;
	int playerScore;
	float ScoreMulti;
	float timeSinceLastShot = 0.0f;


	Application();
	~Application();

	bool done = false;

	void SetUp();
	void Update();
	void Render();
	void Input();
	void ShootLaser(Entity*);
	void SetUpLasers();
	void SetUpEnemies(int);
	void AIControl(Entity*);
	float getTime();
	void StartScreen();
	void EndScreen();
	GLuint LoadTexture(const char*);
	void DrawText(int, std::string, float, float);
	void Explode(Entity*);

};
#endif