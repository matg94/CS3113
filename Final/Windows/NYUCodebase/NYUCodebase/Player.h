#pragma once
#include "ShaderProgram.h"
#include "Matrix.h"


class Player {

public:

	float xPos, yPos;
	float xVel, yVel;
	float xAcc, yAcc;
	float xRotation, yRotation, Angle;
	float width, height;
	int textureID;
	float xDir, yDir;

	Matrix modelMatrix;

	Player();
	void Update(float);
	float lerp(float, float, float);

	void MoveX(int val);
	void MoveY(int val);
	void Rotate(float x, float y);
	void CollisionResponse(int x, int y, bool yAxis);
};