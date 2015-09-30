#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include <math.h>
#include <vector>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;


GLuint LoadTexture(const char *image_path) {
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

class Player;

class Food {
public:
	float xPos;
	float yPos;
	float radius = 0.2f;
	bool eaten = false;

	Food(float x, float y) : xPos(x), yPos(y) {

	}

	float getDist(float x, float y) {
		float xDist = xPos - x;
		float yDist = yPos - y;
		float temp = std::pow(xDist, 2) + std::pow(yDist, 2);
		float dist = sqrt(temp);

		return dist;
	}

	float getAngle(float x, float y) {
		float xDist, yDist;
		xDist = xPos - x;
		yDist = yPos - y;


		return atan(yDist / xDist);
	}

};

class Player {
public:
	float energy;
	float xPos, yPos;
	float xVel = 0.0f;
	float yVel = 0.0f;
	float angle = 0.0f;
	float height = 0.5f;
	float width = 0.2f;
	float tarDis = -1;
	int directionX = 1;
	int directionY = 1;
	int index = -1;

	Player(float x, float y, float ener) : xPos(x), yPos(y)
		, energy(ener) {

	}

	void findFood(std::vector<Food*>& foods) {
		directionX = 1;
		directionY = 1;
		for (int i = 0; i < foods.size(); ++i) {
			if (!foods[i]->eaten) {
				
				float dist = foods[i]->getDist(xPos, yPos);

				if (index == -1 || dist <= foods[index]->getDist(xPos, yPos)) {
					index = i;
					if (xPos > foods[index]->xPos) {
						directionX = -1;
					}
					else { directionX = 1; }
					if (yPos > foods[index]->yPos) {
						directionY = -1;
					}
					else { directionY = 1; }
				}
				float ddist = foods[index]->getDist(xPos, yPos);
				if (ddist <= 0.3) {
					energy += 5.0f;
					foods[index]->eaten = true;
					index = -1;
				}

			}
		}
		if (index >= 0)
			angle = foods[index]->getAngle(xPos, yPos);
	}

	void move(float elapsed) {
		printf("Angle:  %f\n ", angle*(180.0f/3.1415));
		xVel = 0.0f;
		yVel = 0.0f;
		if (energy > 0 && index != -1) {
			xVel = cos(angle);
			yVel = sin(angle);
			xPos += xVel*elapsed*0.5f*directionX;
			yPos += yVel*elapsed*0.5f*directionY;
			energy -= 0.1f*elapsed;
		}
	}
};





int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 360);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;

	Matrix foodMatrix;

	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	GLuint textureID = LoadTexture("tallShroom_red.png");
	GLuint panelTexID = LoadTexture("playerRed1.png");

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Player *player = new Player(0.0f, 0.0f, 1000.0f);
	std::vector<Food*> foods;

	float lastFrameTicks = 0.0f;
	int msX, msY;

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			if (event.type == SDL_MOUSEBUTTONUP) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					SDL_GetMouseState(&msX, &msY);

					float x = (float)msX;
					x = (x-320.0f) / (640.0f / 7.1f);
					

					float y = (float)msY;
					y = (y-180.0f) / (360.0f / 4.0f);


					foods.push_back(new Food(x, -y));
				}
			}
		}

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		glUseProgram(program.programID);

		glClearColor(0.2f, 0.2f, 0.2, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		// Food
		for (int i = 0; i < foods.size(); ++i) {

			// ---
			if (foods[i] && !foods[i]->eaten) {
				program.setModelMatrix(modelMatrix);
				float vertices2[] = { -0.2f, -0.2f, 0.2f, 0.2f, -0.2f, 0.2f, 0.2f, 0.2f, -0.2f, -0.2f, 0.2f, -0.2f };
				glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
				glEnableVertexAttribArray(program.positionAttribute);

				float texCoords2[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
				glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
				glEnableVertexAttribArray(program.texCoordAttribute);

				modelMatrix.identity();

				glBindTexture(GL_TEXTURE_2D, textureID);

				modelMatrix.Translate(foods[i]->xPos, foods[i]->yPos, 0.0f);


				glDrawArrays(GL_TRIANGLES, 0, 6);

				glDisableVertexAttribArray(program.positionAttribute);
				glDisableVertexAttribArray(program.texCoordAttribute);
			}

		}
		program.setModelMatrix(foodMatrix);
		// Player
		float panelVOne[] = { -0.2f, -0.3f, 0.2f, 0.3f, -0.2f, 0.3f, 0.2f, 0.3f, -0.2f, -0.3f, 0.2f, -0.3f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, panelVOne);
		glEnableVertexAttribArray(program.positionAttribute);

		float texPanelTwo[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texPanelTwo);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, panelTexID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
		player->findFood(foods);

		foodMatrix.identity();
		player->move(elapsed);
		foodMatrix.Translate(player->xPos, player->yPos, 0.0f);

		foodMatrix.Rotate(player->angle);

		if (player->xVel < 0) {
			foodMatrix.Rotate(3.1415f);
		}
		



		foodMatrix.Rotate(player->angle);
		
		foodMatrix.Scale(0.5f, 0.5f, 1.0f);
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
