////////////////////
// Include Guards //
////////////////////
#ifndef NUMBERS_H
#define NUMBERS_H
#pragma once

//////////////
// Includes //
//////////////
#include "Enemy.h"

class Numbers
{
public:
	/////////////
	// Methods //
	/////////////
	Numbers();
	Numbers(const Numbers&);
	Numbers(LPCSTR, DXDevice*);
	~Numbers();
	void Draw(short number, D3DCOLOR color, D3DXVECTOR2 scale, D3DXVECTOR3 pos); // draws thew specified number
private:
	/////////////
	// Methods //
	/////////////
	short CharCount(short); // retrieves the specified number char count
	///////////////
	// Variables //
	///////////////
	LPCSTR size; // indicates whether to draw large number textures or small
	LPD3DXSPRITE numSprite; // the numbers sprite
	LPDIRECT3DTEXTURE9 numArr[10]; // stores all the textures of all numbers
	DXDevice* pDevice;
};

#endif

