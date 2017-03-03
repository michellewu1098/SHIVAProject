#include "System/Window.h"

//----------------------------------------------------------------------------------

ShivaGUI::Window::Window( int _posX, int _posY, int _width, int _height )
{
	SDL_Init( SDL_INIT_EVERYTHING );

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY );
	//SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	m_window = SDL_CreateWindow(	"SHIVA-GUI",
									_posX, _posY,
									_width, _height,
									SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL );

	if( m_window == NULL )
		std::cerr << "ERROR: Window ctor, cannot create SDL Window: " << SDL_GetError() << std::endl;


	m_GLContext = SDL_GL_CreateContext( m_window );

	if( m_GLContext == NULL )
		std::cerr << "ERROR: Window ctor, cannot create OpenGL Context: " << SDL_GetError() << std::endl;

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit(); 
	if( glewError != GLEW_OK ) { 
		printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) ); 
	}

	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
	glViewport( 0, 0, _width, _height );

	//----------------------------------------------------------------------------------
	
	// Print some useful info

	SDL_DisplayMode mode;
	SDL_GetCurrentDisplayMode( 0, &mode );
	std::cout << "INFO: Window BPP: " << SDL_BITSPERPIXEL( mode.format )
			  << "\n      Vendor    : " << glGetString( GL_VENDOR )
			  << "\n      Renderer  : " << glGetString( GL_RENDERER )
		      << "\n      Version   : " << glGetString( GL_VERSION )
		      << "\n      GLSL Version: " << glGetString( GL_SHADING_LANGUAGE_VERSION )
		      << std::endl;

	//----------------------------------------------------------------------------------

	m_width = _width;
    m_height = _height;
    m_fullscreen = false;

	m_requestedUse = ANYTHING;
}

//----------------------------------------------------------------------------------

ShivaGUI::Window::~Window()
{
	SDL_DestroyWindow( m_window );
}

//----------------------------------------------------------------------------------

void ShivaGUI::Window::SetRequestedUse( std::string _use )
{
	// The boost iequals is a case-insensitive string comparison
	if( boost::algorithm::iequals( _use, "ANYTHING" ) )
	{
		m_requestedUse = ANYTHING;
	}
	else if( boost::algorithm::iequals( _use, "INPUT" ) )
	{
		m_requestedUse = INPUT;
	}
	else if( boost::algorithm::iequals( _use, "OUTPUT" ) )
	{
		m_requestedUse = OUTPUT;
	}
	else if( boost::algorithm::iequals( _use, "INPUT_OUTPUT" ) )
	{
		m_requestedUse = INPUT_OUTPUT;
	}
	else
	{
		std::cerr << "WARNING: Window::SetRequestedUse() to unknown use: " << _use << std::endl;
	}
}

//----------------------------------------------------------------------------------

unsigned int ShivaGUI::Window::GetSDLWindowID()
{
	return SDL_GetWindowID( m_window );
}

//----------------------------------------------------------------------------------

void ShivaGUI::Window::MakeCurrent()
{
	SDL_GL_MakeCurrent( m_window, m_GLContext );
}

//----------------------------------------------------------------------------------

void ShivaGUI::Window::SwapBuffers()
{
	SDL_GL_SwapWindow( m_window );
}

//----------------------------------------------------------------------------------

void ShivaGUI::Window::GetSize( unsigned int &_width, unsigned int &_height )
{
	int w, h;
	SDL_GetWindowSize( m_window, &w, &h );
	_width = w;
	_height = h;
}

//----------------------------------------------------------------------------------

void ShivaGUI::Window::SetSize( const unsigned int &_width, const unsigned int &_height )
{
	if( !m_fullscreen )
		SDL_SetWindowSize( m_window, _width, _height );
}

//----------------------------------------------------------------------------------

void ShivaGUI::Window::SetFullScreen( bool _fullscreen )
{
	if( SDL_SetWindowFullscreen( m_window, _fullscreen ? SDL_TRUE : SDL_FALSE ) == 0 )
	{
		m_fullscreen = _fullscreen;
		SDL_GetWindowSize( m_window, &m_width, &m_height );
	}
	else
		m_fullscreen = 0;
}

//----------------------------------------------------------------------------------

void ShivaGUI::Window::Maximise()
{
	SDL_MaximizeWindow( m_window );
}

//----------------------------------------------------------------------------------

void ShivaGUI::Window::Minimise()
{
	SDL_MinimizeWindow( m_window );
}

//----------------------------------------------------------------------------------

void ShivaGUI::Window::Restore()
{
	SDL_RestoreWindow( m_window );
}

//----------------------------------------------------------------------------------


