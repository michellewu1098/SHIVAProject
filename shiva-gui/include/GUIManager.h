///-----------------------------------------------------------------------------------------------
/// \file GUIManager.h
/// \brief Manages Drawables, expands them from xml, loads bitmaps from file into OpenGL, converts text into a BitmapDrawable
/// and handles the sharing of OpenGL resources across Views and across Activities. 
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_GUI_GUIMANAGER__
#define __SHIVA_GUI_GUI_GUIMANAGER__

#include <string>
#include <vector>
#include <list>
#include <map>
#include <boost/algorithm/string.hpp>
#include <SDL.h>

#include "System/Activity.h"
#include "System/ProfileManager.h"
#include "Input/InputController.h"
#include "Audio/AudioManager.h"

#ifdef WIN32
	#undef CreateWindow
#endif

namespace ShivaGUI
{

	class GUIManager
	{
	public:

		
		// INITIALISATION
		
		//----------------------------------------------------------------------------------
		/// \brief Ctor
		/// \param [in] _programName The name of your program, to be used for user profile authorship - human readable (i.e. can be long and include spaces etc)
		/// \param [in] _programProfileDir The directory to use for storing program-specific data within the current profile
		//----------------------------------------------------------------------------------
		GUIManager( std::string _programName, std::string _programOptionsDir );
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~GUIManager();
		//----------------------------------------------------------------------------------
		/// \brief This is an 'easy start' function that handles lots of things that you could do yourself
		/// It will load the system profile and enters the main event loop
		/// This means that the function will only return once the program is finished
		/// Then it display a 'profile manager' window for the user to choose and manage profiles
		/// Once the user has chosen a profile, this is loaded and the next Activity is started
		/// You can also supply a profile directory and profile name to go straight to starting the next Activity
		/// \param [in] _startActivity Name of activity
		/// \param [in] _profileDir Directory of profile
		/// \param [in] _profileName Name of profile
		//----------------------------------------------------------------------------------
		void StartWithProfileChooser( std::string _startActivity, std::string _profileDir, std::string _profileName );
		//----------------------------------------------------------------------------------
		/// \brief The ProfileManager is to be used with care
		/// Any functions that have equivalents within the GUIManager should be preferred
		///  - there are several cases where the GUIManager needs to do things too
		/// \return m_profileManager
		//----------------------------------------------------------------------------------
		ProfileManager* GetProfileManager() { return m_profileManager; }
		//----------------------------------------------------------------------------------
		/// \brief The AudioManager is to be used with care
		/// You should use your Activity's AudioController for accessing audio functions
		/// This should be accessible through your Activity's GUIController
		/// \return m_audioManager
		//----------------------------------------------------------------------------------
		AudioManager* GetAudioManager() { return m_audioManager; }
		//----------------------------------------------------------------------------------
		/// \brief For manually setting the profile directory
		/// \param [in] _profileDir
	    /// \return false if directory does not exist
		//----------------------------------------------------------------------------------
		bool SetProfileDirectory( std::string _profileDir );
		//----------------------------------------------------------------------------------
		/// \brief Retrieves the options directory for the current profile. This is where program-specific data should be stored
		//----------------------------------------------------------------------------------
		std::string GetProfileOptionsDir();
		//----------------------------------------------------------------------------------
		/// \brief Retrieves profile directory
		//----------------------------------------------------------------------------------
		std::string GetProfileDir();
		//----------------------------------------------------------------------------------
		/// \brief Retrieves current profile file name
		//----------------------------------------------------------------------------------
		std::string GetCurrentProfileFileName();
		//----------------------------------------------------------------------------------
		/// \brief For manually loading a user profile
		/// This would normally be used if you don't want to use the profile chooser dialog
		/// \param [in] _profileName
		/// \return false on error
		//----------------------------------------------------------------------------------
		bool LoadProfile( std::string _profileName );
		//----------------------------------------------------------------------------------
		/// \brief This is for changing a profile while Activities are already running
		/// This function will close all current windows and reload the window layout from the new profile
		/// \param [in] profileName
		//----------------------------------------------------------------------------------
		void RequestBootstrapChangeProfile( std::string _profileName );
		//----------------------------------------------------------------------------------
		/// \brief For saving a layout to the current user profile's directory
		/// \param [in] filename
		/// \param [in] rootNode
		/// \param [in] resources
		//----------------------------------------------------------------------------------
		void SaveLayoutToProfile( std::string _filename, View* _rootNode, ResourceManager* _resources );
		//----------------------------------------------------------------------------------
		/// \brief The user profile can specify a set of Windows
		/// this function creates windows according to these requirements.
		/// A profile must have been loaded before using this function
		/// If the profile does not specify any windows for creation
		/// it does not create any windows and returns false to allow the program to specify a default setup. 
		/// Similarly, there is a cap at 10 windows, above which no windows are created and the function returns false.
		/// If windows are created, the Activity should query the windows for hints on how they should be used
		/// the intention here is that different layouts could be loaded
		//----------------------------------------------------------------------------------
		bool CreateWindowsFromProfile();
		//----------------------------------------------------------------------------------
		/// \brief This is the API method that must be used to directly create a Window
		//----------------------------------------------------------------------------------
		void CreateWindow( unsigned int _width = 640, unsigned int _height = 480, int _positionX = 50, int _positionY = 50 );
		//----------------------------------------------------------------------------------
		/// These are non-volatile options that are accessible across Activities
		/// They are profile- and program-specific
		/// You must have set the profile directory before this can be used
		//----------------------------------------------------------------------------------
		SharedPreferences* GetProgSpecificOptions();
		//----------------------------------------------------------------------------------
		
