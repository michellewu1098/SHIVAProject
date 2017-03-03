///-----------------------------------------------------------------------------------------------
/// \file Application.h
/// \brief Applications are generally about holding states that are common to multiple Activities, which together form a single program
/// Application should hold the Resource Manager. Should also hold methods for creation of Activities and other things moved over from the GUIManager?
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_GUI_APPLICATION__
#define __SHIVA_GUI_GUI_APPLICATION__

namespace ShivaGUI
{
	// Users write their own ones of this

	class Application
	{
		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		Application();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~Application();
		//----------------------------------------------------------------------------------

	};
}

#endif



