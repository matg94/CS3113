#include "Application.h"
#include <string>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <vector>
#include <SDL_mixer.h>
#include "Map.h"
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif


void Application::SetUp() {
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif
	done = false;
	screen = true;

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 1024);

	jumpSound = Mix_LoadWAV("JumpSound.wav");
	destroySound = Mix_LoadWAV("DestroyBlock.wav");
	  // Mix_Music* Music = Mix_LoadMUS("");


	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glViewport(0, 0, 640, 360);

	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	map = new Map();
	map->SetUp(SDL_GetTicks());

	player = new Player();

	player->Texture = LoadTexture("characters.png");
	TileTextureSheet = LoadTexture("tiles_spritesheet.png");
	Font = LoadTexture("font1.png");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	// map->Chunks[1]->readChunk();

	LastFrameTick = 0.0f;
}


void Application::StartScreen() {

	std::string welcomeMessage = "Hello, and welcome to Planetarria!";
	std::string Intro1 = "Before you explore this wonderous land of dirt...";
	std::string Intro2 = "You'll need to know how to handle yourself.";
	std::string Intro3 = "Use 'A' and 'D' to move around.";
	std::string Intro4 = "Use 'W', 'S', 'Q', and 'E' to dig!";
	std::string Intro8 = "You can also use the arrow keys to place wooden blocks!";
	std::string Intro5 = "please go and explore now, and remember, you can jump with SpaceBar!";
	std::string Intro6 = "and since this is a small planet, you'll jump quite high! enjoy!";
	std::string Intro7 = "To continue to your new home, please hit the space bar.";
	std::string Intro9 = "But be careful.. the grass does not grow back!";

	std::vector<std::string> messages;
	messages.push_back(welcomeMessage);
	messages.push_back(Intro1);
	messages.push_back(Intro2);
	messages.push_back(Intro3);
	messages.push_back(Intro4);
	messages.push_back(Intro8);
	messages.push_back(Intro9);
	messages.push_back(Intro5);
	messages.push_back(Intro6);
	messages.push_back(Intro7);

	glClear(GL_COLOR_BUFFER_BIT);

	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);

	for (int i = 0; i < messages.size(); ++i) {

		Matrix textMatrix;

		textMatrix.identity();
		float spacing = (messages[i].size()*-0.12f);
		float x = (((float)messages[i].size())/2.0f)*0.2f + (spacing/2.0f);
		float y = (1.8f - (i*0.3f));

		textMatrix.Translate(-x, y, 0.0f);

		program->setModelMatrix(textMatrix);

		DrawText(Font, messages[i], 0.2f, -0.12f);

	}

	SDL_GL_SwapWindow(displayWindow);
}

void Application::Input() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		if (screen) {
			if (keys[SDL_SCANCODE_SPACE]) {
				LastFrameTick = SDL_GetTicks() / 1000.0f;
				screen = false;
			}
		}
		else {
			player->xAcc = 0.0f;
			if (keys[SDL_SCANCODE_D]) {
				player->xAcc = 1.0f;
			}
			else if (keys[SDL_SCANCODE_A]) {
				player->xAcc = -1.0f;
			} if (keys[SDL_SCANCODE_SPACE]) {
				if (player->CollidedBottom) {
					Mix_PlayChannel(-1, jumpSound, 0);
					player->Jump();
				}
			} if (keys[SDL_SCANCODE_E]) {
				map->CurrentChunk->CheckDelete(player->xPos + 0.5f, -player->yPos, destroySound);
			}
			else if (keys[SDL_SCANCODE_Q]) {
				map->CurrentChunk->CheckDelete(player->xPos - 0.5f, -player->yPos, destroySound);
			}
			else if (keys[SDL_SCANCODE_W]) {
				map->CurrentChunk->CheckDelete(player->xPos, -player->yPos - 0.5f, destroySound);
			}
			else if (keys[SDL_SCANCODE_S]) {
				map->CurrentChunk->CheckDelete(player->xPos, -player->yPos + 0.5f, destroySound);
			} if (keys[SDL_SCANCODE_RIGHT])  {
				map->CurrentChunk->PlaceChunk(player->xPos + 0.5f, -player->yPos, destroySound);
			}
			else if (keys[SDL_SCANCODE_LEFT]) {
				map->CurrentChunk->PlaceChunk(player->xPos - 0.5f, -player->yPos, destroySound);
			}
			else if (keys[SDL_SCANCODE_UP]) {
				map->CurrentChunk->PlaceChunk(player->xPos, -player->yPos - 0.5f, destroySound);
			}
			else if (keys[SDL_SCANCODE_DOWN]) {
				map->CurrentChunk->PlaceChunk(player->xPos, -player->yPos + 0.5f, destroySound);
			}
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
	player->Animate(elapsed);
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

	DrawChunk(TileTextureSheet, map->CurrentChunk, 0.5f, 0);
	// DrawChunk(TileTextureSheet, map->FirstChunk, 0.5f, -1);
	// DrawChunk(TileTextureSheet, map->LastChunk, 0.5f, 1);

	viewMatrix.identity();
	viewMatrix.Translate(-player->xPos, -player->yPos, 0.0f);

	printf("Position : %d \n", player->Position);

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


void Application::DrawChunk(int SpriteSheet, Chunk* chunk, float TILE_SIZE, int pos) {

	std::vector<float> vertexData;
	std::vector<float> texCoordData;

	for (int y = 0; y < 18; ++y) {
		for (int x = 0; x < 31; ++x) {

			if (chunk->Data[y][x]->type != 0) {

				float u = (float)((((int)chunk->Data[y][x]->textureIndex) % 13) / (float)13);
				float v = (float)((((int)chunk->Data[y][x]->textureIndex) / 13) / (float)13);

				float spriteWidth = 1.0f / (float)13;
				float spriteHeight = 1.0f / (float)13;
				vertexData.insert(vertexData.end(), {
					TILE_SIZE * x + pos * 32 * TILE_SIZE - pos, -TILE_SIZE * y,
					TILE_SIZE * x + pos * 32 * TILE_SIZE - pos, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE + pos * 32 * TILE_SIZE - pos, (-TILE_SIZE * y) - TILE_SIZE,
					TILE_SIZE * x + pos * 32 * TILE_SIZE - pos, -TILE_SIZE * y,
					(TILE_SIZE * x) + TILE_SIZE + pos * 32 * TILE_SIZE - pos, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE + pos * 32 * TILE_SIZE - pos, -TILE_SIZE * y
				});

				float d = 0.008f;
				if (chunk->Data[y][x]->type == 4) {
					u -= 0.01f;
				}
				u += 0.01f;

				texCoordData.insert(texCoordData.end(), {
					u + d, v + d,
					u + d, v + (spriteHeight)-d,
					u + spriteWidth - d, v + (spriteHeight)-d,
					u + d, v + d,
					u + spriteWidth - d, v + (spriteHeight)-d,
					u + spriteWidth - d, v + d
				});
			}
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
		glDrawArrays(GL_TRIANGLES, 0, (vertexData.size() / 2));

		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);


}