		// ACTIVITIES
		
		//----------------------------------------------------------------------------------
		/// \brief Register an activity
		/// \param [in] _activityName
		/// \param [in] func
		//----------------------------------------------------------------------------------
		void RegisterActivityCreator( std::string _activityName, Activity* (*)() );
		//----------------------------------------------------------------------------------
		/// \brief Start activity
		/// \param [in] _activityName
		/// \param [in] _data
		//----------------------------------------------------------------------------------
		void StartActivity( std::string _activityName, Bundle* _data = NULL );
		//----------------------------------------------------------------------------------
		/// \brief Start activity
		/// \param [in] _activityName
		/// \param [in] _data
		//----------------------------------------------------------------------------------
		void StartActivityForResult( std::string _activityName, Bundle* _data = NULL );
		//----------------------------------------------------------------------------------
		/// \brief Sets the result data for the current Activity
		/// \param [in] _dataIna
		//----------------------------------------------------------------------------------
		void SetActivityResult(Bundle* _dataIn );
		//----------------------------------------------------------------------------------
		/// \brief Requests that the current activity is stopped
		//----------------------------------------------------------------------------------
		void FinishActivity();
		//----------------------------------------------------------------------------------
		/// \brief Get current activity
		//----------------------------------------------------------------------------------
		Activity* GetCurrentActivity();
		//----------------------------------------------------------------------------------
		/// \brief For allowing user-defined Views to be created through xml Layouts
		/// \param [in] _viewName
		/// \param [in] func
		//----------------------------------------------------------------------------------
		void RegisterViewCreator( std::string _viewName, View* (*)() );
		//----------------------------------------------------------------------------------
		/// \brief For creating a custom View
		/// Will return NULL if View is unknown
		//----------------------------------------------------------------------------------
		View* CreateView( std::string _name );
		//----------------------------------------------------------------------------------
		/// \brief Return if current activity command queue is empty
		//----------------------------------------------------------------------------------
		bool CommandQueueEmpty() { return m_currentActivityCommand == NONE; }
		//----------------------------------------------------------------------------------
		
		// RUNNING
		
		//----------------------------------------------------------------------------------
		/// \brief Main loop entry point, exits when program needs to exit
		/// You can interrupt this with SetExitEvent()
		//----------------------------------------------------------------------------------
		void EnterMainLoop();
		//----------------------------------------------------------------------------------
		/// \brief Set layout
		//----------------------------------------------------------------------------------
		void Layout();
		//----------------------------------------------------------------------------------
		/// \brief Update method
		/// \param [in] _deltaTs Timestep
		//----------------------------------------------------------------------------------
		void Update( float _deltaTs );
		//----------------------------------------------------------------------------------
		/// \brief Draw method
		//----------------------------------------------------------------------------------
		void Draw();
		//----------------------------------------------------------------------------------
		/// \brief Set exit event to true to stop main loop
		//----------------------------------------------------------------------------------
		void SetExitEvent() { m_exitEvent = true; }
		//----------------------------------------------------------------------------------
		/// \brief Get value of exitEvent variable
		/// \return true if an exit event has been generated at some point and the program should exit
		//----------------------------------------------------------------------------------
		bool GetExitEvent() const { return m_exitEvent; }
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		// SETTINGS
		//----------------------------------------------------------------------------------
		/// \brief Profile manager for user profiles
		//----------------------------------------------------------------------------------
		ProfileManager *m_profileManager;
		//----------------------------------------------------------------------------------
		/// \bried Reference to audio manager
		//----------------------------------------------------------------------------------
		AudioManager *m_audioManager;
		//----------------------------------------------------------------------------------
		/// \brief Internal class for storing information related to a Window
		//----------------------------------------------------------------------------------
		class WindowGroup
		{
		public:

