#include "Input/Scanners/Autoscanner.h"

#define __DEBUG_SCANNER__

//----------------------------------------------------------------------------------

ShivaGUI::Autoscanner::Autoscanner()
{
	m_needFirstFocus = true;
	m_issueHighlight = false;
	m_issueSelection = false;
	m_repeatRateTime = 1.0f;
	m_scanTimer = 0.0f;
	m_eventCount = 0;
	m_goTimer = true;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::Autoscanner::Update( InternalEvent::EventType &_outputEvent, float _deltaTs )
{
	if( m_needFirstFocus )
	{
		m_needFirstFocus = false;
		_outputEvent = InternalEvent::SCAN_FIRSTFOCUS;
		DebugOutput( "INFO: Autoscanner issuing SCAN_FIRSTFOCUS" );
		return true;
	}
	else if( m_issueHighlight )
	{
		m_issueHighlight = false;
		// Stop timer
		m_goTimer = false;
		_outputEvent = InternalEvent::SCAN_HIGHLIGHTFOCUS;
		DebugOutput( "INFO: Autoscanner issuing SCAN_HIGHLIGHTFOCUS" );
		return true;
	}
	else if( m_issueSelection )
	{
		// Reset timer
		m_scanTimer = 0.0f;
		m_eventCount = 0;
		m_goTimer = true;

		m_issueSelection = false;

		// Issue event
		_outputEvent = InternalEvent::SCAN_SELECTFOCUS;
		DebugOutput( "INFO: Autoscanner issuing SCAN_SELECTFOCUS" );
		return true;
	}
	else if( m_goTimer )
	{
		// Update timer
		m_scanTimer += _deltaTs;
		unsigned int modTime =  ( unsigned int )( m_scanTimer / m_repeatRateTime );
		if( modTime > m_eventCount )
		{
			_outputEvent = InternalEvent::SCAN_NEXTFOCUS;
			++m_eventCount;
			DebugOutput( "INFO: Autoscanner issuing SCAN_NEXTFOCUS" );
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------------

void ShivaGUI::Autoscanner::SetInput( InternalEvent::EventType _inputEvent )
{

	if( _inputEvent == InternalEvent::SCAN_HIGHLIGHT )
	{
		m_issueHighlight = true;
		DebugOutput( "INFO: Autoscanner received highlight request" );
	}
	else if( _inputEvent == InternalEvent::SCAN_SELECT )
	{
		m_issueSelection = true;
		DebugOutput( "INFO: Autoscanner received selection request" );
	}
}

//----------------------------------------------------------------------------------