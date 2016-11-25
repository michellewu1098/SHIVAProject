
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUI_GUI_GUIMANAGER__
#define __SHIVA_GUI_GUI_GUIMANAGER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>
#include <list>
#include <map>

//////////////////////////////////////////////////////////////////////////
#include "System/Activity.h"
#include "System/ProfileManager.h"
#include "Input/InputController.h"
#include "Audio/AudioManager.h"

#ifdef WIN32
	#undef CreateWindow
#endif

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	class GUIManager
	{
	public:

		// Initialisation

			/// \param programName The name of your program, to be used for user profile authorship - human readable (i.e. can be long and include spaces etc)
			/// \param programProfileDir The directory to use for storing program-specific data within the current profile
			GUIManager( std::string programName, std::string programOptionsDir );
			~GUIManager();

			/// This is an 'easy start' function that handles lots of things that you could do yourself
			/// It will load the system profile and enters the main event loop
			/// This means that the function will only return once the program is finished
			/// Then it display a 'profile manager' window for the user to choose and manage profiles
			/// Once the user has chosen a profile, this is loaded and the next Activity is started
			/// You can also supply a profile directory and profile name to go straight to starting the next Activity
			void StartWithProfileChooser( std::string startActivity, std::string profileDir, std::string profileName );

			/// The ProfileManager is to be used with care
			/// Any functions that have equivalents within the GUIManager should be preferred
			///  - there are several cases where the GUIManager needs to do things too
			ProfileManager* GetProfileManager() {return _profileManager;}

			/// The AudioManager is to be used with care
			/// You should use your Activity's AudioController for accessing audio functions
			/// This should be accessible through your Activity's GUIController
			AudioManager* GetAudioManager() {return _audioManager;}

			/// For manually setting the profile directory
			/// Returns false if directory does not exist
			bool SetProfileDirectory( std::string profileDir );

			/// Retrieves the options directory for the current profile
			/// This is where program-specific data should be stored
			std::string GetProfileOptionsDir();

			std::string GetProfileDir();

			std::string GetCurrentProfileFileName();

			/// For manually loading a user profile
			/// This would normally be used if you don't want to use the profile chooser dialog
			/// Returns false on error
			bool LoadProfile( std::string profileName );

			/// This is for changing a profile while Activities are already running
			/// This function will close all current windows and reload the window layout from the new profile
			void RequestBootstrapChangeProfile( std::string profileName );

			/// For saving a layout to the current user profile's directory
			void SaveLayoutToProfile( std::string filename, View *rootNode, ResourceManager *resources );

			/// The user profile can specify a set of Windows
			/// this function creates windows according to these requirements.
			/// A profile must have been loaded before using this function
			/// If the profile does not specify any windows for creation
			/// it does not create any windows and returns false to allow the program to specify a default setup.
			/// Similarly, there is a cap at 10 windows, above which no windows are created and the function returns false.
			/// If windows are created, the Activity should query the windows for hints on how they should be used
			/// the intention here is that different layouts could be loaded
			bool CreateWindowsFromProfile();

			/// This is the API method that must be used to directly create a Window
			void CreateWindow( unsigned int width = 640, unsigned int height = 480, int positionX = 50, int positionY = 50 );


			/// These are non-volatile options that are accessible across Activities
			/// They are profile- and program-specific
			/// You must have set the profile directory before this can be used
			SharedPreferences* GetProgSpecificOptions();


		// Activities

			void RegisterActivityCreator(std::string activityName, Activity* (*)() );
			void StartActivity(std::string activityName, Bundle *data = NULL);
			void StartActivityForResult(std::string activityName, Bundle *data = NULL);
			/// Sets the result data for the current Activity
			void SetActivityResult(Bundle *);
			/// Requests that the current activity is stopped
			void FinishActivity();

			Activity* GetCurrentActivity();

			/// For allowing user-defined Views to be created through xml Layouts
			void RegisterViewCreator(std::string viewName, View* (*)() );
			/// For creating a custom View
			/// Will return NULL if View is unknown
			View* CreateView(std::string name);

			bool CommandQueueEmpty() {return _currentActivityCommand == NONE;}

		// Running

			/// Main loop entry point, exits when program needs to exit
			/// You can interrupt this with SetExitEvent()
			void EnterMainLoop();

			void Layout();

			void Update(float deltaTs);

			void Draw();

			void SetExitEvent() {_exitEvent=true;}
			/// \return true if an exit event has been generated at some point and the program should exit
			bool GetExitEvent() {return _exitEvent;}

	protected:

		// Settings

			/// For user profiles
			ProfileManager *_profileManager;

			AudioManager *_audioManager;

			/// Internal class for storing information related to a Window
			class WindowGroup
			{
			public:
				WindowGroup(Window *win, ResourceManager *rman) {_window = win; _resourceManager = rman;}
				Window *_window;
				ResourceManager *_resourceManager;
			};

			std::vector<WindowGroup> _windows;

			/// The internal SDL handles for the windows
			int *_windowSDLIDs;

			/// Default font to use when creating resource managers
			std::string _defaultFont;

			/// Default theme to use when creating resource managers
			std::string _defaultTheme;

			std::string _programNameDir;

			/// Used for storing program-specific options
			SharedPreferences *_preferences;

		// Events

			/// Each Activity must have its own input controller
			/// This is because the controller stores state that is associated with the Activity
			/// If we just use one controller it gets confused when you push / pop Activities
			std::vector<InputController*> _inputControllers;
			//InputController *_inputController;

			/// Is set to true when we are supposed to exit
			bool _exitEvent;

			// Timing variables
			unsigned int _lastTime;
			float _deltaTs;


		// Activities

			enum ActivityCommand
			{
				NONE,
				START_ACTIVITY,
				START_ACTIVITY_FOR_RESULT,
				STOP_ACTIVITY,
				BOOTSTRAP_PROFILE_CHANGE
			};

			ActivityCommand _currentActivityCommand;

			std::map<std::string,Activity* (*)()> _activityCreators;

			Bundle *_activityResult;

			/// When an activity is started, you can send it a data Bundle. This stores the Bundle
			Bundle *_activityStartData;

			std::list<Activity*> _activityStack;

			/// When a request is made to start a new activity, this string holds its name
			std::string _startActivityName;

			/// Main factory method for creating an activity by name
			/// Creates the thing, adds GUIController and InputController
			Activity* CreateActivity(std::string name);

			ResourceManager* CreateResourceManager(unsigned int windowIndex);

			/// Queries the event queue and deals with events
			void HandleEvents();

			InputEvent* GetNextEvent();
			int GetWindowIDFromSDLID(int);

			std::map<std::string,View* (*)()> _viewCreators;

			/// Name of profile to change to
			std::string _bootstrapChangeProfileName;

			bool BootstrapChangeProfile( std::string profileName );

	};
}

//////////////////////////////////////////////////////////////////////////
#endif
