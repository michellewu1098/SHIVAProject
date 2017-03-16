///-----------------------------------------------------------------------------------------------
/// \file ShivaMetamorphosis.h
/// \brief Metamorphosis activity
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_METAMORPHOSIS_ACTIVITY__
#define __SHIVA_METAMORPHOSIS_ACTIVITY__

#include <vector>

#include "System/Activity.h"
#include "GUI/GUIController.h"
#include "GUI/Views/Slider.h"

#include "SDFView.h"


class ShivaMetamorphosis : public ShivaGUI::Activity
{
public:

	//----------------------------------------------------------------------------------
	/// \brief This static method is used by the GUIManager to create an instance of our Activity when required
	//----------------------------------------------------------------------------------
	static ShivaGUI::Activity* Factory() { return new ShivaMetamorphosis(); }
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
	virtual void UtilityEventReceived( UtilityEventHandler *_handler, ShivaGUI::View *_view );
	//----------------------------------------------------------------------------------
	/// \brief SDF views
	//----------------------------------------------------------------------------------
	std::vector< SDFView* > m_SDFViews;
	//----------------------------------------------------------------------------------
	/// \brief Sliders
	//----------------------------------------------------------------------------------
	std::vector< ShivaGUI::Slider* > m_sliders;
	//----------------------------------------------------------------------------------
	/// \brief Handler to "back" button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_backButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Handler to "next" button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_nextButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Handler to morph slider
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_morphSliderHandler;
	//----------------------------------------------------------------------------------
	/// \brief Handler to left morphing button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_leftMorphButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Handler to right morphing button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_rightMorphButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Start bundle of data
	//----------------------------------------------------------------------------------
	ShivaGUI::Bundle *m_startBundle;
	//----------------------------------------------------------------------------------
	/// \brief Morph value
	//----------------------------------------------------------------------------------
	float m_morphValue;
	//----------------------------------------------------------------------------------
	/// \brief Morph step size
	//----------------------------------------------------------------------------------
	float m_morphStepsize;
	//----------------------------------------------------------------------------------
	/// \brief Updates SDF View and Sliders to the current m_morphValue
	/// \param [in] _issuingSlider
	//----------------------------------------------------------------------------------
	void UpdateViews( ShivaGUI::Slider *_issuingSlider = NULL );
	//----------------------------------------------------------------------------------
	/// \brief Function to load SDF view data
	/// \param [in] _sdfView
	/// \param [in] _data
	/// \param [in] _guiController
	//----------------------------------------------------------------------------------
	void LoadSDFViewData( SDFView* _sdfView, ShivaGUI::Bundle *_data, ShivaGUI::GUIController *_guiController );
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
	/// TODO
	//void InitInputWindow( ShivaGUI::GUIController *_guiController );

};

#endif

