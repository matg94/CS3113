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

	Ball(float x, float y, float xV, float yV, float radius) :
		xPos(x), yPos(y), xVel(xV), yVel(yV), rad(radius) {

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
	float starty; 
	float pos = 0.0f;
	float vel = 0.0f;
	float width = 0.1f*0.5f;
	float height = 0.75f*0.5f;
	Ball *ball;

	Panel(float start, Ball *bb) : starty(start), ball(bb) {
		if (starty < 1) {
			starty += width;
		}
		else {
			starty -= width;
		}
	}

	void update() {
		if (ball->yPos > pos) {
			vel = 3.0f;
		}
		else if (ball->yPos < pos) {
			vel = -3.0f;
		}
		if (ball->xPos - ball->rad <= starty + width/2 && ball->xPos + ball->rad >= starty) {
			if (ball->yPos - ball->rad <= pos + height/2 && ball->yPos + ball->rad >= pos - height/2) {
				ball->xVel = -ball->xVel;
				ball->xPos += ball->xVel / 600;
			}
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
	GLuint panelTexID = LoadTexture("red_panel.png");

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Ball *ball = new Ball(0.0f, 0.0f, 3.0f, 3.0f, 0.2f);
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
		}

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

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
		float panelVTwo[] = { -0.1f, -0.75f, 0.1f, 0.75f, -0.1f, 0.75f, 0.1f, 0.75f, -0.1f, -0.75f, 0.1f, -0.75f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, panelVTwo);
		glEnableVertexAttribArray(program.positionAttribute);

		float texPanelOne[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texPanelOne);
		glEnableVertexAttribArray(program.texCoordAttribute);
		panelOneMatrix.Translate(panelOne->starty, panelOne->pos, 0.0f);

		glBindTexture(GL_TEXTURE_2D, panelTexID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
		program.setModelMatrix(panelTwoMatrix);

		// PANEL TWO
		float panelVOne[] = { -0.1f, -0.75f, 0.1f, 0.75f, -0.1f, 0.75f, 0.1f, 0.75f, -0.1f, -0.75f, 0.1f, -0.75f };
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

		panelOne->update();
		panelTwo->update();

		panelOne->pos += panelOne->vel*elapsed;
		panelTwo->pos += panelTwo->vel*elapsed;

		ball->update();
		ball->xPos += ball->xVel*elapsed;
		ball->yPos += ball->yVel*elapsed;

		angle += elapsed;

		modelMatrix.Translate(ball->xPos, ball->yPos, 0.0f);		
		modelMatrix.Scale(0.6f, 0.6f, 1.0f);
		modelMatrix.Rotate(angle);

		panelOneMatrix.Translate(panelOne->starty, panelOne->pos, 0.0f);
		panelOneMatrix.Scale(0.5f, 0.5f, 1.0f);

		panelTwoMatrix.Translate(panelTwo->starty, panelTwo->pos, 0.0f);
		panelTwoMatrix.Scale(0.5f, 0.5f, 1.0f);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
