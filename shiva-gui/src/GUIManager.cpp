#include "GUIManager.h"
#include "System/Activities/ProfileChooserActivity.h"


#include <boost/algorithm/string.hpp>

ShivaGUI::GUIManager::GUIManager( std::string programName, std::string programOptionsDir )
{

	_activityResult = NULL;
	_activityStartData = NULL;
	_currentActivityCommand = NONE;
	_exitEvent = false;
	_windowSDLIDs = NULL;
	//_inputController = new InputController();
	_profileManager = new ProfileManager(programName, programOptionsDir);

	_audioManager = new AudioManager();

	_defaultFont = "Resources/Fonts/FreeSans.ttf";
	_defaultTheme = "Resources/Themes/system.xml";

	_lastTime = SDL_GetTicks();
	_deltaTs = 0.0f;
}

ShivaGUI::GUIManager::~GUIManager()
{
	while( !_windows.empty() )
	{
		WindowGroup currentWindowGroup = _windows.back();
		_windows.pop_back();
		delete currentWindowGroup._window;
		delete currentWindowGroup._resourceManager;
	}

	delete [] _windowSDLIDs;

	for( std::vector<InputController*>::iterator it = _inputControllers.begin(); it != _inputControllers.end(); ++it )
	{
		delete *it;
	}
	//delete _inputController;

	delete _profileManager;

	delete _audioManager;
}


void ShivaGUI::GUIManager::StartWithProfileChooser( std::string startActivity, std::string profileDir, std::string profileName )
{
	// Load system profile
	SetProfileDirectory("Profiles");
	LoadProfile("System");

	// Create window(s) from profile
	if( !CreateWindowsFromProfile() )
	{
		CreateWindow(640,480,100,100);
	}

	// Need to make sure activity creator is registered
	RegisterActivityCreator("ProfileChooserActivity",ProfileChooserActivity::Factory);

	Bundle *dataBundle = new Bundle();
	dataBundle->PutString("StartActivity",startActivity);
	if( !profileDir.empty() )
		dataBundle->PutString("ProfileDirectory",profileDir);
	if( !profileName.empty() )
		dataBundle->PutString("LoadProfile",profileName);

	StartActivity("ProfileChooserActivity",dataBundle);

	EnterMainLoop();

}

bool ShivaGUI::GUIManager::SetProfileDirectory( std::string profileDir )
{
	bool result = _profileManager->SetDirectory( profileDir );

	if( result )
	{
		_preferences = new SharedPreferences( profileDir + _programNameDir + "/prefs.xml");
	}

	return result;
}

std::string ShivaGUI::GUIManager::GetProfileOptionsDir()
{
	return _profileManager->GetCurrentOptionsDir();
}

std::string ShivaGUI::GUIManager::GetProfileDir()
{
	return _profileManager->GetDirectory();
}

std::string ShivaGUI::GUIManager::GetCurrentProfileFileName()
{
	return _profileManager->GetCurrentProfileFileName();
}

bool ShivaGUI::GUIManager::LoadProfile( std::string profileName )
{
	bool loadSuccess = _profileManager->Load( profileName );
	if( loadSuccess )
	{
		// Load some settings from the profile
		_defaultTheme = _profileManager->GetString( "Theme", _defaultTheme );
		_defaultFont  = _profileManager->GetString( "Font", _defaultFont );

		// TODO: load other settings
		// The difficulty here is that the other settings are needed for each Activity, so loading settings here makes no sense
	}
	return loadSuccess;
}

void ShivaGUI::GUIManager::RequestBootstrapChangeProfile( std::string profileName )
{
	_bootstrapChangeProfileName = profileName;

	if( _currentActivityCommand != NONE )
		std::cerr<<"WARNING: GUIManager issuing multiple Activity commands in one update cycle"<<std::endl;
	_currentActivityCommand = BOOTSTRAP_PROFILE_CHANGE;
}


