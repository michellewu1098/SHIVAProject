//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_ASSEMBLEACTIVITY__
#define __SHIVA_ASSEMBLEACTIVITY__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
#include "System/Activity.h"

#include "VolView.h"
#include "GUI/Views/TextView.h"
#include "GUI/Views/AdapterViews/ListView.h"
#include "GUI/Views/AdapterViews/DataAdapter.h"
#include "Totem/TotemController.h"

#include <vector>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// This is our main Activity
/// It is responsible for a 'task' or widget layout on the screen
/// It will load the layouts to screen and handle events from buttons
class AssembleActivity : public ShivaGUI::Activity
{
public:

	/// This static method is used by the GUIManager to create an instance of our Activity when required
	static ShivaGUI::Activity* Factory() {return new AssembleActivity();}

protected:

	/// This function is called when the Activity is started
	virtual void OnCreate(ShivaGUI::Bundle*);
	
	virtual void OnResume();

	/// This function is called when the Activity is stopped
	virtual void OnDestroy();

	virtual void OnUpdate(float deltaTs);

	/// This will handle events from buttons etc
	virtual void UtilityEventReceived(UtilityEventHandler*,ShivaGUI::View*);

	/// When you start an activity that is supposed to return a result, this function is called when that activity returns the result
	/// The Bundle will contain the data that was passed back from the
	virtual void OnActivityResult(ShivaGUI::Bundle *data);

	/// These will be registered with the GUI System and will be given to the UtilityEventReceived function when an event
	/// It's not a brilliant way of doing things, more a lazy way as there's less code for the library user
	//UtilityEventHandler *_rotateLeftHandler, *_rotateRightHandler, *_rotateUpHandler, *_rotateDownHandler;

	//UtilityEventHandler *_deleteTopHandler, *_editHandler;
	UtilityEventHandler *_buttonHandler, *_addPrimitiveHandler, *_launchActivityHandler;


	float _rotationStepsize;
	float _rotationX, _rotationY, _rotationZ;

	/// A list of Vol Views, mainly for updating them with new objects etc
	std::vector< std::pair<VolView*,ShivaGUI::GUIController*> > _VolViews;
	std::vector< std::pair<ShivaGUI::AdapterView*,ShivaGUI::GUIController*> > _listViews;
	std::vector< std::pair<ShivaGUI::TextView*,ShivaGUI::GUIController*> > _saveConfirmViews;

	/// This will convert the data provided by the ShivaModelManager into a format usable by a ListView
	ShivaGUI::DataAdapter *_modelListAdapter;

	/// Mainly for updating current Vol Views
	void UpdateViews();

	void ResetRotation();

	void RebuildTrees(bool justparams = false);

	/// Initialises a main window with input and output capabilities
	void InitIOWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data);

	/// Initialises a display-only window with no inputs
	void InitOutputWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data);

	Totem::Controller *_totemController;

	std::string _saveDir, _saveName;
	float _saveRescaleSize;

	bool _showSaveConfirmation;
	float _saveTextCounter;
	
	float _objectColourR, _objectColourG, _objectColourB;
	bool _setObjectColour;

	float _originalBlendingAmount;
};



#endif

