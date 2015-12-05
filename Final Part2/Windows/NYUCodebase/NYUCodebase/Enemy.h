#pragma once
#include "ShaderProgram.h"
#include "Matrix.h"
#include "Player.h"



class Enemy {

public:

	float xAcc, yAcc;
	float xPos, yPos;
	float xVel, yVel;
	float xRotation, yRotation, Angle;
	float width, height;
	int textureID;
	float xDir, yDir;
	bool draw;

	float health;

	float SightRadius;
	float HearingRadius;

	std::string State;

	Matrix modelMatrix;

	Enemy();
	void Update(float, Player&);
	float lerp(float, float, float);
	void Rotate(float x, float y);


};