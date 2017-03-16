///-----------------------------------------------------------------------------------------------
/// \file RotationChooser.h
/// \brief Rotation chooser activity
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_ROTATIONCHOOSER__
#define __SHIVA_ROTATIONCHOOSER__


#include <boost/filesystem.hpp>

#include "System/Activity.h"
#include "SDFView.h"


class RotationChooser : public ShivaGUI::Activity
{
public:

	//----------------------------------------------------------------------------------
	/// \brief This static method is used by the GUIManager to create an instance of our Activity when required
	//----------------------------------------------------------------------------------
	static ShivaGUI::Activity* Factory() { return new RotationChooser(); }
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
	/// \brief SDF views
	//----------------------------------------------------------------------------------
	std::vector< SDFView* > m_SDFViews;
	//----------------------------------------------------------------------------------
	/// \brief Handler to "back" button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_backButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Handle to "left" button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_leftButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Handle to "right" button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_rightButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Handle to "up" button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_upButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Handle to "down" button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_downButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Handle to "save" button
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_saveButtonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Rotation step size
	//----------------------------------------------------------------------------------
	float m_rotationStepsize;
	//----------------------------------------------------------------------------------
	/// \brief Rotation about x-axis
	//----------------------------------------------------------------------------------
	float m_rotationX;
	//----------------------------------------------------------------------------------
	/// \brief Rotation about y-axis
	//----------------------------------------------------------------------------------
	float m_rotationY;
	//----------------------------------------------------------------------------------
	/// \brief Rotation about z-axis
	//----------------------------------------------------------------------------------
	float m_rotationZ;
	//----------------------------------------------------------------------------------
	/// \brief Save directory
	//----------------------------------------------------------------------------------
	std::string m_saveDir;
	//----------------------------------------------------------------------------------
	/// \brief The saveName is the start of the filename, it will have a number after this (and then file extension)
	//----------------------------------------------------------------------------------
	std::string m_saveName;
	//----------------------------------------------------------------------------------
	/// \brief Update views
	//----------------------------------------------------------------------------------
	void UpdateViews();
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

