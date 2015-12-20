#include "MapGenerator.h"

MapGenerator::MapGenerator(int x, int y, int seed) : Width(x), Height(y) {

	Data = new int*[Width];

	for (int i = 0; i < Width; ++i) {
		Data[i] = new int[Height];
	}

	for (int i = 0; i < Width; ++i) {
		for (int y = 0; y < Height; ++y) {

			Data[i][y] = 0;

		}
	}

	GenerateLevel(seed);

}

void MapGenerator::GenerateLevel(int seed) {

	srand(seed);

	for (int x = 0; x < Width; ++x) {
		for (int y = 0; y < Height; ++y) {

			float Random = (float)rand() / (float)RAND_MAX;

			if (x == 0 || x == (Width-1)) {
				Data[x][y] = 10;
			} 
			if (y == 0 || y == (Height-1)) {
				Data[x][y] = 9;
			}
			if (x == 0 && y == 0) {
				Data[x][y] = 11;
			}
			if (x == 0 && y == Height-1) {
				Data[x][y] = 12;
			}
			if (y == Height-1 && x == Width-1) {
				Data[x][y] = 13;
			}
			if (x == Width-1 && y == 0) {
				Data[x][y] = 14;
			}

			if (Random >= 0.9 && x > 0 && x < Width-1) {
				if (y > 0 && y < Height - 1) {
					Data[x][y] = rand()%6;
				}
			}
			else if (Random <= 0.075 && x > 0 && x < Width - 1) {
				if (y > 3 && y < Height - 3) {
					if (x > 3 && x < Width - 3) {
						Data[x][y] = 11;
					}
				}
			}


		}
	
	}

	for (int x = 1; x < Width - 2; ++x) {
		for (int y = 1; y < Height - 2; ++y) {

			float Random = (float)rand() / (float)RAND_MAX;
			float Texture = rand() % (7);
			float TextureLava = rand() % (8 - 6 + 1) + 6;

			if (Data[x - 1][y] != 0 && Random >= 0.5) {
				Data[x][y] = Texture;
			}
			if (Data[x][y - 1] != 0 && Random >= 0.95) {
				Data[x][y] = TextureLava;
			}
			if (Data[x][y - 1] == 11) {
				Data[x][y] = 12;
			}
			else if (Data[x - 1][y] == 11) {
				Data[x][y] = 14;
			}
			else if (Data[x - 1][y - 1] == 11) {
				Data[x][y] = 13;
			}
			
		}
	}
}

void MapGenerator::RenderMap(ShaderProgram* program) {

		std::vector<float> vertexData;
		std::vector<float> texCoordData;
		std::vector<float> colorData;

		float TILE_SIZE = 0.4f;

		for (int y = 0; y < Height; ++y) {
			for (int x = 0; x < Width; ++x) {

				// if (Data[x][y] != 0) {

					float u = (float)((((int)Data[y][x]) % 5) / (float)5);
					float v = (float)((((int)Data[y][x]) / 5) / (float)5);

					float spriteWidth = 1.0f / (float)5;
					float spriteHeight = 1.0f / (float)5;
					vertexData.insert(vertexData.end(), {
						TILE_SIZE * x, -TILE_SIZE * y,
						TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
						(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
						TILE_SIZE * x, -TILE_SIZE * y, (TILE_SIZE * x) + TILE_SIZE,
						(-TILE_SIZE * y) - TILE_SIZE,
						(TILE_SIZE * x) + TILE_SIZE,-TILE_SIZE * y
					});


					texCoordData.insert(texCoordData.end(), {
						u, v,
						u, v + (spriteHeight),
						u + spriteWidth, v + (spriteHeight),
						u, v,
						u + spriteWidth, v + (spriteHeight),
						u + spriteWidth, v
					});

					colorData.insert(colorData.end(), {
						1.0f, 1.0f, 1.0f, 1.0f,
						1.0f, 1.0f, 1.0f, 1.0f,
						1.0f, 1.0f, 1.0f, 1.0f,
						1.0f, 1.0f, 1.0f, 1.0f,
						1.0f, 1.0f, 1.0f, 1.0f,
						1.0f, 1.0f, 1.0f, 1.0f
					});

				//}
			}
		}
		glUseProgram(program->programID);

		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0,
			vertexData.data());
		glEnableVertexAttribArray(program->positionAttribute);

		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0,
			texCoordData.data());
		glEnableVertexAttribArray(program->texCoordAttribute);

		glVertexAttribPointer(program->colorAttribute, 4, GL_FLOAT, false, 0,
			colorData.data());
		glEnableVertexAttribArray(program->colorAttribute);

		glBindTexture(GL_TEXTURE_2D, SpriteSheetID);
		glDrawArrays(GL_TRIANGLES, 0, (vertexData.size() / 2));

		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
		glDisableVertexAttribArray(program->colorAttribute);
}
