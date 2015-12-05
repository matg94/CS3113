#include "Emitter.h"



Emitter::Emitter(unsigned int count, std::string type) {

	xVel = 0.0f;
	yVel = 0.0f;

	this->type = type;

	for (int i = 0; i < count; ++i) {
		Particle p;
		p.xPos = xPos;
		p.yPos = yPos;

		float Random = (float)rand() / (float)RAND_MAX;
		float RandomY = (float)rand() / (float)RAND_MAX;


		p.sizeDeviation = Random / 10.0f;

		if (type == "FlameThrower") {
			p.lifetime = Random*2.0f;
			p.xVel = 0.0f;
			p.yVel = 0.0f;

			p.startRed = 0.9f + Random;
			p.endRed = Random / 10.0f;

			p.startGreen = 0.3f + (0.2f*Random) - 0.1f;
			p.endGreen = 0.2f*Random - 0.1f;

			p.startBlue = 0.2f + (0.2f*Random) - 0.1f;
			p.endBlue = (0.3f*Random) - 0.15f;

			p.startAlpha = 0.95f + (0.05*Random) - 0.025f;
			p.endAlpha = (0.1f*Random) - 0.05f;

			p.draw = false;
		}
		else {

			p.lifetime = 0.0f;
			p.xVel = 0.0f;
			p.yVel = 0.0f;

			p.startRed = 0.9f + Random;
			p.endRed = Random / 10.0f;

			p.startGreen = 0.3f + (0.2f*Random) - 0.1f;
			p.endGreen = 0.2f*Random - 0.1f;

			p.startBlue = 0.2f + (0.2f*Random) - 0.1f;
			p.endBlue = (0.3f*Random) - 0.15f;

			p.startAlpha = 1.0f;
			p.endAlpha = 0.0f;

			p.draw = false;
		}

		particles.push_back(p);

	}

}

void Emitter::CheckEnemyCollision(std::vector<Enemy>& enemies) {
	
	if (type == "FlameThrower") {

		for (int i = 0; i < particles.size(); ++i) {

			if (particles[i].draw) {

				for (int e = 0; e < enemies.size(); ++e) {

					if (enemies[e].draw) {

						float distance = sqrt(pow((particles[i].xPos - enemies[e].xPos), 2) + pow((particles[i].yPos - enemies[e].yPos), 2));

						if (distance < 0.1f) {

							enemies[e].health -= 0.75f*(maxLifeTime / particles[i].lifetime);
							particles[i].draw = false;

						}
					}
				}
			}
		}
	}


}

