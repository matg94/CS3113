#include "Application.h"



void Application::Initialize() {

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640*2, 360*2, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	// SDL_SetWindowFullscreen(displayWindow, SDL_WINDOW_FULLSCREEN);
	// SDL_ShowCursor(0);

	program = new ShaderProgram(RESOURCE_FOLDER"vertex_colored.glsl", RESOURCE_FOLDER"fragment_textured_colored.glsl");
	programText = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glViewport(0, 0, 640*2, 360*2);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 1024);

	playerOneController = SDL_JoystickOpen(0);

	//projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, 1.0f, -1.0f);

	projectionMatrix.setPerspectiveProjection(0.78f, 1.7777f, 1.0f, -1.0f);


	glEnable(GL_BLEND);

	BloodSpatterSound = Mix_LoadWAV("ZombieAttack.wav");
	OutOfMana = Mix_LoadWAV("OutOfMana.wav");
	ZombieHit = Mix_LoadWAV("ZombieHit.wav");
	ZombieDeath = Mix_LoadWAV("ZombieDeath.wav");
	LightSound = Mix_LoadWAV("SendLight.wav");
	PauseSound = Mix_LoadWAV("PauseSound.wav");

	FireSound = Mix_LoadMUS("FireSound.mp3");

	printf("Mix_LoadWAV: %s\n", Mix_GetError());
	quit = false;

	font = LoadTexture("font1.png");

	GameManager = new GameState(100, 100, 3.5f, 250);
	GameManager->NumberOfEnemies = 50;
	GameManager->EnemiesLeft = 50;
	GameManager->MainMenu = true;
	GameManager->EndMenu = false;
	GameManager->MainGame = false;
	GameManager->Paused = true;
	GameManager->ActivatedLightSpell = false;

	player = new Player();
	player->textureID = LoadTexture("playerRed1.png");

	// FireSound = Mix_LoadWAV("Fire.wav");

}

void Application::DeathScreen() {

	std::string DeathMessage = "The zombies have consumed you...";
	std::string DeathMessage2 = "Better luck in the next apocalypse!";
	std::string DeathMessage3 = "Press Select to return to Main Menu.";

	int score = ((GameManager->EnemyHealth*GameManager->NumberOfKills*50) / 1000);

	std::string EnemiesKilled = "You have killed : " + std::to_string(GameManager->NumberOfKills) + " Zombies";
	std::string Score = "Your score is : " + std::to_string(score);

	std::vector<std::string> Messages;

	Messages.push_back(DeathMessage);
	Messages.push_back(DeathMessage2);

	Messages.push_back(EnemiesKilled);
	Messages.push_back(Score);
	Messages.push_back(DeathMessage3);

	glClear(GL_COLOR_BUFFER_BIT);

	viewMatrix.identity();
	viewMatrix.Translate(0.0f, 0.0f, -5.0f);

	programText->setProjectionMatrix(projectionMatrix);
	programText->setViewMatrix(viewMatrix);

	for (int i = 0; i < Messages.size(); ++i) {

		Matrix textMatrix;

		textMatrix.identity();

		float spacing = (Messages[i].size()*-0.08f);
		float x = (((float)Messages[i].size()) / 2.0f)*0.18f + (spacing / 2.0f);
		float y = (1.2f - (i*0.6f));

		textMatrix.Translate(-x, y, 0.0f);

		programText->setProjectionMatrix(projectionMatrix);
		programText->setViewMatrix(viewMatrix);
		programText->setModelMatrix(textMatrix);

		DrawText(font, Messages[i], 0.18f, -0.08f);

	}

	SDL_GL_SwapWindow(displayWindow);

}

