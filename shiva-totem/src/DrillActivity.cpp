#include "DrillActivity.h"
#include "System/SharedPreferences.h"
#include "GUIManager.h"

//----------------------------------------------------------------------------------

void DrillActivity::OnCreate( ShivaGUI::Bundle *_data )
{
	// This is like our constructor
	// We use it to initialise our variables and load the layouts to window

	m_totemController = Totem::Controller::GetInstance();
	m_totemController->ShowSelection( false );

	m_selectionVolView = NULL;
	
	m_buttonHandler = new UtilityEventHandler( this );

	m_rotationStepsize = 7.5f;
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;

	m_drillSize = m_originalDrillSize = 0.1f;
	m_drillSizeStep = 0.01f;

	m_crosshairNudgeAmount = 0.1f;
	
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

		m_crosshairNudgeAmount = prefs->GetFloat( "CrosshairNudge", m_crosshairNudgeAmount );

		m_rotationStepsize = prefs->GetFloat( "RotationStepsize", m_rotationStepsize );

		m_drillSize = m_originalDrillSize = prefs->GetFloat( "DrillSize", m_drillSize );

		m_drillSizeStep = prefs->GetFloat( "DrillSizeStep", m_drillSizeStep );
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

void DrillActivity::OnDestroy()
{
	// This is like our destructor, so delete our data etc
	delete m_buttonHandler;
}

//----------------------------------------------------------------------------------

void DrillActivity::OnUpdate( float _deltaTs )
{

}

//----------------------------------------------------------------------------------

void DrillActivity::UtilityEventReceived( UtilityEventHandler *_handler, ShivaGUI::View *_view )
{
	// This function is called when an event is received
	// the handler we are given should be one of those we registered
	// The view we are given should be the View that generated the event (e.g. the button)
	// Here we'll change the text that is displayed based on which button is pressed
	if( _handler == m_buttonHandler )
	{
		if( _view->GetID() == "RotateLeft" )
		{
			std::cout << "INFO: DrillActivity request to rotate left" << std::endl;
			m_rotationZ -= m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateRight" )
		{
			std::cout << "INFO: DrillActivity request to rotate right" << std::endl;
			m_rotationZ += m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateUp" )
		{
			std::cout << "INFO: DrillActivity request to rotate up" << std::endl;
			m_rotationX -= m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateDown" )
		{
			std::cout << "INFO: DrillActivity request to rotate down" << std::endl;
			m_rotationX += m_rotationStepsize;
		}
		else if( _view->GetID() == "CrosshairLeft" )
		{
			for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
			{
				( *it ).first->NudgeCrosshairs( -m_crosshairNudgeAmount, 0.0f );
			}
		}
		else if( _view->GetID() == "CrosshairRight" )
		{
			for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
			{
				( *it ).first->NudgeCrosshairs( m_crosshairNudgeAmount, 0.0f );
			}
		}
		else if( _view->GetID() == "CrosshairUp" )
		{
			for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
			{
				( *it ).first->NudgeCrosshairs( 0.0f, m_crosshairNudgeAmount );
			}
		}
		else if( _view->GetID() == "CrosshairDown" )
		{
			for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
			{
				( *it ).first->NudgeCrosshairs( 0.0f, -m_crosshairNudgeAmount );
			}
		}
		else if( _view->GetID() == "DrillSizeIncrease" )
		{
			m_drillSize += m_drillSizeStep;
			if( m_drillSize > 1.0f )
				m_drillSize = 1.0f;
		}
		else if( _view->GetID() == "DrillSizeDecrease" )
		{
			m_drillSize -= m_drillSizeStep;
			if( m_drillSize < 0.05f )
				m_drillSize = 0.05f;
		}
		else if( _view->GetID() == "DrillSizeReset" )
		{
			m_drillSize = m_originalDrillSize;
		}
		else if( _view->GetID() == "PerformDrill" )
		{
			std::cout << "INFO: DrillActivity request to perform drilling operation" << std::endl;
			
			float selectVecOrigX, selectVecOrigY, selectVecOrigZ, selectVecDirX, selectVecDirY, selectVecDirZ;
			if( m_selectionVolView != NULL )
			{
				m_selectionVolView->GetSelectionVector( selectVecOrigX, selectVecOrigY, selectVecOrigZ, selectVecDirX, selectVecDirY, selectVecDirZ );
				
				cml::vector3f origin( selectVecOrigX, selectVecOrigY, selectVecOrigZ );
				cml::vector3f direction( selectVecDirX, selectVecDirY, selectVecDirZ );

				cml::vector3f point2 = origin + direction;

				float t = - ( cml::dot( origin, ( point2 - origin ) ) ) / ( pow( cml::length( point2 - origin ), 2 ) );

				std::cout << "INFO: DrillActivity, calculating drill position: t = " << t << std::endl;

				cml::vector3f drillCentre = origin + ( direction * t );

				Totem::Operations::Drill *drill = new Totem::Operations::Drill( m_drillSize );
				drill->SetDrill( drillCentre[ 0 ], drillCentre[ 1 ], drillCentre[ 2 ], selectVecDirX, selectVecDirY, selectVecDirZ, 100.0f );
				
				DrillCommand* drillCmd = new DrillCommand();
				drillCmd->SetOperation( drill );

				m_commandManager->Execute( drillCmd );
				//m_totemController->AddOperation( drill );

				RebuildTrees();
			}
		}
		else if( _view->GetID() == "UndoButton" )
		{
			std::cout << "INFO: DrillActivity request to undo operation" << std::endl;
			m_commandManager->Undo();
			//m_totemController->RemoveLastOperation();
			RebuildTrees();
		}
		else if( _view->GetID() == "RedoButton" )
		{
			std::cout << "INFO: DrillActivity request to redo operation" << std::endl;
			m_commandManager->Redo();
			RebuildTrees();
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

void DrillActivity::UpdateViews()
{
	for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
	{
		( *it ).first->AddWorldRotationOffsetDegs( m_rotationX, m_rotationY, m_rotationZ );
		( *it ).first->SetCrosshairSize( m_drillSize );
	}
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;

}

//----------------------------------------------------------------------------------

void DrillActivity::ResetRotation()
{
	for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
	{
		( *it ).first->ResetWorldRotation();
	}
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;
}

//----------------------------------------------------------------------------------

void DrillActivity::RebuildTrees()
{
	for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
	{
		( *it ).second->MakeCurrent();
		( *it ).first->RefreshTree();
	}
}

//----------------------------------------------------------------------------------

void DrillActivity::InitIOWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
{
	// Here we are going to initialise an I/O window, which will have a couple of buttons

	// Register our UtilityEventHandlers with the GUIController.
	_guiController->RegisterListener( m_buttonHandler, "buttonHandler" );

	// The layout xml is where the widget hierarchy is specified
	// This function will load the file and expand the hierarchy ready for display on screen
	_guiController->LoadContentView( "DrillIO.xml" );

	VolView *volView = dynamic_cast< VolView* >( _guiController->GetResources()->GetViewFromID( "MainVolView" ) );
	if( volView != NULL )
	{
		m_volViews.push_back( std::pair< VolView*, ShivaGUI::GUIController* >( volView, _guiController ) );

		// Drill activity needs less perspective, so change to very narrow camera angle
		volView->SetCameraAngle( 1.0f );

		if( m_selectionVolView == NULL )
		{
			m_selectionVolView = volView;
		}
		volView->ShowCrosshairs( true );
		volView->SetCrosshairSize( m_drillSize );
		if( m_setObjectColour )
		{
			volView->SetObjectColour( m_objectColourR, m_objectColourG, m_objectColourB );
		}
	}

	UpdateViews();
}

//----------------------------------------------------------------------------------

void DrillActivity::InitOutputWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
{
	// This window will be used for output only, so we don't need to register the listeners as there are no buttons

	// Just load the layout xml
	// Note that this is a different xml file to the IO window, so it will show different things (i.e. no buttons)
	_guiController->LoadContentView( "VolViewOutput.xml" );


	VolView *volView = dynamic_cast< VolView* >( _guiController->GetResources()->GetViewFromID( "MainVolView" ) );
	if( volView != NULL )
	{
		m_volViews.push_back( std::pair< VolView*, ShivaGUI::GUIController* >( volView, _guiController ) );

		// Drill activity needs less perspective, so change to very narrow camera angle
		volView->SetCameraAngle( 1.0f );

		if( m_setObjectColour )
		{
			volView->SetObjectColour( m_objectColourR, m_objectColourG, m_objectColourB );
		}
	}

	UpdateViews();
}

//----------------------------------------------------------------------------------
