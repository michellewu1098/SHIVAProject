
#ifndef __SHIVA_GUI_INPUTSYSTEM_INTERNALEVENT__
#define __SHIVA_GUI_INPUTSYSTEM_INTERNALEVENT__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <string>
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	/// These are the processed events that are sent to the GUI System by the Input Controller
	class InternalEvent
	{
	public:

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

		/// Parses a string to convert to an EventType enum
		static EventType ConvertEventType(std::string);

		InternalEvent(EventType);


		EventType GetType() {return _type;}

		void SetWindowSize(int w, int h) {_winWidth = w; _winHeight=h;}
		void GetWindowSize(int &w, int &h) {w=_winWidth;h=_winHeight;}

		void SetPosition(int x, int y) {_posX=x; _posY=y;}
		void GetPosition(int &x, int &y) {x = _posX; y = _posY;}

	protected:
		EventType _type;

		int _winWidth, _winHeight;

		int _posX, _posY;
	};

}

//////////////////////////////////////////////////////////////////////////
#endif
