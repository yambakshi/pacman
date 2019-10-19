////////////////////
// Include Guards //
////////////////////
#ifndef PACMANGAME_H
#define PACMANGAME_H
#pragma once

/////////////////
// My Includes //
/////////////////
#include "Window.h"
#include "CageEnemy.h"
#include "Numbers.h"

typedef struct ThreadData
{
	Maze* maze;
	DXDevice* pDevice;
	HANDLE hMutex;
} *PTDATA;

class PacmanGame
{
public:
	/////////////
	// Methods //
	/////////////
	PacmanGame(); // default ctor
	PacmanGame(const PacmanGame&);
	PacmanGame(Window*);
	~PacmanGame();	
	void Run(UINT&, WPARAM&); // update and draw the game

private:
	/////////////
	// Methods //
	/////////////
	void Draw(); // Draw's the game
	void Update(UINT&, WPARAM&); // Updates the game components
	bool InitializeThread();
	bool InitializeMutex();	

	///////////////
	// Variables //
	///////////////
	bool 
		go, // if true, countdown ended, and GO is written
		update; // if true, pacman game is updated
	const unsigned short* scoreWidth;
	const unsigned short* scoreHeight;
	short alpha; // the alpha value of the GO texture	
    HANDLE 
		hMutex,
		hTimerThread; // the countdown thread
	PTDATA pData; // the data that is used by the countdown thread
	LPD3DXSPRITE 
		ScoreSprite, // the score sprite
		GoSprite; // the "GO!" sprite
	LPDIRECT3DTEXTURE9 
		pLifeTexture, // the pacman lives texture
		pScoreTexture, // the "SCORE:" texture 
		pGoTexture;	// the "GO!" texture
	Numbers* scoreNumbers; // pointer to the Numbers object the draws the score numbers
	DXDevice* device; // pointer to the DXDevice object
	Maze* maze;	// pointer to Maze object
	Food* food; // pointer to Food object
	Pacman* pacman; // pointer to Pacman object 
	Enemy* red; // pointer to Enemy object
	CageEnemy 
		*tur, // pointer to CageEnemy object
		*orange, // pointer to CageEnemy object
		*pink; // pointer to CageEnemy object
};

DWORD WINAPI MyThreadFunction( LPVOID lpParam ); // the hTimerThread function
#endif