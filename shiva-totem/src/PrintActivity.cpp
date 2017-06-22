#include "PrintActivity.h"
#include "System/SharedPreferences.h"
#include "GUIManager.h"

//----------------------------------------------------------------------------------

void PrintActivity::OnCreate( ShivaGUI::Bundle *_data )
{
	// This is like our constructor
	// We use it to initialise our variables and load layouts to window

	m_totemController = Totem::Controller::GetInstance();
	m_totemController->ShowSelection( false );

	m_buttonHandler = new UtilityEventHandler( this );

	m_rotationStepsize = 7.5f;
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;

	m_objectColourR = 0.88f;
	m_objectColourG = 0.78f;
	m_objectColourB = 0.54f;

	m_setObjectColour = false;
	m_saveRescaleSize = 20.0f;

	m_saveDir = "Savefiles/";
	m_saveName = "Totem";

	ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();
	if( prefs != NULL )
	{
		m_saveDir = prefs->GetString( "SaveDirectory", m_saveDir );
		m_saveName = prefs->GetString( "SaveFilename", m_saveName );

		m_rotationStepsize = prefs->GetFloat( "RotationStepsize", m_rotationStepsize );
		m_saveRescaleSize = prefs->GetFloat( "SaveRescaleSize", m_saveRescaleSize );

		if( prefs->Contains( "ObjectColourR" ) && prefs->Contains( "ObjectColourG" ) && prefs->Contains( "ObjectColourB" ) )
		{
			m_objectColourR = prefs->GetFloat( "ObjectColourR", m_objectColourR );
			m_objectColourG = prefs->GetFloat( "ObjectColourG", m_objectColourG );
			m_objectColourB = prefs->GetFloat( "ObjectColourB", m_objectColourB );
			m_setObjectColour = true;
		}
	}

	// We will now tell the system what to display on the windows
	// First, find out how many windows we have
	int numWindows = GetNumGUIControllers();

	// For each of these
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
			// This window should be used for output info only, with no other widgets
			InitOutputWindow( guiController, _data );
		}
		else
		{
			InitIOWindow( guiController, _data );
		}
	}
}

//----------------------------------------------------------------------------------

void PrintActivity::OnDestroy()
{
	delete m_buttonHandler;
}

//----------------------------------------------------------------------------------

void PrintActivity::UtilityEventReceived( UtilityEventHandler *_handler, ShivaGUI::View *_view )
{
	if( _handler == m_buttonHandler )
	{
		if( _view->GetID() == "Print" )
		{
			std::cout << "INFO: PrintActivity request to 3D print model. Wooho!" << std::endl;
			VolumeTree::Tree tmpTree;
			tmpTree.SetRoot( m_totemController->GetNodeTree() );
			//tmpTree.GetTotemPole();
			//tmpTree.TestDeletePole();

			RebuildTrees();
		}
		else if( _view->GetID() == "Export" )
		{
			std::cout << "INFO: PrintActivity request to export model to .vol" << std::endl;
			VolumeTree::Tree tempTree;
			
			VolumeTree::TransformNode *rootScaleNode = new VolumeTree::TransformNode( m_totemController->GetNodeTree() );
			rootScaleNode->SetScale( m_saveRescaleSize );
			tempTree.SetRoot( rootScaleNode );

			bool fileFound = false;
			std::string fullFilename;
			std::string extension = ".vol";
			unsigned int i = 1;
			do
			{
				std::stringstream fileNum;
				fileNum<<i;
				fullFilename = m_saveDir + m_saveName + fileNum.str() + extension;
				fileFound = !boost::filesystem::exists( fullFilename );
				i++;
			}
			while( ( i < 10000 ) && !fileFound );

			if( fileFound )
			{
				if( !boost::filesystem::exists( m_saveDir ) )
				{
					boost::filesystem::create_directory( m_saveDir );
				}

				tempTree.Save( fullFilename );
			}
			else
			{
				std::cerr << "WARNING: Cannot export file. Try removing previous files, limit is 10000 files" << std::endl;
			}

			delete rootScaleNode;
		}
		else if( _view->GetID() == "DeletePole" )
		{

		}
		else if( _view->GetID() == "DeleteBase" )
		{

		}
	}

	// Update our views
	UpdateViews();
}

//----------------------------------------------------------------------------------

void PrintActivity::OnActivityResult( ShivaGUI::Bundle *_data )
{
	RebuildTrees();
}

//----------------------------------------------------------------------------------

void PrintActivity::RebuildTrees()
{
	std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it;
	for( it = m_volViews.begin(); it != m_volViews.end(); ++it )
	{
		( *it ).second->MakeCurrent();
		( *it ).first->RefreshTree();
	}
}

//----------------------------------------------------------------------------------

void PrintActivity::UpdateViews()
{
	std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it;
	for( it = m_volViews.begin(); it != m_volViews.end(); ++it )
	{
		( *it ).first->AddWorldRotationOffsetDegs( m_rotationX, m_rotationY, m_rotationZ );
	}

	m_rotationX = m_rotationY = m_rotationZ = 0.0f; 
}

//----------------------------------------------------------------------------------

void PrintActivity::ResetRotation()
{
	std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it;
	for( it = m_volViews.begin(); it != m_volViews.end(); ++it )
	{
		( *it ).first->ResetWorldRotation();
	}
	
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;
}

//----------------------------------------------------------------------------------

void PrintActivity::InitIOWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
{
	_guiController->RegisterListener( m_buttonHandler, "buttonHandler" );
	_guiController->LoadContentView( "PrintIO.xml" );

	VolView *volView = dynamic_cast< VolView* >( _guiController->GetResources()->GetViewFromID( "MainVolView" ) );
	
	if( volView != NULL )
	{
		m_volViews.push_back( std::make_pair( volView, _guiController ) );
		if( m_setObjectColour )
		{
			volView->SetObjectColour( m_objectColourR, m_objectColourG, m_objectColourB );
		}
	}

	UpdateViews();
}

//----------------------------------------------------------------------------------

void PrintActivity::InitOutputWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
{
	_guiController->LoadContentView( "VolViewOutput.xml" );

	VolView *volView = dynamic_cast< VolView* >( _guiController->GetResources()->GetViewFromID( "MainVolView" ) );

	if( volView != NULL )
	{
		m_volViews.push_back( std::make_pair( volView, _guiController ) );
		if( m_setObjectColour )
		{
			volView->SetObjectColour( m_objectColourR, m_objectColourG, m_objectColourB );
		}
	}

	UpdateViews();
}

//----------------------------------------------------------------------------------
