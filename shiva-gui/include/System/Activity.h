
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUISYSTEM_ACTIVITY__
#define __SHIVA_GUISYSTEM_ACTIVITY__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>
#include <map>

//////////////////////////////////////////////////////////////////////////
#include "Input/InternalEvent.h"
#include "GUI/Views/ViewEventListener.h"
#include "GUI/GUIController.h"
#include "System/Bundle.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{
	// Annoying forward declaration :(
	class GUIManager;
	class ResourceManager;


	class Activity
	{
	public:

		// Initialisation and lifecycle - mainly for use by GUIManager
			Activity();
			virtual ~Activity();

			/// If this is true, when the Activity stops it is expected to return a result
			bool GetStartedForResult() {return _startedForResult;}

			void Create(GUIManager*,Bundle*, bool startedForResult);
			void Destroy();

			GUIManager* GetGUIManager() {return _guiManager;}

			void Update(float deltaTs);

			void Draw();

			/// This is called by the GUIManager when a child activity returns a result
			void ReturnActivityResult(Bundle*);

			/// TODO: Currently unused, may never be used...
			//Bundle* SaveInstanceState();

			/// Called after the window setup has changed and the GUIControllers have been replaced
			void ConfigurationChanged();


		// Layout methods
			/// For use by GUIManager when creating the Activity
			void SetNumGUIControllers(unsigned int numControllers);
			/// For use by GUIManager
			/// Indices must work sequentially
			void AddGUIController(GUIController*,unsigned int index);

			/// Returns the number of GUIControllers available to the Activity
			/// Each GUIController is an interface to a Window and the GUI Layout that will be displayed on that Window
			unsigned int GetNumGUIControllers();
			GUIController* GetGUIController(unsigned int index);

			/// Must be called to arrange and correctly size the Views
			void Layout();


		// Methods for you to call in derived Activities:

			/// Call this when you want to stop and exit the Activity
			void Finish();

			/// For returning a result to a calling activity
			void SetResult(Bundle*);


		// Event Methods

			/// Issues events to all layout trees
			/// If you want to issue an event to a specific layout tree, you must retrieve the GUIController for the tree you want
			void IssueEvent(InternalEvent*);

			/// Registers a Listener so it can be accessed through xml
			/// In xml, events like button presses can specify the name of the listener that gets called
			//void RegisterListener(ViewEventListener *listener, std::string name);

			/// Retrieves an event listener
			/// Views that generate events, such as buttons, can use this to retrieve a listener
			//ViewEventListener* GetListener(std::string name);

	protected:

		virtual void OnCreate(Bundle*) {}
		virtual void OnStart() {}
		virtual void OnResume() {}

		virtual void OnPause() {}
		virtual void OnStop() {}
		virtual void OnDestroy() {}

		virtual void OnConfigurationChanged() {};
		virtual void OnUpdate(float deltaTs) {};

		virtual void OnBackPressed() { Finish(); }

		/// This is called when the state needs to be saved, when the Activity will be destroyed to save memory etc
		/// TODO: Currently unused, may never be used...
		//virtual Bundle* OnSaveInstanceState() {return NULL;}

		/// When you start an activity that is supposed to return a result, this function is called when that activity returns the result
		/// The Bundle will contain the data that was passed back from the
		virtual void OnActivityResult(Bundle *data) {}


		class UtilityEventHandler : public ViewEventListener
		{
		public:
			UtilityEventHandler(Activity*);
			virtual void HandleEvent(View *);
		protected:
			Activity *_parent;
		};

		virtual void UtilityEventReceived(UtilityEventHandler*,View*) {}


	private:

		GUIController** _GUIControllers;
		unsigned int _numGUIControllers;

		/// Main parent of the visible View hierarchy
		//View *_contentView;

		/// Views for selection
		//View *_firstFocusView, *_currentFocussedView;

		std::map<std::string,ViewEventListener*> _eventListeners;

		GUIManager *_guiManager;

		/// Whether the Activity was started with the expectation that it would return a result
		bool _startedForResult;

	};

}

//////////////////////////////////////////////////////////////////////////
#endif
