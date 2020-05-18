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
	//_CrtDumpMemoryLeaks();
}

//----------------------------------------------------------------------------------

void ShivaGUI::Activity::CreateBBoxVAOs()
{
	if( m_GUIControllers != NULL )
	{
		for( unsigned int i = 0; i < m_numGUIControllers; i++ )
		{
			if( m_GUIControllers[ i ] != NULL )
			{
				m_GUIControllers[ i ]->CreateBBoxVAOs( i );
			}
		}
	}
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
				m_GUIControllers[ i ]->Draw( i );
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
/*
	if( _currentEvent->GetType() == InternalEvent::QUIT )
		m_guiManager->SetExitEvent();
	else if( _currentEvent->GetType() == InternalEvent::BACK )
		OnBackPressed();
*/

//	if (_currentEvent->GetType() == InternalEvent::BACK)
//	{

	bool alreadyAsked = false;

	if( m_GUIControllers != NULL )
	{
		for( unsigned int i = 0; i < m_numGUIControllers; i++ )
		{
			GUIController *currentController = m_GUIControllers[ i ];
			if( currentController != NULL )
			{
				if ( _currentEvent->GetType() == InternalEvent::EDIT_CUSTOMLAYOUT )
				{
					// User has pressed the 'F5' key meaning that they want to edit the current screen 
					std::cout << "F5 key pressed - edit mode requested!: " << InternalEvent::EventType( _currentEvent->GetType()) << std::endl;
					currentController->IssueEvent( _currentEvent );
				}
				else
				{
					if ( _currentEvent->GetType() == InternalEvent::BACK)
					{
						// 'Esc' key has been pressed
						if ((currentController->GetContentViewFilename() == "AssembleIOWithSymbols.xml") ||
							currentController->GetContentViewFilename() == "VolViewOutput.xml")//"ProfileChooserLayout.xml"
						{

							// This is the main screen - exiting from this screen now closes the application
							// (Before, pressing 'Esc' returned to the profile selector)
							// This is a hack to stop previous undesirable behaviours that I couldn't find a way of resolving:
							// (1). Once an eye gaze profile is exited, the profile screen became eye gaze accessible!!
							// (2). The application would crash with an exception if an object was added to the pole and then the 'Esc' key was pressed and another profile loaded

							// WARNING: This seems a bit brutal and may leave some 'loose ends'!!!
							//			I don't know how else to do this - this code seems complex with events firing all over the place!!
							// I don't know what this will do for 2 window profiles
	
							// Check that user really wants to exit
							if (!alreadyAsked)
							{
								alreadyAsked = true;
								if (tinyfd_messageBox("Exit", "Are you sure you want to exit?\n\nAnything not saved will be lost.", "yesno", "question", 0) == 1)
								{
//	currentController->IssueEvent( _currentEvent );
///	currentController->IssueEvent( new InternalEvent( InternalEvent::QUIT ) );
//	if( _currentEvent->GetType() == InternalEvent::QUIT )
									m_guiManager->SetExitEvent();
								}
							}	
						}
						else
							// 'Esc' key pressed, but not on main screen
//					else if( _currentEvent->GetType() == InternalEvent::BACK )
							OnBackPressed();
					}
				else
					// Otherwise, just let the event through - something else will happen?
					currentController->IssueEvent( _currentEvent );
				}
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
