////////////////////
// Include Guards //
////////////////////
#ifndef WINDOW_H
#define WINDOW_H
#pragma once

//////////////
// Includes //
//////////////
#include<Windows.h>

class Window
{
public:
	/////////////
	// Methods //
	/////////////
	Window(LPCTSTR str_Title,int int_XPos, int int_YPos, int int_Width, int int_Height);
	~Window();
	 
	///////////////
	// Variables //
	///////////////
	HWND hWindow; // handle to the window
	short WindowWidth, WindowHeight; // window size
};

static LRESULT CALLBACK OurWindowProcedure(HWND ,UINT ,WPARAM ,LPARAM);
#endif