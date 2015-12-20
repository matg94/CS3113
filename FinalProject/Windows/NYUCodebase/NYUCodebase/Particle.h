#pragma once

class Particle {

public:

	float xPos;
	float yPos;
	float yVel;
	float xVel;
	float lifetime;
	float sizeDeviation;

	float startRed;
	float endRed;

	float startBlue;
	float endBlue;
	
	float startGreen;
	float endGreen;
	
	float startAlpha;
	float endAlpha;

	bool draw;
};