#include "System/Activities/ProfileChooserActivity.h"
#include "System/ProfileManager.h"
#include "GUIManager.h"

//----------------------------------------------------------------------------------

void ShivaGUI::ProfileChooserActivity::OnCreate( Bundle *_data )
{
	m_startNextActivity = false;


	m_startHandler = new UtilityEventHandler( this );
	m_chooseProfileListHandler = new UtilityEventHandler( this );
	m_copyProfileHandler =  new UtilityEventHandler( this );
	m_deleteProfileHandler = new UtilityEventHandler( this );
	m_buttonHandler = new UtilityEventHandler( this );

	m_profileFilesProvider = new FilesystemDataProvider();
	m_profileFilesProvider->SetShowDirectories(false);


	// Create and configure our list adapter
	m_profileFilesAdapter =  new ShivaGUI::DataAdapter();
	m_profileFilesAdapter->SetDataProvider( m_profileFilesProvider );

	// create mapping:
	std::string fromAttrib( "FILENAME_NO_EXTENSION" ); // This must correspond to what we want out of the DataProvider
	std::string toView( "textbutton" );		// This must be the View ID in the layout xml that we want to map the above data to

	m_profileFilesAdapter->SetMapping( &fromAttrib, &toView, 1 );
	m_profileFilesAdapter->SetLayoutFile( "ProfileChooserEntry.xml" );	// This is the layout xml that specifies the single entry for the ListView



	// The data bundle should contain the name of the activity we need to start once the profile has been chosen
	// It may also contain information passed in, e.g. through the command line, about which profile to load
	if( _data != NULL )
	{
		if( _data->ContainsKey( "StartActivity" ) )
		{
			m_startActivity = _data->GetString( "StartActivity" );
		}
		if( _data->ContainsKey( "ProfileDirectory" ) )
		{
			if( !GetGUIManager()->SetProfileDirectory( _data->GetString( "ProfileDirectory" ) ) )
				std::cerr << "WARNING: Could not set profile directory to: " << _data->GetString( "ProfileDirectory" ) << std::endl;
		}
		if( _data->ContainsKey( "LoadProfile" ) && !m_startActivity.empty() )
		{
			// We are given a profile to load, probably from commandline, to skip this ProfileChooserActivity and start the next one already
			m_chosenProfile = _data->GetString( "LoadProfile" );

			StartNextActivity();
		}
	}

	//_profileFilesProvider->SetCurrentDirectory( GetGUIManager()->GetProfileDir() );
	m_profileFilesProvider->SetCurrentDirectoryNew( GetGUIManager()->GetProfileDir() );

	LoadViews();
	UpdateViews();
}

//----------------------------------------------------------------------------------

void ShivaGUI::ProfileChooserActivity::OnConfigurationChanged()
{
	m_currentProfileDisplays.clear();
	m_currentProfDirDisplays.clear();
	LoadViews();
}

//----------------------------------------------------------------------------------

