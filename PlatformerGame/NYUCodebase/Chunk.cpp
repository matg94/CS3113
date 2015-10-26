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

			printf("[[%d][%d]%d]", Data[y][x]->type, x, y);

		}
		printf("\n");
	}
}

void Chunk::GenerateData() {

	srand(1);

	for (int y = (Height); y >= 0; --y) {
		for (int x = 0; x < (Width); ++x) {

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
}

void Chunk::DrawChunk() {



}