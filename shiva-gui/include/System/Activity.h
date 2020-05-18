///-----------------------------------------------------------------------------------------------
/// \file Activity.h
/// \brief Base class for activity
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUISYSTEM_ACTIVITY__
#define __SHIVA_GUISYSTEM_ACTIVITY__

//#define _CRTDBG_MAP_ALLOC
//#define _CRTDBG_MAP_ALLOC_NEW
//#include <stdlib.h>
//#include <crtdbg.h>
//
//#ifdef _DEBUG
//   #ifndef DBG_NEW
//      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//      #define new DBG_NEW
//   #endif
//#endif  // _DEBUG

#include "Input/InternalEvent.h"
#include "GUI/GUIController.h"
#include "System/Bundle.h"
#include "Utility/tinyfiledialogs.h"

namespace ShivaGUI
{
	// Annoying forward declaration :(
	class GUIManager;
	class ResourceManager;

	class Activity
	{
	public:

		// INITIALISATION AND LIFECYCLE - mainly for use by GUIManager

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		Activity();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~Activity();
		//----------------------------------------------------------------------------------
		/// \brief If this is true, when the Activity stops it is expected to return a result
		/// \return m_startedForResult
		//----------------------------------------------------------------------------------
		bool GetStartedForResult() { return m_startedForResult; }
		//----------------------------------------------------------------------------------
		/// \brief Create activity
		/// \param [in] _guiManager
		/// \param [in] _savedState
		/// \param [in] _startedForResult
		//----------------------------------------------------------------------------------
		void Create( GUIManager *_guiManager, Bundle *_savedState, bool _startedForResult );
		//----------------------------------------------------------------------------------
		/// \brief Destroy
		//----------------------------------------------------------------------------------
		void Destroy();
		//----------------------------------------------------------------------------------
		/// \brief Returns GUI manager
		/// \return m_guiManager
		//----------------------------------------------------------------------------------
		GUIManager* GetGUIManager() { return m_guiManager; }
		//----------------------------------------------------------------------------------
		/// \brief Update activity
		/// \param [in] _deltaTs Timestep
		//----------------------------------------------------------------------------------
		void Update( float _deltaTs );
		//----------------------------------------------------------------------------------
		/// \brief Draw method
		//----------------------------------------------------------------------------------
		void Draw();
		//----------------------------------------------------------------------------------
		/// \brief Need to create different VAOs for multiple OGL contexts
		//----------------------------------------------------------------------------------
		void CreateBBoxVAOs();
		//----------------------------------------------------------------------------------
		/// \brief This is called by the GUIManager when a child activity returns a result
		/// \param [in] _data
		//----------------------------------------------------------------------------------
		void ReturnActivityResult( Bundle *_data );
		//----------------------------------------------------------------------------------
		// TODO: Currently unused, may never be used...
		//----------------------------------------------------------------------------------
		//Bundle* SaveInstanceState();
		//----------------------------------------------------------------------------------
		/// \brief Called after the window setup has changed and the GUIControllers have been replaced
		//----------------------------------------------------------------------------------
		void ConfigurationChanged();
		//----------------------------------------------------------------------------------
		
		// LAYOUT METHODS

		//----------------------------------------------------------------------------------
		/// \brief For use by GUIManager when creating the Activity
		/// \param [in] _numControllers
		//----------------------------------------------------------------------------------
		void SetNumGUIControllers( unsigned int _numControllers );
		//----------------------------------------------------------------------------------
		/// \brief For use by GUIManager, add GUI controller
		/// Indices must work sequentially
		/// \param [in] _controller
		/// \param [in] _index
		//----------------------------------------------------------------------------------
		void AddGUIController( GUIController *_controller, unsigned int _index );
		//----------------------------------------------------------------------------------
		/// \brief Returns the number of GUIControllers available to the Activity
		/// Each GUIController is an interface to a Window and the GUI Layout that will be displayed on that Window
		//----------------------------------------------------------------------------------
		unsigned int GetNumGUIControllers();
		//----------------------------------------------------------------------------------
		/// \brief Get GUI controller
		/// \param [in] _index
		//----------------------------------------------------------------------------------
		GUIController* GetGUIController( unsigned int _index );
		//----------------------------------------------------------------------------------
		/// \brief Must be called to arrange and correctly size the Views
		//----------------------------------------------------------------------------------
		void Layout();
		//----------------------------------------------------------------------------------

		// METHODS FOR YOU TO CALL IN DERIVED ACTIVITIES:

		//----------------------------------------------------------------------------------
		/// \brief Call this when you want to stop and exit the Activity
		//----------------------------------------------------------------------------------
		void Finish();
		//----------------------------------------------------------------------------------
		/// \brief For returning a result to a calling activity
		/// \param [in] _data
		//----------------------------------------------------------------------------------
		void SetResult( Bundle *_data );
		//----------------------------------------------------------------------------------

