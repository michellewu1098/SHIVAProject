#include "AssembleActivity.h"
#include "ShivaModelManager.h"

#include "System/SharedPreferences.h"
#include "GUIManager.h"

#include "boost/date_time/posix_time/posix_time.hpp"
#include "windows.h"

//----------------------------------------------------------------------------------

void GetScreenRes(int& hoz, int& vert)
{
   RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (horizontal, vertical)
   hoz = desktop.right;
   vert = desktop.bottom;
}


void AssembleActivity::OnCreate( ShivaGUI::Bundle *_data )
{
	// This is like our constructor
	// We use it to initialise our variables and load the layouts to window

	m_totemController = Totem::Controller::GetInstance();

	m_showSaveConfirmation = false;
	m_saveTextCounter = 0.0f;

	m_objectColourR = 0.88f;
	m_objectColourG = 0.78f;
	m_objectColourB = 0.54f;
	m_setObjectColour = false;

	m_rotationStepsize = 7.5f;
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;

	m_saveRescaleSize = 20.0f;
	
	m_saveDir = "Savefiles/";
	m_saveName = "Model";//"Totem";
	ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();
	m_originalBlendingAmount = 0.0f;
	if( prefs != NULL )
	{
		m_saveDir = prefs->GetString( "SaveDirectory", m_saveDir );
		m_saveName = prefs->GetString( "SaveFilename", m_saveName );

		m_originalBlendingAmount = prefs->GetFloat( "BlendingAmount", m_originalBlendingAmount );
		
		if( prefs->Contains( "ObjectColourR" ) && prefs->Contains( "ObjectColourG" ) && prefs->Contains( "ObjectColourB" ) )
		{
			m_objectColourR = prefs->GetFloat( "ObjectColourR", m_objectColourR );
			m_objectColourG = prefs->GetFloat( "ObjectColourG", m_objectColourG );
			m_objectColourB = prefs->GetFloat( "ObjectColourB", m_objectColourB );
			m_setObjectColour = true;
		}
		
		m_rotationStepsize = prefs->GetFloat( "RotationStepsize", m_rotationStepsize );

		m_saveRescaleSize = prefs->GetFloat( "SaveRescaleSize", m_saveRescaleSize );

	}

	m_totemController->SetBlend( m_originalBlendingAmount );

	// Create the button event handlers
	m_buttonHandler = new UtilityEventHandler( this );
	m_addPrimitiveHandler = new UtilityEventHandler( this );
	m_launchActivityHandler = new UtilityEventHandler( this );

	// Create and configure our list adapter
	m_modelListAdapter =  new ShivaGUI::DataAdapter();
	m_modelListAdapter->SetDataProvider( ShivaModelManager::GetInstance() );

	// create mapping:
	std::string fromAttrib( "thumbnail_file" ); // This must correspond to the entry in the Resources/Models/index.xml that we want to map
	std::string toView( "imagebutton" );		// This must be the View ID in the layout xml that we want to map the above data to

	m_modelListAdapter->SetMapping( &fromAttrib, &toView, 1 );
	m_modelListAdapter->SetLayoutFile( "ModelListEntry.xml" );	// This is the layout xml that specifies the single entry for the ListView
	m_modelListAdapter->SetIsModelAdapter( true );
			   
	int horizontal = 0;    
	int vertical = 0;

	// We will now tell the system what to display on the windows

	// Find out how many windows we have
	int numWindows = GetNumGUIControllers();

	// For each of these...
	for( int i = 0; i < numWindows; i++ )
	{
		// We retrieve a GUIController
		// This is our main method of accessing the resources associated with a Window
		ShivaGUI::GUIController *guiController = GetGUIController( i );
				
		// Maximise window to fill the screen
	    GetScreenRes(horizontal, vertical);
	    std::cout << "INFO: Screen resolution: " << horizontal << " x " << vertical << std::endl;

		guiController->ChangeWindowSize(horizontal, vertical); //GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN));//200, 200);

		// Pass to resourceManager preferences set for Layout
		guiController->SetLayoutPrefs( prefs );

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

void AssembleActivity::OnResume()
{
	m_totemController->ShowSelection( true );
	std::cout << "INFO: Assemble Activity OnResume()" << std::endl;
}

//----------------------------------------------------------------------------------

void AssembleActivity::OnDestroy()
{
	// This is like our destructor, so delete our data etc
	delete m_buttonHandler;
	delete m_addPrimitiveHandler;
	delete m_modelListAdapter;
	delete m_launchActivityHandler;
}

//----------------------------------------------------------------------------------

void AssembleActivity::OnUpdate( float _deltaTs )
{
	if( m_showSaveConfirmation )
	{
		m_saveTextCounter -= _deltaTs;
		if( m_saveTextCounter <= 0.0f )
		{
			m_showSaveConfirmation = false;
			UpdateViews();
		}
	}
}

//----------------------------------------------------------------------------------

void AssembleActivity::UtilityEventReceived( UtilityEventHandler *_handler, ShivaGUI::View *_view )
{
	// This function is called when an event is received
	// the handler we are given should be one of those we registered
	// The view we are given should be the View that generated the event (e.g. the button)
	// Here we'll change the text that is displayed based on which button is pressed

	if( _handler == m_buttonHandler )
	{
		if( _view->GetID() == "RotateLeft" )
		{
#ifdef _DEBUG
			std::cout << "INFO: AssembleActivity request to rotate left" << std::endl;
#endif
			m_rotationZ -= m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateRight" )
		{ 
#ifdef _DEBUG
			std::cout << "INFO: AssembleActivity request to rotate right" << std::endl;
#endif
			m_rotationZ += m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateUp" )
		{
#ifdef _DEBUG
			std::cout << "INFO: AssembleActivity request to rotate up" << std::endl;
#endif
			m_rotationX -= m_rotationStepsize;
		}
		else if( _view->GetID() == "RotateDown" )
		{
#ifdef _DEBUG
			std::cout << "INFO: AssembleActivity request to rotate down" << std::endl;
#endif
			m_rotationX += m_rotationStepsize;
		}
		else if( _view->GetID() == "DeleteSelected" )
		{
#ifdef _DEBUG
			std::cout << "INFO: AssembleActivity request to delete selected object" << std::endl;
#endif
			DeleteObjectCommand* deleteObjCmd = new DeleteObjectCommand();
			m_commandManager->Execute( deleteObjCmd );

			//m_totemController->DeleteSelectedObject();
			RebuildTrees();
		}
		else if( _view->GetID() == "SelectAbove" )
		{
#ifdef _DEBUG
			std::cout << "INFO: AssembleActivity request to select object above" << std::endl;
#endif
			SelectCommand* selectCmd = new SelectCommand();
			selectCmd->SetSelection( "above" );
			m_commandManager->Execute( selectCmd );

			//m_totemController->SelectObjectAbove();
		}
		else if( _view->GetID() == "SelectBelow" )
		{
#ifdef _DEBUG
			std::cout << "INFO: AssembleActivity request to select object below" << std::endl;
#endif
			SelectCommand* selectCmd = new SelectCommand();
			selectCmd->SetSelection( "below" );
			m_commandManager->Execute( selectCmd );

			//m_totemController->SelectObjectBelow();
		}
		else if( _view->GetID() == "SwapAbove" )
		{
#ifdef _DEBUG
			std::cout << "INFO: AssembleActivity request to swap with object above" << std::endl;
#endif
			//m_totemController->ReorderSelectedObject( true );
			
			SwapCommand* swapCmd = new SwapCommand();
			swapCmd->SetShiftOrder( true );
			m_commandManager->Execute( swapCmd );

			RebuildTrees();
		}
		else if( _view->GetID() == "SwapBelow" )
		{
#ifdef _DEBUG
			std::cout << "INFO: AssembleActivity request to swap with object below" << std::endl;
#endif
			//m_totemController->ReorderSelectedObject( false );

			SwapCommand* swapCmd = new SwapCommand();
			swapCmd->SetShiftOrder( false );
			m_commandManager->Execute( swapCmd );

			RebuildTrees();
		}
		//else if( _view->GetID() == "NudgeUp" )
		//{
		//	std::cout << "INFO: AssembleActivity request to nudge object upward" << std::endl;
		//	m_totemController->MoveSelectedObject( 0.0f, 0.0f, 0.1f );
		//	RebuildTrees( true );
		//}
		//else if( _view->GetID() == "NudgeDown" )
		//{
		//	std::cout << "INFO: AssembleActivity request to nudge object downward" << std::endl;
		//	m_totemController->MoveSelectedObject( 0.0f, 0.0f, -0.1f );
		//	RebuildTrees( true );
		//}
		else if( _view->GetID() == "RotateReset" )
		{
			ResetRotation();
		}
		else if( _view->GetID() == "DuplicateObj" )
		{
		// We use the button index to create that primitive:
		DuplicateObjectCommand* duplicateObjCmd = new DuplicateObjectCommand();
		// We don't know the type of selected yet, so pass 9 to indicate this
		// (range of primitives is 0-4: 0 Sphere, 1 Cone, 2 Cylinder, 3 Cube, 4 Cuboid)
		duplicateObjCmd->SetParameters( 9, GetNumGUIControllers() );
		m_commandManager->Execute( duplicateObjCmd );
		RebuildTrees();
//		duplicateObjCmd-> SetParameters( dataEntryIndex, GetNumGUIControllers() );

//		m_commandManager->Execute( duplicateObjCmd );
//			DuplicateObjectCommand();
		}
		else if( _view->GetID() == "ChangeColour" )
		{			
			// We will now tell the system what to display on the windows
			// First, find out how many windows we have
			int numWindows = GetNumGUIControllers();

			// For each of these
			for( int i = 0; i < numWindows; i++ )
				{
				// We retrieve a GUIController
				// This is our main method of accessing the resources associated with a Window

				ShivaGUI::GUIController *guiController = GetGUIController( i );

				ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();

				float r = 0.0f;
				float g = 0.0f;
				float b = 0.0f;

				// The User Profile can specify how the windows are supposed to be used
				// We can retrieve this using the GUIController and load a different window layout
				ShivaGUI::Window::RequestedUse windowUse = guiController->GetRequestedUse();

		//		if( windowUse == ShivaGUI::Window::OUTPUT )
					{
					// This window should be used for output info only, with no other widgets
					VolView *volView = dynamic_cast< VolView* >(guiController->GetResources()->GetViewFromID( "MainVolView" ) );
					static int ColCount = 1;
			
					switch (ColCount)
						{
							case 1: // Red
								{
									r = 1.0f;
									g = 0.0f;
									b = 0.0f;
								}
								break;

							case 2: // Green
								{
									r = 0.0f;
									g = 1.0f;
									b = 0.0f;
								}
								break;

							case 3: // Blue
								{
									r = 0.0f;
									g = 0.0f;
									b = 1.0f;
								}
								break;

							case 4: // Yellow
								{
									r = 1.0f;
									g = 1.0f;
									b = 0.0f;
//									ColCount = 0;
								}
								break;

							case 5: // White
								{
									r = 1.0f;
									g = 1.0f;
									b = 1.0f;
//									ColCount = 0;
								}
								break;

							case 6: // Black
								{
									r = 0.0f;
									g = 0.1f;
									b = 0.1f;
//									ColCount = 0;
								}
								break;

							case 7: // Original/Clay
								{
									r = 0.88f;
									g = 0.78f;
									b = 0.54f;
									ColCount = 0;
								}
								break;
						}

						ColCount++;

						// Store the colour settings
						prefs->SetTotColourR(r);
						prefs->SetTotColourG(g);
						prefs->SetTotColourB(b);

						// Set the model colour
						volView->SetObjectColour(r, g, b);
					}
			}
		}

		else if( _view->GetID() == "Edit" )
		{
			// If we say we expect a result we will be notified when it returns, so we can rebuild the trees
			GetGUIManager()->StartActivityForResult( "EditMenuActivity", NULL );
		}
		else if( _view->GetID() == "New" )
		{
			if( tinyfd_messageBox( "New", "Are you sure?", "yesno", "question", 0 ) )
			{	
				m_totemController->DeleteAll();
				m_totemController->SetBlend( m_originalBlendingAmount );
				ResetRotation();
				RebuildTrees( true );

				m_commandManager->Clear();
			}
		}
		else if( _view->GetID() == "Load" )
		{
			char const * lFilterPatterns[ 1 ] = { "*.xml" };
			char const* fileName = tinyfd_openFileDialog( "SHIVA Models", "Savefiles/", 1, lFilterPatterns, NULL, 0 );
			
			/*if ( !fileName )
			{
				tinyfd_messageBox( "Error", "No file selected. Couldn't load anything.", "ok", "error", 1);
			}*/
			if( fileName )
			{
				m_totemController->DeleteAll();
				m_totemController->SetBlend( m_originalBlendingAmount );
				ResetRotation();
			
				VolumeTree::Tree tmpTree;

				if( tmpTree.Load( fileName ) )
				{ 
					m_totemController->LoadModel( tmpTree.GetReverseTree(), GetNumGUIControllers() );
				    m_totemController->SelectTopObject();
				}
			
				RebuildTrees( true );
			}
		}
		else if( _view->GetID() == "Save" )
		{
#ifdef _DEBUG
			std::cout << "INFO: AssembleActivity request to save tree" << std::endl;
#endif
			VolumeTree::Tree tempTree;

			tempTree.SetRoot( m_totemController->GetNodeTree() );
			bool fileFound = false;
			std::string fullFilename;
			std::string extension = ".xml";
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

				tempTree.SaveXML( fullFilename );

				m_showSaveConfirmation = true;
				m_saveTextCounter = 3.0f;
			}
			else
			{
				std::cerr << "WARNING: Cannot save file. Try removing previous files, limit is 10000 files" << std::endl;
			}
		}
		else if( _view->GetID() == "Export" )
		{
#ifdef _DEBUG
			std::cout << "INFO: AssembleActivity request to export tree" << std::endl;
#endif

			VolumeTree::Tree tempTree;
			// Scaling 1:20 ---- Remove this? If it's saving, I think it is needed when exporting .vol file
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
				// We want to find the highest number that *doesn't* exist
				fileFound = !boost::filesystem::exists( fullFilename );
				i++;
			}
			while( (i < 10000) && !fileFound );

			if( fileFound )
			{
				if( !boost::filesystem::exists( m_saveDir ) )
				{
					boost::filesystem::create_directory( m_saveDir );
				}
				tempTree.Save( fullFilename );

				m_showSaveConfirmation = true;
				m_saveTextCounter = 3.0f;
			}
			else
			{
				std::cerr << "WARNING: Cannot export file. Try removing previous files, limit is 10000 files" << std::endl;
			}
		}
		else if( _view->GetID() == "UndoButton" )
		{
			m_commandManager->Undo();
			RebuildTrees( true );
		}
		else if( _view->GetID() == "RedoButton" )
		{
			m_commandManager->Redo();
			RebuildTrees( true );
		}
	}
	else if( _handler == m_addPrimitiveHandler )
	{
		// We have received a request to add a primitive to the totempole
		// We don't know which button was selected though, so we need to find out:

		//std::cout<<"INFO: AssembleActivity attempting to add primitive: "<<modelManager->GetAttributeString(dataEntryIndex,"name")<<std::endl;

		int dataEntryIndex = -1;
		std::vector< std::pair< ShivaGUI::AdapterView*, ShivaGUI::GUIController* > >::iterator it = m_listViews.begin();
		do
		{
			dataEntryIndex = ( *it ).first->GetDataIndex( _view );
			++it;
		}
		while( it != m_listViews.end() && dataEntryIndex < 0 );

		if( dataEntryIndex == -1 )
		{
			std::cerr << "WARNING: AssembleActivity received request to add primitive from unknown button origin" << std::endl;
			return;
		}

		// We use the button index to create that primitive:
		AddObjectCommand* addObjCmd = new AddObjectCommand();
		addObjCmd->SetParameters( dataEntryIndex, GetNumGUIControllers() );

		m_commandManager->Execute( addObjCmd );

        // This did sort of work, but not on first press!!!
///		_view->SetID( addObjCmd->GetPrimType() );

		//GetGUIManager()->StartActivityForResult( _view->GetID(), NULL );

		//m_totemController->AddObjectToTop( dataEntryIndex, GetNumGUIControllers() );
		//m_totemController->SelectTopObject();

		// The Views now need to rebuild their trees:
		RebuildTrees();
	}
	else if( _handler == m_launchActivityHandler )
	{
		if( _view != NULL )
		{
			// Use the ID of the view as the activity name
			// If we say we expect a result we will be notified when it returns, so we can rebuild the trees
			GetGUIManager()->StartActivityForResult( _view->GetID(), NULL );
		}
	}
	
	// Update our views
	UpdateViews();
}

//----------------------------------------------------------------------------------

void AssembleActivity::OnActivityResult( ShivaGUI::Bundle *_data )
{
	RebuildTrees();
}

//----------------------------------------------------------------------------------

void AssembleActivity::UpdateViews()
{
	for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
	{
		( *it ).first->AddWorldRotationOffsetDegs( m_rotationX, m_rotationY, m_rotationZ );
	}
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;

	for( std::vector< std::pair< ShivaGUI::TextView*, ShivaGUI::GUIController* > >::iterator it = m_saveConfirmViews.begin(); it != m_saveConfirmViews.end(); ++it )
	{
		( *it ).first->SetVisibility( m_showSaveConfirmation );
	}
}

//----------------------------------------------------------------------------------

void AssembleActivity::ResetRotation()
{
	for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = m_volViews.begin(); it != m_volViews.end(); ++it )
	{
		( *it ).first->ResetWorldRotation();
	}
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;
}

//----------------------------------------------------------------------------------

void AssembleActivity::RebuildTrees( bool _justparams )
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
			( *it ).first->RefreshTree();
		}
	}
}

