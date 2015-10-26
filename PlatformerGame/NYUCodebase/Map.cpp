#include "Map.h"
#include "Chunk.h"
#include <random>

Map::Map() :
	Height(18), Width(32) {
}

Map::~Map() {

}

void Map::SetUp() {
	
	Chunk* tempChunk = new Chunk(0,32,18,true);

	tempChunk->GenerateData();

	Chunks.push_back(tempChunk);

	CurrentChunk = tempChunk;

	Chunk* tempChunk2 = new Chunk(-1, 32, 18, true);

	tempChunk2->GenerateData();

	Chunks.push_back(tempChunk2);

	FirstChunk = tempChunk2;

	Chunk* tempChunk3 = new Chunk(1, 32, 18, true);

	tempChunk3->GenerateData();

	Chunks.push_back(tempChunk3);

	LastChunk = tempChunk3;



}

void Map::CreateChunk(int direction) {

	if (direction < 1) {
		// Create Left Chunk

		Chunk* NewChunk = new Chunk(FirstChunk->Position - 1, 32, 18, false);
		NewChunk->GenerateData();
		Chunks.push_back(NewChunk);

		delete NewChunk;

	}
	else if (direction >= 1) {
		// Create Right Chunk

		Chunk* NewChunk = new Chunk(LastChunk->Position + 1, 32, 18, false);
		NewChunk->GenerateData();
		Chunks.push_back(NewChunk);

		delete NewChunk;

	}

}