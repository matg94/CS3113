#pragma once
#include "Chunk.h"
#include <vector>

class Map {
public:
	int Height;
	int Width;

	std::vector<Chunk*> Chunks;

	Chunk* LastChunk;
	Chunk* FirstChunk;
	Chunk* CurrentChunk;

	void CreateChunk(int direction);
	void Draw();
	void Update();
	void SetUp();
	Map();
	~Map();

};