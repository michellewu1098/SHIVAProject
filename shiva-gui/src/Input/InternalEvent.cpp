#include "Input/InternalEvent.h"

//----------------------------------------------------------------------------------

ShivaGUI::InternalEvent::EventType ShivaGUI::InternalEvent::ConvertEventType( std::string _inputString )
{
	// Could use a std::map for this, but want it to be case-independent so will manually check instead
	if( boost::algorithm::iequals( "QUIT", _inputString ) )
	{
		return QUIT;
	}
	else if( boost::algorithm::iequals( "FIRSTFOCUS", _inputString ) )
	{
		return FIRSTFOCUS;
	}
	else if( boost::algorithm::iequals( "NEXTFOCUS_UP", _inputString ) )
	{
		return NEXTFOCUS_UP;
	}
	else if( boost::algorithm::iequals( "NEXTFOCUS_DOWN", _inputString ) )
	{
		return NEXTFOCUS_DOWN;
	}
	else if( boost::algorithm::iequals( "NEXTFOCUS_LEFT", _inputString ) )
	{
		return NEXTFOCUS_LEFT;
	}
	else if( boost::algorithm::iequals( "NEXTFOCUS_RIGHT", _inputString ) )
	{
		return NEXTFOCUS_RIGHT;
	}
	else if( boost::algorithm::iequals( "CLEARFOCUS", _inputString ) )
	{
		return CLEARFOCUS;
	}
	else if( boost::algorithm::iequals( "WINDOW_RESIZE", _inputString ) )
	{
		return WINDOW_RESIZE;
	}
	else if( boost::algorithm::iequals( "BACK", _inputString ) )
	{
		return BACK;
	}
	else if( boost::algorithm::iequals( "POSITIONAL_SELECT", _inputString ) )
	{
		return POSITIONAL_SELECT;
	}
	else if( boost::algorithm::iequals( "POSITIONAL_DESELECT", _inputString ) )
	{
		return POSITIONAL_DESELECT;
	}
	else if( boost::algorithm::iequals( "POSITIONAL_DRAG", _inputString ) )
	{
		return POSITIONAL_DRAG;
	}
	else if( boost::algorithm::iequals( "FIRSTFOCUS", _inputString ) )
	{
		return FIRSTFOCUS;
	}
	else if( boost::algorithm::iequals( "FOCUS_HIGHLIGHT", _inputString ) )
	{
		return FOCUS_HIGHLIGHT;
	}
	else if( boost::algorithm::iequals( "FOCUS_SELECT_HOLD", _inputString ) )
	{
		return FOCUS_SELECT_HOLD;
	}
	else if( boost::algorithm::iequals( "FOCUS_SELECT_RELEASE", _inputString ) )
	{
		return FOCUS_SELECT_RELEASE;
	}
	else if( boost::algorithm::iequals( "FOCUS_DESELECT", _inputString ) )
	{
		return FOCUS_DESELECT;
	}
	else if( boost::algorithm::iequals( "SCAN_HIGHLIGHT", _inputString ) )
	{
		return SCAN_HIGHLIGHT;
	}
	else if( boost::algorithm::iequals( "SCAN_SELECT", _inputString ) )
	{
		return SCAN_SELECT;
	}
	else if( boost::algorithm::iequals( "SCAN_START", _inputString ) )
	{
		return SCAN_START;
	}
	else if( boost::algorithm::iequals( "SCAN_STOP", _inputString ) )
	{
		return SCAN_STOP;
	}
	else if( boost::algorithm::iequals( "SCAN_FIRSTFOCUS", _inputString ) )
	{
		return SCAN_FIRSTFOCUS;
	}
	else if( boost::algorithm::iequals( "SCAN_SELECTFOCUS", _inputString ) )
	{
		return SCAN_SELECTFOCUS;
	}
	else if( boost::algorithm::iequals( "SCAN_HIGHLIGHTFOCUS", _inputString ) )
	{
		return SCAN_HIGHLIGHTFOCUS;
	}
	else if( boost::algorithm::iequals( "SCAN_NEXTFOCUS", _inputString ) )
	{
		return SCAN_NEXTFOCUS;
	}
	else if( boost::algorithm::iequals( "SCAN_PREVFOCUS", _inputString ) )
	{
		return SCAN_PREVFOCUS;
	}
	else if( boost::algorithm::iequals( "EDIT_CUSTOMLAYOUT", _inputString ) )
	{
		return EDIT_CUSTOMLAYOUT;
	}

	return INVALID;
}

//----------------------------------------------------------------------------------

ShivaGUI::InternalEvent::InternalEvent( EventType _inputType )
{
	m_type = _inputType;
	m_winWidth = m_winHeight = 0;
	m_posX = m_posY = 0;
}

//----------------------------------------------------------------------------------