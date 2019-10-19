#include "S.h"

// Gets the texture height
const unsigned short* S::getTextureHeightBMP (LPCSTR filename)
{
     ifstream f_DataFile;
     unsigned short* HEIGHT = new unsigned short;
     short dummy;
 
     f_DataFile.open(filename, ios::binary);
 
     if (f_DataFile.is_open())
     {
         for (int i = 0; i < 22; i++)
         {
             dummy = f_DataFile.get();
         }
 
         *HEIGHT = f_DataFile.get();
         *HEIGHT += f_DataFile.get()*256;
         *HEIGHT += f_DataFile.get()*256*256;
         *HEIGHT += f_DataFile.get()*256*256*256;
 
         f_DataFile.close();
     }
     return HEIGHT;
 }
 
// gets the texture width
const unsigned short*  S::getTextureWidthBMP (LPCSTR filename)
 {
     ifstream f_DataFile;
     unsigned short *WIDTH = new unsigned short;
     short dummy;
 
     f_DataFile.open(filename, ios::binary);
 
     if (f_DataFile.is_open())
     {
 
         for (int i = 0; i < 18; i++)
         {
             dummy = f_DataFile.get();
         }
 
         *WIDTH = f_DataFile.get();
         *WIDTH += f_DataFile.get()*256;
         *WIDTH += f_DataFile.get()*256*256;
         *WIDTH += f_DataFile.get()*256*256*256;
 
         f_DataFile.close();
     }
     return WIDTH;
 }

// gets the texture width
const unsigned short*  S::getTextureWidthPNG (LPCSTR filename)
 {
     ifstream f_DataFile;
     unsigned short *WIDTH = new unsigned short;
     short dummy;
 
     f_DataFile.open(filename, ios::binary);
 
     if (f_DataFile.is_open())
     {
		 for(int i=0; i<20; i++)
			 dummy = f_DataFile.get();
		 
		 *WIDTH = dummy;

         f_DataFile.close();
     }
     return WIDTH;
 }

// gets the texture height
const unsigned short*  S::getTextureHeightPNG (LPCSTR filename)
 {
     ifstream f_DataFile;
     unsigned short *HEIGHT = new unsigned short;
     short dummy;
 
     f_DataFile.open(filename, ios::binary);
 
     if (f_DataFile.is_open())
     {
		 for(int i=0; i<24; i++)
			 dummy = f_DataFile.get();

		 *HEIGHT = dummy;
         f_DataFile.close();
     }
     return HEIGHT;
 }

// Creates the sprite
void S::CreateSprite(LPD3DXSPRITE* sprite, LPDIRECT3DDEVICE9 pDevice)
{
	*sprite=NULL;

	if(FAILED(D3DXCreateSprite(pDevice,sprite)))
	{
		MessageBox(NULL,"Can't Crearte Sprite!","Sprite Failed",MB_OK);
	}
}

// Loads a Texture and
// assigns a pointer to 
// the texture location 
void S::CreateTextureFromFile(LPCSTR filename, LPDIRECT3DTEXTURE9* ppTexture, LPDIRECT3DDEVICE9 pDevice)
{
	if(FAILED(D3DXCreateTextureFromFileEx(
		pDevice, // Pointer to the d3dx device
		filename, // File name
		D3DX_DEFAULT_NONPOW2, // Texture width. if not  specified, taken from file
		D3DX_DEFAULT_NONPOW2, // Texture height. if not  specified, taken from file
		D3DX_DEFAULT, // Number of mip levels. if not specified take from file
		D3DPOOL_DEFAULT, // Specifies the texture usage
		D3DFMT_FROM_FILE, // file's pixel format
		D3DPOOL_DEFAULT, //Specifies the memory class to which the texture is loaded
		D3DX_DEFAULT, // How the image is filtered
		D3DX_DEFAULT, // How the image is filtered. also specify the number of mip levels to be skipped
		0, // Color key. zero to disable 
		NULL, // Points to a texture info variable
		NULL, // Points to the texture color palette
		ppTexture))) // The texture pointer address
	{
		std::stringstream str;
		str << filename << " Failed";
		MessageBox(NULL,str.str().c_str(),"Can't Create Texture from file!",MB_OK);
	}
}

void S::Draw(LPCSTR filename, D3DXVECTOR3 pos, D3DXVECTOR2 scale, D3DCOLOR color, LPDIRECT3DDEVICE9 pDevice)
{
	LPD3DXSPRITE draw;
	LPDIRECT3DTEXTURE9 pTexture;
	D3DXVECTOR2 flatPos; // pivot of the pacman transformations
	D3DXMATRIX mat;

	CreateSprite(&draw,pDevice);
	CreateTextureFromFile(filename,&pTexture,pDevice);	

	flatPos.x = pos.x;
	flatPos.y = pos.y;
	D3DXMatrixTransformation2D(&mat,&flatPos,NULL,&scale,NULL,NULL,NULL); // applies the matrix

	draw->Begin(D3DXSPRITE_ALPHABLEND);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);

	draw->SetTransform(&mat);
	draw->Draw(pTexture,NULL,NULL,&pos,color);
	draw->End();

	draw->Release();
	pTexture->Release();
}

void S::Draw(LPD3DXSPRITE sprite, LPDIRECT3DTEXTURE9 pTexture, D3DXVECTOR3 pos, D3DXVECTOR2 scale, D3DCOLOR color, LPDIRECT3DDEVICE9 pDevice)
{
	D3DXVECTOR2 flatPos; // pivot of the pacman transformations
	D3DXMATRIX mat;	

	flatPos.x = pos.x;
	flatPos.y = pos.y;
	D3DXMatrixTransformation2D(&mat,&flatPos,NULL,&scale,NULL,NULL,NULL); // applies the matrix

	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);

	sprite->SetTransform(&mat);
	sprite->Draw(pTexture,NULL,NULL,&pos,color);
	sprite->End();
}

void S::Draw(LPD3DXSPRITE sprite, LPDIRECT3DTEXTURE9 pTexture, D3DXVECTOR3 pos, D3DXVECTOR2 scale, LPDIRECT3DDEVICE9 pDevice)
{
	D3DXVECTOR2 flatPos; // pivot of the pacman transformations
	D3DXMATRIX mat;	

	flatPos.x = pos.x;
	flatPos.y = pos.y;
	D3DXMatrixTransformation2D(&mat,&flatPos,NULL,&scale,NULL,NULL,NULL); // applies the matrix

	sprite->Begin(D3DXSPRITE_ALPHABLEND);

	sprite->SetTransform(&mat);
	sprite->Draw(pTexture,NULL,NULL,&pos,0xFFFFFFFF);
	sprite->End();
}