#include "Input/InputEvent.h"

//----------------------------------------------------------------------------------

ShivaGUI::InputEvent::InputEvent()
{
	m_type = INVALID;
	m_mouseX = m_mouseY = 0;
	m_winW = m_winH = 0;
	m_key = 0;
	m_mouseButton = 0;
	m_winID = -1;
}

//----------------------------------------------------------------------------------

void ShivaGUI::InputEvent::SetQuitEvent()
{
	m_type = QUIT;
}

//----------------------------------------------------------------------------------

void ShivaGUI::InputEvent::SetKeyEvent( bool _keyUp, int _key, int _windowID )
{
	if( _keyUp )
		m_type = KEYUP;
	else
		m_type = KEYDOWN;
	m_key = _key;
	m_winID = _windowID;
}

//----------------------------------------------------------------------------------

void ShivaGUI::InputEvent::SetMouseMotionEvent( int _mouseX, int _mouseY, int _windowID )
{
	m_type = MOUSEMOTION;
	m_mouseX = _mouseX;
	m_mouseY = _mouseY;
	m_winID = _windowID;
}

//----------------------------------------------------------------------------------

void ShivaGUI::InputEvent::SetMouseButtonEvent( bool _buttonUp, int _button, float _mouseX, float _mouseY, int _windowID )
{
	if( _buttonUp )
		m_type = MOUSEBUTTONUP;
	else
		m_type = MOUSEBUTTONDOWN;
	m_mouseButton = _button;
	m_mouseX = ( int )_mouseX;
	m_mouseY = ( int )_mouseY;
	m_winID = _windowID;
}

//----------------------------------------------------------------------------------

void ShivaGUI::InputEvent::SetWindowSizeEvent( int _w, int _h, int _windowID )
{
	m_type = WINDOWRESIZE;
	m_winW = _w;
	m_winH = _h;
	m_winID = _windowID;
}

//----------------------------------------------------------------------------------

void ShivaGUI::InputEvent::SetWindowCloseEvent( int _windowID )
{
	m_type = WINDOWCLOSE;
	m_winID = _windowID;
}

//----------------------------------------------------------------------------------