bool ShivaGUI::GUIManager::BootstrapChangeProfile( std::string profileName )
{
	if( LoadProfile(profileName) )
	{
		std::cout<<"INFO: GUIManager attempting to pull itself out of the mud by its own bootstraps..."<<std::endl;
		// Delete all windows
		while( !_windows.empty() )
		{
			WindowGroup currentWindowGroup = _windows.back();
			_windows.pop_back();
			delete currentWindowGroup._window;
			delete currentWindowGroup._resourceManager;
		}
		_windows.clear();
		delete [] _windowSDLIDs;
		_windowSDLIDs = NULL;

		// Recreate all windows
		CreateWindowsFromProfile();


		// Activities store handles to windows, so these need to all have new GUIControllers

		for( std::list<Activity*>::iterator currentActivityIt = _activityStack.begin(); currentActivityIt != _activityStack.end(); ++currentActivityIt )
		{
			Activity *currentActivity =  *currentActivityIt;

			if( currentActivity != NULL )
			{
				// This will also delete the existing controllers
				currentActivity->SetNumGUIControllers( _windows.size() );

				// Bind windows to current Activity, creating the Window -> InputController -> GUIController chains
				unsigned int index = 0;
				for( std::vector<WindowGroup>::iterator it = _windows.begin(); it != _windows.end(); ++it )
				{
					currentActivity->AddGUIController( new GUIController( it->_window, it->_resourceManager, this), index );
					++index;
				}

				currentActivity->ConfigurationChanged();
			}
		}

		std::cout<<"INFO: GUIManager hopefully out the mud now"<<std::endl;
		return true;
	}

	std::cerr<<"ERROR: GUIManager::BootstrapChangeProfile failed on LoadProfile(), results undefined"<<std::endl;
	return false;
}

void ShivaGUI::GUIManager::SaveLayoutToProfile( std::string inputFilename, View *rootNode, ResourceManager *resources )
{
	if( inputFilename.empty() || rootNode == NULL || resources == NULL )
		return;

	// figure out where to save it
	// Strip any leading directories from the inputFilename:
	boost::filesystem::path pathFilename(inputFilename);
	boost::filesystem::path actualFilename = pathFilename.filename();

	boost::filesystem::path fullfilename( _profileManager->GetCurrentOptionsDir() + "/Layout/" + actualFilename.string() );
	// If the directories don't exist, we need to create them:
	boost::filesystem::create_directories( _profileManager->GetCurrentOptionsDir() + "/Layout/" );

	if( resources->OutputLayout(fullfilename.string(),rootNode) )
		std::cout<<"INFO: Layout Saved to Profile: filename = "<< fullfilename<<std::endl;
	else
		std::cout<<"WARN: Layout Failed to Save to Profile: filename = "<< fullfilename<<std::endl;
}

bool ShivaGUI::GUIManager::CreateWindowsFromProfile()
{
	// See how many windows the profile says we need to create
	int numWindows = _profileManager->ContainsOption( "window" );
	if( numWindows <= 0 || numWindows > 10 )
		return false;

	for( int i = 0; i < numWindows; i++ )
	{
		// The options for our window are stored in a 'window' node in the profile
		if( _profileManager->EnterOptionNode( "window", i ) )
		{
			// We can now query our window settings
			unsigned int width = _profileManager->GetInt( "width", 640 );
			unsigned int height = _profileManager->GetInt( "height", 480 );
			int positionX = _profileManager->GetInt( "positionX", 50 );
			int positionY = _profileManager->GetInt( "positionY", 50 );
			std::string fontName = _profileManager->GetString( "font", _defaultFont );
			std::string themeName = _profileManager->GetString( "theme", _defaultTheme );
			std::string windowUse = _profileManager->GetString( "use", "ANYTHING" );

			// Create our window
			CreateWindow(width,height,positionX,positionY);


			// Apply settings to the window's resource manager
			_windows.back()._resourceManager->SetDefaultFont( fontName );
			_windows.back()._resourceManager->SetTheme( themeName );

			_windows.back()._window->SetRequestedUse(windowUse);


			// Need to allow the Resource Manager to load const options from the Profile
			// Exit the Window option node and pass the profile manager to the window's resource manager
			_profileManager->ExitOptionNode();
			_windows.back()._resourceManager->LoadProfileAttributeConsts(_profileManager);

		}
		_profileManager->ExitOptionNode();
	}

	return true;
}