		// EVENT METHODS

		//----------------------------------------------------------------------------------
		/// \brief Issues events to all layout trees
		/// If you want to issue an event to a specific layout tree, you must retrieve the GUIController for the tree you want
		//----------------------------------------------------------------------------------
		void IssueEvent( InternalEvent *_currentEvent );
		//----------------------------------------------------------------------------------
		/// \brief Registers a Listener so it can be accessed through xml
		/// In xml, events like button presses can specify the name of the listener that gets called
		//----------------------------------------------------------------------------------
		//void RegisterListener( ViewEventListener *_listener, std::string _name );
		//----------------------------------------------------------------------------------
		/// \brief Retrieves an event listener
		/// Views that generate events, such as buttons, can use this to retrieve a listener
		//----------------------------------------------------------------------------------
		//ViewEventListener* GetListener(std::string name);
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Called when activity is created
		/// \param [in] _data
		//----------------------------------------------------------------------------------
		virtual void OnCreate( Bundle* _data ) {}
		//----------------------------------------------------------------------------------
		/// \brief Called when activity is started
		//----------------------------------------------------------------------------------
		virtual void OnStart() {}
		//----------------------------------------------------------------------------------
		/// \brief Called when activity is resumed
		//----------------------------------------------------------------------------------
		virtual void OnResume() {}
		//----------------------------------------------------------------------------------
		/// \brief Called when activity is paused
		//----------------------------------------------------------------------------------
		virtual void OnPause() {}
		//----------------------------------------------------------------------------------
		/// \brief Called when activity is stopped
		//----------------------------------------------------------------------------------
		virtual void OnStop() {}
		//----------------------------------------------------------------------------------
		/// \brief Called when activity is destroyed
		//----------------------------------------------------------------------------------
		virtual void OnDestroy() {}
		//----------------------------------------------------------------------------------
		/// \brief Called when configuration is changed
		//----------------------------------------------------------------------------------
		virtual void OnConfigurationChanged() {};
		//----------------------------------------------------------------------------------
		/// \brief Called when activity is updated
		/// \param [in] _deltaTs Timestep
		//----------------------------------------------------------------------------------
		virtual void OnUpdate( float _deltaTs ) {};
		//----------------------------------------------------------------------------------
		/// \brief called when back button is pressed
		//----------------------------------------------------------------------------------
		virtual void OnBackPressed() { Finish(); }
		//----------------------------------------------------------------------------------
		/// \brief This is called when the state needs to be saved, when the Activity will be destroyed to save memory etc
		/// TODO: Currently unused, may never be used...
		//----------------------------------------------------------------------------------
		//virtual Bundle* OnSaveInstanceState() {return NULL;}
		//----------------------------------------------------------------------------------
		/// \brief When you start an activity that is supposed to return a result, this function is called when that activity returns the result
		/// The Bundle will contain the data that was passed back from the
		//----------------------------------------------------------------------------------
		virtual void OnActivityResult( Bundle *_data ) {}
		//----------------------------------------------------------------------------------

		class UtilityEventHandler : public ViewEventListener
		{
		public:
			
			//----------------------------------------------------------------------------------
			/// \brief Ctor
			/// \param [in] _parent
			//----------------------------------------------------------------------------------
			UtilityEventHandler( Activity *_parent );
			//----------------------------------------------------------------------------------
			/// \brief Handle event
			/// \param [in] _view
			//----------------------------------------------------------------------------------
			virtual void HandleEvent( View *_view );
			//----------------------------------------------------------------------------------

		protected:

			//----------------------------------------------------------------------------------
			/// \brief Parent activity
			//----------------------------------------------------------------------------------
			Activity *m_parent;
			//----------------------------------------------------------------------------------

		};

		//----------------------------------------------------------------------------------
		virtual void UtilityEventReceived( UtilityEventHandler* , View* ) {}
		//----------------------------------------------------------------------------------


	private:

		//----------------------------------------------------------------------------------
		/// \brief GUI controllers
		//----------------------------------------------------------------------------------
		GUIController** m_GUIControllers;
		//----------------------------------------------------------------------------------
		/// \brief Number of GUI controllers
		//----------------------------------------------------------------------------------
		unsigned int m_numGUIControllers;
		//----------------------------------------------------------------------------------
		/// \brief Event listeners
		//----------------------------------------------------------------------------------
		std::map< std::string, ViewEventListener* > m_eventListeners;
		//----------------------------------------------------------------------------------
		/// \brief Link to GUI manager
		//----------------------------------------------------------------------------------
		GUIManager *m_guiManager;
		//----------------------------------------------------------------------------------
		/// \brief Whether the Activity was started with the expectation that it would return a result
		//----------------------------------------------------------------------------------
		bool m_startedForResult;
		//----------------------------------------------------------------------------------

	};
}

#endif
