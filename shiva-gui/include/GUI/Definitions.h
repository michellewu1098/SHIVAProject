///-----------------------------------------------------------------------------------------------
/// \file Definitions.h
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_GUI_VIEWS_DEFINITIONS__
#define __SHIVA_GUI_GUI_VIEWS_DEFINITIONS__

namespace ShivaGUI
{
	namespace Definitions
	{
		//----------------------------------------------------------------------------------
		/// \brief Focus direction 
		//----------------------------------------------------------------------------------
		enum FocusDirection
		{
			INVALID = -1,
			Left = 0,
			Right = 1,
			Up = 2,
			Down = 3
		};
		//----------------------------------------------------------------------------------
		/// \brief Get the opposite focus direction of input direction
		/// \param [in] _dir
		//----------------------------------------------------------------------------------
		FocusDirection GetOppositeFocusDirection( FocusDirection _dir );
		//----------------------------------------------------------------------------------
		/// \brief Gravity
		//----------------------------------------------------------------------------------
		enum Gravity
		{
			CENTRE 				= 0x00000000,
			VERTICAL_MASK		= 0x0000FFFF,
			BOTTOM 				= 0x00000001,
			TOP 				= 0x00000002,
			CENTRE_VERTICAL 	= 0x00000003,

			HORIZONTAL_MASK		= 0xFFFF0000,
			CENTRE_HORIZONTAL 	= 0x00010000,
			LEFT 				= 0x00020000,
			RIGHT 				= 0x00030000
		};
		//----------------------------------------------------------------------------------
		
	}
}

#endif