void ShivaGUI::GUIManager::CreateWindow(unsigned int width, unsigned int height, int positionX, int positionY)
{
	unsigned int newWindowIndex = _windows.size();
	_windows.push_back( WindowGroup(new Window(positionX,positionY,width,height), CreateResourceManager(newWindowIndex) ) );

	delete [] _windowSDLIDs;

	_windowSDLIDs = new int[_windows.size()];
	for( unsigned int i = 0; i < _windows.size(); i++ )
	{
		_windowSDLIDs[i] = _windows.at(i)._window->GetSDLWindowID();
	}
}

ShivaGUI::SharedPreferences* ShivaGUI::GUIManager::GetProgSpecificOptions()
{
	return _profileManager->GetProgSpecificOptions();
}



void ShivaGUI::GUIManager::RegisterActivityCreator(std::string activityName, Activity* (*func)() )
{
	_activityCreators[activityName] = func;
}


void ShivaGUI::GUIManager::StartActivity(std::string activityName, Bundle *data)
{
	_startActivityName = activityName;
	_activityStartData = data;
	if( _currentActivityCommand != NONE )
		std::cerr<<"WARNING: GUIManager issuing multiple Activity commands in one update cycle"<<std::endl;
	_currentActivityCommand = START_ACTIVITY;
}

void ShivaGUI::GUIManager::StartActivityForResult(std::string activityName, Bundle *data)
{
	_startActivityName = activityName;
	_activityStartData = data;
	if( _currentActivityCommand != NONE )
		std::cerr<<"WARNING: GUIManager issuing multiple Activity commands in one update cycle"<<std::endl;
	_currentActivityCommand = START_ACTIVITY_FOR_RESULT;
}

void ShivaGUI::GUIManager::SetActivityResult(Bundle *dataIn)
{
	_activityResult = dataIn;
}

void ShivaGUI::GUIManager::FinishActivity()
{
	if( _currentActivityCommand != NONE )
		std::cerr<<"WARNING: GUIManager issuing multiple Activity commands in one update cycle"<<std::endl;
	_currentActivityCommand = STOP_ACTIVITY;
}

ShivaGUI::Activity* ShivaGUI::GUIManager::GetCurrentActivity()
{
	if( !_activityStack.empty() )
		return _activityStack.back();
	return NULL;
}

void ShivaGUI::GUIManager::RegisterViewCreator(std::string viewName, View* (*func)() )
{
	_viewCreators[viewName] = func;
}

ShivaGUI::View* ShivaGUI::GUIManager::CreateView(std::string name)
{
	if( _viewCreators.find(name) != _viewCreators.end() )
	{
		return (_viewCreators[name])();
	}
	return NULL;
}


void ShivaGUI::GUIManager::EnterMainLoop()
{
	do
	{
		unsigned int current = SDL_GetTicks();
		_deltaTs = (float) (current - _lastTime) / 1000.0f;
		_lastTime = current;

		if( _deltaTs > 1.0f )
			_deltaTs = 1.0f;
		else if( _deltaTs < 0.00001f )
			_deltaTs = 0.00001f;

		Update(_deltaTs);

		Draw();

		// Limiter in case we're running really quick
		if( _deltaTs < (1.0f/50.0f) )	// not sure how accurate the SDL_Delay function is..
			SDL_Delay((unsigned int) (((1.0f/50.0f) - _deltaTs)*1000.0f) );
	}
	while( !_exitEvent );
}


