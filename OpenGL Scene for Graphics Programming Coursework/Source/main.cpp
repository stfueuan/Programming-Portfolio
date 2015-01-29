//Graphics Programming Coursework
//Euan Watt 1200755

//Includes
#include <windows.h>
#include "Input.h"
#include <iostream>
#include <mmsystem.h>
#include <math.h>
#include "Scene3D.h"
#include <gl/gl.h>
#include <gl/glu.h>

//Globals
HWND hwnd;
Input myInput;
Scene3D myScene;

//Prototypes
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
void RegisterMyWindow(HINSTANCE);
BOOL InitialiseMyWindow(HINSTANCE, int);
int WINAPI WinMain (HINSTANCE, HINSTANCE, PSTR, int);
BOOL WaitFor(unsigned long delay);
void setBuffers();
void displayFrame();

void RegisterMyWindow(HINSTANCE hInstance)
{
    WNDCLASSEX  wcex;				// Contains window class information

    wcex.cbSize        = sizeof (wcex);			// Size in bytes of structu
    wcex.style         = CS_HREDRAW | CS_VREDRAW;	// Class style (in this case re-draw when width or 						    height changes)
    wcex.lpfnWndProc   = WndProc;			// Pointer to the window procedure
    wcex.cbClsExtra    = 0;			// extra bytes
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;			// Handle to the instance that contains Window Proc
    wcex.hIcon         = 0; 			// System default icon
    wcex.hCursor       = LoadCursor (NULL, IDC_ARROW);	// System default cursor
    wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW+0);	// Handle for background brush (colour)
    wcex.lpszMenuName  = NULL;			// Set menu, in this case none
    wcex.lpszClassName = "FirstWindowClass";		// Window’s classname
    wcex.hIconSm       = 0; 			// Handle to small icon

RegisterClassEx (&wcex);			// Registers the window for further calls
}

BOOL InitialiseMyWindow(HINSTANCE hInstance, int nCmdShow)
{

 	  hwnd = CreateWindow ("FirstWindowClass",		// Classname (same as previous slide)
 			 "Graphics Programming Coursework, Created by Euan Watt 1200755",  		// Window name
			 WS_OVERLAPPEDWINDOW,		// style of window being created
			 CW_USEDEFAULT,			
			 CW_USEDEFAULT,
			 800,		// width of window
			 600,		// Height of window
			 NULL,			// parent of window
			 NULL,			// handle to menu
			 hInstance,			// Handle to instance associated with window
			 NULL);			// pointer to value to pass to window
	if (!hwnd)
	{
		return FALSE;
	}

    ShowWindow (hwnd, nCmdShow);
    UpdateWindow (hwnd);
	
	return TRUE;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
	MSG msg;
	RegisterMyWindow(hInstance);
	
   	if (!InitialiseMyWindow(hInstance, nCmdShow))
		return FALSE;

	myScene.Init(hwnd);

	while (TRUE)
    {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
		      if (msg.message == WM_QUIT)
		             break;
		      TranslateMessage (&msg);
	   	      DispatchMessage (&msg);
		}
		else
		{
			if(myInput.isKeyDown(VK_ESCAPE))
			{
				break;

				myInput.SetKeyUp(VK_ESCAPE);
			}

			myScene.DrawScene(myInput);
		}
  	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
		case WM_CREATE:
			break;

		case WM_SIZE:
			//myScene.Resize(hwnd);
			break;

		case WM_PAINT:
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_KEYDOWN:
			myInput.SetKeyDown(wParam);	//pass key value to input class
			break;

		case WM_KEYUP:
			myInput.SetKeyUp(wParam);		// pass key value to input class
			break;

		case WM_MOUSEMOVE:
			// pass cursor x position to input class
			myInput.setMouseX(LOWORD (lParam));	
			// pass cursor y position to input class
			myInput.setMouseY(HIWORD (lParam));
			break;

		case WM_LBUTTONUP:
			myInput.LMouseUp();
			break;

		case WM_LBUTTONDOWN:
			myInput.LMouseClick();
			break;

		case WM_RBUTTONUP:
			myInput.LMouseUp();
			break;

		case WM_RBUTTONDOWN:
			myInput.RMouseClick();
			break;
	}

	return DefWindowProc (hwnd, message, wParam, lParam);
}
