#include "GUI/GUIController.h"
#include "GUIManager.h"

//----------------------------------------------------------------------------------

ShivaGUI::GUIController::GUIController( Window *_inputWindow, ResourceManager *_resources, GUIManager *_guiman )
{
	m_attachedWindow = _inputWindow;
	m_resourceManager = _resources;
	m_GUIManager = _guiman;
	m_contentView = NULL;
}

//----------------------------------------------------------------------------------

ShivaGUI::GUIController::~GUIController()
{
	delete m_contentView;
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIController::RegisterViewID( ShivaGUI::View *_view, std::string _id )
{
	if( m_viewIDs.find( _id ) != m_viewIDs.end() )
		std::cout << "WARNING: GUIController registering multiple Views of ID: " << _id << std::endl;
	m_viewIDs[ _id ] = _view;
}

//----------------------------------------------------------------------------------

ShivaGUI::View* ShivaGUI::GUIController::GetViewFromID( std::string _id )
{
	std::map< std::string, View* >::iterator it = m_viewIDs.find( _id );
	if( it != m_viewIDs.end() )
		return ( *it ).second;
	return NULL;
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIController::ChangeWindowSize( unsigned int _width, unsigned int _height )
{
	if( m_attachedWindow != NULL )
		m_attachedWindow->SetSize( _width, _height );
	Layout();
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIController::GetWindowSize( unsigned int &_width, unsigned int &_height )
{
	if( m_attachedWindow != NULL )
		m_attachedWindow->GetSize( _width, _height );
}

//----------------------------------------------------------------------------------

bool ShivaGUI::GUIController::GetFullScreen()
{
	if( m_attachedWindow != NULL )
		return m_attachedWindow->GetFullScreen();
	return false;
}

//----------------------------------------------------------------------------------

ShivaGUI::Window::RequestedUse ShivaGUI::GUIController::GetRequestedUse()
{
	if( m_attachedWindow != NULL )
		return m_attachedWindow->GetRequestedUse();
	return Window::ANYTHING;
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIController::RegisterListener( ShivaGUI::ViewEventListener *_listener, std::string _name )
{
	m_eventListeners[ _name ] = _listener;
}

//----------------------------------------------------------------------------------

ShivaGUI::ViewEventListener* ShivaGUI::GUIController::GetListener( std::string _name )
{
	return m_eventListeners[ _name ];
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIController::Layout()
{
	if( m_attachedWindow != NULL && m_contentView != NULL )
	{
		m_attachedWindow->MakeCurrent();
		unsigned int resX, resY;
		m_attachedWindow->GetSize( resX, resY );
		m_contentView->Layout( 0, 0, resX, resY, resX, resY );
	}
}

//----------------------------------------------------------------------------------

bool ShivaGUI::GUIController::IssueEvent( InternalEvent *_currentEvent )
{
	if( _currentEvent->GetType() == InternalEvent::WINDOW_RESIZE )
		Layout();
	else if( _currentEvent->GetType() == InternalEvent::QUIT )
		m_GUIManager->SetExitEvent(); // This event should never be here, as it's not associated with a Window, but just in case
//	else if( currentEvent->GetType() == InternalEvent::BACK )
//		_GUIManager->GetCurrentActivity()->IssueEvent(currentEvent);

	if( m_contentView != NULL ) {
		return m_contentView->HandleEvent( _currentEvent );
	}
	return false;
}
//----------------------------------------------------------------------------------

void ShivaGUI::GUIController::CreateBBoxVAOs( unsigned int _context )
{
	if( m_contentView != NULL )
		m_contentView->CreateBBoxVAOs( _context );
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIController::Update( float _deltaTs )
{
	// Update content View
	if( m_contentView != NULL )
		m_contentView->Update( _deltaTs, this );
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIController::Draw( unsigned int _context )
{
	if( m_attachedWindow == NULL || m_contentView == NULL )
		return;

	m_attachedWindow->MakeCurrent();
	unsigned int resX, resY;
	m_attachedWindow->GetSize( resX, resY );

	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Set up OpenGL
	// Remember that OpenGL uses lower-left corner as origin
	// Viewport function wants lower-left corner too
	glViewport( 0, 0, resX, resY );

	m_contentView->Draw( _context );

	m_attachedWindow->SwapBuffers();
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIController::Draw()
{
	if( m_attachedWindow == NULL || m_contentView == NULL )
		return;

	m_attachedWindow->MakeCurrent();
	unsigned int resX, resY;
	m_attachedWindow->GetSize( resX, resY );

	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Set up OpenGL
	// Remember that OpenGL uses lower-left corner as origin
	// Viewport function wants lower-left corner too
	glViewport( 0, 0, resX, resY );

	m_contentView->Draw();

	m_attachedWindow->SwapBuffers();
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIController::LoadContentView( std::string _layoutName )
{
	m_contentViewFilename = _layoutName;
	m_attachedWindow->MakeCurrent();
	// TODO: Set here size of font in relation to the size of the icon from prefs.xml or possibly write a separate function that will be called before this LoadContent()
	m_contentView = GetResources()->GetLayout( _layoutName );
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIController::SaveLayoutToProfile()
{
	m_GUIManager->SaveLayoutToProfile( m_contentViewFilename, m_contentView, m_resourceManager );
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIController::SetLayoutPrefs( SharedPreferences* _prefs )
{	
	//GetResources()->SetLayoutPrefs( _prefs );
}

//----------------------------------------------------------------------------------