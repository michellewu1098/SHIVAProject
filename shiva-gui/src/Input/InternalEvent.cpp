#include "Input/InternalEvent.h"
#include <boost/algorithm/string.hpp>

ShivaGUI::InternalEvent::EventType ShivaGUI::InternalEvent::ConvertEventType(std::string inputString)
{
	// Could use a std::map for this, but want it to be case-independent so will manually check instead
	if( boost::algorithm::iequals( "QUIT", inputString ) )
	{
		return QUIT;
	}
	else if( boost::algorithm::iequals( "FIRSTFOCUS", inputString ) )
	{
		return FIRSTFOCUS;
	}
	else if( boost::algorithm::iequals( "NEXTFOCUS_UP", inputString ) )
	{
		return NEXTFOCUS_UP;
	}
	else if( boost::algorithm::iequals( "NEXTFOCUS_DOWN", inputString ) )
	{
		return NEXTFOCUS_DOWN;
	}
	else if( boost::algorithm::iequals( "NEXTFOCUS_LEFT", inputString ) )
	{
		return NEXTFOCUS_LEFT;
	}
	else if( boost::algorithm::iequals( "NEXTFOCUS_RIGHT", inputString ) )
	{
		return NEXTFOCUS_RIGHT;
	}
	else if( boost::algorithm::iequals( "CLEARFOCUS", inputString ) )
	{
		return CLEARFOCUS;
	}
	else if( boost::algorithm::iequals( "WINDOW_RESIZE", inputString ) )
	{
		return WINDOW_RESIZE;
	}
	else if( boost::algorithm::iequals( "BACK", inputString ) )
	{
		return BACK;
	}
	else if( boost::algorithm::iequals( "POSITIONAL_SELECT", inputString ) )
	{
		return POSITIONAL_SELECT;
	}
	else if( boost::algorithm::iequals( "POSITIONAL_DESELECT", inputString ) )
	{
		return POSITIONAL_DESELECT;
	}
	else if( boost::algorithm::iequals( "POSITIONAL_DRAG", inputString ) )
	{
		return POSITIONAL_DRAG;
	}

	else if( boost::algorithm::iequals( "FIRSTFOCUS", inputString ) )
	{
		return FIRSTFOCUS;
	}
	else if( boost::algorithm::iequals( "FOCUS_HIGHLIGHT", inputString ) )
	{
		return FOCUS_HIGHLIGHT;
	}
	else if( boost::algorithm::iequals( "FOCUS_SELECT_HOLD", inputString ) )
	{
		return FOCUS_SELECT_HOLD;
	}
	else if( boost::algorithm::iequals( "FOCUS_SELECT_RELEASE", inputString ) )
	{
		return FOCUS_SELECT_RELEASE;
	}
	else if( boost::algorithm::iequals( "FOCUS_DESELECT", inputString ) )
	{
		return FOCUS_DESELECT;
	}

	else if( boost::algorithm::iequals( "SCAN_HIGHLIGHT", inputString ) )
	{
		return SCAN_HIGHLIGHT;
	}
	else if( boost::algorithm::iequals( "SCAN_SELECT", inputString ) )
	{
		return SCAN_SELECT;
	}
	else if( boost::algorithm::iequals( "SCAN_START", inputString ) )
	{
		return SCAN_START;
	}
	else if( boost::algorithm::iequals( "SCAN_STOP", inputString ) )
	{
		return SCAN_STOP;
	}
	else if( boost::algorithm::iequals( "SCAN_FIRSTFOCUS", inputString ) )
	{
		return SCAN_FIRSTFOCUS;
	}
	else if( boost::algorithm::iequals( "SCAN_SELECTFOCUS", inputString ) )
	{
		return SCAN_SELECTFOCUS;
	}
	else if( boost::algorithm::iequals( "SCAN_HIGHLIGHTFOCUS", inputString ) )
	{
		return SCAN_HIGHLIGHTFOCUS;
	}
	else if( boost::algorithm::iequals( "SCAN_NEXTFOCUS", inputString ) )
	{
		return SCAN_NEXTFOCUS;
	}
	else if( boost::algorithm::iequals( "SCAN_PREVFOCUS", inputString ) )
	{
		return SCAN_PREVFOCUS;
	}
	else if( boost::algorithm::iequals( "EDIT_CUSTOMLAYOUT", inputString ) )
	{
		return EDIT_CUSTOMLAYOUT;
	}

	return INVALID;
}

ShivaGUI::InternalEvent::InternalEvent(EventType inputType)
{
	_type = inputType;
	_winWidth = _winHeight = 0;
	_posX = _posY = 0;
}
