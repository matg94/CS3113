#pragma once
#include "Chunk.h"
#include <vector>
#include "Player.h"

class Map {
public:
	int Height;
	int Width;

	std::vector<Chunk*> Chunks;

	Chunk* LastChunk;
	Chunk* FirstChunk;
	Chunk* CurrentChunk;

	void CreateChunk(int direction, int seed);
	void Draw();
	void Update();
	void SetUp(int seed);
	void CheckPosition(int position, Player*, int);
	Map();
	~Map();

};