#include "MyRect.h"

MyRect::MyRect(){}
MyRect::MyRect(const MyRect& other){}
MyRect::MyRect(DXDevice* pDevice)
{
	this->pDevice = pDevice;

	// Default rectangle settings
	this->x = 0;
	this->y = 0;
	this->width = 50;
	this->height = 50;
	this->color = D3DCOLOR_XRGB(0, 0, 0);

	// filling the vertices array
	vertices[0] = CUSTOMFLATVERTEX(x,         y + height,  0.0f, 1.0f, color);
	vertices[1] = CUSTOMFLATVERTEX(x,         y,           0.0f, 1.0f, color);
	vertices[2] = CUSTOMFLATVERTEX(x + width, y + height,  0.0f, 1.0f, color);
	vertices[3] = CUSTOMFLATVERTEX(x + width, y,           0.0f, 1.0f, color);

	// creating the vertex buffer
	pVertexBuffer = NULL;

	pDevice->p_dx_Device->CreateVertexBuffer(
		4*sizeof(CUSTOMFLATVERTEX),
		0,
		CUSTOMFLATFVF,
		D3DPOOL_DEFAULT,
		&pVertexBuffer,
		NULL);

	UpdateBuffer();
}
MyRect::MyRect(short x, short y , short width, short height, DWORD color, DXDevice* pDevice)
{
	this->pDevice = pDevice;
	this->x = x;
	this->y = y;
	this->width = width;
	this-> height = height;
	this->color = color;

	// filling the vertices array
	vertices[0] = CUSTOMFLATVERTEX(x,         y + height,  0.0f, 1.0f, color);
	vertices[1] = CUSTOMFLATVERTEX(x,         y,           0.0f, 1.0f, color);
	vertices[2] = CUSTOMFLATVERTEX(x + width, y + height,  0.0f, 1.0f, color);
	vertices[3] = CUSTOMFLATVERTEX(x + width, y,           0.0f, 1.0f, color);
	
	// creating the vertex buffer
	pVertexBuffer = NULL;

	pDevice->p_dx_Device->CreateVertexBuffer(
		4*sizeof(CUSTOMFLATVERTEX),
		0,
		CUSTOMFLATFVF,
		D3DPOOL_DEFAULT,
		&pVertexBuffer,
		NULL);

	UpdateBuffer();
}

// Places the rectangle
void MyRect::PlaceRect(short newX, short newY)
{
	x = newX;
	y = newY;
	vertices[0] = CUSTOMFLATVERTEX(x,         y + height,  0.0f, 1.0f, color);
	vertices[1] = CUSTOMFLATVERTEX(x,         y,           0.0f, 1.0f, color);
	vertices[2] = CUSTOMFLATVERTEX(x + width, y + height,  0.0f, 1.0f, color);
	vertices[3] = CUSTOMFLATVERTEX(x + width, y,           0.0f, 1.0f, color);

	UpdateBuffer();
}

// Scales the rectangle
void MyRect::ScaleRect(short newWidth, short newHeight)
{
	width = newWidth;
	height = newHeight;
	vertices[0] = CUSTOMFLATVERTEX(x,         y + height,  0.0f, 1.0f, color);
	vertices[1] = CUSTOMFLATVERTEX(x,         y,           0.0f, 1.0f, color);
	vertices[2] = CUSTOMFLATVERTEX(x + width, y + height,  0.0f, 1.0f, color);
	vertices[3] = CUSTOMFLATVERTEX(x + width, y,           0.0f, 1.0f, color);

	UpdateBuffer();
}

// Fills the buffer with the new defined vertices 
void MyRect::UpdateBuffer()
{
	// pointer to the soon-to-be- 
	// locked VertexData
	VOID* pVoid;

	// locks a specific range of bytes 
	// in the vertex buffer in order 
	// to read the bytes or write to the
	// bytes without them being accessed
	// by other applications
	pVertexBuffer->Lock(
		0, // The offset byte to start lock from 
		   // (if this parameter and the next parameter
		   // are set to 0, al the vertex buffer is locked)
		0, // the amount of bytes to lock
		(void**)&pVoid,
		0); // the type of lock (read only/write only etc...)

	memcpy(
		pVoid, // Destination - pointer to the VertexData
		vertices, // Sorce - The Vertex Data the is copied to the VertexBuffer
		sizeof(vertices)); // Size - The Size of the Vertex Data

	// Releases the locked bits
	pVertexBuffer->Unlock();
}

 //Draw's the Rectangle
void MyRect::Draw()
{
	// sets the buffer as the vertext streaming source
	pDevice->p_dx_Device->SetStreamSource(0, pVertexBuffer , 0, sizeof(CUSTOMFLATVERTEX));

	// sets the format of the vertex (and older verson of the vertexdecleration)
    pDevice->p_dx_Device->SetFVF(CUSTOMFLATFVF);
	
	// draws the rectangle
    pDevice->p_dx_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

MyRect::~MyRect()
{
	// releases the bytes held by the vertexbuffer
	pVertexBuffer->Release();
	pVertexBuffer = 0;
}
