///-----------------------------------------------------------------------------------------------
/// \file Scanner.h
/// \brief Base scanner class
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_INPUTSYSTEM_SCANNER__
#define __SHIVA_GUI_INPUTSYSTEM_SCANNER__

#include <boost/algorithm/string.hpp>
#include <iostream>

#include "Input/InternalEvent.h"

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

		//----------------------------------------------------------------------------------
		/// \brief Returns true if an event needs to be issued
		/// \param [in] _outputEvent
		/// \param [in] _deltaTs
		//----------------------------------------------------------------------------------
		virtual bool Update( InternalEvent::EventType &_outputEvent, float _deltaTs ) = 0;
		//----------------------------------------------------------------------------------
		/// \brief Set input 
		/// \param [in] _inputEvent
		//----------------------------------------------------------------------------------
		virtual void SetInput( InternalEvent::EventType _inputEvent ) = 0;
		//----------------------------------------------------------------------------------
		
	protected:

		//----------------------------------------------------------------------------------
		/// \brief Debug output
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void DebugOutput( std::string _value );
		//----------------------------------------------------------------------------------

	};
}

#endif
