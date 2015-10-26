#include "Application.h"
#include <string>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <vector>
#include "Map.h"
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif


void Application::SetUp() {
	
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif
	done = false;

	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glViewport(0, 0, 640, 360);

	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	map = new Map();
	map->SetUp();

	player = new Player();

	player->Texture = LoadTexture("characters.png");
	TileTextureSheet = LoadTexture("tiles_spritesheet.png");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	map->Chunks[1]->readChunk();

	LastFrameTick = 0.0f;
}

void Application::Input() {
	int playerGridX = (int)(player->xPos / 0.4f);
	int playerGridY = (int)(-player->yPos / 0.4f);

	playerGridX += 1;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_D]) {
			player->xAcc = 0.2f;
		} else if (keys[SDL_SCANCODE_A]) {
			player->xAcc = -0.2f;
		} if (keys[SDL_SCANCODE_S]) {
			player->yAcc = 0.2f;
		} else if (keys[SDL_SCANCODE_W]) {
			player->yAcc = -0.2f;
		}
	}
}

void Application::Update() {

	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - LastFrameTick;
	LastFrameTick = ticks;

	float FixedTimeStep = 1 / 60.0f;
	float fixedElapsed = elapsed;
	if (fixedElapsed > FixedTimeStep * 6) {
		fixedElapsed = FixedTimeStep * 6;
	}


	while (fixedElapsed >= FixedTimeStep) {
		fixedElapsed -= FixedTimeStep;
		player->Update(elapsed, map->CurrentChunk);
	}

	player->Update(fixedElapsed, map->CurrentChunk);

}

void Application::Render() {

	glClear(GL_COLOR_BUFFER_BIT);

	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);
	program->setModelMatrix(modelMatrix);

	modelMatrix.identity();
	modelMatrix.Translate(player->xPos, player->yPos, 0.0f);
	modelMatrix.Scale(0.5f, 0.5f, 1.0f);

	player->Draw(player->CurrentAnimationIndex, 8, 4, program);

	program->setModelMatrix(tileMatrix);

	tileMatrix.identity();
	DrawChunk(TileTextureSheet, map->CurrentChunk, 0.5f);

	viewMatrix.identity();
	viewMatrix.Translate(-player->xPos, -player->yPos, 0.0f);

	SDL_GL_SwapWindow(displayWindow);

}

GLuint Application::LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	return textureID;

}

void Application::DrawText(int FontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;

	for (int i = 0; i < text.size(); ++i) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing)*i) + (-0.5f * size), 0.5f * size,
			((size + spacing)*i) + (-0.5f * size), -0.5f * size,
			((size + spacing)*i) + (0.5f * size), 0.5f * size,
			((size + spacing)*i) + (0.5f * size), -0.5f * size,
			((size + spacing)*i) + (0.5f * size), 0.5f * size,
			((size + spacing)*i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}


	glUseProgram(program->programID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0,
		vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0,
		texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, FontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);

}


void Application::DrawChunk(int SpriteSheet, Chunk* chunk, float TILE_SIZE) {

	std::vector<float> vertexData;
	std::vector<float> texCoordData;

	for (int y = 0; y < 18; ++y) {
		for (int x = 0; x < 31; ++x) {

			float u = (float)(((int)chunk->Data[y][x]->textureIndex) % 13) / (float)13;
			float v = (float)(((int)chunk->Data[y][x]->textureIndex) / 13) / (float)13;

			float spriteWidth = 1.0f / (float)13;
			float spriteHeight = 1.0f / (float)13;

			vertexData.insert(vertexData.end(), {
				TILE_SIZE * x, -TILE_SIZE * y,
				TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
				(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
				TILE_SIZE * x, -TILE_SIZE * y,
				(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
				(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
			});

			texCoordData.insert(texCoordData.end(), {
				u , v ,
				u , v + (spriteHeight),
				u + spriteWidth , v + (spriteHeight),
				u , v ,
				u + spriteWidth , v + (spriteHeight),
				u + spriteWidth , v 
			});
		}
	}
		glUseProgram(program->programID);

		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0,
			vertexData.data());
		glEnableVertexAttribArray(program->positionAttribute);

		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0,
			texCoordData.data());
		glEnableVertexAttribArray(program->texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, SpriteSheet);
		glDrawArrays(GL_TRIANGLES, 0, ((18 * 32) * 6));

		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);


}