void Application::MainMenu() {

	std::string mainMenuPlay = "Please press Start to play.";
	std::string mainMenuQuit = "Please press Select to Quit.";

	std::vector<std::string> Messages;

	Messages.push_back(mainMenuPlay);
	Messages.push_back(mainMenuQuit);

	glClear(GL_COLOR_BUFFER_BIT);

	viewMatrix.identity();
	viewMatrix.Translate(0.0f, 0.0f, -5.0f);

	programText->setProjectionMatrix(projectionMatrix);
	programText->setViewMatrix(viewMatrix);


	for (int i = 0; i < Messages.size(); ++i) {

		Matrix textMatrix;

		textMatrix.identity();

		float spacing = (Messages[i].size()*-0.08f);
		float x = (((float)Messages[i].size()) / 2.0f)*0.18f + (spacing / 2.0f);
		float y = (1.2f - (i*0.6f));

		textMatrix.Translate(-x, y, 0.0f);

		programText->setProjectionMatrix(projectionMatrix);
		programText->setViewMatrix(viewMatrix);
		programText->setModelMatrix(textMatrix);

		DrawText(font, Messages[i], 0.18f, -0.08f);

	}

	SDL_GL_SwapWindow(displayWindow);

}

void Application::Loop() {

	glClear(GL_COLOR_BUFFER_BIT);

	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - LastFrameTick;
	LastFrameTick = ticks;
	if (startingGame) {
		elapsed = 0.001f;
		startingGame = false;
	}

	TimeSinceLastHit += elapsed;

	if (!GameManager->Paused) {
		if (FireEmitter->activated && GameManager->CurrentMana > 0.0f) {
			GameManager->CurrentMana -= 40.0f * elapsed;
		}
		if (GameManager->CurrentMana < GameManager->MaxMana) {
			GameManager->CurrentMana += 10.0f * elapsed;
		}
		if (GameManager->CurrentMana <= 0) {
			Mix_PlayChannel(-1, OutOfMana, 0);
			FireEmitter->activated = false;
		}
	}
	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);
	program->setModelMatrix(modelMatrix);

	modelMatrix.identity();
	viewMatrix.identity();

	if (GameManager->EnemiesLeft <= GameManager->NumberOfEnemies) {
		for (int i = 0; i < enemies.size(); ++i) {
			if (enemies[i].draw == false) {

				float DistanceToPlayer = sqrt(pow((enemies[i].xPos - player->xPos), 2) + pow((enemies[i].yPos - player->yPos), 2));

				if (DistanceToPlayer >= 15*0.4f) {
					enemies[i].draw = true;
					enemies[i].health = GameManager->EnemyHealth;
					enemies[i].State = "Wandering";
					GameManager->EnemiesLeft += 1;
				}
			}
		}
	}
		 
	float FixedTimeStep = 1 / 60.0f;
	float fixedElapsed = elapsed;
	if (fixedElapsed > FixedTimeStep * 6) {
		fixedElapsed = FixedTimeStep * 6;
	}

	if (GameManager->Paused) {

	}
	else {

		while (fixedElapsed >= FixedTimeStep) {
			fixedElapsed -= FixedTimeStep;
			player->Update(fixedElapsed, GameManager->PlayerSpeed);
			for (int i = 0; i < enemies.size(); ++i) {
				enemies[i].Update(fixedElapsed, player, GameManager->EnemySpeed);
			}
		}

		player->Update(elapsed, GameManager->PlayerSpeed);

		FireEmitter->CheckEnemyCollision(enemies);
		if (BloodEmitter->activated) {
			BloodEmitter->Update(elapsed, player);
		}


		for (int i = 0; i < enemies.size(); ++i) {
			enemies[i].Update(elapsed, player, GameManager->EnemySpeed);

			float distanceToPlayer = sqrt(pow((enemies[i].xPos - player->xPos), 2) + pow((enemies[i].yPos - player->yPos), 2));
			if (enemies[i].draw) {
				if (distanceToPlayer <= 0.125f) {
					if (TimeSinceLastHit >= 1.0f) {
						GameManager->CurrentHealth -= GameManager->EnemyDamage;
						Mix_PlayChannel(-1, ZombieHit, 0);

						TimeSinceLastHit = 0;
						if (GameManager->CurrentHealth <= 0) {
							GameManager->MainGame = false;
							GameManager->EndMenu = true;
						}
					}
				}
			}
			if (enemies[i].health <= 0 && enemies[i].draw == true) {
				if (GameManager->NumberOfKills >= 20) {
					GameManager->EnemyDamage += 1;
					GameManager->EnemySpeed += 0.1f;
					GameManager->EnemyHealth += 1;
				}
				GameManager->EnemiesLeft -= 1;
				GameManager->NumberOfKills += 1;
				Mix_PlayChannel(-1, ZombieDeath, 0);
				BloodEmitter->xPos = enemies[i].xPos;
				BloodEmitter->yPos = enemies[i].yPos;
				BloodEmitter->activated = true;
				for (int x = 0; x < BloodEmitter->particles.size(); ++x) {
					BloodEmitter->particles[x].draw = true;
					BloodEmitter->particles[x].lifetime = 5.0f;
					BloodEmitter->count = 0;
				}
				enemies[i].draw = false;
			}

		}

		CheckCollision();

		FireEmitter->Update(elapsed, player);
	}
		FireEmitter->xPos = player->xPos;
		FireEmitter->yPos = player->yPos;

		lights[0].xPos = player->xPos;
		lights[0].yPos = player->yPos;
		lights[4].xPos = player->xPos;
		lights[4].yPos = player->yPos;

		if (GameManager->ActivatedLightSpell) {

			float length = sqrt(pow(player->xDir, 2) + pow(player->yDir, 2));
			float xDirection = player->xDir / length;
			float yDirection = player->yDir / length;

			GameManager->LightSpell = true;
			lights[5].xPos = player->xPos + 2.1f*(xDirection);
			lights[5].yPos = player->yPos + 2.1f*(yDirection);;
			lights[4].xPos = player->xPos + 2.3f*(xDirection);
			lights[4].yPos = player->yPos + 2.3f*(yDirection);;
			GameManager->CurrentMana -= 20;
			GameManager->ActivatedLightSpell = false;
			GameManager->LightSpellTime = 0.0f;
		} 
		if (GameManager->LightSpell) {
			if (GameManager->LightSpellTime >= 5.0f) {
				GameManager->LightSpell = false;
				lights[5].xPos = -50.0f;
				lights[5].yPos = -50.0f;
				lights[4].xPos = -50.0f;
				lights[4].yPos = -50.0f;
			}
			else {
				GameManager->LightSpellTime += elapsed;
			}
		}

		if (FireEmitter->activated && GameManager->Paused == false) {
			float length = sqrt(pow(player->xDir, 2) + pow(player->yDir, 2));
			float xDirection = player->xDir / length;
			float yDirection = player->yDir / length;
			lights[1].xPos = player->xPos + 0.75f*(xDirection);
			lights[1].yPos = player->yPos + 0.75f*(yDirection);
			lights[2].xPos = player->xPos + 1.75f*(xDirection);
			lights[2].yPos = player->yPos + 1.75f*(yDirection);
			lights[3].xPos = player->xPos + 1.25f*(xDirection);
			lights[3].yPos = player->yPos + 1.25f*(yDirection);

		}
		else {
			lights[1].xPos = -15.0f;
			lights[1].yPos = -15.0f;

			lights[2].xPos = -15.0f;
			lights[2].yPos = -15.0f;

			lights[3].xPos = -15.0f;
			lights[3].yPos = -15.0f;
		}


		GLint lightPositionsUniform = glGetUniformLocation(program->programID, "LightPositions");

		GLfloat lightPositions[6 * 2];

		for (int i = 0; i < 6; ++i) {
			lightPositions[i * 2] = lights[i].xPos;
			lightPositions[(i * 2) + 1] = lights[i].yPos;
		}

		glUniform2fv(lightPositionsUniform, 6, lightPositions);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		map->RenderMap(program);
		BloodEmitter->Render(program, SDL_GetTicks());

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		FireEmitter->Render(program, SDL_GetTicks());

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		DrawPlayer();
		DrawEnemy();

		viewMatrix.Translate(-player->xPos, -player->yPos, -6.0f);

		std::string Mana = "Mana: " + std::to_string((int)(GameManager->CurrentMana)) + " %";
		std::string Health = "Health: " + std::to_string((int)(GameManager->CurrentHealth)) + " %";
		std::string Kills = "Killed: " + std::to_string(GameManager->NumberOfKills);
		std::string PausedMessage = "Press Select to return to main menu.";
		std::string PausedMess = " -- GAME PAUSED --";
		
		Matrix manaMatrix;
		manaMatrix.identity();

		glUseProgram(programText->programID);

		programText->setProjectionMatrix(projectionMatrix);
		programText->setViewMatrix(viewMatrix);

		manaMatrix.Translate(player->xPos - 3.90f, player->yPos - 2.0f, 0.0f);
		manaMatrix.Scale(1.5f, 1.5f, 1.0f);

		programText->setModelMatrix(manaMatrix);

		DrawText(font, Health, 0.16f, -0.08f);

		manaMatrix.identity();

		manaMatrix.Translate(player->xPos + 2.70f, player->yPos - 2.0f, 0.0f);
		manaMatrix.Scale(1.5f, 1.5f, 1.0f);

		programText->setModelMatrix(manaMatrix);

		DrawText(font, Mana, 0.16f, -0.08f);
		if (GameManager->Paused) {

			float spacing = (PausedMessage.size()*-0.08f);
			float x = (((float)PausedMessage.size()) / 2.0f)*0.18f + (spacing / 2.0f);
			manaMatrix.identity();

			manaMatrix.Translate(player->xPos - x, player->yPos + 1.4f, 0.0f);

			programText->setModelMatrix(manaMatrix);

			DrawText(font, PausedMessage, 0.18f, -0.08f);

			spacing = (PausedMess.size()*-0.08f);
			x = (((float)PausedMess.size()) / 2.0f)*0.25f + (spacing / 2.0f);
			manaMatrix.identity();

			manaMatrix.Translate(player->xPos - x, player->yPos + 2.0f, 0.0f);

			programText->setModelMatrix(manaMatrix);

			DrawText(font, PausedMess, 0.25f, -0.08f);
		}
		SDL_GL_SwapWindow(displayWindow);

}

