#pragma once
#include <vector>
#include "ShaderProgram.h"
#include "Chunk.h"

class Player {
public:
	float xPos;
	float yPos;
	float xVel;
	float yVel;
	float xAcc;
	float yAcc;
	float Gravity;
	float Friction;
	int Texture;
	int CurrentAnimationIndex;
	int RunAnimation[4];
	int JumpAnimation[4];
	bool InAir;
	float AnimationElapsed;

	Player();

	void Update(float, Chunk*);
	void Animate();
	void Draw(int, int, int, ShaderProgram*);
	void Jump();
	void CheckCollision(Chunk*, int);
};