#include "Application.h"


void Application::Initialize() {

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	program = new ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glViewport(0, 0, 640, 360);

	playerOneController = SDL_JoystickOpen(0);

	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, 1.0f, -1.0f);

	// projectionMatrix.setPerspectiveProjection((75.0f*180.0f/3.14f), 1.7777f, 1.0f, -1.0f);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	quit = false;

	map = new MapGenerator(30, 30, 123412);

	map->SpriteSheetID = LoadTexture("tiles_spritesheet.png");
	font = LoadTexture("Font1.png");

	player.textureID = LoadTexture("playerRed1.png");

}

void Application::Loop() {

	glClear(GL_COLOR_BUFFER_BIT);

	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);
	program->setModelMatrix(modelMatrix);

	modelMatrix.identity();
	viewMatrix.identity();

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
		player.Update(fixedElapsed);
		for (int i = 0; i < enemies.size(); ++i) {
			enemies[i].Update(fixedElapsed, player);
		}
	}
	player.Update(elapsed);
	for (int i = 0; i < enemies.size(); ++i) {
		enemies[i].Update(elapsed, player);
	}

	map->RenderMap(program);

	DrawPlayer();
	DrawEnemy();

	viewMatrix.Translate(-player.xPos, -player.yPos, 0.0f);

	SDL_GL_SwapWindow(displayWindow);

}

void Application::SetUpGame() {

	LastFrameTick = 0.0f;
	int textureID = LoadTexture("playerRed1.png");

	for (int i = 0; i < 80; ++i) {

		Enemy enemy;
		enemy.textureID = 3;
		enemies.push_back(enemy);
		std::cout << "X : " << enemy.xPos / 0.4f << "  Y: " << enemy.yPos / 0.4f << std::endl;
	}


}

void Application::GameInput() {

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			quit = true;
		}

		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		if (event.type == SDL_JOYAXISMOTION) {
			if (event.jaxis.which == 0) {
				if (event.jaxis.axis == 0) {
					player.MoveX(event.jaxis.value);
				}
				if (event.jaxis.axis == 1) {
					player.MoveY(event.jaxis.value);
				}
				if (event.jaxis.axis == 5) {
					player.yDir = -event.jaxis.value;
				}
				if (event.jaxis.axis == 2) {
					player.xDir = event.jaxis.value;
				}
			}
		}
	}
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

	float colorVar[4];
	colorVar[0] = 1.0f;
	colorVar[1] = 1.0f;
	colorVar[2] = 1.0f;
	colorVar[3] = 1.0f;

	glVertexAttribPointer(program->colorAttribute, 4, GL_FLOAT, false, 0, colorVar);
	glEnableVertexAttribArray(program->colorAttribute);

	glBindTexture(GL_TEXTURE_2D, FontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
	glDisableVertexAttribArray(program->colorAttribute);

}


void Application::DrawPlayer() {

	program->setModelMatrix(player.modelMatrix);

	player.modelMatrix.identity();

	player.modelMatrix.Translate(player.xPos, player.yPos, 0.0f);
	player.modelMatrix.Rotate(player.Angle);
	player.modelMatrix.Scale(0.35f, 0.35f, 1.0f);

	float vertices[] = { -0.2f, -0.3f, 0.2f, 0.3f, -0.2f, 0.3f, 0.2f, 0.3f, -0.2f, -0.3f, 0.2f, -0.3f };
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	float texCoord[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoord);
	glEnableVertexAttribArray(program->texCoordAttribute);

	std::vector<float> colorVar;
	for (int i = 0; i < 6; ++i) {
		colorVar.insert(colorVar.end(), { 1.0f, 1.0f, 1.0f, 1.0f });
	}

	glVertexAttribPointer(program->colorAttribute, 4, GL_FLOAT, false, 0, colorVar.data());
	glEnableVertexAttribArray(program->colorAttribute);

	glBindTexture(GL_TEXTURE_2D, player.textureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
	glDisableVertexAttribArray(program->colorAttribute);


}

void Application::DrawEnemy() {

	for (int i = 0; i < enemies.size(); ++i) {

		program->setModelMatrix(enemies[i].modelMatrix);

		enemies[i].modelMatrix.identity();

		enemies[i].modelMatrix.Translate(enemies[i].xPos, enemies[i].yPos, 0.0f);
		enemies[i].modelMatrix.Rotate(-enemies[i].Angle);
		enemies[i].modelMatrix.Scale(0.35f, 0.35f, 1.0f);

		float vertices[] = { -0.2f, -0.3f, 0.2f, 0.3f, -0.2f, 0.3f, 0.2f, 0.3f, -0.2f, -0.3f, 0.2f, -0.3f };
		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);

		float texCoord[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoord);
		glEnableVertexAttribArray(program->texCoordAttribute);

		std::vector<float> colorVar;
		if (enemies[i].State == "Attacking") {
			for (int i = 0; i < 6; ++i) {
				colorVar.insert(colorVar.end(), { 1.4f, 0.8f, 0.8f, 1.0f });
			}
		}
		else if (enemies[i].State == "Wandering") {
			for (int i = 0; i < 6; ++i) {
				colorVar.insert(colorVar.end(), { 0.8f, 0.8f, 0.8f, 1.0f });
			}
		}
		else {
			for (int i = 0; i < 6; ++i) {
				colorVar.insert(colorVar.end(), { 1.0f, 0.8f, 0.8f, 1.0f });
			}
		}

		glVertexAttribPointer(program->colorAttribute, 4, GL_FLOAT, false, 0, colorVar.data());
		glEnableVertexAttribArray(program->colorAttribute);


		glBindTexture(GL_TEXTURE_2D, enemies[i].textureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
		glDisableVertexAttribArray(program->colorAttribute);

	}


}