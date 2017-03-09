///-----------------------------------------------------------------------------------------------
/// \file ProfileChooserActivity.h
/// \brief This is our main Activity
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_SYSTEM_PROFILECHOOSERACTIVITY__
#define __SHIVA_SYSTEM_PROFILECHOOSERACTIVITY__


#include <boost/filesystem.hpp>

#include "System/Activity.h"
#include "System/Activities/FilesystemDataProvider.h"
#include "GUI/Views/AdapterViews/ListView.h"
#include "GUI/Views/AdapterViews/DataAdapter.h"
#include "Utility/tinyfiledialogs.h"

namespace ShivaGUI
{ 
	class ProfileChooserActivity : public Activity
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief This static method is used by the GUIManager to create an instance of our Activity when required
		//----------------------------------------------------------------------------------
		static ShivaGUI::Activity* Factory() { return new ProfileChooserActivity(); }
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief This function is called when the Activity is started
		/// \param [in] _data
		//----------------------------------------------------------------------------------
		virtual void OnCreate( Bundle* );
		//----------------------------------------------------------------------------------
		/// \brief This function is called when configuration changes
		//----------------------------------------------------------------------------------
		virtual void OnConfigurationChanged();
		//----------------------------------------------------------------------------------
		/// \brief This function is called when the Activity is stopped
		//----------------------------------------------------------------------------------
		virtual void OnDestroy();
		//----------------------------------------------------------------------------------
		/// \brief This function is called when activity is updated
		/// \param [in] _deltaTs Timestep
		//----------------------------------------------------------------------------------
		virtual void OnUpdate( float _deltaTs );
		//----------------------------------------------------------------------------------
		/// \brief This will handle events from buttons etc
		/// \param [in] _handler
		/// \param [in] _view
		//----------------------------------------------------------------------------------
		virtual void UtilityEventReceived( UtilityEventHandler*, View* );
		//----------------------------------------------------------------------------------
		/// \brief The button to press to start the next activity, once the profile has been chosen
		//----------------------------------------------------------------------------------
		UtilityEventHandler *m_startHandler;
		//----------------------------------------------------------------------------------
		/// \brief List of profiles to choose from
		//----------------------------------------------------------------------------------
		UtilityEventHandler *m_chooseProfileListHandler;
		//----------------------------------------------------------------------------------
		/// \brief Handler to copy profile
		//----------------------------------------------------------------------------------
		UtilityEventHandler *m_copyProfileHandler;
		//----------------------------------------------------------------------------------
		/// \brief Handler to delete profile
		//----------------------------------------------------------------------------------
		UtilityEventHandler *m_deleteProfileHandler;
		//----------------------------------------------------------------------------------
		UtilityEventHandler *m_buttonHandler;
		//----------------------------------------------------------------------------------
		/// \brief Name of activity to start
		//----------------------------------------------------------------------------------
		std::string m_startActivity;
		//----------------------------------------------------------------------------------
		/// \brief Name of chosen profile
		//----------------------------------------------------------------------------------
		std::string m_chosenProfile;
		//----------------------------------------------------------------------------------
		/// \brief Loads the selected profile and starts the next Activity
		//----------------------------------------------------------------------------------
		void StartNextActivity();
		//----------------------------------------------------------------------------------
		/// \brief Vector of the current profile name to display
		//----------------------------------------------------------------------------------
		std::vector< std::pair< TextView*, unsigned int > > m_currentProfileDisplays;
		//----------------------------------------------------------------------------------
		/// \brief Vector of the current profile directory to display
		//----------------------------------------------------------------------------------
		std::vector< std::pair< TextView*, unsigned int > > m_currentProfDirDisplays;
		//----------------------------------------------------------------------------------
		/// \brief Vector of profile file list to display
		//----------------------------------------------------------------------------------
		std::vector< std::pair< ShivaGUI::ListView*, unsigned int > > m_profileFileListDisplays;
		//----------------------------------------------------------------------------------
		/// \brief Profile files data provider
		//----------------------------------------------------------------------------------
		FilesystemDataProvider *m_profileFilesProvider;
		//----------------------------------------------------------------------------------
		/// \brief Profile files data adapter
		//----------------------------------------------------------------------------------
		ShivaGUI::DataAdapter *m_profileFilesAdapter;
		//----------------------------------------------------------------------------------
		/// \brief Update views
		//----------------------------------------------------------------------------------
		void UpdateViews();
		//----------------------------------------------------------------------------------
		/// \brief Load views
		//----------------------------------------------------------------------------------
		void LoadViews();
		//----------------------------------------------------------------------------------
		/// \brief Indicates whether to start next activity
		//----------------------------------------------------------------------------------
		bool m_startNextActivity;
		//----------------------------------------------------------------------------------

	};
}

#endif