void Application::SetUpGame() {

	if (player) {
		delete player;
	}

	player = new Player();
	player->textureID = LoadTexture("playerRed1.png");

	LastFrameTick = 0.0f;
	int textureID = LoadTexture("playerRed1.png");

	if (map) {
		delete map;
	}

	map = new MapGenerator(30, 30, SDL_GetTicks());

	map->SpriteSheetID = LoadTexture("tiles_spritesheet.png");

	if (BloodEmitter) {
		delete BloodEmitter;
	}

	BloodEmitter = new Emitter(10, "Blood");

	BloodEmitter->ParticleTextureID = LoadTexture("blood_2.png");

	BloodEmitter->maxLifeTime = 2.0f;
	BloodEmitter->activated = false;
	BloodEmitter->startAlpha = 1.0f;
	BloodEmitter->endAlpha = 1.0f;
	BloodEmitter->sizeDeviation = 0.05f;
	BloodEmitter->startSize = 0.1f;
	BloodEmitter->endSize = 0.05f;

	lights.clear();

	for (int i = 0; i < 6; ++i) {
		Entity light(-35.0f,-35.0f,0.0f,1.0f,1.0f);
		lights.push_back(light);
	}

	enemies.clear();

	for (int i = 0; i < 50; ++i) {

		Enemy enemy;
		enemy.textureID = textureID;
		enemies.push_back(enemy);

	}

	if (FireEmitter) {
		delete FireEmitter;
	}

	FireEmitter = new Emitter(150, "FlameThrower");

	FireEmitter->ParticleTextureID = LoadTexture("Fire2.png");

	FireEmitter->maxLifeTime = 1.0f;
	FireEmitter->activated = false;
	FireEmitter->startAlpha = 1.0f;
	FireEmitter->endAlpha = 0.0f;
	FireEmitter->sizeDeviation = 0.02f;
	FireEmitter->startSize = 0.08f;
	FireEmitter->endSize = 0.04f;

	GameManager->reset();

}

