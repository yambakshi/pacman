////////////////////
// Include Guards //
////////////////////
#ifndef CAGEENEMY_H
#define CAGEENEMY_H
#pragma once

//////////////
// Includes //
//////////////
#include "Enemy.h"

class CageEnemy : public Enemy
{
public:
	/////////////
	// Methode //
	/////////////
	CageEnemy();
	CageEnemy(const CageEnemy&);
	CageEnemy(Maze*, Pacman*, Food*, D3DCOLOR color, short radius, Cage cage, short place, DXDevice*);
	~CageEnemy();
	void Update(); // updates the enemy's eyes 
				   // direction and position
private:
	/////////////
	// Methods //
	/////////////
	void DoLaps(); // UP and DOWNs in the cage
	void Exit(); // exit the cage
	void TurnToExit(); // turn to the exit's direction
					   // (this function only applies to enemies
					   // who are not in the middle cell)
	///////////////
	// Variables //
	///////////////
	bool
		outside, // is true when enemy is outside the cage
		up; // indicates whether the initial movement 
			 // direction inside the cage
	short
		place, // the number of the enemy's cell in the cage (left to right)
		CageTime, // How many UP and DOWNs the enemy will
				  // do in the cage 
		Laps; // Number of UP and DOWNs the enemy will make in the cage
	Cage cage;
};

#endif