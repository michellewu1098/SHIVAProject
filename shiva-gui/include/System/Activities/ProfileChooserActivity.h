//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_SYSTEM_PROFILECHOOSERACTIVITY__
#define __SHIVA_SYSTEM_PROFILECHOOSERACTIVITY__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
#include "System/Activity.h"
#include "GUI/Views/TextView.h"
#include "System/Activities/FilesystemDataProvider.h"
#include "GUI/Views/AdapterViews/ListView.h"
#include "GUI/Views/AdapterViews/DataAdapter.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{
	/// This is our main Activity
	class ProfileChooserActivity : public Activity
	{
	public:

		/// This static method is used by the GUIManager to create an instance of our Activity when required
		static ShivaGUI::Activity* Factory() {return new ProfileChooserActivity();}

	protected:

		/// This function is called when the Activity is started
		virtual void OnCreate(Bundle*);

		virtual void OnConfigurationChanged();

		/// This function is called when the Activity is stopped
		virtual void OnDestroy();

		virtual void OnUpdate(float deltaTs);

		/// This will handle events from buttons etc
		virtual void UtilityEventReceived(UtilityEventHandler*,View*);

		/// The button to press to start the next activity, once the profile has been chosen
		UtilityEventHandler *_startHandler, *_chooseProfileListHandler, *_copyProfileHandler, *_deleteProfileHandler;

		std::string _startActivity, _chosenProfile;

		/// Loads the selected profile and starts the next Activity
		void StartNextActivity();

		/// Display the current profile name
		std::vector< std::pair<TextView*,unsigned int> > _currentProfileDisplays;
		std::vector< std::pair<TextView*,unsigned int> > _currentProfDirDisplays;
		std::vector< std::pair<ShivaGUI::ListView*,unsigned int> > _profileFileListDisplays;

		FilesystemDataProvider *_profileFilesProvider;

		ShivaGUI::DataAdapter *_profileFilesAdapter;

		void UpdateViews();

		void LoadViews();

		bool _startNextActivity;

	};
}



#endif

