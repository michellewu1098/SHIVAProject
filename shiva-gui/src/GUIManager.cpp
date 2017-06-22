#include "GUIManager.h"
#include "System/Activities/ProfileChooserActivity.h"

//----------------------------------------------------------------------------------

ShivaGUI::GUIManager::GUIManager( std::string _programName, std::string _programOptionsDir )
{

	m_activityResult = NULL;
	m_activityStartData = NULL;
	m_preferences = NULL;
	m_currentActivityCommand = NONE;
	m_exitEvent = false;
	m_windowSDLIDs = NULL;
	m_profileManager = new ProfileManager( _programName, _programOptionsDir );
	m_audioManager = new AudioManager();

	m_defaultFont = "Resources/Fonts/FreeSans.ttf";
	m_defaultTheme = "Resources/Themes/system.xml";

	m_lastTime = SDL_GetTicks();
	m_deltaTs = 0.0f;

	// Load system profile
	SetProfileDirectory( "Profiles" );
	LoadProfile( "System" );

	//Create window(s) from profile
	if( !CreateWindowsFromProfile() )
	{
		CreateWindow( 640, 480, 100, 100 );
	}

}

//----------------------------------------------------------------------------------

ShivaGUI::GUIManager::~GUIManager()
{
	while( !m_windows.empty() )
	{
		WindowGroup currentWindowGroup = m_windows.back();
		m_windows.pop_back();
		delete currentWindowGroup.m_window;
		delete currentWindowGroup.m_resourceManager;
	}

	delete [] m_windowSDLIDs;

	for( std::vector< InputController* >::iterator it = m_inputControllers.begin(); it != m_inputControllers.end(); ++it )
	{
		delete *it;
	}

	delete m_profileManager;
	delete m_audioManager;
	delete m_preferences;

	delete m_activityStartData;

	m_profileManager = NULL;
	m_audioManager = NULL;
	m_preferences = NULL;

	while( !m_activityStack.empty() )
	{
		Activity *currentActivity = m_activityStack.back();
		if( currentActivity != NULL )
		{
			currentActivity->Destroy();
		}

		m_activityStack.pop_back();
	}
	
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIManager::StartWithProfileChooser( std::string _startActivity, std::string _profileDir, std::string _profileName )
{
	
	// Need to make sure activity creator is registered
	RegisterActivityCreator( "ProfileChooserActivity", ProfileChooserActivity::Factory );

	Bundle *dataBundle = new Bundle();
	dataBundle->PutString( "StartActivity", _startActivity );
	if( !_profileDir.empty() )
		dataBundle->PutString( "ProfileDirectory", _profileDir );
	if( !_profileName.empty() )
		dataBundle->PutString( "LoadProfile", _profileName );

	StartActivity( "ProfileChooserActivity", dataBundle );

	EnterMainLoop();
}

//----------------------------------------------------------------------------------

bool ShivaGUI::GUIManager::SetProfileDirectory( std::string _profileDir )
{
	bool result = m_profileManager->SetDirectory( _profileDir );

	if( result )
	{
		m_preferences = new SharedPreferences( _profileDir + m_programNameDir + "/prefs.xml" );
	}
	return result;
}

//----------------------------------------------------------------------------------

std::string ShivaGUI::GUIManager::GetProfileOptionsDir()
{
	return m_profileManager->GetCurrentOptionsDir();
}

//----------------------------------------------------------------------------------

std::string ShivaGUI::GUIManager::GetProfileDir()
{
	return m_profileManager->GetDirectory();
}

//----------------------------------------------------------------------------------

std::string ShivaGUI::GUIManager::GetCurrentProfileFileName()
{
	return m_profileManager->GetCurrentProfileFileName();
}

//----------------------------------------------------------------------------------

bool ShivaGUI::GUIManager::LoadProfile( std::string _profileName )
{
	bool loadSuccess = m_profileManager->Load( _profileName );
	if( loadSuccess )
	{
		// Load some settings from the profile
		m_defaultTheme = m_profileManager->GetString( "Theme", m_defaultTheme );
		m_defaultFont  = m_profileManager->GetString( "Font", m_defaultFont );

		// TODO: load other settings
		// The difficulty here is that the other settings are needed for each Activity, so loading settings here makes no sense
	}
	return loadSuccess;
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIManager::RequestBootstrapChangeProfile( std::string _profileName )
{
	m_bootstrapChangeProfileName = _profileName;

	if( m_currentActivityCommand != NONE )
		std::cerr << "WARNING: GUIManager issuing multiple Activity commands in one update cycle" << std::endl;
	m_currentActivityCommand = BOOTSTRAP_PROFILE_CHANGE;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::GUIManager::BootstrapChangeProfile( std::string _profileName )
{
	if( LoadProfile( _profileName ) )
	{
		std::cout << "INFO: GUIManager attempting to pull itself out of the mud by its own bootstraps..." << std::endl;
		// Delete all windows
		while( !m_windows.empty() )
		{
			WindowGroup currentWindowGroup = m_windows.back();
			m_windows.pop_back();
			delete currentWindowGroup.m_window;
			delete currentWindowGroup.m_resourceManager;
		}
		m_windows.clear();
		delete [] m_windowSDLIDs;
		m_windowSDLIDs = NULL;

		// Recreate all windows
		CreateWindowsFromProfile();


		// Activities store handles to windows, so these need to all have new GUIControllers

		for( std::list<Activity*>::iterator currentActivityIt = m_activityStack.begin(); currentActivityIt != m_activityStack.end(); ++currentActivityIt )
		{
			Activity *currentActivity =  *currentActivityIt;

			if( currentActivity != NULL )
			{
				// This will also delete the existing controllers
				currentActivity->SetNumGUIControllers( m_windows.size() );

				// Bind windows to current Activity, creating the Window -> InputController -> GUIController chains
				unsigned int index = 0;
				for( std::vector< WindowGroup >::iterator it = m_windows.begin(); it != m_windows.end(); ++it )
				{
					currentActivity->AddGUIController( new GUIController( it->m_window, it->m_resourceManager, this ), index );
					++index;
				}

				currentActivity->ConfigurationChanged();
			}
		}

		std::cout << "INFO: GUIManager hopefully out the mud now" << std::endl;
		return true;
	}

	std::cerr << "ERROR: GUIManager::BootstrapChangeProfile failed on LoadProfile(), results undefined" << std::endl;
	return false;
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIManager::SaveLayoutToProfile( std::string _inputFilename, View* _rootNode, ResourceManager* _resources )
{
	if( _inputFilename.empty() || _rootNode == NULL || _resources == NULL )
		return;

	// figure out where to save it
	// Strip any leading directories from the inputFilename:
	boost::filesystem::path pathFilename( _inputFilename );
	boost::filesystem::path actualFilename = pathFilename.filename();

	boost::filesystem::path fullfilename( m_profileManager->GetCurrentOptionsDir() + "/Layout/" + actualFilename.string() );
	// If the directories don't exist, we need to create them:
	boost::filesystem::create_directories( m_profileManager->GetCurrentOptionsDir() + "/Layout/" );

	if( _resources->OutputLayout( fullfilename.string(), _rootNode ) )
		std::cout << "INFO: Layout Saved to Profile: filename = " << fullfilename << std::endl;
	else
		std::cout << "WARN: Layout Failed to Save to Profile: filename = " << fullfilename << std::endl;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::GUIManager::CreateWindowsFromProfile()
{
	// See how many windows the profile says we need to create
	int numWindows = m_profileManager->ContainsOption( "window" );
	if( numWindows <= 0 || numWindows > 10 )
		return false;

	for( int i = 0; i < numWindows; i++ )
	{
		// The options for our window are stored in a 'window' node in the profile
		if( m_profileManager->EnterOptionNode( "window", i ) )
		{
			// We can now query our window settings
			unsigned int width = m_profileManager->GetInt( "width", 640 );
			unsigned int height = m_profileManager->GetInt( "height", 480 );
			int positionX = m_profileManager->GetInt( "positionX", 50 );
			int positionY = m_profileManager->GetInt( "positionY", 50 );
			std::string fontName = m_profileManager->GetString( "font", m_defaultFont );
			std::string themeName = m_profileManager->GetString( "theme", m_defaultTheme );
			std::string windowUse = m_profileManager->GetString( "use", "ANYTHING" );

			// Create our window
			CreateWindow( width, height, positionX, positionY );

			// Apply settings to the window's resource manager
			m_windows.back().m_resourceManager->SetDefaultFont( fontName );
			m_windows.back().m_resourceManager->SetTheme( themeName );

			m_windows.back().m_window->SetRequestedUse( windowUse ); 

			// Need to allow the Resource Manager to load const options from the Profile
			// Exit the Window option node and pass the profile manager to the window's resource manager
			m_profileManager->ExitOptionNode();
			m_windows.back().m_resourceManager->LoadProfileAttributeConsts( m_profileManager );
		}
		m_profileManager->ExitOptionNode();
	}
	return true;
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIManager::CreateWindow( unsigned int _width, unsigned int _height, int _positionX, int _positionY )
{
	unsigned int newWindowIndex = m_windows.size();
	m_windows.push_back( WindowGroup( new Window( _positionX, _positionY, _width, _height ), CreateResourceManager( newWindowIndex ) ) );

	delete [] m_windowSDLIDs;

	m_windowSDLIDs = new int[ m_windows.size() ];
	for( unsigned int i = 0; i < m_windows.size(); i++ )
	{
		m_windowSDLIDs[ i ] = m_windows.at( i ).m_window->GetSDLWindowID();
	}
}

//----------------------------------------------------------------------------------

ShivaGUI::SharedPreferences* ShivaGUI::GUIManager::GetProgSpecificOptions()
{
	return m_profileManager->GetProgSpecificOptions();
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIManager::RegisterActivityCreator( std::string _activityName, Activity* ( *func )() )
{
	m_activityCreators[ _activityName ] = func;
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIManager::StartActivity( std::string _activityName, Bundle* _data )
{
	m_startActivityName = _activityName;
	m_activityStartData = _data;
	if( m_currentActivityCommand != NONE )
		std::cerr << "WARNING: GUIManager issuing multiple Activity commands in one update cycle" << std::endl;
	m_currentActivityCommand = START_ACTIVITY;
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIManager::StartActivityForResult( std::string _activityName, Bundle* _data )
{
	m_startActivityName = _activityName;
	m_activityStartData = _data;
	if( m_currentActivityCommand != NONE )
		std::cerr << "WARNING: GUIManager issuing multiple Activity commands in one update cycle" << std::endl;
	m_currentActivityCommand = START_ACTIVITY_FOR_RESULT;
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIManager::SetActivityResult( Bundle* _dataIn )
{
	m_activityResult = _dataIn;
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIManager::FinishActivity()
{
	if( m_currentActivityCommand != NONE )
		std::cerr << "WARNING: GUIManager issuing multiple Activity commands in one update cycle" << std::endl;
	m_currentActivityCommand = STOP_ACTIVITY;
}

//----------------------------------------------------------------------------------

ShivaGUI::Activity* ShivaGUI::GUIManager::GetCurrentActivity()
{
	if( !m_activityStack.empty() )
		return m_activityStack.back();
	return NULL;
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIManager::RegisterViewCreator( std::string _viewName, View* ( *func )() )
{
	m_viewCreators[ _viewName ] = func;
}

//----------------------------------------------------------------------------------

ShivaGUI::View* ShivaGUI::GUIManager::CreateView( std::string _name )
{
	if( m_viewCreators.find( _name ) != m_viewCreators.end() )
	{
		return ( m_viewCreators[ _name ] )();
	}
	return NULL;
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIManager::EnterMainLoop()
{
	do
	{
		unsigned int current = SDL_GetTicks();
		m_deltaTs = ( float )( current - m_lastTime ) / 1000.0f;
		m_lastTime = current;

		if( m_deltaTs > 1.0f )
			m_deltaTs = 1.0f;
		else if( m_deltaTs < 0.00001f )
			m_deltaTs = 0.00001f;

		Update( m_deltaTs );

		Draw();

		// Limiter in case we're running really quick
		if( m_deltaTs < ( 1.0f / 50.0f ) )	// not sure how accurate the SDL_Delay function is..
			SDL_Delay( ( unsigned int )( ( ( 1.0f / 50.0f ) - m_deltaTs ) * 1000.0f ) );
	}
	while( !m_exitEvent );
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIManager::Update( float _deltaTs )
{
	if( m_currentActivityCommand == START_ACTIVITY || m_currentActivityCommand == START_ACTIVITY_FOR_RESULT )
	{
		std::cout << "Creating new activity" << std::endl;
		Activity *newActivity = CreateActivity( m_startActivityName );
		if( newActivity != NULL )
		{
			m_activityStack.push_back( newActivity );
			newActivity->Create( this, m_activityStartData, m_currentActivityCommand == START_ACTIVITY_FOR_RESULT );
		}
		else
			std::cerr << "WARNING: Could not start new Activity named: " << m_startActivityName << std::endl;
		m_startActivityName.clear();
		delete m_activityStartData;
		m_activityStartData = NULL;
		Layout();
		m_currentActivityCommand = NONE;
	}
	else if( m_currentActivityCommand == STOP_ACTIVITY )
	{
		bool transferResult = false;
		
		if( !m_activityStack.empty() )
		{
			Activity *currentActivity = m_activityStack.back();
			if( currentActivity != NULL )
			{
				if( currentActivity->GetStartedForResult() )
				{
					transferResult = true;
				}
				currentActivity->Destroy();
			}
		}
		else
			std::cerr << "WARNING: GUIManager stopping Activity but none present" << std::endl;

		m_activityStack.pop_back();

		InputController *currentInputController = m_inputControllers.back();
		if( currentInputController != NULL )
			delete currentInputController;
		else
			std::cerr << "WARNING: possible synchronisation issue between Activity stack and InputController stack: stopping Activity but no associated InputController" << std::endl;
		m_inputControllers.pop_back();

		if( !m_activityStack.empty() )
		{
			if( transferResult )
			{
				Activity *currentActivity = m_activityStack.back();
				if( currentActivity != NULL )
				{
					currentActivity->ReturnActivityResult( m_activityResult );
					delete m_activityResult;
					m_activityResult = NULL;
				}
			}
		}
		else
		{
			// TODO: for the moment we just exit, it should be up to the main Activity to give a confirm dialog, if required
			// Though it would be nice to exit to some sort of 'desktop' activity perhaps...
			//_attachedWindow->SetExitEvent();
			m_exitEvent = true;
		}
		m_currentActivityCommand = NONE;

		Layout();
	}
	else if( m_currentActivityCommand == BOOTSTRAP_PROFILE_CHANGE )
	{
		BootstrapChangeProfile( m_bootstrapChangeProfileName );
		m_currentActivityCommand = NONE;
	}

	HandleEvents();

	if( !m_inputControllers.empty() )
	{
		InputController *currentInputController = m_inputControllers.back();
		if( currentInputController != NULL )
			currentInputController->Update( _deltaTs, GetCurrentActivity() );
		// It is perfectly valid here for there to be no currentInputController as the current Activity may just have exited
	//	else
	//		std::cerr<<"WARNING: possible synchronisation issue between Activity stack and InputController stack: updating Activity but no associated InputController"<<std::endl;
	}

	if( !m_activityStack.empty() )
	{
		Activity *currentActivity = m_activityStack.back();
		if( currentActivity != NULL )
		{
			// Update Activity
			currentActivity->Update( _deltaTs );
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIManager::Draw()
{
	if( !m_activityStack.empty() )
	{
		Activity *currentActivity = m_activityStack.back();
		if( currentActivity != NULL )
		{
			currentActivity->Draw();
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIManager::Layout()
{
	if( !m_activityStack.empty() )
	{
		Activity *currentActivity = m_activityStack.back();
		if( currentActivity != NULL )
		{
			currentActivity->Layout();
		}
	}
}

//----------------------------------------------------------------------------------

ShivaGUI::Activity* ShivaGUI::GUIManager::CreateActivity( std::string _name )
{
	std::cout << "Name of activity: "<< _name << std::endl;
	if( m_activityCreators.find( _name ) != m_activityCreators.end() )
	{
		Activity *currentActivity =  ( m_activityCreators[ _name ] )();

		if( currentActivity != NULL )
		{
			/*
			AudioController *audioController = m_audioManager->CreateAudioController();
			currentActivity->SetAudioController( audioController );
			audioController->LoadSettings( m_profileManager, m_defaultTheme );
			*/

			// Bind windows to current Activity, creating the Window -> InputController -> GUIController chains
			currentActivity->SetNumGUIControllers( m_windows.size() );
			unsigned int index = 0;
			for( std::vector< WindowGroup >::iterator it = m_windows.begin(); it != m_windows.end(); ++it )
			{
				currentActivity->AddGUIController( new GUIController( it->m_window, it->m_resourceManager, this ), index );
				++index;
			}
			
			// Create InputController for it
			m_inputControllers.push_back( new InputController( m_profileManager ) );
		}
		else
			std::cerr << "GUIManager::CreateActivity could not create activity named: " << _name << std::endl;

		// Return our new Activity
		return currentActivity;
	}
	else
		std::cerr << "WARNING: GUIManager could not find Activity to start named: " << _name << std::endl;
	return NULL;
}

//----------------------------------------------------------------------------------

ShivaGUI::ResourceManager* ShivaGUI::GUIManager::CreateResourceManager( unsigned int _windowIndex )
{
	ResourceManager *rman = new ResourceManager( this, _windowIndex );

	/// These are the default 'system' settings
	/// They will be overridden by what is in the profile
	/// We set them here in case the profile is missing these settings
	rman->SetDefaultFont( m_defaultFont );
	rman->SetTheme( m_defaultTheme );

	return rman;
}

//----------------------------------------------------------------------------------

void ShivaGUI::GUIManager::HandleEvents()
{
	if( m_inputControllers.empty() )
		return;	// We are probably in the process of exiting, so don't worry about events

	InputEvent *currentInputEvent = NULL;
	do
	{
		currentInputEvent = GetNextEvent();

		if( currentInputEvent != NULL && currentInputEvent->GetType() == InputEvent::QUIT )
			m_exitEvent = true; // We actually look for this all over the place, just to make sure it's caught

		// Ok, got our event, now what do we do with it??!
		// Give it to the InputController and this will deal with it
		// Anything simple, it will push through now
		// We will call Update on the controller later
		// Anything complex, it will push through later :)

		InputController *currentInputController = m_inputControllers.back();
		if( currentInputController != NULL ) {
			currentInputController->IssueEvent( currentInputEvent, GetCurrentActivity() );
		}
		else
			std::cerr << "WARNING: possible synchronisation issue between Activity stack and InputController stack: handling events for Activity but no associated InputController" << std::endl;

		delete currentInputEvent;
	}
	while( currentInputEvent != NULL );
}

//----------------------------------------------------------------------------------

ShivaGUI::InputEvent* ShivaGUI::GUIManager::GetNextEvent()
{
	SDL_Event currentSDLEvent;
	if( SDL_PollEvent( &currentSDLEvent ) == 0 )
		return NULL;

	InputEvent *currentEvent = new InputEvent();
	switch( currentSDLEvent.type )
	{
		case SDL_QUIT:
			currentEvent->SetQuitEvent();
			break;
		case SDL_KEYDOWN:
			std::cout << "Key pressed:" << currentSDLEvent.key.keysym.scancode << std::endl;
			currentEvent->SetKeyEvent( false, currentSDLEvent.key.keysym.scancode, GetWindowIDFromSDLID( currentSDLEvent.key.windowID ) );
			break;
		case SDL_KEYUP:
			currentEvent->SetKeyEvent( true, currentSDLEvent.key.keysym.scancode, GetWindowIDFromSDLID( currentSDLEvent.key.windowID ) );
			break;
		case SDL_MOUSEBUTTONDOWN:
			{
				currentEvent->SetMouseButtonEvent( false, currentSDLEvent.button.button, ( float )currentSDLEvent.button.x, ( float )currentSDLEvent.button.y, GetWindowIDFromSDLID( currentSDLEvent.button.windowID ) );
				break;
			}
		case SDL_MOUSEBUTTONUP:
			{
				currentEvent->SetMouseButtonEvent( true, currentSDLEvent.button.button, ( float )currentSDLEvent.button.x, ( float )currentSDLEvent.button.y, GetWindowIDFromSDLID( currentSDLEvent.button.windowID ) );
				break;
			}
		case SDL_MOUSEMOTION:
			{
				currentEvent->SetMouseMotionEvent( currentSDLEvent.button.x, currentSDLEvent.button.y, GetWindowIDFromSDLID( currentSDLEvent.motion.windowID ) );
				break;
			}
		case SDL_WINDOWEVENT:
			{
				switch( currentSDLEvent.window.event )
				{
					case SDL_WINDOWEVENT_RESIZED:
					{
						unsigned int width = currentSDLEvent.window.data1;
						unsigned int height = currentSDLEvent.window.data2;
						currentEvent->SetWindowSizeEvent( width, height, GetWindowIDFromSDLID( currentSDLEvent.window.windowID ) );
						break;
					}
					case SDL_WINDOWEVENT_CLOSE:
					{
						currentEvent->SetWindowCloseEvent( GetWindowIDFromSDLID( currentSDLEvent.window.windowID ) );
						break;
					}
				}
				break;
			}
	}

	return currentEvent;
}

//----------------------------------------------------------------------------------

int ShivaGUI::GUIManager::GetWindowIDFromSDLID( int _inputID )
{
	if( m_windowSDLIDs != NULL && _inputID > 0 )
	{
		for( unsigned int i = 0; i < m_windows.size(); i++ )
		{
			if( m_windowSDLIDs[ i ] == _inputID )
			{
				return i;
			}
		}
	}
	return -1;
}

//----------------------------------------------------------------------------------

