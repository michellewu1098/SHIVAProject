#include "System/Activity.h"
#include "GUIManager.h"

//----------------------------------------------------------------------------------

ShivaGUI::Activity::Activity()
{
	m_guiManager = NULL;
	m_startedForResult = false;
	m_numGUIControllers = 0;
	m_GUIControllers = NULL;
}

//----------------------------------------------------------------------------------

ShivaGUI::Activity::~Activity()
{
	for( unsigned int i = 0; i < m_numGUIControllers ; i++ )
	{
		delete m_GUIControllers[ i ];
	}
	delete [] m_GUIControllers;
}

//----------------------------------------------------------------------------------

void ShivaGUI::Activity::Create( GUIManager *_guiManager, Bundle *_savedState, bool _startedForResult )
{
	m_guiManager = _guiManager;
	m_startedForResult = _startedForResult;
	OnCreate( _savedState );
}

//----------------------------------------------------------------------------------

void ShivaGUI::Activity::Destroy()
{
	OnDestroy();
}

//----------------------------------------------------------------------------------

void ShivaGUI::Activity::Update( float _deltaTs )
{
	if( m_GUIControllers != NULL )
	{
		for( unsigned int i = 0; i < m_numGUIControllers; i++ )
		{
			if( m_GUIControllers[ i ] != NULL )
			{
				m_GUIControllers[ i ]->Update( _deltaTs );
			}
		}
	}
	OnUpdate( _deltaTs );
}

//----------------------------------------------------------------------------------

void ShivaGUI::Activity::Draw()
{
	if( m_GUIControllers != NULL )
	{
		for( unsigned int i = 0; i < m_numGUIControllers; i++ )
		{
			if( m_GUIControllers[ i ] != NULL )
			{
				m_GUIControllers[ i ]->Draw();
			}
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::Activity::Layout()
{
	if( m_GUIControllers != NULL )
	{
		for( unsigned int i = 0; i < m_numGUIControllers; i++ )
		{
			if( m_GUIControllers[ i ] != NULL )
			{
				m_GUIControllers[ i ]->Layout();
			}
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::Activity::ReturnActivityResult( Bundle *_data )
{
	OnActivityResult( _data );
	// delete it here? - no, deleting it in GUIManager
}

//----------------------------------------------------------------------------------

void ShivaGUI::Activity::ConfigurationChanged()
{
	OnConfigurationChanged();
}

//----------------------------------------------------------------------------------

void ShivaGUI::Activity::Finish()
{
	m_guiManager->FinishActivity();
}

//----------------------------------------------------------------------------------

void ShivaGUI::Activity::SetResult( Bundle *_data )
{
	m_guiManager->SetActivityResult( _data );
}

//----------------------------------------------------------------------------------

void ShivaGUI::Activity::SetNumGUIControllers( unsigned int _numControllers )
{
	// Delete existing controllers
	for( unsigned int i = 0; i < m_numGUIControllers ; i++ )
	{
		delete m_GUIControllers[ i ];
	}
	m_numGUIControllers = _numControllers;
	m_GUIControllers = new GUIController *[ _numControllers ];
	for( unsigned int i = 0; i < _numControllers; i++ )
		m_GUIControllers[ i ] = NULL;
}

//----------------------------------------------------------------------------------

void ShivaGUI::Activity::AddGUIController( GUIController *_controller, unsigned int _index )
{
	m_GUIControllers[ _index ] = _controller;
}

//----------------------------------------------------------------------------------

unsigned int ShivaGUI::Activity::GetNumGUIControllers()
{
	return m_numGUIControllers;
}

//----------------------------------------------------------------------------------

ShivaGUI::GUIController* ShivaGUI::Activity::GetGUIController( unsigned int _index )
{
	if( _index >= m_numGUIControllers )
		return NULL;
	return m_GUIControllers[ _index ];
}

//----------------------------------------------------------------------------------

void ShivaGUI::Activity::IssueEvent( InternalEvent *_currentEvent )
{
	if( _currentEvent->GetType() == InternalEvent::QUIT )
		m_guiManager->SetExitEvent();
	else if( _currentEvent->GetType() == InternalEvent::BACK )
		OnBackPressed();

	if( m_GUIControllers != NULL )
	{
		for( unsigned int i = 0; i < m_numGUIControllers; i++ )
		{
			GUIController *currentController = m_GUIControllers[ i ];
			if( currentController != NULL )
			{
				currentController->IssueEvent( _currentEvent );
			}
		}
	}
}

//----------------------------------------------------------------------------------

ShivaGUI::Activity::UtilityEventHandler::UtilityEventHandler( Activity *_parent )
{
	m_parent = _parent;
}

//----------------------------------------------------------------------------------

void ShivaGUI::Activity::UtilityEventHandler::HandleEvent( View *_view )
{
	m_parent->UtilityEventReceived( this, _view );
}

//----------------------------------------------------------------------------------
