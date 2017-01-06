///-----------------------------------------------------------------------------------------------
/// \file AssembleActivity.h
/// \brief Responsible for assemble activity or widget layout on the screen. It will load the layouts to screen and handle events from buttons
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_ASSEMBLEACTIVITY__
#define __SHIVA_ASSEMBLEACTIVITY__

#include <vector>
#include <boost/filesystem.hpp>

#include "GUI/Views/TextView.h"
#include "GUI/Views/AdapterViews/ListView.h"
#include "GUI/Views/AdapterViews/DataAdapter.h"
#include "System/Activity.h"
#include "Totem/TotemController.h"
#include "VolView.h"


class AssembleActivity : public ShivaGUI::Activity
{
public:

	//----------------------------------------------------------------------------------
	/// \brief This static method is used by the GUIManager to create an instance of our Activity when required
	//----------------------------------------------------------------------------------
	static ShivaGUI::Activity* Factory() { return new AssembleActivity(); }
	//----------------------------------------------------------------------------------

protected:

	//----------------------------------------------------------------------------------
	/// \brief This function is called when the Activity is started
	//----------------------------------------------------------------------------------
	virtual void OnCreate( ShivaGUI::Bundle* );
	//----------------------------------------------------------------------------------
	/// \brief This function is called when the Activity is resumed
	//----------------------------------------------------------------------------------
	virtual void OnResume();
	//----------------------------------------------------------------------------------
	/// \brief This function is called when the Activity is stopped
	//----------------------------------------------------------------------------------
	virtual void OnDestroy();
	//----------------------------------------------------------------------------------
	/// \brief This function is called when the Activity is updated
	//----------------------------------------------------------------------------------
	virtual void OnUpdate( float deltaTs );
	//----------------------------------------------------------------------------------
	/// \brief This will handle events from buttons etc
	/// \param [in] handler
	/// \param [in] view
	//----------------------------------------------------------------------------------
	virtual void UtilityEventReceived( UtilityEventHandler*, ShivaGUI::View* );
	//----------------------------------------------------------------------------------
	/// \brief When you start an activity that is supposed to return a result, this function is called when that activity returns the result
	/// The Bundle will contain the data that was passed back from the
	/// \param [in] data
	//----------------------------------------------------------------------------------
	virtual void OnActivityResult( ShivaGUI::Bundle *data );
	//----------------------------------------------------------------------------------
	//UtilityEventHandler *_rotateLeftHandler, *_rotateRightHandler, *_rotateUpHandler, *_rotateDownHandler;
	//UtilityEventHandler *_deleteTopHandler, *_editHandler;
	//----------------------------------------------------------------------------------
	/// \brief These will be registered with the GUI System and will be given to the UtilityEventReceived function when an event
	/// It's not a brilliant way of doing things, more a lazy way as there's less code for the library user
	//----------------------------------------------------------------------------------
	UtilityEventHandler *_buttonHandler;
	//----------------------------------------------------------------------------------
	UtilityEventHandler *_addPrimitiveHandler;
	//----------------------------------------------------------------------------------
	UtilityEventHandler *_launchActivityHandler;
	//----------------------------------------------------------------------------------
	float _rotationStepsize;
	//----------------------------------------------------------------------------------
	float _rotationX;
	//----------------------------------------------------------------------------------
	float _rotationY;
	//----------------------------------------------------------------------------------
	float _rotationZ;
	//----------------------------------------------------------------------------------
	// A list of Vol Views, mainly for updating them with new objects etc
	//----------------------------------------------------------------------------------
	std::vector< std::pair< VolView*, ShivaGUI::GUIController* > > _VolViews;
	//----------------------------------------------------------------------------------
	std::vector< std::pair< ShivaGUI::AdapterView*, ShivaGUI::GUIController* > > _listViews;
	//----------------------------------------------------------------------------------
	std::vector< std::pair< ShivaGUI::TextView*, ShivaGUI::GUIController* > > _saveConfirmViews;
	//----------------------------------------------------------------------------------
	/// \brief This will convert the data provided by the ShivaModelManager into a format usable by a ListView
	//----------------------------------------------------------------------------------
	ShivaGUI::DataAdapter *_modelListAdapter;
	//----------------------------------------------------------------------------------
	/// \brief Mainly for updating current Vol Views
	//----------------------------------------------------------------------------------
	void UpdateViews();
	//----------------------------------------------------------------------------------
	/// \brief Reset rotation
	//----------------------------------------------------------------------------------
	void ResetRotation();
	//----------------------------------------------------------------------------------
	/// \brief Rebuild tree
	/// \param [in] justparams 
	//----------------------------------------------------------------------------------
	void RebuildTrees( bool justparams = false );
	//----------------------------------------------------------------------------------
	/// \brief Initialises a main window with input and output capabilities
	/// \param [in] guiController
	/// \param [in] data
	//----------------------------------------------------------------------------------
	void InitIOWindow( ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data );
	//----------------------------------------------------------------------------------
	/// \brief Initialises a display-only window with no inputs
	/// \param [in] guiController
	/// \param [in] data
	//----------------------------------------------------------------------------------
	void InitOutputWindow( ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data );
	//----------------------------------------------------------------------------------
	/// \brief Handle to a totem controller
	//----------------------------------------------------------------------------------
	Totem::Controller *_totemController;
	//----------------------------------------------------------------------------------
	/// \brief Directory where to save file
	//----------------------------------------------------------------------------------
	std::string _saveDir;
	//----------------------------------------------------------------------------------
	/// \brief File name to save
	//----------------------------------------------------------------------------------
	std::string _saveName;
	//----------------------------------------------------------------------------------
	float _saveRescaleSize;
	//----------------------------------------------------------------------------------
	bool _showSaveConfirmation;
	//----------------------------------------------------------------------------------
	float _saveTextCounter;
	//----------------------------------------------------------------------------------
	float _objectColourR;
	//----------------------------------------------------------------------------------
	float _objectColourG;
	//----------------------------------------------------------------------------------
	float _objectColourB;
	//----------------------------------------------------------------------------------
	bool _setObjectColour;
	//----------------------------------------------------------------------------------
	/// \brief Original blending amount used when resetting totem pole
	//----------------------------------------------------------------------------------
	float _originalBlendingAmount;
	//----------------------------------------------------------------------------------
};

#endif

