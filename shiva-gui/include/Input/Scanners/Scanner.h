
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUI_INPUTSYSTEM_SCANNER__
#define __SHIVA_GUI_INPUTSYSTEM_SCANNER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#include "Input/InternalEvent.h"

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
	class Scanner
	{
	public:

		/// Returns true if an event needs to be issued
		virtual bool Update( InternalEvent::EventType &outputEvent, float deltaTs ) = 0;

		virtual void SetInput( InternalEvent::EventType inputEvent ) = 0;



	protected:

		/// Settings

		/// Variables

			void DebugOutput(std::string);

	};

}

//////////////////////////////////////////////////////////////////////////
#endif
