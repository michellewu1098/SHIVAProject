///-----------------------------------------------------------------------------------------------
/// \file DrillActivity.h
/// \brief It is responsible for a 'task' or widget layout on the screen. It will load the layouts to screen and handle events from buttons
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_DRILLACTIVITY__
#define __SHIVA_DRILLACTIVITY__

#include <vector>
#include <cml/cml.h>

#include "Totem/Operations/TotemOpDrill.h"
#include "System/Activity.h"
#include "VolView.h"
#include "Totem/TotemController.h"
#include "CommandManager.h"


class DrillActivity : public ShivaGUI::Activity
{
public:

	//----------------------------------------------------------------------------------
	/// \brief This static method is used by the GUIManager to create an instance of our Activity when required
	//----------------------------------------------------------------------------------
	static ShivaGUI::Activity* Factory() { return new DrillActivity(); }
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
	/// \brief This function is called when the Activity is updated
	/// \param [in] _deltaTs
	//----------------------------------------------------------------------------------
	virtual void OnUpdate( float _deltaTs );
	//----------------------------------------------------------------------------------
	/// \brief This will handle events from buttons etc
	//----------------------------------------------------------------------------------
	virtual void UtilityEventReceived( UtilityEventHandler*, ShivaGUI::View* );
	//----------------------------------------------------------------------------------
	/// \brief These will be registered with the GUI System and will be given to the UtilityEventReceived function when an event
	/// It's not a brilliant way of doing things, more a lazy way as there's less code for the library user
	//----------------------------------------------------------------------------------
	UtilityEventHandler *m_buttonHandler;
	//----------------------------------------------------------------------------------
	/// \brief Rotation stepsize
	//----------------------------------------------------------------------------------
	float m_rotationStepsize;
	//----------------------------------------------------------------------------------
	/// \brief Rotation about x
	//----------------------------------------------------------------------------------
	float m_rotationX;
	//----------------------------------------------------------------------------------
	/// \brief Rotation about y
	//----------------------------------------------------------------------------------
	float m_rotationY;
	//----------------------------------------------------------------------------------
	/// \brief Rotation about z
	//----------------------------------------------------------------------------------
	float m_rotationZ;
	//----------------------------------------------------------------------------------
	/// \brief A list of Vol Views, mainly for updating them with new objects etc
	//----------------------------------------------------------------------------------
	std::vector< std::pair< VolView*, ShivaGUI::GUIController* > > m_volViews;
	//----------------------------------------------------------------------------------
	/// \brief Volume view selection
	//----------------------------------------------------------------------------------
	VolView *m_selectionVolView;
	//----------------------------------------------------------------------------------
	/// \brief Mainly for updating current Vol Views
	//----------------------------------------------------------------------------------
	void UpdateViews();
	//----------------------------------------------------------------------------------
	/// \brief Reset rotation
	//----------------------------------------------------------------------------------
	void ResetRotation();
	//----------------------------------------------------------------------------------
	/// \brief Rebuild volume tree
	//----------------------------------------------------------------------------------
	void RebuildTrees();
	//----------------------------------------------------------------------------------
	/// \brief Initialises a main window with input and output capabilities
	/// \param [in] _guiController
	/// \param [in[ _data
	//----------------------------------------------------------------------------------
	void InitIOWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data );
	//----------------------------------------------------------------------------------
	/// \brief Initialises a display-only window with no inputs
	//----------------------------------------------------------------------------------
	void InitOutputWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data );
	//----------------------------------------------------------------------------------
	/// \brief Handle to a totem controller
	//----------------------------------------------------------------------------------
	Totem::Controller *m_totemController;
	//----------------------------------------------------------------------------------
	/// \brief Crosshair nudge amount
	//----------------------------------------------------------------------------------
	float m_crosshairNudgeAmount;
	//----------------------------------------------------------------------------------
	/// \brief Red component of object colour
	//----------------------------------------------------------------------------------
	float m_objectColourR;
	//----------------------------------------------------------------------------------
	/// \brief Green component of object colour
	//----------------------------------------------------------------------------------
	float m_objectColourG;
	//----------------------------------------------------------------------------------
	/// \brief Blue component of object colour
	//----------------------------------------------------------------------------------
	float m_objectColourB;
	//----------------------------------------------------------------------------------
	/// \brief Whether to set object colour
	//----------------------------------------------------------------------------------
	bool m_setObjectColour;
	//----------------------------------------------------------------------------------
	/// \brief Drill size
	//----------------------------------------------------------------------------------
	float m_drillSize;
	//----------------------------------------------------------------------------------
	/// \brief Original drill size
	//----------------------------------------------------------------------------------
	float m_originalDrillSize;
	//----------------------------------------------------------------------------------
	/// \brief Drill sizestep
	//----------------------------------------------------------------------------------
	float m_drillSizeStep;
	//----------------------------------------------------------------------------------
	/// \brief Reference to command manager
	//----------------------------------------------------------------------------------
	Totem::CommandManager* m_commandManager;
	//----------------------------------------------------------------------------------

};

#endif