void Application::CheckCollision() {

	int xPosTile = (int)((player->xPos+0.1f)/0.4f);
	int yPosTile = (int)((player->yPos)/0.4f);

	if (map->Data[-yPosTile][xPosTile] == 11 || map->Data[-yPosTile][xPosTile] == 12 || map->Data[-yPosTile][xPosTile] == 13
		|| map->Data[-yPosTile][xPosTile] == 14 || map->Data[-yPosTile][xPosTile] == 10 || map->Data[-yPosTile][xPosTile] == 9) {

		player->xVel = 0.0f;

		float penetration = ((player->xPos + 0.1f)) - ((float)(xPosTile*0.4f));

		player->xPos -= penetration + 0.001f;

	}
	xPosTile = (int)((player->xPos - 0.1f) / 0.4f);
	yPosTile = (int)((player->yPos) / 0.4f);

	if (map->Data[-yPosTile][xPosTile] == 11 || map->Data[-yPosTile][xPosTile] == 12 || map->Data[-yPosTile][xPosTile] == 13
		|| map->Data[-yPosTile][xPosTile] == 14 || map->Data[-yPosTile][xPosTile] == 10 || map->Data[-yPosTile][xPosTile] == 9) {

		player->xVel = 0.0f;

		float penetration = ((float)(xPosTile*0.4f) + 0.4f) - ((player->xPos - 0.1f));

		player->xPos += penetration + 0.001f;

	}

	xPosTile = (int)((player->xPos) / 0.4f);
	yPosTile = (int)((player->yPos - 0.1f) / 0.4f);

	if (map->Data[-yPosTile][xPosTile] == 11 || map->Data[-yPosTile][xPosTile] == 12 || map->Data[-yPosTile][xPosTile] == 13
		|| map->Data[-yPosTile][xPosTile] == 14 || map->Data[-yPosTile][xPosTile] == 10 || map->Data[-yPosTile][xPosTile] == 9) {

		player->yVel = 0.0f;

		float penetration = ((float)(yPosTile*0.4f)) - ((player->yPos - 0.1f));

		player->yPos += penetration + 0.001f;

	}

	xPosTile = (int)((player->xPos) / 0.4f);
	yPosTile = (int)((player->yPos + 0.1f) / 0.4f);

	if (map->Data[-yPosTile][xPosTile] == 11 || map->Data[-yPosTile][xPosTile] == 12 || map->Data[-yPosTile][xPosTile] == 13
		|| map->Data[-yPosTile][xPosTile] == 14 || map->Data[-yPosTile][xPosTile] == 10 || map->Data[-yPosTile][xPosTile] == 9) {

		player->yVel = 0.0f;

		float penetration = ((player->yPos + 0.1f)) - ((float)(yPosTile*0.4f) - 0.4f);

		player->yPos -= penetration + 0.001f;

	}

	for (int i = 0; i < enemies.size(); ++i) {

		xPosTile = (int)((enemies[i].xPos + 0.1f) / 0.4f);
		yPosTile = (int)((enemies[i].yPos) / 0.4f);

		if (map->Data[-yPosTile][xPosTile] == 11 || map->Data[-yPosTile][xPosTile] == 12 || map->Data[-yPosTile][xPosTile] == 13
			|| map->Data[-yPosTile][xPosTile] == 14 || map->Data[-yPosTile][xPosTile] == 10 || map->Data[-yPosTile][xPosTile] == 9) {

			enemies[i].xVel = 0.0f;

			float penetration = ((enemies[i].xPos + 0.1f)) - ((float)(xPosTile*0.4f));

			enemies[i].xPos -= penetration + 0.001f;

		}

		xPosTile = (int)((enemies[i].xPos - 0.1f) / 0.4f);
		yPosTile = (int)((enemies[i].yPos) / 0.4f);

		if (map->Data[-yPosTile][xPosTile] == 11 || map->Data[-yPosTile][xPosTile] == 12 || map->Data[-yPosTile][xPosTile] == 13
			|| map->Data[-yPosTile][xPosTile] == 14 || map->Data[-yPosTile][xPosTile] == 10 || map->Data[-yPosTile][xPosTile] == 9) {

			enemies[i].xVel = 0.0f;

			float penetration = ((float)(xPosTile*0.4f) + 0.4f) - ((enemies[i].xPos - 0.1f));

			enemies[i].xPos += penetration + 0.001f;

		}

		xPosTile = (int)((enemies[i].xPos) / 0.4f);
		yPosTile = (int)((enemies[i].yPos - 0.1f) / 0.4f);

		if (map->Data[-yPosTile][xPosTile] == 11 || map->Data[-yPosTile][xPosTile] == 12 || map->Data[-yPosTile][xPosTile] == 13
			|| map->Data[-yPosTile][xPosTile] == 14 || map->Data[-yPosTile][xPosTile] == 10 || map->Data[-yPosTile][xPosTile] == 9) {

			enemies[i].yVel = 0.0f;

			float penetration = ((float)(yPosTile*0.4f)) - ((enemies[i].yPos - 0.1f));

			enemies[i].yPos += penetration + 0.001f;

		}

		xPosTile = (int)((enemies[i].xPos) / 0.4f);
		yPosTile = (int)((enemies[i].yPos + 0.1f) / 0.4f);

		if (map->Data[-yPosTile][xPosTile] == 11 || map->Data[-yPosTile][xPosTile] == 12 || map->Data[-yPosTile][xPosTile] == 13
			|| map->Data[-yPosTile][xPosTile] == 14 || map->Data[-yPosTile][xPosTile] == 10 || map->Data[-yPosTile][xPosTile] == 9) {

			enemies[i].yVel = 0.0f;

			float penetration = ((enemies[i].yPos + 0.1f)) - ((float)(yPosTile*0.4f) - 0.4f);

			enemies[i].yPos -= penetration + 0.001f;

		}
	}

}

