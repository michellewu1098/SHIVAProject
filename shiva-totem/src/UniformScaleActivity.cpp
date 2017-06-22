
#include "UniformScaleActivity.h"

//----------------------------------------------------------------------------------

void UniformScaleActivity::OnCreate( ShivaGUI::Bundle *_data )
{
	// This is like our constructor
	// We use it to initialise our variables and load the layouts to window

	m_totemController = Totem::Controller::GetInstance();
	m_totemController->ShowSelection( true );
	
	m_buttonHandler = new UtilityEventHandler( this );

	m_rotationStepsize = 7.5f;
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;

	m_scaleStepsize = 0.1f;
	
	m_objectColourR = 0.88f;
	m_objectColourG = 0.78f;
	m_objectColourB = 0.54f;
	m_setObjectColour = false;
	
	ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();
	if( prefs != NULL )
	{
		if( prefs->Contains( "ObjectColourR" ) && prefs->Contains( "ObjectColourG" ) && prefs->Contains( "ObjectColourB" ) )
		{
			m_objectColourR = prefs->GetFloat( "ObjectColourR", m_objectColourR );
			m_objectColourG = prefs->GetFloat( "ObjectColourG", m_objectColourG );
			m_objectColourB = prefs->GetFloat( "ObjectColourB", m_objectColourB );
			m_setObjectColour = true;
		}
		
		m_rotationStepsize = prefs->GetFloat( "RotationStepsize", m_rotationStepsize );

		m_scaleStepsize = prefs->GetFloat( "ScaleStepsize", m_scaleStepsize );
	}


	// We will now tell the system what to display on the windows

	// Find out how many windows we have
	int numWindows = GetNumGUIControllers();

	// For each of these...
	for( int i = 0; i < numWindows; i++ )
	{
		// We retrieve a GUIController
		// This is our main method of accessing the resources associated with a Window
		ShivaGUI::GUIController *guiController = GetGUIController( i );

		// The User Profile can specify how the windows are supposed to be used
		// We can retrieve this using the GUIController and load a different window layout
		ShivaGUI::Window::RequestedUse windowUse = guiController->GetRequestedUse();
		if( windowUse == ShivaGUI::Window::OUTPUT )
		{
			// This window should be used for output information only, with no buttons etc

			InitOutputWindow( guiController, _data );
		}
		else
		{
			// We just assume that this is a generic input/output window

			InitIOWindow( guiController, _data );
		}
	}

	m_commandManager = Totem::CommandManager::GetInstance();
}

//----------------------------------------------------------------------------------

void UniformScaleActivity::OnDestroy()
{
	// This is like our destructor, so delete our data etc
	delete m_buttonHandler;
}

//----------------------------------------------------------------------------------

