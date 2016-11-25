//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_DRILLACTIVITY__
#define __SHIVA_DRILLACTIVITY__
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
class DrillActivity : public ShivaGUI::Activity
{
public:

	/// This static method is used by the GUIManager to create an instance of our Activity when required
	static ShivaGUI::Activity* Factory() {return new DrillActivity();}

protected:

	/// This function is called when the Activity is started
	virtual void OnCreate(ShivaGUI::Bundle*);

	/// This function is called when the Activity is stopped
	virtual void OnDestroy();

	virtual void OnUpdate(float deltaTs);

	/// This will handle events from buttons etc
	virtual void UtilityEventReceived(UtilityEventHandler*,ShivaGUI::View*);

	/// These will be registered with the GUI System and will be given to the UtilityEventReceived function when an event
	/// It's not a brilliant way of doing things, more a lazy way as there's less code for the library user
	UtilityEventHandler *_buttonHandler;


	float _rotationStepsize;
	float _rotationX, _rotationY, _rotationZ;

	/// A list of Vol Views, mainly for updating them with new objects etc
	std::vector< std::pair<VolView*,ShivaGUI::GUIController*> > _VolViews;
	VolView *_selectionVolView;

	/// Mainly for updating current Vol Views
	void UpdateViews();

	void ResetRotation();

	void RebuildTrees();

	/// Initialises a main window with input and output capabilities
	void InitIOWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data);

	/// Initialises a display-only window with no inputs
	void InitOutputWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data);

	Totem::Controller *_totemController;

	float _crosshairNudgeAmount;

	float _objectColourR, _objectColourG, _objectColourB;
	bool _setObjectColour;

	float _drillSize, _originalDrillSize, _drillSizeStep;
};



#endif