void ShivaGUI::ProfileChooserActivity::OnDestroy()
{
	delete m_startHandler;
	delete m_buttonHandler;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ProfileChooserActivity::OnUpdate( float _deltaTs )
{
	// Need to make sure the command queue is empty as bootstrapping takes a while
	if( m_startNextActivity && GetGUIManager()->CommandQueueEmpty() )
	{
		std::cout << "ProfileChooserActivity::OnUpdate trying to start activity: " << m_startActivity << std::endl;
		m_startNextActivity = false;
		GetGUIManager()->StartActivity( m_startActivity );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ProfileChooserActivity::UtilityEventReceived( UtilityEventHandler *_handler, ShivaGUI::View *_view )
{
	if( _handler == m_startHandler )
	{
		StartNextActivity();
	}
	else if( _handler == m_chooseProfileListHandler )
	{
		// We have received a request to select a profile
		// We don't know which button it came from,
		// but the text of the button will be the profile name that we need

		TextButton *pressedButton = dynamic_cast< ShivaGUI::TextButton* >( _view );
		if( pressedButton != NULL )
		{
			m_chosenProfile = pressedButton->GetText();
			std::cout << "INFO: ProfileChooserActivity request to change to profile: " << pressedButton->GetText() << std::endl;

			UpdateViews();
		}
	}
	else if( _handler == m_copyProfileHandler )
	{
		if( !m_chosenProfile.empty() )
		{
			GetGUIManager()->GetProfileManager()->Copy( m_chosenProfile, m_chosenProfile + "_copy" );

			//std::string profileDirectory = GetGUIManager()->GetProfileDir() + "/";
			//_profileFilesProvider->CopyFile(_chosenProfile,_chosenProfile+"_copy");
			//boost::filesystem3::copy_file(profileDirectory+_chosenProfile+".xml",profileDirectory+_chosenProfile+"_copy.xml");

			for( std::vector< std::pair< ShivaGUI::ListView*, unsigned int > >::iterator it = m_profileFileListDisplays.begin(); it != m_profileFileListDisplays.end(); ++it )
				( *it ).first->RefreshFromSource();

			UpdateViews();
		}
	}
	else if( _handler == m_buttonHandler )
	{
		if( _view->GetID() == "Browse" )
		{
			char const* lFilterPatterns = "*.xml";
			char const* filename = tinyfd_openFileDialog( "Choose a Profile", "Profiles/", 1, &lFilterPatterns, NULL, 0 );

			if( !filename )
			{
				tinyfd_messageBox( "Error", "No file selected. Couldn't load profile.", "ok", "error", 1 );
			}
			else
			{
				#if defined _WIN32
					const char *lastSeparator = strrchr( filename, '\\' );
				#else
					const char *lastSeparator = strrchr( filename, '/' );
				#endif
				
				
				std::string profileName = std::string( lastSeparator ? lastSeparator + 1 : filename );

				size_t lastdot = profileName.find_last_of( "." );
				if( lastdot != std::string::npos )
				{	
					// chop off the ".xml" for the _profileName
					profileName = profileName.substr( 0, lastdot );
				}

				m_chosenProfile = profileName;
				UpdateViews();
			}
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ProfileChooserActivity::UpdateViews()
{
	m_profileFilesProvider->SetCurrentDirectoryNew( GetGUIManager()->GetProfileDir() );

	for( std::vector< std::pair< TextView*, unsigned int > >::iterator it = m_currentProfileDisplays.begin(); it != m_currentProfileDisplays.end(); ++it )
	{
		ShivaGUI::GUIController *guiController = GetGUIController( it->second );
		if( guiController != NULL )
			it->first->SetText( m_chosenProfile, guiController->GetResources() );
	}

	for( std::vector< std::pair< TextView*, unsigned int > >::iterator it = m_currentProfDirDisplays.begin(); it != m_currentProfDirDisplays.end(); ++it )
	{
		ShivaGUI::GUIController *guiController = GetGUIController( it->second );
		if( guiController != NULL )
			it->first->SetText( GetGUIManager()->GetProfileDir(), guiController->GetResources() );
	}
	Layout();
}

//----------------------------------------------------------------------------------

void ShivaGUI::ProfileChooserActivity::StartNextActivity()
{
	// Load our selected profile
	GetGUIManager()->RequestBootstrapChangeProfile( m_chosenProfile );
	m_startNextActivity = true;
	//GetGUIManager()->StartActivity(_startActivity);
	/*
	if( )
	{
		// Start the next Activity
		//GetGUIManager()->StartActivity(_startActivity);
	}
	else
	{
		std::cerr<<"WARNING: ProfileChooserActivity: Could not load profile: "<<_chosenProfile<<" please select another"<<std::endl;
	}
	*/
}

//----------------------------------------------------------------------------------

void ShivaGUI::ProfileChooserActivity::LoadViews()
{
	unsigned int numWindows = GetNumGUIControllers();
	for( unsigned int i = 0; i < numWindows; i++ )
	{
		ShivaGUI::GUIController *guiController = GetGUIController( i );

		if( guiController->GetRequestedUse() != ShivaGUI::Window::OUTPUT )
		{
			guiController->RegisterListener( m_startHandler, "StartActivityHandler" );
			guiController->RegisterListener( m_chooseProfileListHandler, "SelectProfileHandler" );
			guiController->RegisterListener( m_copyProfileHandler, "CopyProfileHandler" );
			guiController->RegisterListener( m_deleteProfileHandler, "DeleteProfileHandler" );
			guiController->RegisterListener( m_buttonHandler, "BrowseProfileDirectoryHandler" );


			guiController->LoadContentView( "ProfileChooserLayout.xml" );

			TextView *currentProfileDisplayView = dynamic_cast< TextView* >( guiController->GetResources()->GetViewFromID( "CurrentProfileDisplay" ) );
			if( currentProfileDisplayView != NULL )
			{
				m_currentProfileDisplays.push_back( std::pair< TextView*, unsigned int >( currentProfileDisplayView, i ) );
			}

			TextView *currentProfDirDisplayView = dynamic_cast< TextView* >( guiController->GetResources()->GetViewFromID( "CurrentProfileDirectory" ) );
			if( currentProfDirDisplayView != NULL )
			{
				m_currentProfDirDisplays.push_back( std::pair< TextView*, unsigned int >( currentProfDirDisplayView, i ) );
			}

			// Retrieve primitive list
			ShivaGUI::ListView *profilesListView = dynamic_cast< ShivaGUI::ListView* >( guiController->GetResources()->GetViewFromID( "ProfilesList" ) );
			if( profilesListView != NULL )
			{
				profilesListView->SetAdapter( m_profileFilesAdapter );
				m_profileFileListDisplays.push_back( std::pair< ShivaGUI::ListView*, unsigned int >( profilesListView, i ) );
			}
		}
	}

	//UpdateViews();
}

//----------------------------------------------------------------------------------