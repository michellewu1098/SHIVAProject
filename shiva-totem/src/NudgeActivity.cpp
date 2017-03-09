#include "NudgeActivity.h"
#include "ShivaModelManager.h"

//----------------------------------------------------------------------------------

void NudgeActivity::OnCreate( ShivaGUI::Bundle *_data )
{
	// This is like our constructor
	// We use it to initialise our variables and load the layouts to window

	m_totemController = Totem::Controller::GetInstance();

	m_objectColourR = 0.88f;
	m_objectColourG = 0.78f;
	m_objectColourB = 0.54f;
	m_setObjectColour = false;

	m_rotationStepsize = 7.5f;
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;

	m_nudgeStep = 0.1f;
	
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
		m_nudgeStep = prefs->GetFloat( "NudgeStep", m_nudgeStep );
	}

	m_buttonHandler = new UtilityEventHandler(this);


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

			InitOutputWindow( guiController, _data);
		}
		else
		{
			// We just assume that this is a generic input/output window

			InitIOWindow( guiController, _data);
		}
	}
}

//----------------------------------------------------------------------------------

void NudgeActivity::OnResume()
{
}

//----------------------------------------------------------------------------------

void NudgeActivity::OnDestroy()
{
	// This is like our destructor, so delete our data etc
	delete m_buttonHandler;
}

//----------------------------------------------------------------------------------

void NudgeActivity::OnUpdate( float _deltaTs )
{
}

//----------------------------------------------------------------------------------

