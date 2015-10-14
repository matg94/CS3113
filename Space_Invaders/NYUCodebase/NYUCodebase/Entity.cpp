#include "Entity.h"

Entity::Entity(float x, float y, float xv, float yv, float w, float h, ShaderProgram* p)
	: xPos(x), yPos(y), xVel(xv), yVel(yv), width(w), height(h), 
	program(p), active(false), Friction(0.05f), type(0), exploding(false), currentExplosionIndex(0),
	Acceleration_x(0.0f), Acceleration_y(0.0f) {
	
};


void Entity::DrawSpriteSheetSprite(int index, int countX, int countY) {

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

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0,
		vertices.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0,
		texCoords.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	if (exploding) {
		glBindTexture(GL_TEXTURE_2D, ExplosionSheet);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);

}

void Entity::DrawSprite() {

	std::vector<float> vertices;
	std::vector<float> texCoords;

	vertices.insert(vertices.end(), { -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5,
		-0.5, -0.5, 0.5, -0.5 });

	texCoords.insert(texCoords.end(), {
			0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
	});

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0,
		vertices.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0,
		texCoords.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);

}

void Entity::Update(float elapsed) {
	if (yPos > fabs(2.0f) && type == 3) {
		active = false;
	}
	if (exploding) {
		if (active) {
			animationElapsed += elapsed;
			xVel = 0.0f;
			yVel = 0.0f;
		}
	}
	xVel += Acceleration_x*elapsed;
	yVel += Acceleration_y*elapsed;
	if (type == 1) {
		yVel = lerp(yVel, 0.0f, Friction);
		xVel = lerp(xVel, 0.0f, Friction);
	}
	xPos += xVel*elapsed*5.0f;
	yPos += yVel*elapsed*5.0f;
	if (hp <= 0 && type == 1) {
		end();
	}
}

void Entity::MoveX(int pow) {
	if (abs(pow) < 3200) {
		pow = 0.0f;
	}
	Acceleration_x = (float)2.0f*pow * 1.0f / 32767.0f;
}



float Entity::lerp(float v0, float v1, float t) {
	return v0 + t*(v1 - v0);
}

float Entity::GetAngle() {
	float angle = atan(yVel / xVel);
	if (xVel > 0.0f) {
		angle += 3.145f;
	}
	return angle;
}

bool Entity::CheckCollision(Entity* other) {

	if ((xPos + width) < (other->xPos - other->width)) {
		return false;
	}
	if (xPos - width > other->xPos + other->width) {
		return false;
	}
	if ((yPos + height) < (other->yPos - other->height)) {
		return false;
	}
	if (yPos - height > other->yPos + other->height) {
		return false;
	}
	return true;
}

void Entity::CollisionResponse(Entity* other) {
	if (other->active) {
		if (CheckCollision(other)) {
			if (type == 1 && other->type == 2) {
				if (!other->exploding) {
					other->active = false;
					hp -= 1;
					if (hp == 0) {
						end();
					}
				}
			}
			else if (type == 2 && other->type == 3) {
				other->exploding = true;
				active = false;
			}
		}
	}

}

void Entity::end() {
	
	if (type == 1) {
		endScreen = true;
		exploding = true;
	}

}