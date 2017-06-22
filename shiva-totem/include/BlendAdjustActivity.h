///-----------------------------------------------------------------------------------------------
/// \file BlendAdjustActivity.h
/// \brief It is responsible for a 'task' or widget layout on the screen. It will load the layouts to screen and handle events from buttons
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_BLENDADJUSTACTIVITY__
#define __SHIVA_BLENDADJUSTACTIVITY__

#include <vector>

#include "System/Activity.h"
#include "VolView.h"
#include "Totem/TotemController.h"
#include "Totem/Operations/TotemOpDrill.h"
#include "CommandManager.h"


class BlendAdjustActivity : public ShivaGUI::Activity
{
public:

	//----------------------------------------------------------------------------------
	/// \brief This static method is used by the GUIManager to create an instance of our Activity when required
	//----------------------------------------------------------------------------------
	static ShivaGUI::Activity* Factory() { return new BlendAdjustActivity(); }
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
	//----------------------------------------------------------------------------------
	virtual void UtilityEventReceived( UtilityEventHandler*, ShivaGUI::View* );
	//----------------------------------------------------------------------------------
	/// \brief When you start an activity that is supposed to return a result, this function is called when that activity returns the result
	/// The Bundle will contain the data that was passed back from the
	/// \param [in] _data
	//----------------------------------------------------------------------------------
	virtual void OnActivityResult( ShivaGUI::Bundle *data );
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
	/// \brief Initial blending amount
	//----------------------------------------------------------------------------------	
	float m_initialBlending;
	//----------------------------------------------------------------------------------
	/// \brief A list of Vol Views, mainly for updating them with new objects etc
	//----------------------------------------------------------------------------------
	std::vector< std::pair< VolView*, ShivaGUI::GUIController* > > m_volViews;
	//----------------------------------------------------------------------------------
	/// \brief Mainly for updating current Vol Views
	//----------------------------------------------------------------------------------
	void UpdateViews();
	//----------------------------------------------------------------------------------
	/// \brief Used to reset rotation
	//----------------------------------------------------------------------------------
	void ResetRotation();
	//----------------------------------------------------------------------------------
	/// \brief Used to rebuild volume tree
	//----------------------------------------------------------------------------------
	void RebuildTrees();
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
	/// \brief Handle to a totem controller
	//----------------------------------------------------------------------------------
	Totem::Controller *m_totemController;
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
	/// \brief Reference to command manager
	//----------------------------------------------------------------------------------
	Totem::CommandManager* m_commandManager;
	//----------------------------------------------------------------------------------

};

#endif

