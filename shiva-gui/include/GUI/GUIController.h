
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUI_GUI_GUICONTROLLER__
#define __SHIVA_GUI_GUI_GUICONTROLLER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <vector>
#include <string>
#include <list>
#include <map>

//////////////////////////////////////////////////////////////////////////
#include "System/Window.h"
#include "GUI/Views/View.h"
#include "GUI/Views/ViewEventListener.h"
#include "Input/InternalEvent.h"
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////


namespace ShivaGUI
{

	/// This is what Activities use to access the GUI associated with a Window
	class GUIController
	{
	public:
		GUIController(Window*, ResourceManager *, GUIManager*);
		~GUIController();

		// Methods for controlling the Window associated with this GUI
			void ChangeWindowSize(unsigned int width, unsigned int height);
			void GetWindowSize(unsigned int &width, unsigned int &height);
			bool GetFullScreen();

			/// Used to indicate the requested use of a window according to the user profile
			/// The idea here is that some users will have multiple windows, but will want to use them for different things
			Window::RequestedUse GetRequestedUse();


			/// Each Window has its own ResourceManager
			ResourceManager* GetResources() {return _resourceManager;}

			/// Sets the ID for a View
			void RegisterViewID(View*, std::string ID);
			/// Retrieves a View from ID
			/// \return NULL if View is unknown
			View* GetViewFromID(std::string ID);


			/// Registers a Listener so it can be accessed through Layout xml
			/// In Layout xml, events like button presses can specify the name of the listener that gets called
			void RegisterListener(ViewEventListener *listener, std::string name);

			/// Retrieves an event listener
			/// Views that generate events, such as buttons, can use this to retrieve a listener
			ViewEventListener* GetListener(std::string name);




			/// Must be called to arrange and correctly size the Views
			/// This is normally called automatically by the GUIManager when it is needed
			void Layout();



			/// Loads the View hierarchy specified by the filename
			void LoadContentView(std::string layoutName);


			/// The current layout will be saved within the active user profile
			void SaveLayoutToProfile();



			/// Issues an event to the GUIController
			/// \return true if the event was absorbed (handled) by some View
			bool IssueEvent(InternalEvent*);


			/*
			 * New Focus mechanism:
			 *  Gaining focus happens through events and is kept track of by the View
			 *  Want to allow future support of multi-touch, so multiple Views can have focus simultaneously
			 *
			/// Notifies the controller that the specific view has gained focus
			/// \note This does not set the View to having focus, this is achieved with events. \
			/// This function is mainly for a View that has determined it has gained focus to tell the GUIController that it is now in focus
			void NotifySetFocus(View *value);

			/// Returns the currently focussed View
			View* GetCurrentFocus();
			*/


		void MakeCurrent() {_attachedWindow->MakeCurrent();}

		void Update(float deltaTs);

		void Draw();




	protected:

		/// Stored mainly in case we need to write it back out again
		std::string _contentViewFilename;

		/// Root of the visible View hierarchy
		View *_contentView;

		/// Window associated with this controller
		Window *_attachedWindow;

		/// Link to the GUIManager
		GUIManager *_GUIManager;

		/// Link to the ResourceManager for this Window
		ResourceManager *_resourceManager;

		/// Listeners for use when loading View hierarchy from Layout xml
		std::map<std::string,ViewEventListener*> _eventListeners;

		/// Access to Views
		/// This has been moved from the ResourceManager, since the Views are only persistent with the GUIController (and hence Activity), while a ResourceManager's lifetime is that of the Window
		std::map<std::string,View*> _viewIDs;

	};

}

//////////////////////////////////////////////////////////////////////////
#endif
