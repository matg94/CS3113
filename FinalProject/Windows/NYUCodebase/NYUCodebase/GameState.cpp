#include "GameState.h"

GameState::GameState(int maxhp, int maxman, float enSpd, int enHP) : 
	MaxHealth(maxhp), MaxMana(maxman), EnemySpeed(enSpd), EnemyHealth(enHP) {

	CurrentMana = MaxMana;
	CurrentHealth = MaxHealth;
	CurrentRound = 0;
	NumberOfKills = 0;
	EnemyDamage = 5;
	Paused = false;
	MainMenu = true;
	EndMenu = false;
	MainGame = false;
	PlayerSpeed = 1.0f;

}

void GameState::reset() {

	CurrentMana = MaxMana;
	CurrentHealth = MaxHealth;
	CurrentRound = 0;
	NumberOfKills = 0;
	EnemyDamage = 5;
	PlayerSpeed = 1.0f;
	EnemySpeed = 3.5f;
	EnemyHealth = 250;

}