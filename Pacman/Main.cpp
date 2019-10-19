//////////////
// Includes //
//////////////
#include<windows.h>
#include<d3d9.h> 
#include<d3dx9.h> 
#include<sys/utime.h>

/////////////////
// My Includes //
/////////////////
#include "Window.h"
#include "PacmanGame.h"

//504 / 558
//672 / 774

///////////////
// Variables //
///////////////

// Creates the Main application window
const short 
	ScreenWidth = GetSystemMetrics(SM_CXFULLSCREEN),
	ScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN),
	WindowWidth = ScreenWidth / 2.4,
	WindowHeight = 1.08 * WindowWidth;

Window MainWindow(
	"Pacman++",
	(ScreenWidth/2)-(WindowWidth/2),
	(ScreenHeight/2)-(WindowHeight/2),
	WindowWidth,WindowHeight); // Creates the window

// Entry point
int CALLBACK WinMain(HINSTANCE hInstance,HINSTANCE hPreviousInstance,LPSTR lpcmdline,int nCmdShow)
{
	// this line makes sure that every run of the program
	// is different than last time in term of randomness.
	// without this line, everything that is supposed to
	// be random in the program will be the same every run
	srand(time(NULL));

	// Initialize the pacman game
	PacmanGame* pg = new PacmanGame(&MainWindow);

	// Main message loop:
    MSG msg;
	
	while(true)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			// Translate keyboard messages into WM_CHAR messages.
			TranslateMessage(&msg);
			// Sends the messages to the window procedure
			DispatchMessage(&msg);
		}	

		// update and draw the game
		pg->Run(msg.message,msg.wParam);
	}
	
	delete pg;
	pg = 0;

    return 0;
}