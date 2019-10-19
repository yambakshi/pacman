#include "Numbers.h"

Numbers::Numbers(){}
Numbers::Numbers(const Numbers& other){}
Numbers::Numbers(LPCSTR size, DXDevice* pDevice)
{
	this->pDevice = pDevice;
	this->size = size;
	std::stringstream str;	
	S::CreateSprite(&numSprite,(*pDevice).p_dx_Device);

	// Fills the textures array
	// with the numbers textures
	for(int i=0;i<10;i++)
	{
		str << "Textures/Numbers/" << size <<"/"<< i <<".png";
		S::CreateTextureFromFile(str.str().c_str(),&numArr[i],(*pDevice).p_dx_Device);
		str.str("");
	}
}

// Draws the number according to
// the parameters
void Numbers::Draw(short number, D3DCOLOR color, D3DXVECTOR2 scale, D3DXVECTOR3 pos)
{	
	short 
		currChar, // the currunt char
				  // thats about to be drawn
		pw, // the result of powering 10
		numberLength = CharCount(number); // the number of characters
										  // in the number
	std::stringstream str;
	D3DXVECTOR2 
		flatPos; // pivot of the pacman transformations
	D3DXMATRIX mat;

	flatPos.x = pos.x;
	flatPos.y = pos.y;

	D3DXMatrixTransformation2D(&mat,&flatPos,NULL,&scale,NULL,NULL,NULL); // applies the matrix

	numSprite->Begin(D3DXSPRITE_ALPHABLEND);
	pDevice->p_dx_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);

	for(short i = numberLength; i>0; i--)
	{
		pw = pow(10,(float)(i-1));
		currChar = (number/pw)%10;
		str << "Textures/Numbers/" << size <<"/"<< currChar <<".png";
		numSprite->SetTransform(&mat);
		numSprite->Draw(numArr[currChar],NULL,NULL,&pos,color);
		pos.x+=(short)(*S::getTextureWidthPNG(str.str().c_str()))+4;
		str.str("");
	}
	
	numSprite->End();
}

// Returns the number of characters
// in the specified number
short Numbers::CharCount(short number)
{
	short 
		cpyNumber = number,
		count = 0;
	
	if(cpyNumber == 0)
		return 1;
	while(cpyNumber!=0)
	{
		cpyNumber/=10;
		count++;
	}
	return count;
}

Numbers::~Numbers()
{
	numSprite->Release();
	numSprite = 0;

	for(short i=0; i<10; i++)
	{
		numArr[i]->Release();
		numArr[i] = 0;

	}
}