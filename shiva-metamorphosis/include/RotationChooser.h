//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_ROTATIONCHOOSER__
#define __SHIVA_ROTATIONCHOOSER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#include "System/Activity.h"

#include "SDFView.h"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class RotationChooser : public ShivaGUI::Activity
{
public:
	static ShivaGUI::Activity* Factory() {return new RotationChooser();}

protected:
	virtual void OnCreate(ShivaGUI::Bundle*);

	virtual void OnDestroy();

	virtual void UtilityEventReceived(UtilityEventHandler*,ShivaGUI::View*);


	std::vector<SDFView*> _SDFViews;

	UtilityEventHandler *_backButtonHandler, *_leftButtonHandler, *_rightButtonHandler, *_upButtonHandler, *_downButtonHandler, *_saveButtonHandler;

	float _rotationStepsize;
	float _rotationX, _rotationY, _rotationZ;

	/// The saveName is the start of the filename, it will have a number after this (and then file extension)
	std::string _saveDir, _saveName;

	void UpdateViews();

	void LoadSDFViewData(SDFView*,ShivaGUI::Bundle *data, ShivaGUI::GUIController *guiController);

	/// Initialises a main window with input and output capabilities
	void InitIOWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data);

	/// Initialises a display-only window with no inputs
	void InitOutputWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data);
};

#endif

