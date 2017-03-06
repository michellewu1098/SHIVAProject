///-----------------------------------------------------------------------------------------------
/// \file Autoscanner.h
/// \brief Autoscanner class
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_INPUTSYSTEM_SCANNERS_AUTOSCANNER__
#define __SHIVA_GUI_INPUTSYSTEM_SCANNERS_AUTOSCANNER__

#include "Input/Scanners/Scanner.h"

namespace ShivaGUI
{
	// Scan modes:
	// * Autoscan - automatic progression, one button chooses item with SCAN_SELECT
	// * Userscan - manual progression, one button issues SCAN_START on press and SCAN_SELECT on release
	// * Stepscan - buttons to progress with SCAN_NEXT and optionally SCAN_PREV, one to select with SCAN_SELECT
	// * beyond Stepscan, should use focus rather than scanner
	// TODO:
	// * Overscan - fast auto progression, one press slows and reverses, second press selects

	class Autoscanner : public Scanner
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		Autoscanner();
		//----------------------------------------------------------------------------------
		/// \brief Returns true if an event needs to be issued
		/// \param [in] _outputEvent
		/// \param [in] _deltaTs
		//----------------------------------------------------------------------------------
		virtual bool Update( InternalEvent::EventType &_outputEvent, float _deltaTs );
		//----------------------------------------------------------------------------------
		/// \brief Set input
		/// \param [in] _inputEvent
		//----------------------------------------------------------------------------------
		virtual void SetInput( InternalEvent::EventType _inputEvent );
		//----------------------------------------------------------------------------------
		/// \brief Set delay time
		/// \param [in] _timeS
		//----------------------------------------------------------------------------------
		void SetDelayTime( float _timeS ) { m_repeatRateTime = _timeS; }


	protected:

		// SETTINGS

		//----------------------------------------------------------------------------------
		/// \brief Time between issuing progression events
		//----------------------------------------------------------------------------------
		float m_repeatRateTime;
		//----------------------------------------------------------------------------------

		/// VARIABLES

		//----------------------------------------------------------------------------------
		/// \brief Scanning start time
		//----------------------------------------------------------------------------------
		float m_scanTimer;
		//----------------------------------------------------------------------------------
		/// \brief If the timer is active
		//----------------------------------------------------------------------------------
		bool m_goTimer;
		//----------------------------------------------------------------------------------
		/// \brief Count of the number of 'next' events we have issued
		//----------------------------------------------------------------------------------
		unsigned int m_eventCount;
		//----------------------------------------------------------------------------------
		/// \brief If we need to issue a selection event
		//----------------------------------------------------------------------------------
		bool m_issueSelection;
		//----------------------------------------------------------------------------------
		/// \brief If we should stop scanning and highlight the current item
		//----------------------------------------------------------------------------------
		bool m_issueHighlight;
		//----------------------------------------------------------------------------------
		/// \brief If we need to issue a first focus event as our first selection
		//----------------------------------------------------------------------------------
		bool m_needFirstFocus;
		//----------------------------------------------------------------------------------

	};
}

#endif
