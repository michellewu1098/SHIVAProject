///-----------------------------------------------------------------------------------------------
/// \file CreateNewProfileActivity.h
/// \brief Responsible for creating a new profile. It will load the layouts to screen and handle events from buttons
/// \author Michelle Wu
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_CREATENEWPROFILECTIVITY__
#define __SHIVA_CREATENEWPROFILEACTIVITY__

#include <boost/filesystem.hpp>

#include "Utility/tinyfiledialogs.h"
#include "GUI/Views/TextView.h"
#include "GUI/Views/AdapterViews/ListView.h"
#include "GUI/Views/AdapterViews/DataAdapter.h"
#include "GUI/Views/ColourSelector.h"
#include "System/Activity.h"


namespace ShivaGUI
{ 
	class CreateNewProfileActivity : public ShivaGUI::Activity
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief This static method is used by the GUIManager to create an instance of our Activity when required
		//----------------------------------------------------------------------------------
		static ShivaGUI::Activity* Factory() { return new CreateNewProfileActivity(); }
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief This function is called when the Activity is started
		//----------------------------------------------------------------------------------
		virtual void OnCreate( ShivaGUI::Bundle* );
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
		virtual void UtilityEventReceived( UtilityEventHandler* _handler, ShivaGUI::View* _view );
		//----------------------------------------------------------------------------------
		// These will be registered with the GUI System and will be given to the UtilityEventReceived function when an event
		// It's not a brilliant way of doing things, more a lazy way as there's less code for the library user
		//----------------------------------------------------------------------------------
		/// \brief Button handler
		//----------------------------------------------------------------------------------
		UtilityEventHandler *m_buttonHandler;
		//----------------------------------------------------------------------------------
		/// \brief Colour changer handler
		//----------------------------------------------------------------------------------
		UtilityEventHandler *m_colourChangeHandler;
		//----------------------------------------------------------------------------------
		/// \brief Mainly for updating current Vol Views
		//------------------------------------------------------------------- ---------------
		void UpdateViews();
		//----------------------------------------------------------------------------------
		/// \brief Load views
		//----------------------------------------------------------------------------------
		void LoadViews();
		//----------------------------------------------------------------------------------
		/// \brief Directory where to save file
		//----------------------------------------------------------------------------------
		std::string m_saveDir;
		//----------------------------------------------------------------------------------
		/// \brief File name to save
		//----------------------------------------------------------------------------------
		std::string m_saveName;
		//----------------------------------------------------------------------------------
		/// \brief Rescale size
		//----------------------------------------------------------------------------------
		float m_saveRescaleSize;
		//----------------------------------------------------------------------------------
		/// \brief Custom profile document
		//----------------------------------------------------------------------------------
		TiXmlDocument m_profile;
		//----------------------------------------------------------------------------------
		/// \brief Options parameters in .xml profile
		//----------------------------------------------------------------------------------
		TiXmlElement* m_options;
		//----------------------------------------------------------------------------------
		/// \brief Custom profile name
		//----------------------------------------------------------------------------------
		std::string m_profileName;
		//----------------------------------------------------------------------------------
		/// \brief Red component
		//----------------------------------------------------------------------------------
		std::stringstream m_colourR;
		//----------------------------------------------------------------------------------
		/// \brief Green component
		//----------------------------------------------------------------------------------
		std::stringstream m_colourG;
		//----------------------------------------------------------------------------------
		/// \brief Blue component
		//----------------------------------------------------------------------------------
		std::stringstream m_colourB;
		//----------------------------------------------------------------------------------
		/// \brief Flag used to check if user has clicked on custom background colour, else set default
		//----------------------------------------------------------------------------------
		bool m_changedBackgroundColour;
		//----------------------------------------------------------------------------------
		/// \brief Flag used to check if user has clicked on custom dimensions, else set default
		//----------------------------------------------------------------------------------
		bool m_customDimensions;
		//----------------------------------------------------------------------------------
		/// \brief Flag used to check if a theme has been chosen 
		//----------------------------------------------------------------------------------
		bool m_chosenTheme;
		//----------------------------------------------------------------------------------

	};
}

#endif

