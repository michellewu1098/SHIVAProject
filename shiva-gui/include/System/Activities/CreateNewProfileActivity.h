///-----------------------------------------------------------------------------------------------
/// \file CreateNewProfileActivity.h
/// \brief Responsible for creating a new profile. It will load the layouts to screen and handle events from buttons
/// \author Leigh McLoughlin, Michelle Wu
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_CREATENEWPROFILECTIVITY__
#define __SHIVA_CREATENEWPROFILEACTIVITY__

#include <boost/filesystem.hpp>

#include "Utility/tinyfiledialogs.h"
#include "GUI/Views/TextView.h"
#include "GUI/Views/AdapterViews/ListView.h"
#include "GUI/Views/AdapterViews/DataAdapter.h"
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
	/// \brief When you start an activity that is supposed to return a result, this function is called when that activity returns the result
	/// The Bundle will contain the data that was passed back from the
	/// \param [in] _data
	//----------------------------------------------------------------------------------
	virtual void OnActivityResult( ShivaGUI::Bundle *_data );
	//----------------------------------------------------------------------------------
	// These will be registered with the GUI System and will be given to the UtilityEventReceived function when an event
	// It's not a brilliant way of doing things, more a lazy way as there's less code for the library user
	//----------------------------------------------------------------------------------
	/// \brief Button handler
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_buttonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Mainly for updating current Vol Views
	//------------------------------------------------------------------- ---------------
	void UpdateViews();
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
	/// \brief Whether to show save confirmation
	//----------------------------------------------------------------------------------
	bool m_showSaveConfirmation;
	//----------------------------------------------------------------------------------
	float m_saveTextCounter;
	//----------------------------------------------------------------------------------

	};
}

#endif

