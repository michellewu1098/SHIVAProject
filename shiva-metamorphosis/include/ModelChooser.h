///-----------------------------------------------------------------------------------------------
/// \file ModelChooser.h
/// \brief Model chooser activity
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_MODELCHOOSER__
#define __SHIVA_MODELCHOOSER__

#include "System/Activity.h"


class ModelChooser : public ShivaGUI::Activity
{
public:

	//----------------------------------------------------------------------------------
	/// \brief This static method is used by the GUIManager to create an instance of our Activity when required
	//----------------------------------------------------------------------------------
	static ShivaGUI::Activity* Factory() { return new ModelChooser(); }
	//----------------------------------------------------------------------------------

protected:

	//----------------------------------------------------------------------------------
	/// \brief This function is called when the Activity is started
	/// \param [in] _data
	//----------------------------------------------------------------------------------
	virtual void OnCreate( ShivaGUI::Bundle* _data );
	//----------------------------------------------------------------------------------
	/// \brief This function is called when the Activity is stopped
	//----------------------------------------------------------------------------------
	virtual void OnDestroy();
	//----------------------------------------------------------------------------------
	/// \brief This will handle events from buttons etc
	/// \param [in] _handler
	/// \param [in] _view
	//----------------------------------------------------------------------------------
	virtual void UtilityEventReceived( UtilityEventHandler*, ShivaGUI::View* );
	//----------------------------------------------------------------------------------
	/// \brief Handler to "back" button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_backButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Handle to model sector button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_modelSelectButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Model A
	//----------------------------------------------------------------------------------
	int m_modelA;
	//----------------------------------------------------------------------------------
	/// \brief Model B
	//----------------------------------------------------------------------------------
	int m_modelB;
	//----------------------------------------------------------------------------------
	/// \brief Initialises a main window with input and output capabilities
	/// \param [in] _guiController
	//----------------------------------------------------------------------------------
	void InitIOWindow( ShivaGUI::GUIController *_guiController );
	//----------------------------------------------------------------------------------
	/// \brief Initialises a display-only window with no inputs
	/// \param [in] _guiController
	//----------------------------------------------------------------------------------
	void InitOutputWindow( ShivaGUI::GUIController *_guiController );
	//----------------------------------------------------------------------------------

};

#endif