void Application::GameInput() {

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			quit = true;
		}

		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		if (SDL_JoystickGetButton(playerOneController, 5)) {
			if (GameManager->CurrentMana >= 8.0f) {
				FireEmitter->activated = true;
				if (!GameManager->Paused) {
					if (!Mix_PlayingMusic()) {
						Mix_VolumeMusic(MIX_MAX_VOLUME);
						Mix_PlayMusic(FireSound, 0);
					}
				}
			}
		}
		else {
			if (FireEmitter) {
				FireEmitter->activated = false;
				Mix_FadeOutMusic(600);
			}
		}

		if (event.type == SDL_JOYAXISMOTION) {
			if (event.jaxis.which == 0) {
				if (event.jaxis.axis == 0) {
					player->MoveX(event.jaxis.value);
				}
				if (event.jaxis.axis == 1) {
					player->MoveY(event.jaxis.value);
				}
				if (event.jaxis.axis == 5) {
					player->yDir = -event.jaxis.value;
				}
				if (event.jaxis.axis == 2) {
					player->xDir = event.jaxis.value;
				}
			}
		} if (event.type == SDL_JOYBUTTONDOWN) {

			if (event.jbutton.button == 9) {
				if (GameManager->MainGame) {
					GameManager->Paused = !GameManager->Paused;
					Mix_PlayChannel(-1, PauseSound, 0);
				}
				else if (GameManager->MainMenu) {
					SetUpGame();
					startingGame = true;
					GameManager->MainGame = true;
					GameManager->MainMenu = false;
				}
			}
			if (event.jbutton.button == 4) {
				if (GameManager->Paused == false) {
					if (GameManager->CurrentMana >= 20.0f) {
						GameManager->ActivatedLightSpell = true;
						Mix_PlayChannel(-1, LightSound, 0);
					}
				}
			}
			else if (event.jbutton.button == 8) {
				if (GameManager->Paused && GameManager->MainMenu == false) {
					GameManager->MainGame = false;
					GameManager->EndMenu = true;
					GameManager->Paused = true;
				}
				if (GameManager->EndMenu) {
					GameManager->EndMenu = false;
					GameManager->MainMenu = true;
				} else if (GameManager->MainMenu) {
					quit = true;
				}
			}
		}
	}
}


