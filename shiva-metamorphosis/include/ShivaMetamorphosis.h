//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_METAMORPHOSIS_ACTIVITY__
#define __SHIVA_METAMORPHOSIS_ACTIVITY__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <vector>

//////////////////////////////////////////////////////////////////////////
#include "System/Activity.h"
#include "GUI/GUIController.h"
#include "GUI/Views/Slider.h"

#include "SDFView.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


class ShivaMetamorphosis : public ShivaGUI::Activity
{
public:
	static ShivaGUI::Activity* Factory() {return new ShivaMetamorphosis();}

protected:
	virtual void OnCreate(ShivaGUI::Bundle*);

	virtual void OnDestroy();

	virtual void UtilityEventReceived(UtilityEventHandler *handler, ShivaGUI::View *view);

	std::vector<SDFView*> _SDFViews;

	std::vector<ShivaGUI::Slider*> _sliders;

	UtilityEventHandler *_backButtonHandler, *_nextButtonHandler, *_morphSliderHandler, *_leftMorphButtonHandler, *_rightMorphButtonHandler;

	ShivaGUI::Bundle *_startBundle;

	float _morphValue;
	float _morphStepsize;

	/// Updates SDF View and Sliders to the current _morphValue;
	void UpdateViews(ShivaGUI::Slider *issuingSlider = NULL);

	void LoadSDFViewData(SDFView*,ShivaGUI::Bundle *data, ShivaGUI::GUIController *guiController);

	/// Initialises a main window with input and output capabilities
	void InitIOWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data);

	/// Initialises a display-only window with no inputs
	void InitOutputWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data);

	/// TODO
//	void InitInputWindow(ShivaGUI::GUIController *guiController);
};

#endif

