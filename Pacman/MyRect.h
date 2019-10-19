////////////////////
// Include Guards //
////////////////////
#ifndef MYRECT_H
#define MYRECT_H
#pragma once

/////////////////
// My Includes //
/////////////////
#include "DXDevice.h"
#include "S.h"

class MyRect
{
public:
	/////////////
	// Methods //
	/////////////
	MyRect();
	MyRect(const MyRect&);
	MyRect(DXDevice*);
	MyRect(short x, short y, short width, short height, DWORD color, DXDevice* pDevice);
	
	~MyRect();
	
	void Draw(); // Draw's the rectangle
	void PlaceRect(short x, short y); // Places the rectangle
	void ScaleRect(short width, short height); // Scales the rectangle

	///////////////
	// Variables //
	///////////////	
	short 
		x,
		y,
		width,
		height;

private:
	/////////////
	// Methods //
	/////////////
	void UpdateBuffer(); // Fills the buffer with the new defined vertices	

	///////////////
	// Variables //
	///////////////
	DWORD color;
	DXDevice* pDevice; // Pointer to directX device	
	CUSTOMFLATVERTEX vertices[4]; // The rectangle's vertices array	
	LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;// Pointer to vertex Buffer
};

#endif