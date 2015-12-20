#pragma once
#include <random>
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
#include "ShaderProgram.h"

class MapGenerator {

public:

	int Width;
	int Height;
	int **Data;
	GLuint SpriteSheetID;

	MapGenerator(int, int, int);

	void GenerateLevel(int);
	void RenderMap(ShaderProgram*);
	void GeneratePathData();

};