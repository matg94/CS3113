#include "Player.h"
#include <math.h>

Player::Player() : xPos(1.0f), yPos(-1.0f), xAcc(0.0f), yAcc(0.0f), yVel(0.0f),
xVel(0.0f), width(0.25f), height(0.15f) {


}



void Player::Update(float elapsed) {

	float Friction = 4.00f;

	yVel += yAcc*elapsed;
	xVel += xAcc*elapsed;

	xVel = lerp(xVel, 0.0f, Friction*elapsed);
	yVel = lerp(yVel, 0.0f, Friction*elapsed);

	yPos += yVel*elapsed;

	// Y Collisions

	xPos += xVel*elapsed;

	// X Collisions

	Rotate(xDir, yDir);


}


float Player::lerp(float v0, float v1, float t) {
	return (1.0 - t)*v0 + t*v1;
}

void Player::MoveX(int val) {

	if (abs(val) < 3200) {
		val = 0.0f;
	}
	xAcc = (float)5.0f*val * 1.0f / 32767.0f;

}

void Player::MoveY(int val) {

	if (abs(val) < 3200) {
		val = 0.0f;
	}
	yAcc = -(float)5.0f*val * 1.0f / 32767.0f;

}

void Player::Rotate(float x, float y) {

	if (fabs(x) > 3200 || fabs(y) > 3200) {
		Angle = std::atan2(y, x);
	} 

}