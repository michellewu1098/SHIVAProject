//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_BLENDADJUSTACTIVITY__
#define __SHIVA_BLENDADJUSTACTIVITY__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
#include "System/Activity.h"

#include "VolView.h"
#include "Totem/TotemController.h"

#include <vector>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// This is our main Activity
/// It is responsible for a 'task' or widget layout on the screen
/// It will load the layouts to screen and handle events from buttons
class BlendAdjustActivity : public ShivaGUI::Activity
{
public:

	/// This static method is used by the GUIManager to create an instance of our Activity when required
	static ShivaGUI::Activity* Factory() {return new BlendAdjustActivity();}

protected:

	/// This function is called when the Activity is started
	virtual void OnCreate(ShivaGUI::Bundle*);

	/// This function is called when the Activity is stopped
	virtual void OnDestroy();

	/// This will handle events from buttons etc
	virtual void UtilityEventReceived(UtilityEventHandler*,ShivaGUI::View*);

	/// When you start an activity that is supposed to return a result, this function is called when that activity returns the result
	/// The Bundle will contain the data that was passed back from the
	virtual void OnActivityResult(ShivaGUI::Bundle *data);

	/// These will be registered with the GUI System and will be given to the UtilityEventReceived function when an event
	/// It's not a brilliant way of doing things, more a lazy way as there's less code for the library user
	UtilityEventHandler *_buttonHandler;


	float _rotationStepsize;
	float _rotationX, _rotationY, _rotationZ;

	float _initialBlending;

	/// A list of Vol Views, mainly for updating them with new objects etc
	std::vector< std::pair<VolView*,ShivaGUI::GUIController*> > _VolViews;

	/// Mainly for updating current Vol Views
	void UpdateViews();

	void ResetRotation();

	void RebuildTrees();

	/// Initialises a main window with input and output capabilities
	void InitIOWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data);

	/// Initialises a display-only window with no inputs
	void InitOutputWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data);

	Totem::Controller *_totemController;

	float _objectColourR, _objectColourG, _objectColourB;
	bool _setObjectColour;
};



#endif

