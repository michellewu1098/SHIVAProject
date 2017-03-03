
#include "Input/Scanners/Autoscanner.h"

#include <boost/algorithm/string.hpp>
#include <iostream>

#define __DEBUG_SCANNER__

//////////////////////////////////////////////////////////////////////////

ShivaGUI::Autoscanner::Autoscanner()
{
	_needFirstFocus = true;
	_issueHighlight = false;
	_issueSelection = false;
	_repeatRateTime = 1.0f;
	_scanTimer = 0.0f;
	_eventCount = 0;
	_goTimer = true;
}

bool ShivaGUI::Autoscanner::Update( InternalEvent::EventType &outputEvent, float deltaTs )
{
	if( _needFirstFocus )
	{
		_needFirstFocus = false;
		outputEvent = InternalEvent::SCAN_FIRSTFOCUS;
		DebugOutput("INFO: Autoscanner issuing SCAN_FIRSTFOCUS");
		return true;
	}
	else if( _issueHighlight )
	{
		_issueHighlight = false;
		// Stop timer
		_goTimer = false;
		outputEvent = InternalEvent::SCAN_HIGHLIGHTFOCUS;
		DebugOutput("INFO: Autoscanner issuing SCAN_HIGHLIGHTFOCUS");

		return true;
	}
	else if( _issueSelection )
	{
		// Reset timer
		_scanTimer = 0.0f;
		_eventCount = 0;
		_goTimer = true;

		_issueSelection = false;

		// Issue event
		outputEvent = InternalEvent::SCAN_SELECTFOCUS;
		DebugOutput("INFO: Autoscanner issuing SCAN_SELECTFOCUS");
		return true;
	}
	else if( _goTimer )
	{
		// Update timer
		_scanTimer += deltaTs;
		unsigned int modTime =  ( unsigned int )( _scanTimer / _repeatRateTime );
		if( modTime > _eventCount )
		{
			outputEvent = InternalEvent::SCAN_NEXTFOCUS;
			++_eventCount;
			DebugOutput("INFO: Autoscanner issuing SCAN_NEXTFOCUS");
			return true;
		}
	}

	return false;
}

void ShivaGUI::Autoscanner::SetInput( InternalEvent::EventType inputEvent )
{

	if( inputEvent == InternalEvent::SCAN_HIGHLIGHT )
	{
		_issueHighlight = true;
		DebugOutput("INFO: Autoscanner received highlight request");
	}
	else if( inputEvent == InternalEvent::SCAN_SELECT )
	{
		_issueSelection = true;
		DebugOutput("INFO: Autoscanner received selection request");
	}
}

