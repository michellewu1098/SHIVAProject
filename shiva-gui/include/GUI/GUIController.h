///-----------------------------------------------------------------------------------------------
/// \file GUIController.h
/// \brief This is what Activities use to access the GUI associated with a Window
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_GUI_GUICONTROLLER__
#define __SHIVA_GUI_GUI_GUICONTROLLER__

#include <map>

#include "System/Window.h"
#include "GUI/Views/ViewEventListener.h"
#include "System/SharedPreferences.h"

namespace ShivaGUI
{
	class GUIController
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		/// \param [in] _inputWindow
		/// \param [in] _resources
		/// \param [in] _guiman
		//----------------------------------------------------------------------------------
		GUIController( Window *_inputWindow, ResourceManager *_resources, GUIManager *_guiman );
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~GUIController();
		//----------------------------------------------------------------------------------
		// Methods for controlling the Window associated with this GUI
		//----------------------------------------------------------------------------------
		/// \brief Change window size
		/// \param [in] _width
		/// \param [in] _height
		//----------------------------------------------------------------------------------
		void ChangeWindowSize( unsigned int _width, unsigned int _height );
		//----------------------------------------------------------------------------------
		/// \brief Get window size
		/// \param [out] _width
		/// \param [out] _height
		//----------------------------------------------------------------------------------
		void GetWindowSize( unsigned int &_width, unsigned int &_height );
		//----------------------------------------------------------------------------------
		/// \brief Set attached window to be fullscreen
		//----------------------------------------------------------------------------------
		bool GetFullScreen();

		//----------------------------------------------------------------------------------
		/// \brief Get the current 'screen'
		//----------------------------------------------------------------------------------
		std::string GetContentViewFilename() const { return m_contentViewFilename; }

		//----------------------------------------------------------------------------------
		/// \brief Used to indicate the requested use of a window according to the user profile
		/// The idea here is that some users will have multiple windows, but will want to use them for different things
		//----------------------------------------------------------------------------------
		Window::RequestedUse GetRequestedUse();
		//----------------------------------------------------------------------------------
		/// \brief Each Window has its own ResourceManager
		//----------------------------------------------------------------------------------
		ResourceManager* GetResources() { return m_resourceManager; }
		//----------------------------------------------------------------------------------
		/// \brief Sets the ID for a View
		/// \param [in] _view
		/// \param [in] _id
		//----------------------------------------------------------------------------------
		void RegisterViewID( View *_view, std::string _id );
		//----------------------------------------------------------------------------------
		/// \brief Retrieves a View from ID
		/// \return NULL if View is unknown
		//----------------------------------------------------------------------------------
		View* GetViewFromID( std::string _id );
		//----------------------------------------------------------------------------------
		/// \brief Registers a Listener so it can be accessed through Layout xml
		/// In Layout xml, events like button presses can specify the name of the listener that gets called
		//----------------------------------------------------------------------------------
		void RegisterListener( ViewEventListener *_listener, std::string _name );
		//----------------------------------------------------------------------------------
		/// \brief Retrieves an event listener
		/// Views that generate events, such as buttons, can use this to retrieve a listener
		//----------------------------------------------------------------------------------
		ViewEventListener* GetListener( std::string _name );
		//----------------------------------------------------------------------------------
		/// \brief Must be called to arrange and correctly size the Views
		/// This is normally called automatically by the GUIManager when it is needed
		//----------------------------------------------------------------------------------
		void Layout();
		//----------------------------------------------------------------------------------
		/// \brief Loads the View hierarchy specified by the filename
		/// \param [in] _layoutName
		//----------------------------------------------------------------------------------
		void LoadContentView( std::string _layoutName );
		//----------------------------------------------------------------------------------
		/// \brief The current layout will be saved within the active user profile
		//----------------------------------------------------------------------------------
		void SaveLayoutToProfile();
		//----------------------------------------------------------------------------------
		/// \brief Issues an event to the GUIController
		/// \param [in] _currentEvent
		/// \return true if the event was absorbed (handled) by some View
		//----------------------------------------------------------------------------------
		bool IssueEvent( InternalEvent *_currentEvent );
		//----------------------------------------------------------------------------------
		/*
		 * New Focus mechanism:
		 *  Gaining focus happens through events and is kept track of by the View
		 *  Want to allow future support of multi-touch, so multiple Views can have focus simultaneously
		 *
		/// Notifies the controller that the specific view has gained focus
		/// \note This does not set the View to having focus, this is achieved with events.
		/// This function is mainly for a View that has determined it has gained focus to tell the GUIController that it is now in focus
		//----------------------------------------------------------------------------------
		void NotifySetFocus( View *_value );
		//----------------------------------------------------------------------------------
		/// \brief Returns the currently focussed View
		//----------------------------------------------------------------------------------
		View* GetCurrentFocus(); */
		//----------------------------------------------------------------------------------
		/// \brief Make the attached window as the current one
		//----------------------------------------------------------------------------------
		void MakeCurrent() { m_attachedWindow->MakeCurrent(); }
		//----------------------------------------------------------------------------------
		/// \brief Update method
		//----------------------------------------------------------------------------------
		void Update( float _deltaTs );
		//----------------------------------------------------------------------------------
		/// \brief Draw method
		/// \param [in] _content Current OGL context
		//----------------------------------------------------------------------------------
		void Draw( unsigned int _context );
		//----------------------------------------------------------------------------------
		/// \brief Draw method
		//----------------------------------------------------------------------------------
		void Draw();
		//----------------------------------------------------------------------------------
		/// \brief Function is called when there is more than one GUI controller -> more than one OGL context
		//----------------------------------------------------------------------------------
		void CreateBBoxVAOs( unsigned int _context );
		//----------------------------------------------------------------------------------
		/// \brief Pass to resource manager layout preferences set in prefs.xml
		/// \param [in] _prefs Layout preferences
		//----------------------------------------------------------------------------------
		void SetLayoutPrefs( SharedPreferences* _prefs );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Stored mainly in case we need to write it back out again
		//----------------------------------------------------------------------------------
		std::string m_contentViewFilename;
		//----------------------------------------------------------------------------------
		/// \brief Root of the visible View hierarchy
		//----------------------------------------------------------------------------------
		View *m_contentView;
		//----------------------------------------------------------------------------------
		/// \brief Window associated with this controller
		//----------------------------------------------------------------------------------
		Window *m_attachedWindow;
		//----------------------------------------------------------------------------------
		/// \brief Link to the GUIManager
		//----------------------------------------------------------------------------------
		GUIManager *m_GUIManager;
		//----------------------------------------------------------------------------------
		/// \brief Link to the ResourceManager for this Window
		//----------------------------------------------------------------------------------
		ResourceManager *m_resourceManager;
		//----------------------------------------------------------------------------------
		/// \brief Listeners for use when loading View hierarchy from Layout xml
		//----------------------------------------------------------------------------------
		std::map< std::string, ViewEventListener* > m_eventListeners;
		//----------------------------------------------------------------------------------
		/// \brief Access to Views
		/// This has been moved from the ResourceManager, since the Views are only persistent with the GUIController (and hence Activity), while a ResourceManager's lifetime is that of the Window
		//----------------------------------------------------------------------------------
		std::map< std::string, View* > m_viewIDs;
		//----------------------------------------------------------------------------------

	};
}

#endif
