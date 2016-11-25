#include "Input/InputEvent.h"

ShivaGUI::InputEvent::InputEvent()
{
	_type = INVALID;
	_mouseX = _mouseY = 0;
	_winW = _winH = 0;
	_key = 0;
	_mouseButton = 0;
	_winID = -1;
}

void ShivaGUI::InputEvent::SetQuitEvent()
{
	_type = QUIT;
}

void ShivaGUI::InputEvent::SetKeyEvent(bool keyUp, int key, int windowID)
{
	if( keyUp )
		_type = KEYUP;
	else
		_type = KEYDOWN;
	_key = key;
	_winID = windowID;
}

void ShivaGUI::InputEvent::SetMouseMotionEvent(int mouseX, int mouseY, int windowID)
{
	_type = MOUSEMOTION;
	_mouseX = mouseX;
	_mouseY = mouseY;
	_winID = windowID;
}

void ShivaGUI::InputEvent::SetMouseButtonEvent(bool buttonUp, int button, float mouseX, float mouseY, int windowID)
{
	if( buttonUp )
		_type = MOUSEBUTTONUP;
	else
		_type = MOUSEBUTTONDOWN;
	_mouseButton = button;
	_mouseX = mouseX;
	_mouseY = mouseY;
	_winID = windowID;
}

void ShivaGUI::InputEvent::SetWindowSizeEvent(int w, int h, int windowID)
{
	_type = WINDOWRESIZE;
	_winW = w;
	_winH = h;
	_winID = windowID;
}

void ShivaGUI::InputEvent::SetWindowCloseEvent(int windowID)
{
	_type = WINDOWCLOSE;
	_winID = windowID;
}

