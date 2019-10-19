////////////////////
// Include Guards //
////////////////////
#ifndef MAZE_H
#define MAZE_H
#pragma once

//////////////
// Includes //
//////////////
#include<fstream>

#include "MyRect.h"

using namespace std;

enum Direction {UP,DOWN,LEFT,RIGHT};	

class Maze
{
public:
	/////////////
	// Methods //
	/////////////
	Maze(); // default ctor
	Maze(const Maze&);
	Maze(LPCSTR, PIXEL_RGB, short, short, DXDevice*); // un-textured maze ctor	
	Maze(LPCSTR,LPCSTR,PIXEL_RGB,short,short, DXDevice*); // Textured maze ctor
	~Maze();	
	void Draw(); // Draws the maze

	///////////////
	// Variables //
	///////////////
	short x,y;	
	const unsigned short* width;
	const unsigned short* height;
	unsigned short frame,border,path;	 
	PIXEL_RGB** pixels; // 2D array that holds the pixel data
	D3DXVECTOR3 pos; // Texture position
	PIXEL_RGB BackColor; // The color of all pixel surrounding
						 // the maze walls	
	MyRect
		*rightRect,		 
		*leftRect; // The maze right and left Bounds
private:
	/////////////
	// METHODS //
	/////////////
	const short getPixelDataOffsetBMP (); // Returns the byte of the texture format in which
										  // the pixel data off the data begins	
	PIXEL_RGB** CreatePixelArrayBMP(); // Creates an array filled with the texture's pixel data	
	void FrameBorderPath(); // Caculates the maze's 
							// frame width, border width, path width

	///////////////
	// Variables //
	///////////////
	LPCSTR filename; // The Texture's file name
	DXDevice* pDevice; // Pointer to DXDevice instance
	LPD3DXSPRITE mazeSprite; // pointer to maze sprite			
	LPDIRECT3DTEXTURE9 pTexture; // Pointer to texture
};
#endif