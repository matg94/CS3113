#include "Player.h"

Player::Player() :
Gravity(-0.02f), Friction(0.0f), InAir(false), 
	CurrentAnimationIndex(9), yAcc(0.0f), xAcc(0.5f), xPos(0.0f),
	yPos(0.0f), xVel(0.0f), yVel(0.0f) {
}

void Player::Update(float elapsed, Chunk* chunk) {

	yVel += Gravity;

	yVel += yAcc*elapsed;
	yPos += yVel*elapsed;
	CheckCollision(chunk, 0);
	// Check Y Collisions

	xVel += xAcc*elapsed;
	xPos += xVel*elapsed;

	printf("  [  %f  ] [  %f  ] \n", xPos, yPos);
	CheckCollision(chunk, 1);
	// Check X Collisions

}

void Player::CheckCollision(Chunk* chunk, int dir) {

	int playerGridX = (int)(xPos / 0.5f);
	int playerGridY = (int)(-yPos / 0.5f);
	int tileY = playerGridY+1.0f;
	int tileX = playerGridX+1.0f;

	printf(" colliding with : [%d][%d] \n", playerGridX, tileY);

	// Size of a tile is 0.4f

	if (chunk->Data[tileY][playerGridX]->type != 0) {
		if (dir == 0) {
			if (yPos <= (tileY/2.0f)-0.25f) {
				yVel = 0.0f;
				yPos = -tileY / 2.0f;
				yPos += 0.25f;
			}
		}
	}
	else if (chunk->Data[tileY][tileX+1]->type != 0) {
		if (xPos*2.0f >= tileX+1 - 0.5f) {
			xVel = 0.0f;
		}
	}

}

void Player::Animate() {

	if (InAir) {
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
	yVel = 10.0f;

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