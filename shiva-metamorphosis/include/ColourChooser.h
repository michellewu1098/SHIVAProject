///-----------------------------------------------------------------------------------------------
/// \file ColourChooser.h
/// \brief Colour chooser activity
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_COLOURCHOOSER__
#define __SHIVA_COLOURCHOOSER__

#include "System/Activity.h"
#include "GUI/Views/ColourSelector.h"
#include "SDFView.h"


class ColourChooser : public ShivaGUI::Activity
{
public:

	//----------------------------------------------------------------------------------
	/// \brief This static method is used by the GUIManager to create an instance of our Activity when required
	//----------------------------------------------------------------------------------
	static ShivaGUI::Activity* Factory() { return new ColourChooser(); }
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
	/// \brief Handler to "next" button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_nextButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Handler to colour changer
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_colourChangeHandler;
	//----------------------------------------------------------------------------------
	/// \brief Handler to model select button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_modelSelectButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Handler to left column button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_leftColButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Handler to right column button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_rightColButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief SDF views
	//----------------------------------------------------------------------------------
	std::vector< SDFView* > m_SDFViews;
	//----------------------------------------------------------------------------------
	/// \brief Colour selectors
	//----------------------------------------------------------------------------------
	std::vector< ShivaGUI::ColourSelector* > m_selectors;
	//----------------------------------------------------------------------------------
	/// \brief Colour X
	//----------------------------------------------------------------------------------
	float m_colourX;
	//----------------------------------------------------------------------------------
	/// \brief Colour Y
	//----------------------------------------------------------------------------------
	float m_colourY;
	//----------------------------------------------------------------------------------
	/// \brief Colour stepsize
	//----------------------------------------------------------------------------------
	float m_colourStepsize;
	//----------------------------------------------------------------------------------
	/// \brief Start bundle of data
	//----------------------------------------------------------------------------------
	ShivaGUI::Bundle *m_startBundle;
	//----------------------------------------------------------------------------------
	/// \brief Update views
	/// \param [in] _issuingSelector
	//----------------------------------------------------------------------------------
	void UpdateViews( ShivaGUI::ColourSelector *_issuingSelector = NULL );
	//----------------------------------------------------------------------------------
	/// \brief Function to load SDF view data
	/// \param [in] _sdfView
	/// \param [in] _data
	/// \param [in] _guiController
	//----------------------------------------------------------------------------------
	void LoadSDFViewData( SDFView*, ShivaGUI::Bundle *_data, ShivaGUI::GUIController* );
	//----------------------------------------------------------------------------------
	/// \brief Initialises a main window with input and output capabilities
	/// \param [in] _guiController
	/// \param [in] _data
	//----------------------------------------------------------------------------------
	void InitIOWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data );
	//----------------------------------------------------------------------------------
	/// \brief Initialises a display-only window with no inputs
	/// \param [in] _guiController
	/// \param [in] _data
	//----------------------------------------------------------------------------------
	void InitOutputWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data );
	//----------------------------------------------------------------------------------
};

#endif