//----------------------------------------------------------------------------------

void AssembleActivity::InitIOWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
{
	// Here we are going to initialise an I/O window, which will have a couple of buttons

	// Register our UtilityEventHandlers with the GUIController.
	/*
	guiController->RegisterListener(_rotateLeftHandler,"rotateLeftHandler");
	guiController->RegisterListener(_rotateRightHandler,"rotateRightHandler");
	guiController->RegisterListener(_rotateUpHandler,"rotateUpHandler");
	guiController->RegisterListener(_rotateDownHandler,"rotateDownHandler");
	guiController->RegisterListener(_deleteTopHandler,"deleteTopHandler");
	guiController->RegisterListener(_editHandler,"editHandler");
	*/
	_guiController->RegisterListener( m_buttonHandler, "buttonHandler" );
	_guiController->RegisterListener( m_buttonHandler, "saveHandler" );
	_guiController->RegisterListener( m_buttonHandler, "exportHandler" );
	
	// For load event
	
	_guiController->RegisterListener( m_buttonHandler, "loadHandler" );
	_guiController->RegisterListener( m_addPrimitiveHandler, "addPrimitiveHandler" );
	_guiController->RegisterListener( m_launchActivityHandler, "launchActivityHandler" );

	// The layout xml is where the widget hierarchy is specified
	// This function will load the file and expand the hierarchy ready for display on screen
	_guiController->LoadContentView( "AssembleIOWithSymbols.xml" );

	// Retrieve primitive list
	ShivaGUI::AdapterView *primListView = dynamic_cast< ShivaGUI::AdapterView* >( _guiController->GetResources()->GetViewFromID( "PrimitiveList" ) );
	if( primListView != NULL )
	{
		primListView->SetAdapter( m_modelListAdapter );
		m_listViews.push_back( std::pair< ShivaGUI::AdapterView*, ShivaGUI::GUIController* >( primListView, _guiController ) );
	}

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

	ShivaGUI::TextView *saveConfirmView = dynamic_cast< ShivaGUI::TextView* >( _guiController->GetResources()->GetViewFromID( "saveConfirmationText" ) );
	if( saveConfirmView != NULL )
	{
		m_saveConfirmViews.push_back( std::pair< ShivaGUI::TextView*, ShivaGUI::GUIController* >( saveConfirmView, _guiController ) );
	}
	UpdateViews();
}

//----------------------------------------------------------------------------------

void AssembleActivity::InitOutputWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
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