void Emitter::Update(float elapsed, Player& player) {


		for (int i = 0; i < particles.size(); ++i) {
			if (activated) {
				particles[i].lifetime += elapsed;
			}
			if (activated && type == "Blood") {
				particles[i].draw = true;
			}

			if (maxLifeTime <= particles[i].lifetime) {

				if (type == "FlameThrower") {
					if (activated) {
						particles[i].draw = true;
						if (type == "Blood") {
							int x = 0;
						}
					}
					else {
						particles[i].draw = false;
					}
				}

				if (type == "FlameThrower") {

					float length = sqrt(pow(player.xDir, 2) + pow(player.yDir, 2));

					particles[i].xVel = player.xVel + 2.5f*(player.xDir / length);
					particles[i].yVel = player.yVel + 2.5f*(player.yDir / length);

					float Random = (float)rand() / (float)RAND_MAX;

					float RandomX = (float)rand() / (float)RAND_MAX;

					particles[i].yVel += (0.5f*Random) - 0.25f;
					particles[i].xVel += (0.5f*RandomX) - 0.25f;

					particles[i].sizeDeviation = Random / 8.0f;
					particles[i].startRed = 0.8f + RandomX;
					particles[i].endRed = 0.09f;

					particles[i].startGreen = 0.4f + (0.2f*Random) - 0.1f;
					particles[i].endGreen = 0.1f + (0.2f*RandomX) - 0.1f;

					particles[i].startBlue = 0.2f + (0.2f*RandomX) - 0.1f;
					particles[i].endBlue = (0.2f*Random) - 0.1f;

					particles[i].startAlpha = 0.95f + (0.05*RandomX) - 0.025f;
					particles[i].endAlpha = (0.1f*Random) - 0.05f;

					particles[i].lifetime = RandomX*0.1f;
					particles[i].xPos = xPos;
					particles[i].yPos = yPos;
				}
				else {

					float Random = (float)rand() / (float)RAND_MAX;

					float RandomX = (float)rand() / (float)RAND_MAX;

					particles[i].sizeDeviation = Random / 5.0f;
					particles[i].startRed = 0.8f + RandomX;
					particles[i].endRed = 0.09f;

					particles[i].startGreen = 0.4f + (0.2f*Random) - 0.1f;
					particles[i].endGreen = 0.1f + (0.2f*RandomX) - 0.1f;

					particles[i].startBlue = 0.2f + (0.2f*RandomX) - 0.1f;
					particles[i].endBlue = (0.2f*Random) - 0.1f;

					particles[i].startAlpha = 1.0f;
					particles[i].endAlpha = 0.0f;

					particles[i].lifetime = 0.0f;

					particles[i].xPos = xPos + (Random - 0.5f);
					particles[i].yPos = yPos + (RandomX - 0.5f);

					particles[i].xVel = 0.0f;
					particles[i].yVel = 0.0f;
					particles[i].lifetime = 0.0f;

					if (count > 0) {
						count = 0;
						activated = false;
					}
					else {
						count += 1;
					}

				}
			}

			particles[i].xPos += particles[i].xVel*elapsed;
			particles[i].yPos += particles[i].yVel*elapsed;

		}

}

void Emitter::Render(ShaderProgram* program, float seed) {

		srand(seed);

		std::vector<float> vertices;
		std::vector<float> texCoords;
		std::vector<float> colors;

		if (type != "FlameThrower" && activated == true) {
			int x = 0;
		}

		for (int i = 0; i < particles.size(); ++i) {

			if (particles[i].draw) {


				float Random = (float)rand() / (float)RAND_MAX;

				float m = (particles[i].lifetime / maxLifeTime);
				float size = lerp(startSize, endSize, m) + particles[i].sizeDeviation;

				vertices.insert(vertices.end(), {
					particles[i].xPos - size, particles[i].yPos + size,
					particles[i].xPos - size, particles[i].yPos - size,
					particles[i].xPos + size, particles[i].yPos + size,

					particles[i].xPos + size, particles[i].yPos + size,
					particles[i].xPos - size, particles[i].yPos - size,
					particles[i].xPos + size, particles[i].yPos - size
				});

				texCoords.insert(texCoords.end(), {

					0.0f, 0.0f,
					0.0f, 1.0f,
					1.0f, 0.0f,

					1.0f, 0.0f,
					0.0f, 1.0f,
					1.0f, 1.0f

				});

				for (int j = 0; j < 6; j++) {
					colors.push_back(lerp(particles[i].startRed, particles[i].endRed, m));
					colors.push_back(lerp(particles[i].startGreen, particles[i].endGreen, m));
					colors.push_back(lerp(particles[i].startBlue, particles[i].endBlue, m));
					colors.push_back(lerp(particles[i].startAlpha, particles[i].endAlpha, m));
				}

			}

		}

				glUseProgram(program->programID);

				glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0,
					vertices.data());
				glEnableVertexAttribArray(program->positionAttribute);

				glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0,
					texCoords.data());
				glEnableVertexAttribArray(program->texCoordAttribute);

				glVertexAttribPointer(program->colorAttribute, 4, GL_FLOAT, false, 0,
					colors.data());
				glEnableVertexAttribArray(program->colorAttribute);

				glBindTexture(GL_TEXTURE_2D, ParticleTextureID);
				glDrawArrays(GL_TRIANGLES, 0, (vertices.size() / 2));

				glDisableVertexAttribArray(program->positionAttribute);
				glDisableVertexAttribArray(program->texCoordAttribute);
				glDisableVertexAttribArray(program->colorAttribute);

}

float Emitter::lerp(float v0, float v1, float t) {
	return (1.0 - t)*v0 + t*v1;
}