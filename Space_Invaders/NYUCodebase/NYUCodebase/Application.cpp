#include "Application.h"
#include "Entity.h"
#include <string>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <vector>
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif


Application::Application() : LastFrameTick(0.0f), currentLaserIndex(1), startScreen(true), playerScore(0) {

}

void Application::StartScreen() {



	std::string str = "Welcome to Gold Invadarrrsss! ";
	std::string str2 = "Press R2 to continue...";

	glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);


	glUseProgram(program->programID);

	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);
	program->setModelMatrix(modelMatrix);

	modelMatrix.identity();

	float size = (-0.075f / 2)*(float)str.length();
	float size2 = (-0.075f / 2)*(float)str2.length();

	modelMatrix.Translate(size, 1.0f, 0.0f);
	DrawText(font, str, 0.15f, -0.08f);
	
	program->setModelMatrix(textMatrix);

	textMatrix.identity();

	textMatrix.Translate(size2, -1.0f, 0.0f);
	DrawText(font, str2, 0.15f, -0.08f);

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

void Application::SetUp() {

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
	playerOneController = SDL_JoystickOpen(0);
	displayWindow = SDL_CreateWindow("Gold Invadar'", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640*1.5f, 360*1.5f, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glViewport(0, 0, 640*1.5f, 360*1.5f);

	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);


	player = new Entity(0.0f, -1.75f, 0.0f, 0.0f, 0.1f, 0.1f, program);
	player->active = true;
	player->type = 1;
	player->hp = 5;

	player->textureID = LoadTexture("SinglePirate.png");
	PlayerLaserTexture = LoadTexture("Cannonball.png");
	EnemyLaserTexture = LoadTexture("laserRed02.png");
	ExplosionSheet = LoadTexture("ExplosionSheet.png");
	font = LoadTexture("font1.png");

	ScoreMulti = 1;

	SetUpLasers();
	SetUpEnemies(player->textureID);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

}

void Application::SetUpEnemies(int texture) {
	for (int i = 0; i < 5; ++i) {
		enemies[i] = new Entity(0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.1f, program);
		enemies[i]->textureID = texture;
		enemies[i]->ExplosionSheet = ExplosionSheet;
		enemies[i]->type = 2;
		enemies[i]->ID = i;
	}
}

void Application::Input() {

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		if (event.type == SDL_JOYAXISMOTION) {
			if (event.jaxis.which == 0) {
				if (event.jaxis.axis == 0) {
					player->MoveX(event.jaxis.value);
				}
				if (event.jaxis.axis == 1) {
				}
			}
		}
		if (event.type == SDL_JOYBUTTONDOWN) {
			if (event.button.button == 0) {
				if (startScreen) {
					startScreen = false;
				}
				else {
					if (timeSinceLastShot > 1.0f) {
						ShootLaser(player);
						timeSinceLastShot = 0.0f;
					}
				}
			}
		}
	}

}

void Application::Update() {

	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - LastFrameTick;
	LastFrameTick = ticks;
	timeSinceLastShot += elapsed;

	if (elapsed > 1.0f) {
		elapsed = 0.3f;
	}

	player->Update(elapsed);

	ScoreMulti += elapsed / 10;
	
	for (int i = 0; i < 30; ++i) {
		if (lasers[i]->active) {
			lasers[i]->Update(elapsed);
		}
	}
	for (int i = 0; i < 5; ++i) {
		AIControl(enemies[i]);
		enemies[i]->Update(elapsed);
		player->CollisionResponse(enemies[i]);
		for (int j = 0; j < 30; ++j) {
			if (!enemies[i]->exploding) {
				enemies[i]->CollisionResponse(lasers[j]);
			}
		}
	}
}