void UniformScaleActivity::UtilityEventReceived( UtilityEventHandler *_handler, ShivaGUI::View *_view )
{
	// This function is called when an event is received
	// the handler we are given should be one of those we registered
	// The view we are given should be the View that generated the event (e.g. the button)
	// Here we'll change the text that is displayed based on which button is pressed
	if( _handler == m_buttonHandler )
	{
		if( _view->GetID() == "RotateLeft" )
		{
			m_rotationZ -= m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateRight" )
		{
			m_rotationZ += m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateUp" )
		{
			m_rotationX -= m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateDown" )
		{
			m_rotationX += m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateReset" )
		{
			ResetRotation();
		}
		else if( _view->GetID() == "SelectAbove" )
		{
			SelectCommand* selectCmd = new SelectCommand();
			selectCmd->SetSelection( "above" );
			m_commandManager->Execute( selectCmd );

			//m_totemController->SelectObjectAbove();
		}
		else if( _view->GetID() == "SelectBelow" )
		{
			SelectCommand* selectCmd = new SelectCommand();
			selectCmd->SetSelection( "below" );
			m_commandManager->Execute( selectCmd );

			//m_totemController->SelectObjectBelow();
		}
		else if( _view->GetID() == "ScaleUp" )
		{
			ScaleCommand* scaleCmd = new ScaleCommand();
			scaleCmd->SetScaleStepSize( m_scaleStepsize, m_scaleStepsize, m_scaleStepsize );
			scaleCmd->SetScaling( "up" );

			m_commandManager->Execute( scaleCmd );

			/*if( m_totemController != NULL )
			{
				Totem::Object *currentObject = m_totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleX += m_scaleStepsize;
					scaleY += m_scaleStepsize;
					scaleZ += m_scaleStepsize;
					if( scaleX > 2.0f )
						scaleX = 2.0f;
					if( scaleY > 2.0f )
						scaleY = 2.0f;
					if( scaleZ > 2.0f )
						scaleZ = 2.0f;
					std::cout << "INFO: scaling up object to: " << scaleX << " " << scaleY << " " << scaleZ << std::endl;
					currentObject->SetScale( scaleX, scaleY, scaleZ );
				}
			}*/
		}
		else if( _view->GetID() == "ScaleDown" )
		{
			ScaleCommand* scaleCmd = new ScaleCommand();
			scaleCmd->SetScaleStepSize( m_scaleStepsize, m_scaleStepsize, m_scaleStepsize );
			scaleCmd->SetScaling( "down" );

			m_commandManager->Execute( scaleCmd );

			/*if( m_totemController != NULL )
			{
				Totem::Object *currentObject = m_totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleX -= m_scaleStepsize;
					scaleY -= m_scaleStepsize;
					scaleZ -= m_scaleStepsize;
					if( scaleX < 0.1f )
						scaleX = 0.1f;
					if( scaleY < 0.1f )
						scaleY = 0.1f;
					if( scaleZ < 0.1f )
						scaleZ = 0.1f;
					std::cout << "INFO: scaling down object to: " << scaleX << " " << scaleY << " " << scaleZ << std::endl;
					currentObject->SetScale( scaleX, scaleY, scaleZ );
				}
			}*/
		}
		else if( _view->GetID() == "ScaleXUp" )
		{
			ScaleCommand* scaleCmd = new ScaleCommand();
			scaleCmd->SetScaleStepSize( m_scaleStepsize, 0.f, 0.f );
			scaleCmd->SetScaling( "up" );

			m_commandManager->Execute( scaleCmd );
				
			/*if( m_totemController != NULL )
			{
				Totem::Object *currentObject = m_totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleX += m_scaleStepsize;
					if( scaleX > 2.0f )
						scaleX = 2.0f;
					std::cout << "INFO: scaling up object in X to: " << scaleX << std::endl;
					currentObject->SetScale( scaleX, scaleY, scaleZ );
				}
			}*/
		}
		else if( _view->GetID() == "ScaleXDown" )
		{
			ScaleCommand* scaleCmd = new ScaleCommand();
			scaleCmd->SetScaleStepSize( m_scaleStepsize, 0.f, 0.f );
			scaleCmd->SetScaling( "down" );

			m_commandManager->Execute( scaleCmd );

			/*if( m_totemController != NULL )
			{
				Totem::Object *currentObject = m_totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleX -= m_scaleStepsize;
					if( scaleX < 0.1f )
						scaleX = 0.1f;
					std::cout << "INFO: scaling down object in X to: " << scaleX << std::endl;
					currentObject->SetScale( scaleX, scaleY, scaleZ );
				}
			}*/
		}
		else if( _view->GetID() == "ScaleYUp" )
		{
			ScaleCommand* scaleCmd = new ScaleCommand();
			scaleCmd->SetScaleStepSize( 0.f, m_scaleStepsize, 0.f );
			scaleCmd->SetScaling( "up" );

			m_commandManager->Execute( scaleCmd );

			/*if( m_totemController != NULL )
			{
				Totem::Object *currentObject = m_totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleY += m_scaleStepsize;
					if( scaleY > 2.0f )
						scaleY = 2.0f;
					std::cout << "INFO: scaling up object in Y to: " << scaleY << std::endl;
					currentObject->SetScale( scaleX, scaleY, scaleZ );
				}
			}*/
		}
		else if( _view->GetID() == "ScaleYDown" )
		{
			ScaleCommand* scaleCmd = new ScaleCommand();
			scaleCmd->SetScaleStepSize( 0.f, m_scaleStepsize, 0.f );
			scaleCmd->SetScaling( "down" );

			m_commandManager->Execute( scaleCmd );

			/*if( m_totemController != NULL )
			{
				Totem::Object *currentObject = m_totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleY -= m_scaleStepsize;
					if( scaleY < 0.1f )
						scaleY = 0.1f;
					std::cout << "INFO: scaling down object in Y to: " << scaleY << std::endl;
					currentObject->SetScale( scaleX, scaleY, scaleZ );
				}
			}*/
		}
		else if( _view->GetID() == "ScaleZUp" )
		{
			ScaleCommand* scaleCmd = new ScaleCommand();
			scaleCmd->SetScaleStepSize( 0.f, 0.f, m_scaleStepsize );
			scaleCmd->SetScaling( "up" );

			m_commandManager->Execute( scaleCmd );

			/*if( m_totemController != NULL )
			{
				Totem::Object *currentObject = m_totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleZ += m_scaleStepsize;
					if( scaleZ > 2.0f )
						scaleZ = 2.0f;
					std::cout << "INFO: scaling up object in Z to: " << scaleZ << std::endl;
					currentObject->SetScale( scaleX, scaleY, scaleZ );
				}
			}*/
		}
		else if( _view->GetID() == "ScaleZDown" )
		{
			ScaleCommand* scaleCmd = new ScaleCommand();
			scaleCmd->SetScaleStepSize( 0.f, 0.f, m_scaleStepsize );
			scaleCmd->SetScaling( "down" );

			m_commandManager->Execute( scaleCmd );

			/*if( m_totemController != NULL )
			{
				Totem::Object *currentObject = m_totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleZ -= m_scaleStepsize;
					if( scaleZ < 0.1f )
						scaleZ = 0.1f;
					std::cout << "INFO: scaling down object in Z to: " << scaleZ << std::endl;
					currentObject->SetScale( scaleX, scaleY, scaleZ );
				}
			}*/
		}
		else if( _view->GetID() == "ScaleReset" )
		{
			ResetScaleCommand* resetScaleCmd = new ResetScaleCommand();
			
			m_commandManager->Execute( resetScaleCmd );

			/*if( m_totemController != NULL )
			{
				Totem::Object *currentObject = m_totemController->GetSelected();
				if( currentObject != NULL )
				{
					std::cout << "INFO: scaling reset object to: 1" << std::endl;
					currentObject->SetScale( 1.0f, 1.0f, 1.0f );
				}
			}*/
		}
		else if( _view->GetID() == "BackButton" )
		{
			m_totemController->ShowSelection( true );
			Finish();
		}
		else if( _view->GetID() == "UndoButton" )
		{
			m_commandManager->Undo();
			
		}
		else if( _view->GetID() == "RedoButton" )
		{
			m_commandManager->Redo();
		}

		// Update our views
		UpdateViews();
		RebuildTrees( true );
	}
}

//----------------------------------------------------------------------------------

void UniformScaleActivity::OnActivityResult( ShivaGUI::Bundle *_data )
{
	RebuildTrees();
}

//----------------------------------------------------------------------------------

void UniformScaleActivity::UpdateViews()
{
	for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
	{
		( *it ).first->AddWorldRotationOffsetDegs( m_rotationX, m_rotationY, m_rotationZ );
	}

	m_rotationX = m_rotationY = m_rotationZ = 0.0f;
}

//----------------------------------------------------------------------------------

void UniformScaleActivity::ResetRotation()
{
	for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
	{
		( *it ).first->ResetWorldRotation();
	}

	m_rotationX = m_rotationY = m_rotationZ = 0.0f;
}

//----------------------------------------------------------------------------------

void UniformScaleActivity::RebuildTrees( bool _justparams )
{
	for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
	{
		( *it ).second->MakeCurrent();
		if( _justparams )
		{
			//(*it).first->RefreshTreeParams();
			( *it ).first->RefreshTree();
		}
		else
		{
			( *it ).first->RefreshTree();
		}
	}
}

//----------------------------------------------------------------------------------

void UniformScaleActivity::InitIOWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
{
	// Here we are going to initialise an I/O window, which will have a couple of buttons

	// Register our UtilityEventHandlers with the GUIController.
	_guiController->RegisterListener( m_buttonHandler, "buttonHandler" );

	// The layout xml is where the widget hierarchy is specified
	// This function will load the file and expand the hierarchy ready for display on screen
	_guiController->LoadContentView( "UniformScaleIO.xml" );

	VolView *volView = dynamic_cast< VolView* >( _guiController->GetResources()->GetViewFromID( "MainVolView" ) );
	if( volView != NULL )
	{
		m_volViews.push_back( std::pair< VolView*, ShivaGUI::GUIController* >( volView, _guiController ) );
		volView->AllowObjectClickSelection( true );
		if( m_setObjectColour )
		{
			volView->SetObjectColour( m_objectColourR, m_objectColourG, m_objectColourB );
		}
	}

	UpdateViews();
}

//----------------------------------------------------------------------------------

void UniformScaleActivity::InitOutputWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
{
	// This window will be used for output only, so we don't need to register the listeners as there are no buttons

	// Just load the layout xml
	// Note that this is a different xml file to the IO window, so it will show different things (i.e. no buttons)
	_guiController->LoadContentView( "VolViewOutput.xml" );


	VolView *volView = dynamic_cast< VolView* >( _guiController->GetResources()->GetViewFromID( "MainVolView" ) );
	if( volView != NULL )
	{
		m_volViews.push_back( std::pair< VolView*, ShivaGUI::GUIController* >( volView, _guiController ) );
		if( m_setObjectColour )
		{
			volView->SetObjectColour( m_objectColourR, m_objectColourG, m_objectColourB );
		}
	}

	UpdateViews();
}

//----------------------------------------------------------------------------------