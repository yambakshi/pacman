////////////////////
// Include Guards //
////////////////////
#ifndef S_H
#define S_H
#pragma once

//////////////
// Includes //
//////////////
#include<d3dx9.h> 
#include<fstream>
#include<sstream>

using namespace std;
struct Food_XY
{
public:
	short x,y;
	Food_XY(){} // default ctor
	Food_XY(short x, short y)
	{
		this->x = x;
		this->y = y;
	}
};

struct Cage
{
public:
	short x,y,width,height;
	Cage(){} // default Ctor
	Cage(short x, short y, short width, short height) 
	{
		this->x=x;
		this->y=y;
		this->width=width;
		this->height=height;
	}
};

// ABSOLUTE Vertex Struct
struct CUSTOMFLATVERTEX 
{ 
	float 
		x, y, z, rhw;  // The transformed(screen space) position for the vertex.
	DWORD color;

	CUSTOMFLATVERTEX(){} // default ctor
	CUSTOMFLATVERTEX(float x, float y, float z, float rhw, DWORD color)
	{
		this->x=x;
		this->y=y;
		this->z=z;
		this->rhw=rhw;
		this->color=color;
	}
};
#define CUSTOMFLATFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

// ABSOLUTE textured Vertex Struct
struct CUSTOMTEXFLATVERTEX 
{ 
	float 
		x, y, z, rhw,  // The transformed(screen space) position for the vertex.
		tu,tv; //texture coordinates
	DWORD color;

	CUSTOMTEXFLATVERTEX (){} // default ctor
	CUSTOMTEXFLATVERTEX (float x, float y, float z, float rhw, DWORD color, float tu, float tv) // textured ctor
	{
		this->x=x;
		this->y=y;
		this->z=z;
		this->rhw=rhw;
		this->color=color;
		this->tu=tu;
		this->tv=tv;
	}
};
#define CUSTOMTEXFLATFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

struct PIXEL_RGB
{
	short R,G,B;
	PIXEL_RGB(){} // Default Ctor
	PIXEL_RGB(short r,short g,short b)
	{
		R=r;
		G=g;
		B=b;
	}

	// Overriding the equality operator
	bool operator == (PIXEL_RGB color)
	{
		return ((color.R == R) && 	(color.G == G) && 	(color.B == B)); 
	}

	// Overriding the Inequality operator
	bool operator != (PIXEL_RGB color)
	{
		return ((color.R != R) || 	(color.G != G) || 	(color.B != B));
	}
};


class S
{
public:
	// gets the texture height
	static const unsigned short* getTextureHeightBMP (LPCSTR filename);

	// gets the texture width
	static const unsigned short*  getTextureWidthBMP (LPCSTR filename);

	// gets the texture width
	static const unsigned short*  getTextureWidthPNG (LPCSTR filename);

	// gets the texture height
	static const unsigned short*  getTextureHeightPNG (LPCSTR filename);

	// Creates the sprite
	static void CreateSprite(LPD3DXSPRITE* sprite, LPDIRECT3DDEVICE9 pDevice);

	// Loads a Texture and
	// assigns a pointer to 
	// the texture location 
	static void CreateTextureFromFile(LPCSTR filename, LPDIRECT3DTEXTURE9* ppTexture, LPDIRECT3DDEVICE9 pDevice);

	// Draws the specified texture
	static void Draw(LPCSTR filename, D3DXVECTOR3 pos, D3DXVECTOR2 scale, D3DCOLOR color, LPDIRECT3DDEVICE9 pDevice);

	static void Draw(LPD3DXSPRITE sprite, LPDIRECT3DTEXTURE9 pTexture, D3DXVECTOR3 pos, D3DXVECTOR2 scale, D3DCOLOR color, LPDIRECT3DDEVICE9 pDevice);

	static void Draw(LPD3DXSPRITE sprite, LPDIRECT3DTEXTURE9 pTexture, D3DXVECTOR3 pos, D3DXVECTOR2 scale, LPDIRECT3DDEVICE9 pDevice);
};

#endif