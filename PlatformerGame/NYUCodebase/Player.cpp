#include "Player.h"

Player::Player() :
Gravity(-0.02f), Friction(1.15f),
CurrentAnimationIndex(9), yAcc(0.0f), xAcc(0.0f), xPos(0.0f),
yPos(0.0f), xVel(1.0f), yVel(0.0f), AnimationElapsed(0.0f), Position(0) {

	CollidedLeft = false;
	CollidedRight = false;
	CollidedTop = false;
	CollidedBottom = false;

}

float Player::lerp(float v0, float v1, float t) {
	return v0 + t*(v1 - v0);
}

void Player::Update(float elapsed, Chunk* chunk) {

	yVel += Gravity;

	if (xPos < 0.0f) {
		xPos = 0.0f;
	}	

	yVel += yAcc*elapsed;
	yPos += yVel*elapsed;
	CheckCollision(chunk, 0);
	// Check Y Collisions

	xVel += xAcc*elapsed;
	xPos += xVel*elapsed;

	xVel = lerp(xVel, 0.0f, Friction*elapsed);

	CheckCollision(chunk, 1);

}

void Player::CheckCollision(Chunk* chunk, int dir) {
	
	if (dir == 0) {
		int playerGridX = (int)((xPos) / 0.5f);
		int playerGridY = (int)(-(yPos - 0.25f) / 0.5f);
		playerGridX = abs(playerGridX);
		if (chunk->Data[playerGridY][playerGridX]->type != 0) {
			yVel = 0.0f;
			float penetration = ((float)playerGridY - 0.25f) - (2.0f*(-yPos)+0.25f);
			yPos -= penetration + 0.001f;
			CollidedBottom = true;
		}
		else {
			CollidedBottom = false;
		}
			playerGridY = (int)(-(yPos + 0.25f) / 0.5f);
			if (chunk->Data[playerGridY][playerGridX]->type != 0) {
				yVel = 0.0f;
				float penetration = 0.0f;
				yPos -= penetration + 0.001f;
				CollidedTop = true;
			}
			else {
				CollidedTop = false;
			}

	}
	else {
		int playerGridX = (int)(((xPos) + 0.10f) / 0.5f);
		int playerGridY = (int)(-(yPos) / 0.5f);
		playerGridX = abs(playerGridX);
		if (chunk->Data[playerGridY][playerGridX]->type != 0) {
			xVel = 0.0f;
			float penetration = ((float)playerGridX - 0.25f) - (2.0f*(xPos) + 0.10f);
			xPos += penetration;
			xPos -= 0.001f;
			CollidedRight = true;
		}
		else {
			CollidedRight = false;
		}
		
		playerGridX = (int)(((xPos) - 0.10f) / 0.5f);
		playerGridX = abs(playerGridX);
		if (chunk->Data[playerGridY][playerGridX]->type != 0) {
			xVel = 0.0f;
			float penetration = (playerGridX + 0.25f) - (2.0f*(xPos) - 0.98f);
			xPos += penetration;
			xPos += 0.001f;
			CollidedLeft = true;
		}
		else {
			CollidedLeft = false;
		}
	}

}


void Player::Animate(float elapsed) {

	AnimationElapsed += elapsed;

	if (CollidedBottom) {
		if (fabs(xVel) >= 0.2f) {
			if (AnimationElapsed >= 1.0f / 10.0f) {
				++CurrentAnimationIndex;
				AnimationElapsed = 0.0f;
			}
			if (CurrentAnimationIndex >= 12) {
				CurrentAnimationIndex = 8;
			}
		}
		else {
			CurrentAnimationIndex = 8;
		}
	}
	else {
		if (yVel > 0) {
			CurrentAnimationIndex = 13;
		}
		else if (yVel < 0) {
			CurrentAnimationIndex = 14;
		}
	}

}

void Player::Jump() {

	CurrentAnimationIndex = 12;
	yVel = 2.0f;

}

void Player::Draw(int index, int countX, int countY, ShaderProgram* program) {


	float u = (float)(((int)index) % countX) / (float)countX;
	float v = (float)(((int)index) / countX) / (float)countY;
	float spriteHeight = 1.0f / (float)countY;
	float spriteWidth = 1.0f / (float)countX;

	std::vector<float> vertices;
	std::vector<float> texCoords;

	vertices.insert(vertices.end(), { -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5,
		-0.5, -0.5, 0.5, -0.5 });

	texCoords.insert(texCoords.end(), {
		u, v + spriteHeight,
		u + spriteWidth, v,
		u, v,
		u + spriteWidth, v,
		u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight
	});

	glUseProgram(program->programID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0,
		vertices.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0,
		texCoords.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, Texture);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);


}