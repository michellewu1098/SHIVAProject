
#include "System/Window.h"

//////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <SDL.h>
#include <GL/GLee.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <boost/algorithm/string.hpp>

ShivaGUI::Window::Window(int posX, int posY, int width, int height)
{
	_window = SDL_CreateWindow("SHIVA-GUI",
	                        posX, posY,
	                        width, height,
	                        SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);

	if( _window == NULL )
		std::cerr<<"ERROR: Window ctor, cannot create SDL Window: "<< SDL_GetError() << std::endl;


    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);

	_GLContext = SDL_GL_CreateContext(_window);

	if( _GLContext == NULL )
		std::cerr<<"ERROR: Window ctor, cannot create OpenGL Context: "<< SDL_GetError() <<std::endl;

	SDL_DisplayMode mode;
	SDL_GetCurrentDisplayMode(0, &mode);
	std::cout<<"INFO: Window BPP: "<< SDL_BITSPERPIXEL(mode.format)
		   <<"\n      Vendor    : "<< glGetString(GL_VENDOR)
		   <<"\n      Renderer  : "<< glGetString(GL_RENDERER)
		   <<"\n      Version   : "<< glGetString(GL_VERSION)
		   //<<"\n      Extensions: "<< glGetString(GL_EXTENSIONS)
		   << std::endl;

    //SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    //SDL_RenderClear(_renderer);
    //SDL_RenderPresent(_renderer);

    _width = width;
    _height = height;
    _fullscreen = false;

	//_deltaTs = 0.0f;

	_requestedUse = ANYTHING;

	//_lastTime = SDL_GetTicks();
}


ShivaGUI::Window::~Window()
{
	SDL_DestroyWindow(_window);
}

void ShivaGUI::Window::SetRequestedUse(std::string use)
{
	// The boost iequals is a case-insensitive string comparison
	if( boost::algorithm::iequals(use, "ANYTHING") )
	{
		_requestedUse = ANYTHING;
	}
	else if( boost::algorithm::iequals(use, "INPUT") )
	{
		_requestedUse = INPUT;
	}
	else if( boost::algorithm::iequals(use, "OUTPUT") )
	{
		_requestedUse = OUTPUT;
	}
	else if( boost::algorithm::iequals(use, "INPUT_OUTPUT") )
	{
		_requestedUse = INPUT_OUTPUT;
	}
	else
	{
		std::cerr<<"WARNING: Window::SetRequestedUse() to unknown use: "<<use<<std::endl;
	}
}

unsigned int ShivaGUI::Window::GetSDLWindowID()
{
	return SDL_GetWindowID(_window);
}

void ShivaGUI::Window::MakeCurrent()
{
	SDL_GL_MakeCurrent(_window,_GLContext);
}

void ShivaGUI::Window::SwapBuffers()
{
	/*
	GLenum err = glGetError();
	if( err != GL_NO_ERROR)
	{
		std::cerr<<" ShivaGUI::Window, OpenGL Error in SwapBuffers: "<< gluErrorString(err)<<std::endl;
	}
	*/

    //SDL_RenderPresent(_renderer);
	SDL_GL_SwapWindow(_window);
	glClearColor(0.0f,1.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	// Limiter in case we're running really quick
	if( _deltaTs < (1.0f/50.0f) )	// not sure how accurate the SDL_Delay function is..
		SDL_Delay((unsigned int) (((1.0f/50.0f) - _deltaTs)*1000.0f) );

	unsigned int current = SDL_GetTicks();
	_deltaTs = (float) (current - _lastTime) / 1000.0f;
	_lastTime = current;

	if( _deltaTs > 1.0f )
		_deltaTs = 1.0f;
	else if( _deltaTs < 0.00001f )
		_deltaTs = 0.00001f;
	*/
}

void ShivaGUI::Window::GetSize(unsigned int &width, unsigned int &height)
{
//	width = _width;
//	height = _height;
	int w, h;
	SDL_GetWindowSize(_window,&w,&h);
	width = w;
	height = h;
}

void ShivaGUI::Window::SetSize(unsigned int width, unsigned int height)
{
	if( !_fullscreen )
		SDL_SetWindowSize(_window,width,height);
}

void ShivaGUI::Window::SetFullScreen(bool fullscreen)
{
	if( SDL_SetWindowFullscreen(_window,fullscreen ? SDL_TRUE : SDL_FALSE) == 0 )
	{
		_fullscreen = fullscreen;
		SDL_GetWindowSize(_window,&_width,&_height);
	}
	else
		_fullscreen = 0;
}

void ShivaGUI::Window::Maximise()
{
	SDL_MaximizeWindow(_window);
}

void ShivaGUI::Window::Minimise()
{
	SDL_MinimizeWindow(_window);
}

void ShivaGUI::Window::Restore()
{
	SDL_RestoreWindow(_window);
}