void NudgeActivity::UtilityEventReceived( UtilityEventHandler *_handler, ShivaGUI::View *_view )
{
	// This function is called when an event is received
	// the handler we are given should be one of those we registered
	// The view we are given should be the View that generated the event (e.g. the button)
	// Here we'll change the text that is displayed based on which button is pressed
	if( _handler == m_buttonHandler )
	{
		if( _view->GetID() == "RotateLeft" )
		{
			std::cout << "INFO: NudgeActivity request to rotate left" << std::endl;
			m_rotationZ -= m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateRight" )
		{
			std::cout << "INFO: NudgeActivity request to rotate right" << std::endl;
			m_rotationZ += m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateUp" )
		{
			std::cout << "INFO: NudgeActivity request to rotate up" << std::endl;
			m_rotationX -= m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateDown" )
		{
			std::cout << "INFO: NudgeActivity request to rotate down" << std::endl;
			m_rotationX += m_rotationStepsize;
		}
		else if( _view->GetID() == "DeleteSelected" )
		{
			std::cout << "INFO: NudgeActivity request to delete selected object" << std::endl;
			m_totemController->DeleteSelectedObject();
			RebuildTrees();
		}
		else if( _view->GetID() == "SelectAbove" )
		{
			std::cout << "INFO: NudgeActivity request to select object above" << std::endl;
			m_totemController->SelectObjectAbove();
		}
		else if( _view->GetID() == "SelectBelow" )
		{
			std::cout << "INFO: NudgeActivity request to select object below" << std::endl;
			m_totemController->SelectObjectBelow();
		}
		else if( _view->GetID() == "SwapAbove" )
		{
			std::cout << "INFO: NudgeActivity request to swap with object above" << std::endl;
			m_totemController->ReorderSelectedObject( true );
			RebuildTrees();
		}
		else if( _view->GetID() == "SwapBelow" )
		{
			std::cout << "INFO: NudgeActivity request to swap with object below" << std::endl;
			m_totemController->ReorderSelectedObject( false );
			RebuildTrees();
		}
		else if( _view->GetID() == "NudgeUp" )
		{
			std::cout << "INFO: NudgeActivity request to nudge object upward" << std::endl;
			m_totemController->MoveSelectedObject( 0.0f, 0.0f, 0.1f );
			RebuildTrees( true );
		}
		else if( _view->GetID() == "NudgeDown" )
		{
			std::cout << "INFO: NudgeActivity request to nudge object downward" << std::endl;
			m_totemController->MoveSelectedObject( 0.0f, 0.0f, -0.1f );
			RebuildTrees( true );
		}
		else if( _view->GetID() == "NudgeXpositive" )
		{
			std::cout << "INFO: NudgeActivity request to nudge object on positive X" << std::endl;
			m_totemController->MoveSelectedObject( m_nudgeStep, 0.0f, 0.0f );
			RebuildTrees( true );
		}
		else if( _view->GetID() == "NudgeXnegative" )
		{
			std::cout << "INFO: NudgeActivity request to nudge object on negative X" << std::endl;
			m_totemController->MoveSelectedObject( -m_nudgeStep, 0.0f, 0.0f );
			RebuildTrees( true );
		}
		else if( _view->GetID() == "NudgeYpositive" )
		{
			std::cout << "INFO: AssembleActivity request to nudge object on positive Y" << std::endl;
			m_totemController->MoveSelectedObject( 0.0f, m_nudgeStep, 0.0f );
			RebuildTrees( true );
		}
		else if( _view->GetID() == "NudgeYnegative" )
		{
			std::cout << "INFO: AssembleActivity request to nudge object on negative Y" << std::endl;
			m_totemController->MoveSelectedObject( 0.0f, -m_nudgeStep, 0.0f );
			RebuildTrees( true );
		}
		else if( _view->GetID() == "NudgeReset" )
		{
			std::cout << "INFO: AssembleActivity request to reset nudge" << std::endl;
			m_totemController->ResetMoveSelected();
			RebuildTrees( true );
		}
		else if( _view->GetID() == "RotateReset" )
		{
			ResetRotation();
		}
		else if( _view->GetID() == "BackButton" )
		{
			m_totemController->ShowSelection( true );
			Finish();
		}
	}
	// Update our views
	UpdateViews();
}

//----------------------------------------------------------------------------------

void NudgeActivity::OnActivityResult( ShivaGUI::Bundle *_data )
{
	RebuildTrees();
}

//----------------------------------------------------------------------------------

void NudgeActivity::UpdateViews()
{
	for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
	{
		(*it).first->AddWorldRotationOffsetDegs( m_rotationX, m_rotationY, m_rotationZ );
	}
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;
}

//----------------------------------------------------------------------------------

void NudgeActivity::ResetRotation()
{
	for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
	{
		(*it).first->ResetWorldRotation();
	}
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;
}

//----------------------------------------------------------------------------------

void NudgeActivity::RebuildTrees( bool _justparams )
{
	for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
	{
		( *it ).second->MakeCurrent();
		if( _justparams )
		{
			( *it ).first->RefreshTree();
			//(*it).first->RefreshTreeParams();
		}
		else
		{
			(*it).first->RefreshTree();
		}
	}
}

//----------------------------------------------------------------------------------

void NudgeActivity::InitIOWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
{
	// Here we are going to initialise an I/O window, which will have a couple of buttons

	// Register our UtilityEventHandlers with the GUIController.
	_guiController->RegisterListener( m_buttonHandler, "buttonHandler" );

	// The layout xml is where the widget hierarchy is specified
	// This function will load the file and expand the hierarchy ready for display on screen
	_guiController->LoadContentView( "NudgeIO.xml" );

	// Retrieve primitive list
	VolView *volView = dynamic_cast< VolView* >( _guiController->GetResources()->GetViewFromID( "MainVolView" ) );
	if( volView != NULL )
	{
		// For the assemble activity we want to allow the user to click to select primitives
		volView->AllowObjectClickSelection( true );
		m_volViews.push_back( std::pair< VolView*, ShivaGUI::GUIController* >( volView, _guiController ) );
		if( m_setObjectColour )
		{
			volView->SetObjectColour( m_objectColourR, m_objectColourG, m_objectColourB );
		}
	}

	UpdateViews();
}

//----------------------------------------------------------------------------------

void NudgeActivity::InitOutputWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
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