GLuint Application::LoadTexture(const char *image_path) {

	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	SDL_FreeSurface(surface);

	return textureID;

}

void Application::DrawText(int FontTexture, std::string text, float size, float spacing) {

	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;

	for (int i = 0; i < text.size(); ++i) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing)*i) + (-0.5f * size), 0.5f * size,
			((size + spacing)*i) + (-0.5f * size), -0.5f * size,
			((size + spacing)*i) + (0.5f * size), 0.5f * size,
			((size + spacing)*i) + (0.5f * size), -0.5f * size,
			((size + spacing)*i) + (0.5f * size), 0.5f * size,
			((size + spacing)*i) + (-0.5f * size), -0.5f * size,
		});

		float displacement = 0.0f;

		if ((int)text[i] == 32) {
			displacement = 0.015f;
		}

		texCoordData.insert(texCoordData.end(), {
			texture_x + displacement, texture_y + displacement,
			texture_x + displacement, texture_y + texture_size - displacement,
			texture_x + texture_size - displacement, texture_y + displacement,
			texture_x + texture_size - displacement, texture_y + texture_size - displacement,
			texture_x + texture_size - displacement, texture_y + displacement,
			texture_x + displacement, texture_y + texture_size - displacement,
		});
	}


	glUseProgram(programText->programID);

	glVertexAttribPointer(programText->positionAttribute, 2, GL_FLOAT, false, 0,
		vertexData.data());
	glEnableVertexAttribArray(programText->positionAttribute);

	glVertexAttribPointer(programText->texCoordAttribute, 2, GL_FLOAT, false, 0,
		texCoordData.data());
	glEnableVertexAttribArray(programText->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, FontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);

	glDisableVertexAttribArray(programText->positionAttribute);
	glDisableVertexAttribArray(programText->texCoordAttribute);

}

