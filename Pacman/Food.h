////////////////////
// Include Guards //
////////////////////
#ifndef FOOD_H
#define FOOD_H
#pragma once

//////////////
// Includes //
//////////////
#include "Maze.h"

class Food
{
public:
	/////////////
	// Methods //
	/////////////
	Food(); // default ctor
	Food(const Food&);
	Food(Maze*, D3DCOLOR, DXDevice*);
	~Food();
	// Fills the maze with food
	void Draw();

	///////////////
	// Variables //
	///////////////
	const unsigned short* width; // pointer food's texture WIDTH
	const unsigned short* height; // pointer food's texture HEIGHT
	const unsigned short* specialWidth; // pointer food's texture WIDTH
	const unsigned short* specialHeight; // pointer food's texture HEIGHT
	const unsigned short* rows;	// pointer to number of ROWS in food array
	const unsigned short* columns;	// pointer to number of COLUMNS in food array
	Food_XY specialIndex[4]; // stores the the locations 
					         // of the special food
	Food_XY** index; // holds the indices for all 
					 // the food in the maze
	bool** paths; // maps all the paths in the maze
				  // true for path, false for wall
private: 
	/////////////
	// Methods //
	/////////////
	void IndexFood(); // fills the food array
					  // with indices to the food location
	void RemoveFood(); // edits the indexFood array
					  // so some food wont be shown
	///////////////
	// Variables //
	///////////////
	short flickerCount;
	Maze* pMaze; // ponter to the maze in which
				 // the food is placed
	LPD3DXSPRITE foodSprite; // pointer to sprite that draws the food
	D3DXVECTOR3 pos; // food's position
	DXDevice* pDevice; // pointer to dxdevice
	D3DCOLOR color;
	LPDIRECT3DTEXTURE9 pTexture; // pointer to food texture
};

#endif