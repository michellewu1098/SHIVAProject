///-----------------------------------------------------------------------------------------------
/// \file Window.h
/// \brief Creates an SLD/OpenGL window context
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_SYSTEM_WINDOW__
#define __SHIVA_GUI_SYSTEM_WINDOW__

#include <SDL.h>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "Input/InputEvent.h"

namespace ShivaGUI
{
	class Window
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Used to indicate the requested use of a window according to the user profile
		/// The idea here is that some users will have multiple windows, but will want to use them for different things
		//----------------------------------------------------------------------------------
		typedef enum
		{
			ANYTHING,
			OUTPUT,
			INPUT,
			INPUT_OUTPUT
		} RequestedUse;
		//----------------------------------------------------------------------------------
		/// \brief Ctor of window
		/// \param [in] _posX
		/// \param [in] _posY
		/// \param [in] _screenResX
		/// \param [in] _screenResY
		//----------------------------------------------------------------------------------
		Window( int _posX, int _posY, int _screenResX, int _screenResY );
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~Window();
		//----------------------------------------------------------------------------------
		/// \brief Used to indicate the requested use of a window according to the user profile
		/// The idea here is that some users will have multiple windows, but will want to use them for different things
		/// \param [in] _use Requested use
		//----------------------------------------------------------------------------------
		void SetRequestedUse( std::string _use );
		//----------------------------------------------------------------------------------
		/// \brief Set the requested use of the window
		/// \param [in] _use Requested use
		//----------------------------------------------------------------------------------
		void SetRequestedUse( RequestedUse _use ) { m_requestedUse = _use; }
		//----------------------------------------------------------------------------------
		/// \brief Returns the requested use of the window
		/// \return m_requestedUse
		//----------------------------------------------------------------------------------
		RequestedUse GetRequestedUse() { return m_requestedUse; }
		//----------------------------------------------------------------------------------
		/// \brief Returns the unique SDL internal ID for this Window
		/// \note This is for use when figuring out which Window an event is associated with
		//----------------------------------------------------------------------------------
		unsigned int GetSDLWindowID();
		//----------------------------------------------------------------------------------
		/// \brief Activates the drawing context so that OpenGL calls are drawn to this window
		//----------------------------------------------------------------------------------
		void MakeCurrent();
		//----------------------------------------------------------------------------------
		/// \brief Swap opengl window
		//----------------------------------------------------------------------------------
		void SwapBuffers();
		//----------------------------------------------------------------------------------
		/// \brief Set window size (width and height)
		/// \param [in] _width
		/// \param [in] _height
		//----------------------------------------------------------------------------------
		void SetSize( const unsigned int &_width, const unsigned int &_height );
		//----------------------------------------------------------------------------------
		/// \brief Get window size (width and height)
		/// \param [out] _width
		/// \param [out] _height
		//----------------------------------------------------------------------------------
		void GetSize( unsigned int &_width, unsigned int &_height );
		//----------------------------------------------------------------------------------
		/// \brief Get window width
		/// \return m_width
		//----------------------------------------------------------------------------------
		int GetWidth() const { return m_width; }
		//----------------------------------------------------------------------------------
		/// \brief Get window height
		/// \return m_height
		//----------------------------------------------------------------------------------
		int GetHeight() const { return m_height; }
		//----------------------------------------------------------------------------------
		/// \brief Set window fullscreen
		/// \param [in] _fullscreen
		//----------------------------------------------------------------------------------
		void SetFullScreen( bool _fullscreen );
		//----------------------------------------------------------------------------------
		/// \brief Returns whether window is set to fullscreen or not
		/// \return m_fullscreen
		//----------------------------------------------------------------------------------
		bool GetFullScreen() const { return m_fullscreen; }
		//----------------------------------------------------------------------------------
		/// \brief Maximise window
		//----------------------------------------------------------------------------------
		void Maximise();
		//----------------------------------------------------------------------------------
		/// \brief Minimise window
		//----------------------------------------------------------------------------------
		void Minimise();
		//----------------------------------------------------------------------------------
		/// \brief Restore window
		//----------------------------------------------------------------------------------
		void Restore();
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Window width
		//----------------------------------------------------------------------------------
		int m_width;
		//----------------------------------------------------------------------------------
		/// \brief Window height
		//----------------------------------------------------------------------------------
		int m_height;
		//----------------------------------------------------------------------------------
		/// \brief Flag for fullscreen window
		//----------------------------------------------------------------------------------
		bool m_fullscreen;
		//----------------------------------------------------------------------------------
		/// \brief Our SDL window
		//----------------------------------------------------------------------------------
        SDL_Window* m_window;
        //----------------------------------------------------------------------------------
		/// \brief OpenGL context
		//----------------------------------------------------------------------------------
        SDL_GLContext m_GLContext;
		//----------------------------------------------------------------------------------
		/// \brief Used to indicate the requested use of a window according to the user profile
		//----------------------------------------------------------------------------------
        RequestedUse m_requestedUse;
		//----------------------------------------------------------------------------------

	};
}

#endif
