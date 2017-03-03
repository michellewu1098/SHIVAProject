///-----------------------------------------------------------------------------------------------
/// \file ViewEventListener.h
/// \brief Listener interface for widget events
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_GUI_VIEWS_VIEWEVENTLISTENER__
#define __SHIVA_GUI_GUI_VIEWS_VIEWEVENTLISTENER__

#include "GUI/Views/View.h"

namespace ShivaGUI
{
	class ViewEventListener
	{
	public:

		virtual void HandleEvent( View* ) = 0;
	};
}

#endif
