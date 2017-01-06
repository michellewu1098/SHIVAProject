
#include "AssembleActivity.h"
#include "ShivaModelManager.h"

#include "System/SharedPreferences.h"
#include "GUIManager.h"
#include "tinyfiledialogs.h"

//----------------------------------------------------------------------------------

void AssembleActivity::OnCreate( ShivaGUI::Bundle *data )
{
	// This is like our constructor
	// We use it to initialise our variables and load the layouts to window

	_totemController = Totem::Controller::GetInstance();

	_showSaveConfirmation = false;
	_saveTextCounter = 0.0f;

	_objectColourR = 0.88f;
	_objectColourG = 0.78f;
	_objectColourB = 0.54f;
	_setObjectColour = false;

	_rotationStepsize = 7.5f;
	_rotationX = _rotationY = _rotationZ = 0.0f;

	_saveRescaleSize = 20.0f;
	
	_saveDir = "Savefiles/";
	_saveName = "Totem";
	ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();
	_originalBlendingAmount = 0.0f;
	if( prefs != NULL )
	{
		_saveDir = prefs->GetString( "SaveDirectory", _saveDir );
		_saveName = prefs->GetString( "SaveFilename", _saveName );

		_originalBlendingAmount = prefs->GetFloat( "BlendingAmount", _originalBlendingAmount );
		
		if( prefs->Contains( "ObjectColourR" ) && prefs->Contains( "ObjectColourG" ) && prefs->Contains( "ObjectColourB" ) )
		{
			_objectColourR = prefs->GetFloat( "ObjectColourR", _objectColourR );
			_objectColourG = prefs->GetFloat( "ObjectColourG", _objectColourG );
			_objectColourB = prefs->GetFloat( "ObjectColourB", _objectColourB );
			_setObjectColour = true;
		}
		
		_rotationStepsize = prefs->GetFloat( "RotationStepsize", _rotationStepsize );

		_saveRescaleSize = prefs->GetFloat( "SaveRescaleSize", _saveRescaleSize );
	}
	_totemController->SetBlend( _originalBlendingAmount );

	// Create the button event handlers
	_buttonHandler = new UtilityEventHandler( this );
	_addPrimitiveHandler = new UtilityEventHandler( this );
	_launchActivityHandler = new UtilityEventHandler( this );

	// Create and configure our list adapter
	_modelListAdapter =  new ShivaGUI::DataAdapter();
	_modelListAdapter->SetDataProvider( ShivaModelManager::GetInstance() );

	// create mapping:
	std::string fromAttrib( "thumbnail_file" ); // This must correspond to the entry in the Resources/Models/index.xml that we want to map
	std::string toView( "imagebutton" );		// This must be the View ID in the layout xml that we want to map the above data to

	_modelListAdapter->SetMapping( &fromAttrib, &toView, 1 );
	_modelListAdapter->SetLayoutFile( "ModelListEntry.xml" );	// This is the layout xml that specifies the single entry for the ListView


	// We will now tell the system what to display on the windows

	// Find out how many windows we have
	int numWindows = GetNumGUIControllers();

	// For each of these...
	for( int i = 0; i < numWindows; i++ )
	{
		// We retrieve a GUIController
		// This is our main method of accessing the resources associated with a Window
		ShivaGUI::GUIController *guiController = GetGUIController(i);

		// The User Profile can specify how the windows are supposed to be used
		// We can retrieve this using the GUIController and load a different window layout
		ShivaGUI::Window::RequestedUse windowUse = guiController->GetRequestedUse();
		if( windowUse == ShivaGUI::Window::OUTPUT )
		{
			// This window should be used for output information only, with no buttons etc
			InitOutputWindow( guiController, data );
		}
		else
		{
			// We just assume that this is a generic input/output window
			InitIOWindow( guiController, data );
		}
	}
}

//----------------------------------------------------------------------------------

void AssembleActivity::OnResume()
{
	_totemController->ShowSelection( true );
	std::cout << "INFO: Assemble Activity OnResume()" << std::endl;
}

//----------------------------------------------------------------------------------

