#pragma once
#include "Tile.h"

class Chunk {
public:
	int Width;
	int Height;
	bool Render;

	int Position;
	Tile* Data[18][32];

	Chunk(int, int, int, bool);
	void GenerateData();
	void readChunk();
	void DrawChunk();
	~Chunk();
};