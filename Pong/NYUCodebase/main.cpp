#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"


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

class Ball {
public:
	float xPos, yPos, xVel, yVel;
	float rad;
	float angleV;

	Ball(float x, float y, float radius, float angle) :
		xPos(x), yPos(y), rad(radius), angleV(angle) {
		xVel = cos(angleV);
		yVel = sin(angleV);
	}

	void update() {

		if (yPos + rad >= 2.0 || yPos - rad <= -2.0) {
			yVel = -yVel;
		}
		if (xPos + rad >= 3.55 || xPos - rad <= -3.55) {
			reset();
		}
	}
	void reset() {
		xPos = 0.0f;
		yPos = 0.0f;
	}

};


class Panel {
public:
	float startx;
	float pos = 0.0f;
	float radius = 0.5f;
	float distance;
	float vel = 0.0f;
	bool dir = 1;
	Ball *ball;

	Panel(float start, Ball *bb) : startx(start), ball(bb) {

	}

	void update() {
		float xdist, ydist;
		xdist = startx - ball->xPos;
		ydist = pos - ball->yPos;

		distance = sqrt(pow(xdist, 2.0f) + pow(ydist, 2));
		if (distance - radius - ball->rad < 0.0f) {
			float angleCol = atan(ydist / xdist);
			if (startx > 0.0f) {
				ball->xVel = cos(angleCol)*(-ball->xVel / ball->xVel);
			}
			else {
				ball->xVel = cos(angleCol)*(ball->xVel / ball->xVel);
			}
			ball->yVel = sin(angleCol);

			if (startx > 0.0f) {
				ball->xPos -= 0.05f;
			}
			else {
				ball->xPos += 0.05f;
			}

			ball->yVel += vel/3.0f;
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

	Matrix panelOneMatrix;
	Matrix panelTwoMatrix;

	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	GLuint textureID = LoadTexture("ball.png");
	GLuint panelTexID = LoadTexture("elementMetal004.png");

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Ball *ball = new Ball(0.0f, 0.0f, 0.1f, (3.1415f*0.25f));
	Panel *panelOne = new Panel(-3.55f, ball);
	Panel *panelTwo = new Panel(3.55f, ball);

	float lastFrameTicks = 0.0f;
	float angle = 0.0f;

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
					panelOne->vel = 1.2f;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
					panelOne->vel = -1.2f;
				}
			}
			else if (event.type == SDL_KEYUP) {
				panelOne->vel = 0.0f;
			}
		}

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		panelOne->update();
		panelTwo->update();
		panelOne->pos += panelOne->vel*elapsed;
		if (ball->xVel > 0) {
			if (panelTwo->pos < ball->yPos) {
				panelTwo->vel = 2.0f;
			}

			else if (panelTwo->pos > ball->yPos) {
				panelTwo->vel = -2.0f;
			}
		}
		else {
			panelTwo->vel = 0.0f;
		}


		panelTwo->pos += panelTwo->vel*elapsed;

		ball->update();
		ball->xPos += ball->xVel*elapsed*2.0f;
		ball->yPos += ball->yVel*elapsed*2.0f;

		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		glUseProgram(program.programID);

		glClearColor(0.2f, 0.2f, 0.2, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		// BALL
		float vertices[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float texCoords[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);


		glBindTexture(GL_TEXTURE_2D, textureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		program.setModelMatrix(panelOneMatrix);
		// PANEL ONE
		float panelVTwo[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, panelVTwo);
		glEnableVertexAttribArray(program.positionAttribute);

		float texPanelOne[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texPanelOne);
		glEnableVertexAttribArray(program.texCoordAttribute);
		panelOneMatrix.Translate(panelOne->startx, panelOne->pos, 0.0f);

		glBindTexture(GL_TEXTURE_2D, panelTexID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
		program.setModelMatrix(panelTwoMatrix);

		// PANEL TWO
		float panelVOne[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, panelVOne);
		glEnableVertexAttribArray(program.positionAttribute);

		float texPanelTwo[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texPanelTwo);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, panelTexID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		modelMatrix.identity();
		panelOneMatrix.identity();
		panelTwoMatrix.identity();

		modelMatrix.Translate(ball->xPos, ball->yPos, 0.0f);
		modelMatrix.Scale(0.2f, 0.2f, 1.0f);
		modelMatrix.Rotate(angle);

		panelOneMatrix.Translate(panelOne->startx, panelOne->pos, 0.0f);

		panelTwoMatrix.Translate(panelTwo->startx, panelTwo->pos, 0.0f);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
