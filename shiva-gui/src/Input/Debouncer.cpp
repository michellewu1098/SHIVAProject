
#include "Input/Debouncer.h"

#include <SDL.h>
#include <iostream>

//#define __DEBUG_DEBOUNCER__

//////////////////////////////////////////////////////////////////////////

ShivaGUI::Debouncer::Debouncer()
{
	_trailMaxTime = _suppressionMaxTime = _delayedOutputMaxTime = 0.0f;

	_inputPotential = false;
	_currentPhase = INACTIVE;
	_blockTimer = _trailTimer = _delayTimer = _suppressionTimer = 0.0f;
	_leadingEdgeOutput = _delayedLeadOutput = _trailingEdgeOutput = 0;
	_timingDelay = false;

	_repeatEnable = false;
	_repeatDebounced = false;

	_timingRepeat = false;
	_firstRepeat = true;
	_repeatTimer = 0.0f;
}

ShivaGUI::Debouncer::OutputType ShivaGUI::Debouncer::Update( float deltaTs )
{

	// Trailing -> Suppressing
	if( _currentPhase == TRAILING )
	{
		_trailTimer += deltaTs;
		if( _trailTimer >= _trailMaxTime )
		{
			// We haven't had activation input for some time, so we will switch to suppressing
			DebugOutput("Trailing -> Suppressing");
			_currentPhase = SUPPRESSING;
			_suppressionTimer = 0.0f;
			_trailingEdgeOutput = true;

			if( _repeatDebounced )
				_timingRepeat = false;
		}
	}

	// Suppressing -> Inactive
	if( _currentPhase == SUPPRESSING )
	{
		_suppressionTimer += deltaTs;
		if( _suppressionTimer >= _suppressionMaxTime )
		{
			DebugOutput("Suppressing -> Inactive");
			// We've been suppressing input for some time, so allow inputs and go back to inactive
			_currentPhase = INACTIVE;
		}
	}

	if( _timingDelay )
	{
		_delayTimer += deltaTs;
		if( _delayTimer >= _delayedOutputMaxTime )
		{
			_delayedLeadOutput = true;
			_timingDelay = false;
		}
	}

	bool outputRepeat = false;
	if( _timingRepeat )
	{
		_repeatTimer += deltaTs;
		if( _firstRepeat && ( _repeatTimer > _repeatDelayTime ) )
		{
			// First repeat event after start of input
			_firstRepeat = false;
			outputRepeat = true;
			_repeatTimer = 0.0f;
		}
		else if( !_firstRepeat && ( _repeatTimer > _repeatRateTime ) )
		{
			// A normal repeat event
			outputRepeat = true;
			_repeatTimer = 0.0f;
		}
	}

	OutputType outputPhase = OUT_NONE;
	if( _leadingEdgeOutput )
		outputPhase = OUT_LEADING;
	else if( _delayedLeadOutput )
		outputPhase = OUT_DELAYED;
	else if( _trailingEdgeOutput )
		outputPhase = OUT_TRAILING;
	else if( outputRepeat )
		outputPhase = OUT_REPEATING;

	if( _leadingEdgeOutput || _delayedLeadOutput || _trailingEdgeOutput )
	{
		std::stringstream msg;
		msg<<"output: leading: "<<std::boolalpha<<_leadingEdgeOutput<<" delayed = "<< _delayedLeadOutput<<" trailing: "<<_trailingEdgeOutput;
		DebugOutput( msg.str() );
	}

	_leadingEdgeOutput = _delayedLeadOutput = _trailingEdgeOutput = false;
	return outputPhase;
}

void ShivaGUI::Debouncer::SetInput(bool value)
{
	//bool prevPotential = _inputPotential;
	if( _currentPhase != SUPPRESSING )
	{
		_inputPotential = value;
	}

	std::stringstream msg;
	msg<<"pot = "<<std::boolalpha<<_inputPotential<<" _currentPhase = "<< _currentPhase;
	DebugOutput( msg.str() );

	// Inactive -> Active
	if( _inputPotential && _currentPhase == INACTIVE )
	{
		DebugOutput("Inactive -> Active");
		_currentPhase = ACTIVE;
		_leadingEdgeOutput = true;
		_blockTimer = 0.0f;
		_delayTimer = 0.0f;
		_timingDelay = true;
		StartRepeatTimer();
	}
	// Active -> Trailing
	else if( !_inputPotential && _currentPhase == ACTIVE )
	{
		// We are active but our input has stopped, so we start trailing
		DebugOutput("Active -> Trailing");
		_currentPhase = TRAILING;
		_trailTimer = 0.0f;
		if( !_repeatDebounced )
			_timingRepeat = false;
	}
	// Trailing -> Active
	else if( _inputPotential && _currentPhase == TRAILING )
	{
		// We are trailing but our input has just started again, so we go back to active
		DebugOutput("Trailing -> Active");
		_currentPhase = ACTIVE;
		_trailTimer = 0.0f;

		// We will need to restart the timer if it's not working on debounced input
		if( !_repeatDebounced )
			StartRepeatTimer();
		/*
		if( !prevPotential && !_repeatDebounced )
		{
			// Our previous input was  and have another input, so need to update our repeat starting point
			DebugOutput("Active reset repeat");
			//_repeatStartTicks = currentTime;
		}
		*/
	}
	// Suppressing
	else if( _currentPhase == SUPPRESSING )
	{
		DebugOutput("Suppressing");
		// We are suppressing input
		_inputPotential = false;
	}

}

void ShivaGUI::Debouncer::StartRepeatTimer()
{
	_timingRepeat = true;
	_firstRepeat = true;
	_repeatTimer = 0.0f;
}

void ShivaGUI::Debouncer::DebugOutput(std::string value)
{
#ifdef __DEBUG_DEBOUNCER__
	std::cerr<<"INFO: Debouncer: "<<value<<std::endl;
#endif
}

