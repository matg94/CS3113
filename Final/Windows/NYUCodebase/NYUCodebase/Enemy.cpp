#include "Enemy.h"

Enemy::Enemy() : xAcc(0.0f), yAcc(0.0f), yVel(0.0f),
xVel(0.0f), width(0.25f), height(0.15f), HearingRadius(3.5f*0.4f), SightRadius(2.5f*0.4f) {

	xPos = 0.4f*(rand() % (29 - 8) + 8);
	yPos = -0.4f*(rand() % (29 - 8) + 8);
	
	xDir = 0.0f;
	yDir = 0.0f;

	State = "Wandering";

}

void Enemy::Update(float elapsed, Player& player) {

	float DistanceToPlayer = sqrt(pow((xPos - player.xPos),2) + pow((yPos - player.yPos),2));


	if (DistanceToPlayer > HearingRadius*2.0f) {
		State = "Wandering";
		
	}

	if (State == "Wandering") {

		if (DistanceToPlayer <= HearingRadius*2.0f) {

			State = "Aware";

		}

	}
	else if (State == "Aware") {

		xDir = (xPos - player.xPos);
		yDir = (yPos - player.yPos);

		Rotate(xDir, yDir);

		if (DistanceToPlayer <= SightRadius) {
			State = "Attacking";
		}

	}
	else if (State == "Attacking") {

		xDir = (xPos - player.xPos);
		yDir = (yPos - player.yPos);

		Rotate(xDir, yDir);

		xAcc = 2.5f*(-xDir / DistanceToPlayer);
		yAcc = 2.5f*(-yDir / DistanceToPlayer);

		if (DistanceToPlayer >= SightRadius) {
			State = "Aware";
			xAcc = 0.0f;
			yAcc = 0.0f;
		}

	}

	float Friction = 4.00f;

	yVel += yAcc*elapsed;
	xVel += xAcc*elapsed;

	xVel = lerp(xVel, 0.0f, Friction*elapsed);
	yVel = lerp(yVel, 0.0f, Friction*elapsed);

	yPos += yVel*elapsed;

	// Y Collisions

	xPos += xVel*elapsed;

	// X Collisions

	// Rotate(xDir, yDir);


}

float Enemy::lerp(float v0, float v1, float t) {
	return (1.0 - t)*v0 + t*v1;
}

void Enemy::Rotate(float x, float y) {

	Angle = std::atan2(-y,x);
	Angle += 3.1415f;


}