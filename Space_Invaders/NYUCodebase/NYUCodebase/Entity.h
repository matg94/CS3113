#ifndef ENTITY_H
#define ENTITY_H
#include <vector>

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include <string>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <vector>
#include "Entity.h"


class Entity {
public:
	float xPos, yPos;
	float xVel, yVel;
	float width, height;
	int textureID;
	int ExplosionSheet;
	bool active;
	ShaderProgram* program;

	float animationElapsed = 0.0f;

	float Friction;
	float Acceleration_x;
	float Acceleration_y;
	bool exploding;
	bool endScreen = false;
	int currentExplosionIndex;
	int hp;
	int ID;
	
	int type;

	Entity(float, float, float, float, float, float, ShaderProgram*);
	

	float GetAngle();
	void Update(float);
	void DrawSpriteSheetSprite(int, int, int);
	void DrawSprite();
	void MoveX(int);
	void MoveY(int);
	bool CheckCollision(Entity*);
	void end();
	void CollisionResponse(Entity*);
	float lerp(float, float, float);

};
#endif