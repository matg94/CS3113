#include "Chunk.h"
#include <random>

// TILE TYPES:

// 0 = air;
// 1 = Bedrock;
// 2 = Dirt;

Chunk::Chunk(int pos, int xSize, int ySize, bool rend) :
	Position(pos), Width(xSize), Height(ySize), Render(rend) {

}

Chunk::~Chunk() {
	for (int y = 0; y < Height-1; ++y) {
		for (int x = 0; x < Width-1; ++x) {
			delete Data[y][x];
		}
	}
}

void Chunk::readChunk() {

	for (int y = 0; y < Height; ++y) {
		for (int x = 0; x < Width-1; ++x) {

			printf("[%d]", Data[y][x]->type);

		}
		printf("\n");
	}
}

void Chunk::CheckDelete(float x, float y, Mix_Chunk* sound) {

	int GridX = (int)(x / 0.5f);
	int GridY = (int)(y / 0.5f);

	if (Data[GridY][GridX]->type == 2 || Data[GridY][GridX]->type == 4) {
		Mix_PlayChannel(-1, sound, 0);
		Data[GridY][GridX]->type = 0;
	}
}

void Chunk::PlaceChunk(float x, float y, Mix_Chunk* sound) {

	int GridX = (int)(x / 0.5f);
	int GridY = (int)(y / 0.5f);

	if (Data[GridY][GridX]->type == 0) {
		Mix_PlayChannel(-1, sound, 0);
		Data[GridY][GridX]->type = 4;
		Data[GridY][GridX]->textureIndex = 143;
	}
}

void Chunk::GenerateGrass() {
	
	for (int y = (Height - 1); y >= 0; --y) {
		for (int x = 0; x < (Width); ++x) {

			if (Data[y][x]->type == 2) {
				if (Data[y - 1][x]->type == 0) {
					Data[y][x]->textureIndex = 9;
				}
			}

		}
	}

}

void Chunk::GenerateData(int seed) {

	srand(seed);

	for (int y = (Height-1); y >= 0; --y) {
		for (int x = 0; x < (Width); ++x) {

			if (x == 9 && y == 18) {
				int w = 1;
			};
			if (y == 17 || y ==16) {
				Data[y][x] = new Tile(1);
				continue;
			}

			else if (y == 15) {
				Data[y][x] = new Tile(2);
				continue;
			}

			if (y == 14 && x >= 1) {
				int rand = std::rand() % 100 + 1;
				if (Data[y][x - 1]->type == 2) {
					if (rand < 90 && Data[y+1][x]->type == 2) {
						Data[y][x] = new Tile(2);
						continue;
					}
				}
				else if (Data[y][x - 1]->type == 0) {
					if (rand < 80 && Data[y + 1][x]->type == 2) {
						Data[y][x] = new Tile(2);
						continue;
					}
				}

			}

			else {
				int rand = std::rand() % 100 + 1;
				if (x >= 1 && y < 14) {
					if (Data[y][x-1]->type == 2) {
						if (rand < ((y + 1) * 6) && Data[y + 1][x]->type == 2) {
							Data[y][x] = new Tile(2);
							continue;
						}

					}
					else {
						if (rand < ((y+1)*3) && Data[y + 1][x]->type == 2) {
							Data[y][x] = new Tile(2);
							continue;
						}
					}
				}
			}
			Data[y][x] = new Tile(0);
		}
	}
	GenerateGrass();
}

void Chunk::DrawChunk() {



}