
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUI_INPUTSYSTEM_SCANNERS_AUTOSCANNER__
#define __SHIVA_GUI_INPUTSYSTEM_SCANNERS_AUTOSCANNER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#include "Input/Scanners/Scanner.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	/// Scan modes:
	/// * Autoscan - automatic progression, one button chooses item with SCAN_SELECT
	/// * Userscan - manual progression, one button issues SCAN_START on press and SCAN_SELECT on release
	/// * Stepscan - buttons to progress with SCAN_NEXT and optionally SCAN_PREV, one to select with SCAN_SELECT
	/// * beyond Stepscan, should use focus rather than scanner
	/// TODO:
	/// * Overscan - fast auto progression, one press slows and reverses, second press selects
	class Autoscanner : public Scanner
	{
	public:
		Autoscanner();

		/// Returns true if an event needs to be issued
		virtual bool Update( InternalEvent::EventType &outputEvent, float deltaTs );

		virtual void SetInput( InternalEvent::EventType inputEvent );

		void SetDelayTime( float timeS ) {_repeatRateTime = timeS;}


	protected:

		/// Settings

			/// Time between issuing progression events
			float _repeatRateTime;

		/// Variables

			/// Scanning start time
			float _scanTimer;

			/// If the timer is active
			bool _goTimer;

			/// Count of the number of 'next' events we have issued
			unsigned int _eventCount;

			/// If we need to issue a selection event
			bool _issueSelection;

			/// If we should stop scanning and highlight the current item
			bool _issueHighlight;

			/// If we need to issue a first focus event as our first selection
			bool _needFirstFocus;

	};

}

//////////////////////////////////////////////////////////////////////////
#endif
