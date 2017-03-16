
#ifndef __SHIVA_MODELCHOOSER__
#define __SHIVA_MODELCHOOSER__

#include "System/Activity.h"


class ModelChooser : public ShivaGUI::Activity
{
public:
	static ShivaGUI::Activity* Factory() {return new ModelChooser();}

protected:
	virtual void OnCreate(ShivaGUI::Bundle*);

	virtual void OnDestroy();

	virtual void UtilityEventReceived(UtilityEventHandler*,ShivaGUI::View*);

	UtilityEventHandler *m_backButtonHandler, *m_modelSelectButtonHandler;

	int m_modelA, m_modelB;

	/// Initialises a main window with input and output capabilities
	void InitIOWindow(ShivaGUI::GUIController *guiController);

	/// Initialises a display-only window with no inputs
	void InitOutputWindow(ShivaGUI::GUIController *guiController);
};

#endif

