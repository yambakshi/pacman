#include "Window.h"
Window::Window(LPCTSTR str_Title,int int_XPos, int int_YPos, int int_Width, int int_Height)
{
	this->WindowWidth = int_Width;
	this->WindowHeight = int_Height;
	WNDCLASSEX wnd_Structure;
	wnd_Structure.cbSize = sizeof(WNDCLASSEX);
	wnd_Structure.style = CS_HREDRAW | CS_VREDRAW;
	wnd_Structure.lpfnWndProc = &OurWindowProcedure;
	wnd_Structure.cbClsExtra = 0;
	wnd_Structure.cbWndExtra = 0;
	wnd_Structure.hInstance = GetModuleHandle(NULL);
	wnd_Structure.hIcon = NULL;
	wnd_Structure.hCursor = NULL;
	wnd_Structure.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
	wnd_Structure.lpszMenuName = NULL;
	wnd_Structure.lpszClassName = "WindowClassName";
	wnd_Structure.hIconSm = LoadIcon(NULL,IDI_APPLICATION);

	RegisterClassEx(&wnd_Structure);

	 
	hWindow = CreateWindowEx(
		WS_EX_CONTROLPARENT, // The extended window style of the window being created.
		"WindowClassName", // name of the window class
		str_Title, // title of the window
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, // window style
		int_XPos, // x-position of the window
		int_YPos, // y-position of the window
		int_Width, // width of the window
		int_Height, // height of the window
		NULL, // we have no parent window, NULL
		NULL, // we aren't using menus, NULL
		GetModuleHandle(NULL), // application handle
		NULL); // used with multiple windows, NULL
}

// This function is triggered every time an event 
// that is related to the specific window occurs
LRESULT CALLBACK OurWindowProcedure(HWND hWindow, UINT uint_Message, WPARAM parameter1, LPARAM parameter2)
{
	switch(uint_Message)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}
	}
    return DefWindowProc(hWindow,uint_Message,parameter1,parameter2);
}

Window::~Window()
{
	DestroyWindow(hWindow);
}
