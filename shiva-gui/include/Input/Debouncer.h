///-----------------------------------------------------------------------------------------------
/// \file Debouncer.h
/// \brief This keeps track of a single input source
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_INPUTSYSTEM_DEBOUNCER__
#define __SHIVA_GUI_INPUTSYSTEM_DEBOUNCER__

#include <iostream>
#include <sstream>

namespace ShivaGUI
{

	// This keeps track of a single input source
	// How's this going to work?
	// * input -> inputPotential = true and store timestamp, activate output if leading edge
	// * time passes, activate output if delayed leading edge
	// * no input for certain time -> inputPotential = false
	
	// Need:
	// * Repeat bool - if it will repeat output if continuously pressed
	// * Repeat delay - time of continuous activation before repeat starts
	// * Repeat period - time between outputs of repeat
	// * also need option to suppress actual input after a block has completed for a period of time

	class Debouncer
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Phase type
		//----------------------------------------------------------------------------------
		enum Phase
		{
			INACTIVE,	// There is no input
			ACTIVE,		// Inputs are going on
			TRAILING,	// Actual inputs have stopped but we are still active
			SUPPRESSING	// We are inactive and are preventing inputs from reactivating us
		};
		//----------------------------------------------------------------------------------
		/// \brief Output type
		//----------------------------------------------------------------------------------
		enum OutputType
		{
			OUT_NONE = 0,	// There is no active output
			OUT_LEADING,	// The output is the leading edge of the activation
			OUT_DELAYED,	// The output is a delayed response from the leading edge
			OUT_TRAILING,	// The output is trailing
			OUT_REPEATING	// The output is a repeating one
		};
		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		Debouncer();
		//----------------------------------------------------------------------------------
		/// \brief Set trail time
		/// \param [in] _timeS
		//----------------------------------------------------------------------------------
		void SetTrailTime( float _timeS ) { m_trailMaxTime = _timeS; }
		//----------------------------------------------------------------------------------
		/// \brief Set suppression time
		/// \param [in] _timeS
		//----------------------------------------------------------------------------------
		void SetSuppressionTime( float _timeS ) { m_suppressionMaxTime = _timeS; }
		//----------------------------------------------------------------------------------
		/// \brief Set delay time
		/// \param [in] _timeS
		//----------------------------------------------------------------------------------
		void SetDelayTime( float _timeS ) { m_delayedOutputMaxTime = _timeS; }
		//----------------------------------------------------------------------------------
		/// \brief Set repeat
		//----------------------------------------------------------------------------------
		void SetRepeat( bool _enable, float _rateTimeS, float _delayTimeS, bool _repeatDebounced ) { m_repeatEnable = _enable; m_repeatRateTime = _rateTimeS; m_repeatDelayTime = _delayTimeS; m_repeatDebounced = _repeatDebounced; }
		//----------------------------------------------------------------------------------
		/// \brief Processes the input
		/// Returns true if an event should be issued
		/// \param [in] _deltaTs
		//----------------------------------------------------------------------------------
		OutputType Update( float _deltaTs );
		//----------------------------------------------------------------------------------
		/// \brief Set input
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetInput( bool _value );
		//----------------------------------------------------------------------------------

	protected:

		/// SETTINGS

		//----------------------------------------------------------------------------------
		/// \brief Max trail time 
		//----------------------------------------------------------------------------------
		float m_trailMaxTime;
		//----------------------------------------------------------------------------------
		/// \brief Max suppression time
		//----------------------------------------------------------------------------------
		float m_suppressionMaxTime;
		//----------------------------------------------------------------------------------
		/// \brief Max delayed output time
		//----------------------------------------------------------------------------------
		float m_delayedOutputMaxTime;
		//----------------------------------------------------------------------------------
		/// \brief Whether repeat is enabled
		//----------------------------------------------------------------------------------
		bool m_repeatEnable;
		//----------------------------------------------------------------------------------
		/// \brief Will allow the debounced output to trigger a repeat (i.e. a messy long input will be treated as a single press)
		/// default is false
		//----------------------------------------------------------------------------------
		bool m_repeatDebounced;
		//----------------------------------------------------------------------------------
		/// \brief Time between issuing repeat events, after the initial repeat delay
		//----------------------------------------------------------------------------------
		float m_repeatRateTime;
		//----------------------------------------------------------------------------------
		/// \brief Time of continued input before first repeat event
		//----------------------------------------------------------------------------------
		float m_repeatDelayTime;
		//----------------------------------------------------------------------------------

		/// VARIABLES

		//----------------------------------------------------------------------------------
		/// \brief Current phase
		//----------------------------------------------------------------------------------
		Phase m_currentPhase;
		//----------------------------------------------------------------------------------
		/// \brief Timer for the block
		//----------------------------------------------------------------------------------
		float m_blockTimer;
		//----------------------------------------------------------------------------------
		/// \brief Timer for the trail
		//----------------------------------------------------------------------------------
		float m_trailTimer;
		//----------------------------------------------------------------------------------
		/// \brief If we're timing the delay
		//----------------------------------------------------------------------------------
		bool m_timingDelay;
		//----------------------------------------------------------------------------------
		/// \brief Timer for the delay
		//----------------------------------------------------------------------------------
		float m_delayTimer;
		//----------------------------------------------------------------------------------
		/// \brief Timer for the suppression
		//----------------------------------------------------------------------------------
		float m_suppressionTimer;
		//----------------------------------------------------------------------------------
		/// \brief Tracks the actual input
		//----------------------------------------------------------------------------------
		bool m_inputPotential;
		//----------------------------------------------------------------------------------
		// Flags that are set when giving output
		//----------------------------------------------------------------------------------
		/// \brief Flag set for leading edge output
		//----------------------------------------------------------------------------------
		bool m_leadingEdgeOutput;
		//----------------------------------------------------------------------------------
		/// \brief Flag set for delayed lead output
		//----------------------------------------------------------------------------------
		bool m_delayedLeadOutput;
		//----------------------------------------------------------------------------------
		/// \brief Flag set for trailing edge output
		//----------------------------------------------------------------------------------
		bool m_trailingEdgeOutput;
		//----------------------------------------------------------------------------------
		/// \brief Whether to time repeat
		//----------------------------------------------------------------------------------
		bool m_timingRepeat;
		//----------------------------------------------------------------------------------
		/// \brief Whether is first repeat
		//----------------------------------------------------------------------------------
		bool m_firstRepeat;
		//----------------------------------------------------------------------------------
		/// \brief Timer for repeat
		//----------------------------------------------------------------------------------
		float m_repeatTimer;
		//----------------------------------------------------------------------------------
		/// \brief Start repeat timer
		//----------------------------------------------------------------------------------
		void StartRepeatTimer();
		//----------------------------------------------------------------------------------
		/// \brief Debug output
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void DebugOutput( std::string _value );
		//----------------------------------------------------------------------------------

	};
}

#endif