			//----------------------------------------------------------------------------------
			/// \brief Ctor
			/// \param [in] _win Window
			/// \parma [in] _rman Resource manager
			//----------------------------------------------------------------------------------
			WindowGroup( Window* _win, ResourceManager* _rman ) { m_window = _win; m_resourceManager = _rman; }
			//----------------------------------------------------------------------------------
			/// \brief Window
			//----------------------------------------------------------------------------------
			Window *m_window;
			//----------------------------------------------------------------------------------
			/// \brief Resource manager
			//----------------------------------------------------------------------------------
			ResourceManager *m_resourceManager;
			//----------------------------------------------------------------------------------
		};
		//----------------------------------------------------------------------------------
		/// \brief List of windows
		//----------------------------------------------------------------------------------
		std::vector< WindowGroup > m_windows;
		//----------------------------------------------------------------------------------
		/// \brief The internal SDL handles for the windows
		//----------------------------------------------------------------------------------
		int* m_windowSDLIDs;
		//----------------------------------------------------------------------------------
		/// \brief Default font to use when creating resource managers
		//----------------------------------------------------------------------------------
		std::string m_defaultFont;
		//----------------------------------------------------------------------------------
		/// \brief Default theme to use when creating resource managers
		//----------------------------------------------------------------------------------
		std::string m_defaultTheme;
		//----------------------------------------------------------------------------------
		/// \brief Profile directory
		//----------------------------------------------------------------------------------
		std::string m_programNameDir;
		//----------------------------------------------------------------------------------
		/// \brief Used for storing program-specific options
		//----------------------------------------------------------------------------------
		SharedPreferences *m_preferences;
		//----------------------------------------------------------------------------------
		// EVENTS
		//----------------------------------------------------------------------------------
		/// \brief Each Activity must have its own input controller
		/// This is because the controller stores state that is associated with the Activity
		/// If we just use one controller it gets confused when you push / pop Activities
		//----------------------------------------------------------------------------------
		std::vector< InputController* > m_inputControllers;
		//----------------------------------------------------------------------------------
		/// \brief Is set to true when we are supposed to exit
		//----------------------------------------------------------------------------------
		bool m_exitEvent;
		//----------------------------------------------------------------------------------
		// Timing variables
		//----------------------------------------------------------------------------------
		unsigned int m_lastTime;
		//----------------------------------------------------------------------------------
		/// \brief Timestep
		//----------------------------------------------------------------------------------
		float m_deltaTs;
		//----------------------------------------------------------------------------------
		// ACTIVITIES
		//----------------------------------------------------------------------------------
		/// \brief Activity commands
		//----------------------------------------------------------------------------------
		enum ActivityCommand
		{
			NONE,
			START_ACTIVITY,
			START_ACTIVITY_FOR_RESULT,
			STOP_ACTIVITY,
			BOOTSTRAP_PROFILE_CHANGE
		};
		//----------------------------------------------------------------------------------
		/// \brief Current activity command
		//----------------------------------------------------------------------------------
		ActivityCommand m_currentActivityCommand;
		//----------------------------------------------------------------------------------
		/// \brief List of activity creators
		//----------------------------------------------------------------------------------
		std::map< std::string, Activity* (*)() > m_activityCreators;
		//----------------------------------------------------------------------------------
		/// \brief Activity result data
		//----------------------------------------------------------------------------------
		Bundle* m_activityResult;
		//----------------------------------------------------------------------------------
		/// \brief When an activity is started, you can send it a data Bundle. This stores the Bundle
		//----------------------------------------------------------------------------------
		Bundle* m_activityStartData;
		//----------------------------------------------------------------------------------
		/// \brief Activity stack
		//----------------------------------------------------------------------------------
		std::list< Activity* > m_activityStack;
		//----------------------------------------------------------------------------------
		/// \brief When a request is made to start a new activity, this string holds its name
		//----------------------------------------------------------------------------------
		std::string m_startActivityName;
		//----------------------------------------------------------------------------------
		/// \brief Main factory method for creating an activity by name
		/// Creates the thing, adds GUIController and InputController
		//----------------------------------------------------------------------------------
		Activity* CreateActivity( std::string _name );
		//----------------------------------------------------------------------------------
		/// \brief Create resource manager
		/// \param [in] _windowIndex
		//----------------------------------------------------------------------------------
		ResourceManager* CreateResourceManager( unsigned int _windowIndex );
		//----------------------------------------------------------------------------------
		/// \brief Queries the event queue and deals with events
		//----------------------------------------------------------------------------------
		void HandleEvents();
		//----------------------------------------------------------------------------------
		/// \brief Get next event
		//----------------------------------------------------------------------------------
		InputEvent* GetNextEvent();
		//----------------------------------------------------------------------------------
		/// \brief Get window id provided SDL id
		/// \param [in] _inputID
		//----------------------------------------------------------------------------------
		int GetWindowIDFromSDLID( int _inputID );
		//----------------------------------------------------------------------------------
		/// \brief View creators
		//----------------------------------------------------------------------------------
		std::map< std::string, View* (*)() > m_viewCreators;
		//----------------------------------------------------------------------------------
		/// \brief Name of profile to change to
		//----------------------------------------------------------------------------------
		std::string m_bootstrapChangeProfileName;
		//----------------------------------------------------------------------------------
		/// \brief Change profile
		/// \param [in] _profileName
		//----------------------------------------------------------------------------------
		bool BootstrapChangeProfile( std::string _profileName );
		//----------------------------------------------------------------------------------

	};
}

#endif
