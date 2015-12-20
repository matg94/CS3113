#pragma once


class GameState {

public:

	GameState(int MaxHealth, int MaxMana, float EnemySpeed, int EnemyHealth);

	int NumberOfKills;
	float CurrentHealth;
	int MaxHealth;
	float CurrentMana;
	int MaxMana;
	int NumberOfEnemies;
	int CurrentRound;
	int EnemyDamage;
	int EnemiesLeft;
	float EnemyHealth;
	float EnemySpeed;

	float LightSpellTime;

	bool ActivatedLightSpell;
	bool LightSpell;

	float PlayerSpeed;

	void reset();

	bool Paused;
	bool MainMenu;
	bool MainGame;
	bool EndMenu;


};