void AssembleActivity::OnDestroy()
{
	// This is like our destructor, so delete our data etc
//	delete _rotateLeftHandler;
//	delete _rotateRightHandler;
//	delete _rotateUpHandler;
//	delete _rotateDownHandler;
//	delete _deleteTopHandler;
//	delete _editHandler;
	delete _buttonHandler;
	delete _addPrimitiveHandler;
	delete _modelListAdapter;
}

//----------------------------------------------------------------------------------

void AssembleActivity::OnUpdate( float deltaTs )
{
	if( _showSaveConfirmation )
	{
		_saveTextCounter -= deltaTs;
		if( _saveTextCounter <= 0.0f )
		{
			_showSaveConfirmation = false;
			UpdateViews();
		}
	}
}

//----------------------------------------------------------------------------------

void AssembleActivity::UtilityEventReceived( UtilityEventHandler *handler, ShivaGUI::View *view )
{
	// This function is called when an event is received
	// the handler we are given should be one of those we registered
	// The view we are given should be the View that generated the event (e.g. the button)
	// Here we'll change the text that is displayed based on which button is pressed
	/*
	if( handler == _rotateLeftHandler )
	{
		std::cout<<"INFO: AssembleActivity request to rotate left"<<std::endl;
		_rotationZ -= _rotationStepsize;
	}
	else if( handler == _rotateRightHandler )
	{
		std::cout<<"INFO: AssembleActivity request to rotate right"<<std::endl;
		_rotationZ += _rotationStepsize;
	}
	else if( handler == _rotateUpHandler )
	{
		std::cout<<"INFO: AssembleActivity request to rotate up"<<std::endl;
		_rotationX -= _rotationStepsize;
	}
	else if( handler == _rotateDownHandler )
	{
		std::cout<<"INFO: AssembleActivity request to rotate down"<<std::endl;
		_rotationX += _rotationStepsize;
	}
	else if( handler == _deleteTopHandler )
	{
		std::cout<<"INFO: AssembleActivity request to delete selected primitive"<<std::endl;
		_totemController->DeleteSelectedObject();
		RebuildTrees();
	}
	*/
	if( handler == _buttonHandler )
	{
		if( view->GetID() == "RotateLeft" )
		{
			std::cout << "INFO: AssembleActivity request to rotate left" << std::endl;
			_rotationZ -= _rotationStepsize;
		}
		else if( view->GetID() == "RotateRight" )
		{
			std::cout << "INFO: AssembleActivity request to rotate right" << std::endl;
			_rotationZ += _rotationStepsize;
		}
		else if( view->GetID() == "RotateUp" )
		{
			std::cout << "INFO: AssembleActivity request to rotate up" << std::endl;
			_rotationX -= _rotationStepsize;
		}
		else if( view->GetID() == "RotateDown" )
		{
			std::cout << "INFO: AssembleActivity request to rotate down" << std::endl;
			_rotationX += _rotationStepsize;
		}
		else if( view->GetID() == "DeleteSelected" )
		{
			std::cout << "INFO: AssembleActivity request to delete selected object" << std::endl;
			_totemController->DeleteSelectedObject();
			RebuildTrees();
		}
		else if( view->GetID() == "SelectAbove" )
		{
			std::cout << "INFO: AssembleActivity request to select object above" << std::endl;
			_totemController->SelectObjectAbove();
		}
		else if( view->GetID() == "SelectBelow" )
		{
			std::cout << "INFO: AssembleActivity request to select object below" << std::endl;
			_totemController->SelectObjectBelow();
		}
		else if( view->GetID() == "SwapAbove" )
		{
			std::cout << "INFO: AssembleActivity request to swap with object above" << std::endl;
			_totemController->ReorderSelectedObject( true );
			RebuildTrees();
		}
		else if( view->GetID() == "SwapBelow" )
		{
			std::cout << "INFO: AssembleActivity request to swap with object below" << std::endl;
			_totemController->ReorderSelectedObject( false );
			RebuildTrees();
		}
		else if( view->GetID() == "NudgeUp" )
		{
			std::cout << "INFO: AssembleActivity request to nudge object upward" << std::endl;
			_totemController->MoveSelectedObject( 0.0f, 0.0f, 0.1f );
			RebuildTrees(true);
		}
		else if( view->GetID() == "NudgeDown" )
		{
			std::cout << "INFO: AssembleActivity request to nudge object downward" << std::endl;
			_totemController->MoveSelectedObject( 0.0f, 0.0f, -0.1f );
			RebuildTrees( true );
		}
		else if( view->GetID() == "RotateReset" )
		{
			ResetRotation();
		}
		else if( view->GetID() == "Edit" )
		{
			// If we say we expect a result we will be notified when it returns, so we can rebuild the trees
			GetGUIManager()->StartActivityForResult( "EditMenuActivity", NULL );
		}
		else if( view->GetID() == "New" )
		{
			_totemController->DeleteAll();
			_totemController->SetBlend( _originalBlendingAmount );
			ResetRotation();
			RebuildTrees( true );
		}
		else if( view->GetID() == "Load" )
		{
			char const * lFilterPatterns[2] = { "*.xml", "*.vol" };
			char const* fileName = tinyfd_openFileDialog( "SHIVA Models", "Savefiles/", 2, lFilterPatterns, NULL, 0 );
			
			if ( !fileName )
			{
				tinyfd_messageBox( "Error", "No file selected. Couldn't load anything.", "ok", "error", 1);
			}
			else
			{
				_totemController->DeleteAll();
				_totemController->SetBlend( _originalBlendingAmount );
				ResetRotation();
			
				VolumeTree::Tree tmpTree;

				if( tmpTree.Load( fileName ) )
				{ 
					_totemController->loadModel( tmpTree.getReverseTree() );
				    _totemController->SelectTopObject();
				}
			
				RebuildTrees( true );
			}
		}
		else if( view->GetID() == "Save" )
		{
			std::cout << "INFO: AssembleActivity request to save tree" << std::endl;
			VolumeTree::Tree tempTree;

			// Scaling 1:20 ---- Remove this? If it's saving, I think it is needed when exporting .vol file
			/*VolumeTree::TransformNode *rootScaleNode = new VolumeTree::TransformNode( _totemController->GetNodeTree() );
			rootScaleNode->SetScale( _saveRescaleSize );
			tempTree.SetRoot( rootScaleNode );*/

			//bool fileFound = false;
			//std::string fullFilename;
			//std::string extension = ".vol";
			//unsigned int i = 1;
			//do
			//{
			//	std::stringstream fileNum;
			//	fileNum<<i;
			//	fullFilename = _saveDir + _saveName + fileNum.str() + extension;
			//	// We want to find the highest number that *doesn't* exist
			//	fileFound = !boost::filesystem::exists( fullFilename );
			//	i++;
			//}
			//while( (i < 10000) && !fileFound );

			//if( fileFound )
			//{
			//	if( !boost::filesystem::exists( _saveDir ) )
			//	{
			//		boost::filesystem::create_directory( _saveDir );
			//	}
			//	tempTree.Save( fullFilename );
			//	tempTree.SaveXML( "test.xml" );

			//	_showSaveConfirmation = true;
			//	_saveTextCounter = 3.0f;
			//}
			//else
			//{
			//	std::cerr << "WARNING: Cannot save file. Try removing previous files, limit is 10000 files" << std::endl;
			//}

			tempTree.SetRoot( _totemController->GetNodeTree() );
			bool fileFound = false;
			std::string fullFilename;
			std::string extension = ".xml";
			unsigned int i = 1;
			do
			{
				std::stringstream fileNum;
				fileNum<<i;
				fullFilename = _saveDir + _saveName + fileNum.str() + extension;
				fileFound = !boost::filesystem::exists( fullFilename );
				i++;
			}
			while( (i < 10000) && !fileFound );

			if( fileFound )
			{
				if( !boost::filesystem::exists( _saveDir ) )
				{
					boost::filesystem::create_directory( _saveDir );
				}

				tempTree.SaveXML( fullFilename );

				_showSaveConfirmation = true;
				_saveTextCounter = 3.0f;
			}
			else
			{
				std::cerr << "WARNING: Cannot save file. Try removing previous files, limit is 10000 files" << std::endl;
			}
		}
	}
	else if( handler == _addPrimitiveHandler )
	{
		// We have received a request to add a primitive to the totempole
		// We don't know which button was selected though, so we need to find out:

		//std::cout<<"INFO: AssembleActivity attempting to add primitive: "<<modelManager->GetAttributeString(dataEntryIndex,"name")<<std::endl;

		int dataEntryIndex = -1;
		std::vector< std::pair< ShivaGUI::AdapterView*, ShivaGUI::GUIController* > >::iterator it = _listViews.begin();
		do
		{
			dataEntryIndex = ( *it ).first->GetDataIndex( view );
			++it;
		}
		while( it != _listViews.end() && dataEntryIndex < 0 );

		if( dataEntryIndex == -1 )
		{
			std::cerr << "WARNING: AssembleActivity received request to add primitive from unknown button origin" << std::endl;
			return;
		}

		// We use the button index to create that primitive:
		_totemController->AddObjectToTop( dataEntryIndex );
		_totemController->SelectTopObject();

		// The Views now need to rebuild their trees:
		RebuildTrees();


		/*
		// Retrieve the filename from the ShivaModelManager:
		ShivaModelManager *modelManager = ShivaModelManager::GetInstance();

		std::cout<<"INFO: AssembleActivity attempting to add primitive: "<<modelManager->GetAttributeString(dataEntryIndex,"name")<<std::endl;

		std::string filename = modelManager->GetAttributeString(dataEntryIndex,"bdf_file");

		if( filename.empty() )
		{
			std::cerr<<"WARNING: AssembleActivity received request to add primitive (index "<<dataEntryIndex<<") but this entry has no bdf file"<<std::endl;
			return;
		}

		// Give file to SDF Views
		AddTopObject(filename);
		*/
	}
	else if( handler == _launchActivityHandler )
	{
		if( view != NULL )
		{
			// Use the ID of the view as the activity name
			// If we say we expect a result we will be notified when it returns, so we can rebuild the trees
			GetGUIManager()->StartActivityForResult( view->GetID(), NULL );
		}
	}
	/*
	else if( handler == _addPrimitiveHandler )
	{
		// We have received a request to add a primitive to the totempole
		// We don't know which button was selected though, so we need to find out:

		int dataEntryIndex = -1;
		std::vector< std::pair<ShivaGUI::ListView*,ShivaGUI::GUIController*> >::iterator it = _listViews.begin();
		do
		{
			dataEntryIndex = (*it).first->GetDataIndex(view);
			++it;
		}
		while( it != _listViews.end() && dataEntryIndex < 0 );

		if( dataEntryIndex == -1 )
		{
			std::cerr<<"WARNING: AssembleActivity received request to add primitive from unknown button origin"<<std::endl;
			return;
		}

		// Retrieve the filename from the ShivaModelManager:
		ShivaModelManager *modelManager = ShivaModelManager::GetInstance();

		std::cout<<"INFO: AssembleActivity attempting to add primitive: "<<modelManager->GetAttributeString(dataEntryIndex,"name")<<std::endl;

		std::string filename = modelManager->GetAttributeString(dataEntryIndex,"bdf_file");

		if( filename.empty() )
		{
			std::cerr<<"WARNING: AssembleActivity received request to add primitive (index "<<dataEntryIndex<<") but this entry has no bdf file"<<std::endl;
			return;
		}

		// Give file to SDF Views
		AddTopObject(filename);

	}
	*/

	// Update our views
	UpdateViews();
}

