// Input class

#include <Windows.h>
#include "Input.h"


void Input::SetKeyDown(WPARAM key)
{
	keys[key] = true;
}

void Input::SetKeyUp(WPARAM key)
{
	keys[key] = false;
}

bool Input::isMouseDown() //split these for each mouse button
{
	if(mouse.left == true || mouse.right == true)
	{
		return true;
	}
}

bool Input::isLmousedown()
{
	return mouse.left;
}

bool Input::isMouseUp()
{
	if(mouse.left == false || mouse.right == false)
	{
		return false;
	}
}

bool Input::isMouseClicked()
{
	return 0; //not set up
}

bool Input::isKeyDown(int key)
{
	return keys[key];
}

//no mouse click stuff set up properly
void Input::LMouseClick()
{
	mouse.left = true;
}

void Input::LMouseUp()
{
	mouse.left = false;
}

void Input::RMouseClick()
{
	mouse.right = true;
}

void Input::RMouseUp()
{
	mouse.right = false;
}

void Input::MMouseClick()
{
	mouse.middle = true;
}

void Input::DBMouseClick()
{
	mouse.doubleclick = true;
}

//mouse position getters and setters
int Input::setMouseX(int pos)
{
	return mouse.x = pos;
}

int Input::setMouseY(int pos)
{
	return mouse.y = pos;
}

int Input::getMouseX()
{
	return mouse.x;
}

int Input::getMouseY()
{
	return mouse.y;
}
