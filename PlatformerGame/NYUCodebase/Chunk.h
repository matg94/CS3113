#pragma once
#include "Tile.h"
#include <SDL_mixer.h>

class Chunk {
public:
	int Width;
	int Height;
	bool Render;

	int Position;
	Tile* Data[18][32];

	Chunk(int, int, int, bool);
	void GenerateData(int);
	void readChunk();
	void DrawChunk();
	void CheckDelete(float x, float y, Mix_Chunk*);
	void PlaceChunk(float x, float y, Mix_Chunk*);
	void GenerateGrass();
	~Chunk();
};