void ShivaGUI::GUIManager::Update(float deltaTs)
{
	if( _currentActivityCommand == START_ACTIVITY || _currentActivityCommand == START_ACTIVITY_FOR_RESULT )
	{
		std::cout<<"Creating new activity"<<std::endl;
		Activity *newActivity = CreateActivity(_startActivityName);
		if( newActivity != NULL )
		{
			_activityStack.push_back(newActivity);
			newActivity->Create(this, _activityStartData, _currentActivityCommand == START_ACTIVITY_FOR_RESULT );
		}
		else
			std::cerr<<"WARNING: Could not start new Activity named: "<<_startActivityName<<std::endl;
		_startActivityName.clear();
		delete _activityStartData;
		_activityStartData = NULL;
		Layout();
		_currentActivityCommand = NONE;
	}
	else if( _currentActivityCommand == STOP_ACTIVITY )
	{
		bool transferResult = false;

		if( !_activityStack.empty() )
		{
			Activity *currentActivity = _activityStack.back();
			if( currentActivity != NULL )
			{
				if( currentActivity->GetStartedForResult() )
				{
					transferResult = true;
				}
				currentActivity->Destroy();
				delete currentActivity;
			}
		}
		else
			std::cerr<<"WARNING: GUIManager stopping Activity but none present"<<std::endl;

		_activityStack.pop_back();

		InputController *currentInputController = _inputControllers.back();
		if( currentInputController != NULL )
			delete currentInputController;
		else
			std::cerr<<"WARNING: possible synchronisation issue between Activity stack and InputController stack: stopping Activity but no associated InputController"<<std::endl;
		_inputControllers.pop_back();

		if( !_activityStack.empty() )
		{
			if( transferResult )
			{
				Activity *currentActivity = _activityStack.back();
				if( currentActivity != NULL )
				{
					currentActivity->ReturnActivityResult( _activityResult );
					delete _activityResult;
					_activityResult = NULL;
				}
			}
		}
		else
		{
			// TODO: for the moment we just exit, it should be up to the main Activity to give a confirm dialog, if required
			// Though it would be nice to exit to some sort of 'desktop' activity perhaps...
			//_attachedWindow->SetExitEvent();
			_exitEvent = true;
		}
		_currentActivityCommand = NONE;

		Layout();
	}
	else if( _currentActivityCommand == BOOTSTRAP_PROFILE_CHANGE )
	{
		BootstrapChangeProfile(_bootstrapChangeProfileName);
		_currentActivityCommand = NONE;
	}

	HandleEvents();

	if( !_inputControllers.empty() )
	{
		InputController *currentInputController = _inputControllers.back();
		if( currentInputController != NULL )
			currentInputController->Update(deltaTs, GetCurrentActivity() );
		// It is perfectly valid here for there to be no currentInputController as the current Activity may just have exited
	//	else
	//		std::cerr<<"WARNING: possible synchronisation issue between Activity stack and InputController stack: updating Activity but no associated InputController"<<std::endl;
	}

	if( !_activityStack.empty() )
	{
		Activity *currentActivity = _activityStack.back();
		if( currentActivity != NULL )
		{
			// Update Activity
			currentActivity->Update(deltaTs);
		}
	}
}

void ShivaGUI::GUIManager::Draw()
{
	if( !_activityStack.empty() )
	{
		Activity *currentActivity = _activityStack.back();
		if( currentActivity != NULL )
		{
			currentActivity->Draw();
		}
	}
}

void ShivaGUI::GUIManager::Layout()
{
	if( !_activityStack.empty() )
	{
		Activity *currentActivity = _activityStack.back();
		if( currentActivity != NULL )
		{
			currentActivity->Layout();
		}
	}
}


ShivaGUI::Activity* ShivaGUI::GUIManager::CreateActivity(std::string name)
{
	std::cout<<"Name of activity: "<< name << std::endl;
	if( _activityCreators.find(name) != _activityCreators.end() )
	{
		Activity *currentActivity =  (_activityCreators[name])();

		if( currentActivity != NULL )
		{
			/*
			AudioController *audioController = _audioManager->CreateAudioController();
			currentActivity->SetAudioController( audioController );
			audioController->LoadSettings( _profileManager, _defaultTheme );
			*/

			// Bind windows to current Activity, creating the Window -> InputController -> GUIController chains
			currentActivity->SetNumGUIControllers( _windows.size() );
			unsigned int index = 0;
			for( std::vector<WindowGroup>::iterator it = _windows.begin(); it != _windows.end(); ++it )
			{
				currentActivity->AddGUIController( new GUIController( it->_window, it->_resourceManager, this), index );
				++index;
			}
			
			// Create InputController for it
			_inputControllers.push_back(new InputController(_profileManager) );
		}
		else
			std::cerr<<"GUIManager::CreateActivity could not create activity named: "<<name<<std::endl;

		// Return our new Activity
		return currentActivity;
	}
	else
		std::cerr<<"WARNING: GUIManager could not find Activity to start named: "<<name<<std::endl;
	return NULL;
}

