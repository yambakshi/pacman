////////////////////
// Include Guards //
////////////////////
#ifndef PACMAN_H
#define PACMAN_H
#pragma once

//////////////
// Includes //
//////////////
#include "Food.h"

class Pacman
{
public:
	/////////////
	// Methods //
	/////////////
	Pacman(); // default ctor
	Pacman(const Pacman&);
	Pacman(Maze*,Food*, DXDevice*);
	~Pacman();
	void Update(UINT&, WPARAM&); // Updates the pacman, open/close mouth and movement
	void Draw(); // Draws the pacman

	///////////////
	// Variables //
	///////////////
	bool specialFood;
	const unsigned short* influenceTime;
	const unsigned short* width;	
	const unsigned short* height;
	short
		specialFoodInfluence, // how many loops will the influence last
		score;  
	Direction drc; // The currunt pacman direction
	D3DXVECTOR3 pos; // pacman position in the 2D world (z axis is 0)
	LPD3DXSPRITE pacmanSprite; // pointer to  pacman sprite
	LPDIRECT3DTEXTURE9 pTexture; // Pointer to the texture	
private:	
	/////////////
	// Methods //
	/////////////
	void Move(); // Moves the pacman
	void ChangeDirection(bool);
	void Eat(); // eats food
	bool CanTurn(short); // Checks if the pacman can turn
	void LoadTexture(); // Loads the pacman image
	
	///////////////
	// Variables //
	///////////////
	const double* PI;
	bool
		freeze, // Determines whether the pacman 
				// should stay stuck with it's mouth open
		open; // Indicates if the mouth should be open or close
	float 
		MoveSpeed, // Speed of Pacman
		rot,
		pacmanPic, // The number of the image should be rendered
		MouthSpeed;	// The speed of the image switching	
	D3DXMATRIX mat; // matrix of the pacman transforamtions
	D3DXVECTOR2 
		flatPos, // pivot of the pacman transformations
		scale; // holds the pacman scale's information
	Maze* pMaze; // The mazein which tha pacman moves
	Direction WantToTurn; // where the pacman wants to turn
	DXDevice* pDevice; // Pointer to the device		
	Food* pFood; // the food that's in the maze
};

#endif