///-----------------------------------------------------------------------------------------------
/// \file InternalEvent.h
/// \brief These are the processed events that are sent to the GUI System by the Input Controller
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_INPUTSYSTEM_INTERNALEVENT__
#define __SHIVA_GUI_INPUTSYSTEM_INTERNALEVENT__

#include <boost/algorithm/string.hpp>
#include <string>

namespace ShivaGUI
{
	class InternalEvent
	{
	public:

		//----------------------------------------------------------------------------------
		// \brief Event type
		//----------------------------------------------------------------------------------
		enum EventType
		{
			INVALID,
			QUIT,
			BACK,				// Normal behaviour is for the current Activity to exit
			WINDOW_RESIZE,

			FIRSTFOCUS,			// The GUI Layout should select the first focus widget
			NEXTFOCUS_UP,		// The GUI Layout should select the next focus widget
			NEXTFOCUS_DOWN,
			NEXTFOCUS_LEFT,
			NEXTFOCUS_RIGHT,
			CLEARFOCUS,			// The widget that currently holds focus must drop it
			FOCUS_HIGHLIGHT,	// Highlights the widget that currently holds focus, e.g. a button appears 'pressed'
			FOCUS_SELECT_RELEASE,// The widget that currently holds focus is being selected, e.g. a button actually performs the activation
			FOCUS_SELECT_HOLD,// The widget that currently holds focus is being selected, e.g. a button actually performs the activation
			FOCUS_DESELECT,		// The widget that currently holds focus is being deselected, is no longer activated or highlighted

			POSITIONAL_SELECT,	// A positional selection event, e.g. from a mouse
			POSITIONAL_DESELECT,// A positional deselection event, e.g. from a mouse
			POSITIONAL_DRAG,	// A positional select-and-move, e.g. mouse drag

			// Events to be mapped by keys to control the Scanner:
			SCAN_HIGHLIGHT,		// Scanning focus event - stops scanning and focusses the selected item
			SCAN_SELECT,		// Scanning selection event - this is the minimal required event for scanning
			SCAN_START,			// Starts scanning, is stopped by SCAN_SELECT or SCAN_STOP
			SCAN_STOP,			// Tells the scanner to stop on the currently focussed item but does not select it

			// Events to be normally issued by the scanner to control Views:
			SCAN_FIRSTFOCUS,	// The GUI Layout should select the first scan-focus widget
			SCAN_HIGHLIGHTFOCUS,// Selects (presses) the currently focussed View
			SCAN_SELECTFOCUS,	// Deselects (releases) the currently focussed View
			SCAN_NEXTFOCUS,		// Increments scan focus by one - to be issued by a Scanner and handled by Views
			SCAN_PREVFOCUS,		// Decrements scan focus by one - to be issued by a Scanner and handled by Views

			EYEGAZE_ENABLE,		// Issued by and has no effect on buttons set with eyeGaze_RestButton
			EYEGAZE_DISABLE,	// Issued by and has no effect on buttons set with eyeGaze_RestButton

			EDIT_CUSTOMLAYOUT	// Activates the edit mode for any custom layouts
		};

		//----------------------------------------------------------------------------------
		/// \brief Parses a string to convert to an EventType enum
		/// \param [in] _inputString
		//----------------------------------------------------------------------------------
		static EventType ConvertEventType( std::string _inputString );
		//----------------------------------------------------------------------------------
		/// \brief Ctor 
		/// \param [in] _inputType
		//----------------------------------------------------------------------------------
		InternalEvent( EventType _inputType );
		//----------------------------------------------------------------------------------
		/// \brief Returns event type
		/// \return m_type
		//----------------------------------------------------------------------------------
		EventType GetType() { return m_type; }
		//----------------------------------------------------------------------------------
		/// \brief Set window size
		/// \param [in] _w Width
		/// \param [in] _h Height
		//----------------------------------------------------------------------------------
		void SetWindowSize( int _w, int _h ) { m_winWidth = _w; m_winHeight = _h; }
		//----------------------------------------------------------------------------------
		/// \brief Get window size
		/// \param [out] _w
		/// \param [out] _h
		//----------------------------------------------------------------------------------
		void GetWindowSize( int &_w, int &_h ) { _w = m_winWidth; _h = m_winHeight; }
		//----------------------------------------------------------------------------------
		/// \brief Set position
		/// \param [in] _x
		/// \param [in] _y
		//----------------------------------------------------------------------------------
		void SetPosition( int _x, int _y) { m_posX = _x; m_posY = _y; }
		//----------------------------------------------------------------------------------
		/// \brief Get position
		/// \param [out] _x
		/// \param [out] _y
		//----------------------------------------------------------------------------------
		void GetPosition( int &_x, int &_y) { _x = m_posX; _y = m_posY; }
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Event type
		//----------------------------------------------------------------------------------
		EventType m_type;
		//----------------------------------------------------------------------------------
		/// \brief Window width
		//----------------------------------------------------------------------------------
		int m_winWidth;
		//----------------------------------------------------------------------------------
		/// \brief Window height
		//----------------------------------------------------------------------------------
		int m_winHeight;
		//----------------------------------------------------------------------------------
		/// \brief Position x-coord
		//----------------------------------------------------------------------------------
		int m_posX;
		//----------------------------------------------------------------------------------
		/// \brief Position y-coord
		//----------------------------------------------------------------------------------
		int m_posY;
		//----------------------------------------------------------------------------------

	};
}

#endif
