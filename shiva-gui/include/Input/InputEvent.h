
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUI_INPUTSYSTEM_INPUTEVENT__
#define __SHIVA_GUI_INPUTSYSTEM_INPUTEVENT__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	/// These are the raw events that come in from the Window and are sent to the Input Controller for processing
	class InputEvent
	{
	public:

		enum EventType
		{
			INVALID,
			QUIT,
			KEYDOWN,
			KEYUP,
			MOUSEMOTION,
			MOUSEBUTTONDOWN,
			MOUSEBUTTONUP,
			WINDOWRESIZE,
			WINDOWEXPOSE,
			WINDOWCLOSE
		};

		InputEvent();

		void SetQuitEvent();
		void SetKeyEvent(bool keyUp, int key, int windowID);
		void SetMouseMotionEvent(int mouseX, int mouseY, int windowID);
		void SetMouseButtonEvent(bool buttonUp, int button, float mouseX, float mouseY, int windowID);
		void SetWindowSizeEvent(int w, int h, int windowID);
		void SetWindowCloseEvent(int windowID);



		EventType GetType() {return _type;}

		int GetKeycode() {return _key;}

		/// Returns the Window index or -1 if this event is not associated with a Window
		int GetWindowID() {return _winID;}

		/// These are only guaranteed to make sense if the event is mouse-related
		/// Positions are in the same space (coordinate system) that the GUI system uses
		void GetMousePosition(int &mouseX, int &mouseY) {mouseX=_mouseX;mouseY=_mouseY;}

		int GetMouseButton() {return _mouseButton;}

		void GetWindowSize(int &w, int &h) {w=_winW;h=_winH;}

	protected:
		EventType _type;
		int _mouseX, _mouseY;
		int _winW, _winH;
		int _key;
		int _mouseButton;
		int _winID;

	};

}

//////////////////////////////////////////////////////////////////////////
#endif