void Application::Render() {

	std::string str = "";

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program->programID);

	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);

	program->setModelMatrix(modelMatrix);

	modelMatrix.identity();
	modelMatrix.Translate(player->xPos, player->yPos, 0.0f);
	modelMatrix.Rotate(3.1415f);
	modelMatrix.Rotate(player->GetAngle() + 3.1415*0.5);
	modelMatrix.Scale(0.4f, 0.4f, 1.0f);

	player->DrawSpriteSheetSprite(0, 4, 4);

	for (int i = 0; i < 30; i++) {
		if (lasers[i]->active) {
			program->setModelMatrix(laserMatrix);
			laserMatrix.identity();
			laserMatrix.Translate(lasers[i]->xPos, lasers[i]->yPos, 0.0f);
			laserMatrix.Scale(0.05f, 0.05f, 1.0f);
			if (lasers[i]->exploding) {
				lasers[i]->DrawSpriteSheetSprite(lasers[i]->currentExplosionIndex, 4, 4);
				Explode(lasers[i]);
				laserMatrix.Scale(6.0f, 6.0f, 0.0f);
			}
			else {
				lasers[i]->DrawSprite();
			}
		}
	}
	for (int i = 0; i < 5; i++) {
		if (enemies[i]->active) {
			program->setModelMatrix(enemyMatrix);
			enemyMatrix.identity();
			enemyMatrix.Translate(enemies[i]->xPos, enemies[i]->yPos, 0.0f);
			enemyMatrix.Scale(0.35f, 0.35f, 1.0f);
			enemies[i]->DrawSpriteSheetSprite(0, 4, 4);
			}
		}


	SDL_GL_SwapWindow(displayWindow);

}

void Application::AIControl(Entity* enemy) {

	if (!enemy->active) {
		if (!enemy->exploding) {
			enemy->active = true;
			int x = rand() % 335 + 1;
			int t = rand() % 2 + 1;
			if (t == 1) {
				x *= -1;
			}
			float startX = (float)x / 100.0f;
			enemy->xPos = startX;
			enemy->yPos = rand() % 7 + 4;
			enemy->yVel -= 0.005f;
			if (enemy->yVel >= -0.1f) {
				enemy->yVel = -0.15f;
			}
		}
	}
	if (enemy->yPos < -2.1f) {
		enemy->active = false;
		enemy->exploding = false;
		player->hp -= 1;
	}


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

void Application::SetUpLasers() {
	for (int i = 0; i < 30; ++i) {
		lasers[i] = new Entity(-5.0f, -5.0f, 0.0f, 0.0f, 0.1f, 0.1f, program);
		lasers[i]->textureID = PlayerLaserTexture;
		lasers[i]->ExplosionSheet = ExplosionSheet;
		lasers[i]->type = 3;
	}
}

void Application::ShootLaser(Entity* shooter) {

	if (currentLaserIndex >= 29) {
		currentLaserIndex = 0;
	}

	lasers[currentLaserIndex]->active = true;

	float x = shooter->xPos;
	float y = shooter->yPos;


	lasers[currentLaserIndex]->xPos = x;
	lasers[currentLaserIndex]->yPos = y;

	float yV = 0.0f;
	float xV = 1.0f;

	lasers[currentLaserIndex]->yVel = xV;
	lasers[currentLaserIndex]->xVel = yV;
	++currentLaserIndex;
}

Application::~Application() {
	delete program;
	for (int i = 0; i < 30; ++i) {
		delete lasers[i];
	}
	for (int i = 0; i < 5; ++i) {
		delete enemies[i];
	}
}

void Application::Explode(Entity* target) {

	if (target->animationElapsed > 1.0f/60.0f) {
		target->animationElapsed = 0.0f;
		target->currentExplosionIndex++;
	}
	if (target->currentExplosionIndex >= 13) {
		playerScore += 1*ScoreMulti;
		target->active = false;
		target->exploding = false;
		target->currentExplosionIndex = 0;
	}
}

void Application::EndScreen() {

	std::string endMessage = "Thanks for playing, but you lost..";
	std::string score = std::to_string(playerScore);
	std::string endMessage2 = "Your score was only: " + score;

	glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);


	glUseProgram(program->programID);

	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);
	program->setModelMatrix(modelMatrix);

	modelMatrix.identity();

	float size = (-0.075f / 2)*(float)endMessage.length();
	float size2 = (-0.075f / 2)*(float)endMessage2.length();

	modelMatrix.Translate(size, 1.0f, 0.0f);
	DrawText(font, endMessage, 0.15f, -0.08f);

	textMatrix.identity();
	program->setModelMatrix(textMatrix);

	textMatrix.Translate(size2, -1.0f, 0.0f);
	DrawText(font, endMessage2, 0.15f, -0.08f);

	textMatrix.identity();

	SDL_GL_SwapWindow(displayWindow);
}
