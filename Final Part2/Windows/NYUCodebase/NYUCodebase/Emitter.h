#pragma once
#include "Particle.h"
#include "Player.h"
#include "Enemy.h"
#include "ShaderProgram.h"
#include <vector>



class Emitter {

public:
	
	Emitter(unsigned int particleCount, std::string type);

	Emitter();

	~Emitter();

	void Update(float elapsed, Player& player);
	void Render(ShaderProgram*, float);
	void CheckEnemyCollision(std::vector<Enemy>&);

	float xPos;
	float yPos;
	float xVel;
	float yVel;
	float maxLifeTime;
	float power;
	int count;
	std::string type;

	int ParticleTextureID;

	bool activated;

	float startSize;
	float endSize;
	float sizeDeviation;

	float startAlpha;
	float endAlpha;

	float lerp(float, float, float);

	std::vector<Particle> particles;



};