#include "Input/Debouncer.h"

//----------------------------------------------------------------------------------

ShivaGUI::Debouncer::Debouncer()
{
	m_trailMaxTime = m_suppressionMaxTime = m_delayedOutputMaxTime = 0.0f;

	m_inputPotential = false;
	m_currentPhase = INACTIVE;
	m_blockTimer = m_trailTimer = m_delayTimer = m_suppressionTimer = 0.0f;
	m_leadingEdgeOutput = m_delayedLeadOutput = m_trailingEdgeOutput = 0;
	m_timingDelay = false;

	m_repeatEnable = false;
	m_repeatDebounced = false;

	m_timingRepeat = false;
	m_firstRepeat = true;
	m_repeatTimer = 0.0f;

}

//----------------------------------------------------------------------------------

ShivaGUI::Debouncer::OutputType ShivaGUI::Debouncer::Update( float _deltaTs )
{

	// Trailing -> Suppressing
	if( m_currentPhase == TRAILING )
	{
		m_trailTimer += _deltaTs;
		if( m_trailTimer >= m_trailMaxTime )
		{
			// We haven't had activation input for some time, so we will switch to suppressing
			DebugOutput( "Trailing -> Suppressing" );
			m_currentPhase = SUPPRESSING;
			m_suppressionTimer = 0.0f;
			m_trailingEdgeOutput = true;

			if( m_repeatDebounced )
				m_timingRepeat = false;
		}
	}

	// Suppressing -> Inactive
	if( m_currentPhase == SUPPRESSING )
	{
		m_suppressionTimer += _deltaTs;
		if( m_suppressionTimer >= m_suppressionMaxTime )
		{
			DebugOutput( "Suppressing -> Inactive" );
			// We've been suppressing input for some time, so allow inputs and go back to inactive
			m_currentPhase = INACTIVE;
		}
	}

	if( m_timingDelay )
	{
		m_delayTimer += _deltaTs;
		if( m_delayTimer >= m_delayedOutputMaxTime )
		{
			m_delayedLeadOutput = true;
			m_timingDelay = false;
		}
	}

	bool outputRepeat = false;
	if( m_timingRepeat )
	{
		m_repeatTimer += _deltaTs;
		if( m_firstRepeat && ( m_repeatTimer > m_repeatDelayTime ) )
		{
			// First repeat event after start of input
			m_firstRepeat = false;
			outputRepeat = true;
			m_repeatTimer = 0.0f;
		}
		else if( !m_firstRepeat && ( m_repeatTimer > m_repeatRateTime ) )
		{
			// A normal repeat event
			outputRepeat = true;
			m_repeatTimer = 0.0f;
		}
	}

	OutputType outputPhase = OUT_NONE;
	if( m_leadingEdgeOutput )
		outputPhase = OUT_LEADING;
	else if( m_delayedLeadOutput )
		outputPhase = OUT_DELAYED;
	else if( m_trailingEdgeOutput )
		outputPhase = OUT_TRAILING;
	else if( outputRepeat )
		outputPhase = OUT_REPEATING;

	if( m_leadingEdgeOutput || m_delayedLeadOutput || m_trailingEdgeOutput )
	{
		std::stringstream msg;
		msg << "output: leading: " << std::boolalpha << m_leadingEdgeOutput << " delayed = " << m_delayedLeadOutput << " trailing: " << m_trailingEdgeOutput;
		DebugOutput( msg.str() );
	}

	m_leadingEdgeOutput = m_delayedLeadOutput = m_trailingEdgeOutput = false;
	return outputPhase;
}

//----------------------------------------------------------------------------------

void ShivaGUI::Debouncer::SetInput( bool _value )
{
	if( m_currentPhase != SUPPRESSING )
	{
		m_inputPotential = _value;
	}

	std::stringstream msg;
	msg << "pot = " << std::boolalpha << m_inputPotential << " _currentPhase = " << m_currentPhase;
	DebugOutput( msg.str() );

	// Inactive -> Active
	if( m_inputPotential && m_currentPhase == INACTIVE )
	{
		DebugOutput( "Inactive -> Active" );
		m_currentPhase = ACTIVE;
		m_leadingEdgeOutput = true;
		m_blockTimer = 0.0f;
		m_delayTimer = 0.0f;
		m_timingDelay = true;
		StartRepeatTimer();
	}
	// Active -> Trailing
	else if( !m_inputPotential && m_currentPhase == ACTIVE )
	{
		// We are active but our input has stopped, so we start trailing
		DebugOutput( "Active -> Trailing" );
		m_currentPhase = TRAILING;
		m_trailTimer = 0.0f;
		if( !m_repeatDebounced )
			m_timingRepeat = false;
	}
	// Trailing -> Active
	else if( m_inputPotential && m_currentPhase == TRAILING )
	{
		// We are trailing but our input has just started again, so we go back to active
		DebugOutput( "Trailing -> Active" );
		m_currentPhase = ACTIVE;
		m_trailTimer = 0.0f;

		// We will need to restart the timer if it's not working on debounced input
		if( !m_repeatDebounced )
			StartRepeatTimer();
	}
	// Suppressing
	else if( m_currentPhase == SUPPRESSING )
	{
		DebugOutput( "Suppressing" );
		// We are suppressing input
		m_inputPotential = false;
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::Debouncer::StartRepeatTimer()
{
	m_timingRepeat = true;
	m_firstRepeat = true;
	m_repeatTimer = 0.0f;
}

//----------------------------------------------------------------------------------

void ShivaGUI::Debouncer::DebugOutput( std::string _value )
{
#ifdef __DEBUG_DEBOUNCER__
	std::cerr << "INFO: Debouncer: " << _value << std::endl;
#endif
}

//----------------------------------------------------------------------------------
