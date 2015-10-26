#pragma once
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include <string>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <vector>
#include "Chunk.h"
#include "Player.h"
#include "Map.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

class Application {
public:
	SDL_Window* displayWindow;
	bool done;

	SDL_Event event;
	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	Matrix tileMatrix;
	ShaderProgram* program;
	float LastFrameTick;

	GLuint TileTextureSheet;
	Player* player;
	Map* map;


	void SetUp();
	void Input();
	void Update();
	void Render();
	void CheckCollisions(Player* player);
	void DrawText(int, std::string, float, float);
	void DrawChunk(int, Chunk*, float);
	GLuint LoadTexture(const char*);

};