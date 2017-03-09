#include "RotateObjectActivity.h"

//----------------------------------------------------------------------------------

void RotateObjectActivity::OnCreate( ShivaGUI::Bundle *_data )
{
	// This is like our constructor
	// We use it to initialise our variables and load the layouts to window

	m_totemController = Totem::Controller::GetInstance();
	m_totemController->ShowSelection( true );
	
	m_buttonHandler = new UtilityEventHandler( this );

	m_rotationStepsize = ( 3.14159265358979323846f / 24.0f ); // TODO: put in setting
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;
	
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
		
		m_rotationStepsize = prefs->GetFloat( "RotateObjectStepRadians", m_rotationStepsize );
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
}

//----------------------------------------------------------------------------------

void RotateObjectActivity::OnDestroy()
{
	// This is like our destructor, so delete our data etc
	delete m_buttonHandler;
}

//----------------------------------------------------------------------------------

void RotateObjectActivity::UtilityEventReceived( UtilityEventHandler *_handler, ShivaGUI::View *_view )
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
		else if( _view->GetID() == "RotateYUp" )
		{
			m_rotationY -= m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateYDown" )
		{
			m_rotationY += m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateZUp" )
		{
			m_rotationZ -= m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateZDown" )
		{
			m_rotationZ += m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateReset" )
		{
			ResetRotation();
		}
		else if( _view->GetID() == "SelectAbove" )
		{
			m_totemController->SelectObjectAbove();
		}
		else if( _view->GetID() == "SelectBelow" )
		{
			m_totemController->SelectObjectBelow();
		}
		else if( _view->GetID() == "BackButton" )
		{
			m_totemController->ShowSelection( true );
			Finish();
		}

		// Update our views
		UpdateViews();
		RebuildTrees( true );
	}
}

//----------------------------------------------------------------------------------

void RotateObjectActivity::OnActivityResult( ShivaGUI::Bundle *_data )
{
	RebuildTrees();
}

//----------------------------------------------------------------------------------

void RotateObjectActivity::UpdateViews()
{
/*	for( std::vector< std::pair<VolView*,ShivaGUI::GUIController*> >::iterator it = _VolViews.begin(); it != _VolViews.end(); ++it )
	{
		(*it).first->AddWorldRotationOffsetDegs(_rotationX,_rotationY,_rotationZ);
	}
*/
	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();
		if( currentObject != NULL )
		{
			std::cout << "INFO: adding object rotation: " << m_rotationX << " " << m_rotationY << " " << m_rotationZ << std::endl;
			currentObject->AddRotation( m_rotationX, m_rotationY, m_rotationZ );
		}
	}

	m_rotationX = m_rotationY = m_rotationZ = 0.0f;
}

//----------------------------------------------------------------------------------

void RotateObjectActivity::ResetRotation()
{
	for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
	{
		( *it ).first->ResetWorldRotation();
	}

	m_rotationX = m_rotationY = m_rotationZ = 0.0f;
}

//----------------------------------------------------------------------------------

void RotateObjectActivity::RebuildTrees( bool _justparams )
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

void RotateObjectActivity::InitIOWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
{
	// Here we are going to initialise an I/O window, which will have a couple of buttons

	// Register our UtilityEventHandlers with the GUIController.
	_guiController->RegisterListener( m_buttonHandler, "buttonHandler" );

	// The layout xml is where the widget hierarchy is specified
	// This function will load the file and expand the hierarchy ready for display on screen
	_guiController->LoadContentView( "RotateObjectIO.xml" );

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

void RotateObjectActivity::InitOutputWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
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