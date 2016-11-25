//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_COLOURCHOOSER__
#define __SHIVA_COLOURCHOOSER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#include "System/Activity.h"

#include "GUI/Views/ColourSelector.h"

#include "SDFView.h"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class ColourChooser : public ShivaGUI::Activity
{
public:
	static ShivaGUI::Activity* Factory() {return new ColourChooser();}

protected:
	virtual void OnCreate(ShivaGUI::Bundle*);

	virtual void OnDestroy();

	virtual void UtilityEventReceived(UtilityEventHandler*,ShivaGUI::View*);

	UtilityEventHandler *_backButtonHandler, *_nextButtonHandler, *_colourChangeHandler, *_modelSelectButtonHandler, *_leftColButtonHandler, *_rightColButtonHandler;


	std::vector<SDFView*> _SDFViews;

	std::vector<ShivaGUI::ColourSelector*> _selectors;

	float _colourX, _colourY;

	float _colourStepsize;

	ShivaGUI::Bundle *_startBundle;

	void UpdateViews(ShivaGUI::ColourSelector *issuingSelector = NULL);

	void LoadSDFViewData(SDFView*,ShivaGUI::Bundle *data,ShivaGUI::GUIController*);

	/// Initialises a main window with input and output capabilities
	void InitIOWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data);

	/// Initialises a display-only window with no inputs
	void InitOutputWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data);
};

#endif

