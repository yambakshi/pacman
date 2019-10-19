////////////////////
// Include Guards //
////////////////////
#ifndef ENEMY_H
#define ENEMY_H
#pragma once

//////////////
// Includes //
//////////////
#include<random>
#include<ctime>

#include "Pacman.h"

class Enemy
{
public:
	/////////////
	// Methods //
	/////////////
	Enemy(); // default ctor
	Enemy(const Enemy&);
	Enemy(Maze*, Pacman*, Food*, D3DCOLOR, short, DXDevice*);	
	~Enemy();
	void Draw(); // Draws the Enemy
	void Update(); // Update's the enemy's animation
				   // and position

	///////////////
	// Variables //
	///////////////
	D3DXVECTOR3 pos; // Enemy's position
protected:		
	/////////////
	// Methods //
	/////////////
	void EyesDirection(); // determines the eyes looking 
						  // direction according to the moving
						  // direction	
	void LoadTexture(); // switches between textures
	void DeadFaceMode(); // changing the enemy's color
						 // to dead mode colors
	///////////////
	// Variables //
	///////////////
	const unsigned short* width; // pointer food's texture WIDTH
	const unsigned short* height; // pointer food's texture HEIGHT
	float MoveSpeed; // the enemy's movement speed	
	Maze* pMaze; // Ponter to the maze in which the enemy travel
	Pacman* pPacman; // Ponter to the pacman
	Direction WantToTurn; // the direction the enemy 
						  // wants to turn to
private:
	/////////////
	// Methods //
	/////////////
	void Move(); // moves the enemy
	bool CanTurn(short); // checks if the enemy can turn to 
					// the WantToTurn direction	
	void RandomDirection(); // if the pacman is not in the enemys 
					     	// radius, the enemy randomize his
						    // moving directions
	void AvoidPacman(short, short); 	// calculates an escape target for the enemy.
										// the escape target is the farthest point from
										// the current pacman location
	void ScanPaths(short scanRadius); // fills the all paths vector with all the 
									  // routes within the scan radius
	short ShortestPath(short, short, short, short); // Calculates the shortest path to pacman
	void ScanEdgePoint(bool&, vector<vector<Direction>>&, short, Direction); // Checks if the edgePoint is at acrossroads
															 				 // or at edge of the maze and edits the allPaths
																			 // vector accordingly
	void OppositeDirection(short i, Direction followDrc); // in case an individual edgePoint is trying to 
													      // take a step back at the opposite direction
	bool InTunnels(short, short, Direction); // check if the edge point being scanned is in the tunnels
	bool PacmanInRadius(short&, short&); // checks if the pacman is in the radius of the enemy
	
	///////////////
	// Variables //
	///////////////
	bool
		turnedAround, // when pacman eats special food,
					  // the enemy has to turn around
		updatedEnemy, // is true when currRow and currColumn are
					  // updated. they are only updated once
				      // enemy left the previous ROW or COLUMN
		redDeadFace, // if true, the enemys daed face should switch
					 // to the red dead face
		aniDrc; // if set to true the enemy's animation is
				// moving forwars. otherwize its moveing backwards
	short 
		aniSize, // number of textures in the animation
		enemyPic, // the number of the texture to load
				  // during the animation
		currRow, // current enemy row
		currColumn; // current enemy column
	float 
		switchCount, // counter between pic 
					 // the enemy textures switching
		aniSpeed; // The animation speed

	const unsigned short* radius; // the distance (in rows and column) between the enemy 
							      // and pacman from which the enemy tries to track down the pacman
	const unsigned short* eyesWidth; // pointer food's texture WIDTH
	const unsigned short* eyesHeight; // pointer food's texture HEIGHT
	vector<Direction> shortest; // stores the shortest path that is 
							    // calculated in the function ShortestPath()
	vector <vector <Direction> > allPaths; // all the paths that 
										   // lead to the edge points
	vector <Food_XY> edgeP; // all the edge points the are
							// spread to find pacman	
	const D3DCOLOR 
		*deadBodyColorBlue, // dead body color
		*deadBodyColorWhite, // second dead body color
		*deadFaceColorYellow, // deadface color
		*deadFaceColorRed, // second deadface color
		*origColor; // stores te original color when the enemy
				   // is changing is color
	D3DCOLOR 
		deadFaceColor, // the dead face's color
		color; // The Enemy's color
	DXDevice* pDevice; // Ponter to the DXDevice instance
	D3DXMATRIX mat; // matrix of the pacman transforamtions
	D3DXVECTOR3 eyesPos; // the eyes position
	D3DXVECTOR2 
		flatPos, // pivot of the pacman transformations
		scale; // holds the pacman scale's information
	LPD3DXSPRITE 
		enemyEyesSprite, // pointer to sprite that draws the enemy's eyes
		enemyBodySprite; // pointer to sprite that draws the enemy's body
	LPDIRECT3DTEXTURE9 
		deadFaceTexture, // the enemys face when pacman eats special food
		bodyTexture, // pointer to enemy body texture
		eyesRTexture, // pointer to enemys eyes-looking-right texture
		eyesDTexture; // pointer to enemys eyes-looking-down texture			
	Food* pFood; // the enemy is navigating through the food map	
	Direction drc; // the currunt moving direction
};

#endif