void Application::DrawPlayer() {

	program->setModelMatrix(player->modelMatrix);

	player->modelMatrix.identity();

	player->modelMatrix.Translate(player->xPos, player->yPos, 0.0f);
	player->modelMatrix.Rotate(player->Angle);
	player->modelMatrix.Scale(0.35f, 0.35f, 1.0f);

	float vertices[] = { -0.2f, -0.3f, 0.2f, 0.3f, -0.2f, 0.3f, 0.2f, 0.3f, -0.2f, -0.3f, 0.2f, -0.3f };
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	float texCoord[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoord);
	glEnableVertexAttribArray(program->texCoordAttribute);

	std::vector<float> colorVar;
	for (int i = 0; i < 6; ++i) {
		colorVar.insert(colorVar.end(), { 1.0f, 1.0f, 1.0f, 1.0f });
	}

	glVertexAttribPointer(program->colorAttribute, 4, GL_FLOAT, false, 0, colorVar.data());
	glEnableVertexAttribArray(program->colorAttribute);

	glBindTexture(GL_TEXTURE_2D, player->textureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
	glDisableVertexAttribArray(program->colorAttribute);


}

void Application::DrawEnemy() {

	for (int i = 0; i < enemies.size(); ++i) {

		if (enemies[i].draw) {

			program->setModelMatrix(enemies[i].modelMatrix);

			enemies[i].modelMatrix.identity();

			enemies[i].modelMatrix.Translate(enemies[i].xPos, enemies[i].yPos, 0.0f);
			enemies[i].modelMatrix.Rotate(-enemies[i].Angle);
			enemies[i].modelMatrix.Scale(0.35f, 0.35f, 1.0f);

			float vertices[] = { -0.2f, -0.3f, 0.2f, 0.3f, -0.2f, 0.3f, 0.2f, 0.3f, -0.2f, -0.3f, 0.2f, -0.3f };
			glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
			glEnableVertexAttribArray(program->positionAttribute);

			float texCoord[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
			glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoord);
			glEnableVertexAttribArray(program->texCoordAttribute);

			std::vector<float> colorVar;
			if (enemies[i].State == "Attacking") {
				for (int i = 0; i < 6; ++i) {
					colorVar.insert(colorVar.end(), { 1.4f, 0.8f, 0.8f, 1.0f });
				}
			}
			else if (enemies[i].State == "Wandering") {
				for (int i = 0; i < 6; ++i) {
					colorVar.insert(colorVar.end(), { 0.8f, 0.8f, 0.8f, 1.0f });
				}
			}
			else {
				for (int i = 0; i < 6; ++i) {
					colorVar.insert(colorVar.end(), { 1.0f, 0.8f, 0.8f, 1.0f });
				}
			}

			glVertexAttribPointer(program->colorAttribute, 4, GL_FLOAT, false, 0, colorVar.data());
			glEnableVertexAttribArray(program->colorAttribute);


			glBindTexture(GL_TEXTURE_2D, enemies[i].textureID);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glDisableVertexAttribArray(program->positionAttribute);
			glDisableVertexAttribArray(program->texCoordAttribute);
			glDisableVertexAttribArray(program->colorAttribute);

		}

	}


}                                                 