ShivaGUI::ResourceManager* ShivaGUI::GUIManager::CreateResourceManager(unsigned int windowIndex)
{
	ResourceManager *rman = new ResourceManager(this, windowIndex);

	/// These are the default 'system' settings
	/// They will be overridden by what is in the profile
	/// We set them here in case the profile is missing these settings
	rman->SetDefaultFont(_defaultFont);
	rman->SetTheme(_defaultTheme);

	return rman;
}


void ShivaGUI::GUIManager::HandleEvents()
{
	if( _inputControllers.empty() )
		return;	// We are probably in the process of exiting, so don't worry about events

	InputEvent *currentInputEvent = NULL;
	do
	{
		currentInputEvent = GetNextEvent();

		if( currentInputEvent != NULL && currentInputEvent->GetType() == InputEvent::QUIT )
			_exitEvent = true; // We actually look for this all over the place, just to make sure it's caught

		// Ok, got our event, now what do we do with it??!
		// Give it to the InputController and this will deal with it
		// Anything simple, it will push through now
		// We will call Update on the controller later
		// Anything complex, it will push through later :)


		InputController *currentInputController = _inputControllers.back();
		if( currentInputController != NULL ) {
			currentInputController->IssueEvent(currentInputEvent, GetCurrentActivity() );
		}
		else
			std::cerr<<"WARNING: possible synchronisation issue between Activity stack and InputController stack: handling events for Activity but no associated InputController"<<std::endl;

		delete currentInputEvent;
	}
	while( currentInputEvent != NULL );
}

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
			std::cout<<"key pressed:"<<currentSDLEvent.key.keysym.scancode<<std::endl;
			currentEvent->SetKeyEvent(false,currentSDLEvent.key.keysym.scancode, GetWindowIDFromSDLID(currentSDLEvent.key.windowID) );
			break;
		case SDL_KEYUP:
			currentEvent->SetKeyEvent(true,currentSDLEvent.key.keysym.scancode, GetWindowIDFromSDLID(currentSDLEvent.key.windowID) );
			break;
		case SDL_MOUSEBUTTONDOWN:
			{
				currentEvent->SetMouseButtonEvent(false,currentSDLEvent.button.button,currentSDLEvent.button.x,currentSDLEvent.button.y, GetWindowIDFromSDLID(currentSDLEvent.button.windowID) );
				break;
			}
		case SDL_MOUSEBUTTONUP:
			{
				currentEvent->SetMouseButtonEvent(true,currentSDLEvent.button.button,currentSDLEvent.button.x,currentSDLEvent.button.y, GetWindowIDFromSDLID(currentSDLEvent.button.windowID) );
				break;
			}
		case SDL_MOUSEMOTION:
			{
				currentEvent->SetMouseMotionEvent(currentSDLEvent.button.x,currentSDLEvent.button.y, GetWindowIDFromSDLID(currentSDLEvent.motion.windowID) );
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
						currentEvent->SetWindowSizeEvent(width,height, GetWindowIDFromSDLID(currentSDLEvent.window.windowID) );
						break;
					}
					case SDL_WINDOWEVENT_CLOSE:
					{
						currentEvent->SetWindowCloseEvent( GetWindowIDFromSDLID(currentSDLEvent.window.windowID) );
						break;
					}
				}
				break;
			}
	}

	return currentEvent;
}

int ShivaGUI::GUIManager::GetWindowIDFromSDLID(int inputID)
{
	if( _windowSDLIDs != NULL && inputID > 0 )
	{
		for( unsigned int i = 0; i < _windows.size(); i++ )
		{
			if( _windowSDLIDs[i] == inputID )
			{
				return i;
			}
		}
	}
	return -1;
}