//----------------------------------------------------------------------------------

void AssembleActivity::OnActivityResult( ShivaGUI::Bundle *data )
{
	RebuildTrees();
}

//----------------------------------------------------------------------------------

void AssembleActivity::UpdateViews()
{
	for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = _VolViews.begin(); it != _VolViews.end(); ++it )
	{
		( *it ).first->AddWorldRotationOffsetDegs( _rotationX, _rotationY, _rotationZ );
	}
	_rotationX = _rotationY = _rotationZ = 0.0f;

	for( std::vector< std::pair< ShivaGUI::TextView*, ShivaGUI::GUIController* > >::iterator it = _saveConfirmViews.begin(); it != _saveConfirmViews.end(); ++it )
	{
		( *it ).first->SetVisibility( _showSaveConfirmation );
	}
}

//----------------------------------------------------------------------------------

void AssembleActivity::ResetRotation()
{
	for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = _VolViews.begin(); it != _VolViews.end(); ++it )
	{
		( *it ).first->ResetWorldRotation();
	}
	_rotationX = _rotationY = _rotationZ = 0.0f;
}

//----------------------------------------------------------------------------------

void AssembleActivity::RebuildTrees( bool justparams )
{
	for( std::vector< std::pair< VolView*, ShivaGUI::GUIController* > >::iterator it = _VolViews.begin(); it != _VolViews.end(); ++it )
	{
		( *it ).second->MakeCurrent();
		if( justparams )
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

void AssembleActivity::InitIOWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data)
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
	guiController->RegisterListener( _buttonHandler,"buttonHandler" );
	guiController->RegisterListener( _buttonHandler,"saveHandler" );
	// For load event
	guiController->RegisterListener( _buttonHandler,"loadHandler" );
	guiController->RegisterListener( _addPrimitiveHandler,"addPrimitiveHandler" );
	guiController->RegisterListener( _launchActivityHandler,"launchActivityHandler" );

	// The layout xml is where the widget hierarchy is specified
	// This function will load the file and expand the hierarchy ready for display on screen
	guiController->LoadContentView( "AssembleIO.xml" );

	// Retrieve primitive list
	ShivaGUI::AdapterView *primListView = dynamic_cast< ShivaGUI::AdapterView* >( guiController->GetResources()->GetViewFromID( "PrimitiveList" ) );
	if( primListView != NULL )
	{
		primListView->SetAdapter( _modelListAdapter );
		_listViews.push_back( std::pair< ShivaGUI::AdapterView*, ShivaGUI::GUIController* >( primListView, guiController ) );
	}

	VolView *volView = dynamic_cast< VolView* >( guiController->GetResources()->GetViewFromID( "MainVolView" ) );
	if( volView != NULL )
	{
		// For the assemble activity we want to allow the user to click to select primitives
		volView->AllowObjectClickSelection( true );
		_VolViews.push_back( std::pair< VolView*, ShivaGUI::GUIController* >( volView, guiController ) );
		if( _setObjectColour )
		{
			volView->SetObjectColour( _objectColourR, _objectColourG, _objectColourB );
		}
	}

	ShivaGUI::TextView *saveConfirmView = dynamic_cast< ShivaGUI::TextView* >( guiController->GetResources()->GetViewFromID( "saveConfirmationText" ) );
	if( saveConfirmView != NULL )
	{
		_saveConfirmViews.push_back( std::pair< ShivaGUI::TextView*, ShivaGUI::GUIController* >( saveConfirmView, guiController ) );
	}
	UpdateViews();
}

//----------------------------------------------------------------------------------

void AssembleActivity::InitOutputWindow( ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data )
{
	// This window will be used for output only, so we don't need to register the listeners as there are no buttons

	// Just load the layout xml
	// Note that this is a different xml file to the IO window, so it will show different things (i.e. no buttons)
	guiController->LoadContentView( "VolViewOutput.xml" );


	VolView *volView = dynamic_cast< VolView* >( guiController->GetResources()->GetViewFromID( "MainVolView" ) );
	if( volView != NULL )
	{
		_VolViews.push_back( std::pair< VolView*, ShivaGUI::GUIController* >( volView, guiController ) );
		if( _setObjectColour )
		{
			volView->SetObjectColour( _objectColourR, _objectColourG, _objectColourB );
		}
	}
	UpdateViews();
}

//----------------------------------------------------------------------------------