
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUI_SYSTEM_WINDOW__
#define __SHIVA_GUI_SYSTEM_WINDOW__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <SDL.h>
#include <string>

//////////////////////////////////////////////////////////////////////////

#include "Input/InputEvent.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	class Window
	{
	public:
		/// Used to indicate the requested use of a window according to the user profile
		/// The idea here is that some users will have multiple windows, but will want to use them for different things
		typedef enum
		{
			ANYTHING,
			OUTPUT,
			INPUT,
			INPUT_OUTPUT
		} RequestedUse;

		Window(int posX, int posY, int screenResX, int screenResY);
		~Window();

		/// Used to indicate the requested use of a window according to the user profile
		/// The idea here is that some users will have multiple windows, but will want to use them for different things
		void SetRequestedUse(std::string use);
		void SetRequestedUse(RequestedUse use) {_requestedUse = use;}
		RequestedUse GetRequestedUse() {return _requestedUse;}

		/// Returns the unique SDL internal ID for this Window
		/// \note This is for use when figuring out which Window an event is associated with
		unsigned int GetSDLWindowID();

		/// Activates the drawing context so that OpenGL calls are drawn to this window
		void MakeCurrent();

		void SwapBuffers();

		/// Returns the time between SwapBuffers() calls
		//float GetDeltaTs() {return _deltaTs;}

		void SetSize(unsigned int width, unsigned int height);
		void GetSize(unsigned int &width, unsigned int &height);
		int GetWidth() {return _width;}
		int GetHeight() {return _height;}

		void SetFullScreen(bool fullscreen);
		bool GetFullScreen() {return _fullscreen;}

		void Maximise();
		void Minimise();
		void Restore();

	protected:

		unsigned int _lastTime;
		float _deltaTs;

		int _width, _height;
		bool _fullscreen;

        SDL_Window* _window;
        SDL_Renderer* _renderer;
        SDL_GLContext _GLContext;

        RequestedUse _requestedUse;

	};

}

//////////////////////////////////////////////////////////////////////////
#endif
