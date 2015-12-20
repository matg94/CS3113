#include "Enemy.h"

Enemy::Enemy() : xAcc(0.0f), yAcc(0.0f), yVel(0.0f),
xVel(0.0f), width(0.25f), height(0.15f), HearingRadius(3.5f*0.4f), SightRadius(4.0f*0.4f), health(250), draw(true) {

	xPos = 0.4f*(rand() % (29 - 8) + 8);
	yPos = -0.4f*(rand() % (29 - 8) + 8);

	if (xPos >= (29 * 0.4f)) {
		xPos = (29 * 0.4f);
	} if (yPos <= -(29 * 0.4f + 0.2f)) {
		yPos = -(29 * 0.4f + 0.2f);
	} if (xPos <= 0.2f) {
		xPos = 0.2f;
	} if (yPos >= 0.2f) {
		yPos = 0.2f;
	}
	
	xDir = 0.0f;
	yDir = 0.0f;

	wanderingTime = (rand() % (6)+ 2);

	ZombieAlert = Mix_LoadWAV("ZombieAlert.wav");

	State = "Wandering";

}

void Enemy::Update(float elapsed, Player* player, int speed) {

	float DistanceToPlayer = sqrt(pow((xPos - player->xPos),2) + pow((yPos - player->yPos),2));

	if (xPos >= (29 * 0.4f)) {
		xPos = (29 * 0.4f);
	} if (yPos <= -(29 * 0.4f + 0.2f)) {
		yPos = -(29 * 0.4f + 0.2f);
	} if (xPos <= 0.2f) {
		xPos = 0.2f;
	} if (yPos >= 0.2f) {
		yPos = 0.2f;
	}


	if (draw) {

		wanderingTime += elapsed;

		if (DistanceToPlayer > HearingRadius*2.0f) {

			State = "Wandering";

		}

		if (State == "Wandering") {

			float RandomX = (float)rand() / (float)RAND_MAX;
			float RandomY = (float)rand() / (float)RAND_MAX;

			if (wanderingTime >= 5.0f) {
				wanderingTime = 0.0f;

				xDir = RandomX*2.0f - 1.0f;
				yDir = RandomY*2.0f - 1.0f;

				Rotate(-xDir, -yDir);

				xAcc = (0.75f*xDir);
				yAcc = (0.75f*yDir);

			}

			if (DistanceToPlayer <= HearingRadius*2.0f) {

				State = "Aware";

			}

		}
		else if (State == "Aware") {

			xAcc = 0.0f;
			yAcc = 0.0f;

			xDir = (xPos - player->xPos);
			yDir = (yPos - player->yPos);

			Rotate(xDir, yDir);

			if (DistanceToPlayer <= SightRadius) {
				State = "Attacking";
				Mix_PlayChannel(-1, ZombieAlert, 0);
			}

		}
		else if (State == "Attacking") {

			xDir = (xPos - player->xPos);
			yDir = (yPos - player->yPos);

			Rotate(xDir, yDir);

			xAcc = speed*(-xDir / DistanceToPlayer);
			yAcc = speed*(-yDir / DistanceToPlayer);

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

	}

}

float Enemy::lerp(float v0, float v1, float t) {
	return (1.0 - t)*v0 + t*v1;
}

void Enemy::Rotate(float x, float y) {

	Angle = std::atan2(-y,x);
	Angle += 3.1415f;


}