
#include "NudgeActivity.h"
#include "ShivaModelManager.h"

#include "System/SharedPreferences.h"
#include "GUIManager.h"

#include <boost/filesystem.hpp>



void NudgeActivity::OnCreate(ShivaGUI::Bundle *data)
{
	// This is like our constructor
	// We use it to initialise our variables and load the layouts to window

	_totemController = Totem::Controller::GetInstance();

	_objectColourR = 0.88f;
	_objectColourG = 0.78f;
	_objectColourB = 0.54f;
	_setObjectColour = false;

	_rotationStepsize = 7.5f;
	_rotationX = _rotationY = _rotationZ = 0.0f;

	_nudgeStep = 0.1f;
	
	ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();
	if( prefs != NULL )
	{
		if( prefs->Contains("ObjectColourR") && prefs->Contains("ObjectColourG") && prefs->Contains("ObjectColourB") )
		{
			_objectColourR = prefs->GetFloat("ObjectColourR",_objectColourR);
			_objectColourG = prefs->GetFloat("ObjectColourG",_objectColourG);
			_objectColourB = prefs->GetFloat("ObjectColourB",_objectColourB);
			_setObjectColour = true;
		}
		
		_rotationStepsize = prefs->GetFloat("RotationStepsize",_rotationStepsize);
		_nudgeStep = prefs->GetFloat("NudgeStep",_nudgeStep);
	}

	_buttonHandler = new UtilityEventHandler(this);


	// We will now tell the system what to display on the windows

	// Find out how many windows we have
	int numWindows = GetNumGUIControllers();

	// For each of these...
	for( int i = 0; i < numWindows; i++ )
	{
		// We retrieve a GUIController
		// This is our main method of accessing the resources associated with a Window
		ShivaGUI::GUIController *guiController = GetGUIController(i);

		// The User Profile can specify how the windows are supposed to be used
		// We can retrieve this using the GUIController and load a different window layout
		ShivaGUI::Window::RequestedUse windowUse = guiController->GetRequestedUse();
		if( windowUse == ShivaGUI::Window::OUTPUT )
		{
			// This window should be used for output information only, with no buttons etc

			InitOutputWindow(guiController,data);
		}
		else
		{
			// We just assume that this is a generic input/output window

			InitIOWindow(guiController,data);
		}
	}

}

void NudgeActivity::OnResume()
{
}


void NudgeActivity::OnDestroy()
{
	// This is like our destructor, so delete our data etc
	delete _buttonHandler;
}


void NudgeActivity::OnUpdate(float deltaTs)
{
}

void NudgeActivity::UtilityEventReceived(UtilityEventHandler *handler, ShivaGUI::View *view)
{
	// This function is called when an event is received
	// the handler we are given should be one of those we registered
	// The view we are given should be the View that generated the event (e.g. the button)
	// Here we'll change the text that is displayed based on which button is pressed
	if( handler == _buttonHandler )
	{
		if( view->GetID() == "RotateLeft" )
		{
			std::cout<<"INFO: AssembleActivity request to rotate left"<<std::endl;
			_rotationZ -= _rotationStepsize;
		}
		else if( view->GetID() == "RotateRight" )
		{
			std::cout<<"INFO: AssembleActivity request to rotate right"<<std::endl;
			_rotationZ += _rotationStepsize;
		}
		else if( view->GetID() == "RotateUp" )
		{
			std::cout<<"INFO: AssembleActivity request to rotate up"<<std::endl;
			_rotationX -= _rotationStepsize;
		}
		else if( view->GetID() == "RotateDown" )
		{
			std::cout<<"INFO: AssembleActivity request to rotate down"<<std::endl;
			_rotationX += _rotationStepsize;
		}
		else if( view->GetID() == "DeleteSelected" )
		{
			std::cout<<"INFO: AssembleActivity request to delete selected object"<<std::endl;
			_totemController->DeleteSelectedObject();
			RebuildTrees();
		}
		else if( view->GetID() == "SelectAbove" )
		{
			std::cout<<"INFO: AssembleActivity request to select object above"<<std::endl;
			_totemController->SelectObjectAbove();
		}
		else if( view->GetID() == "SelectBelow" )
		{
			std::cout<<"INFO: AssembleActivity request to select object below"<<std::endl;
			_totemController->SelectObjectBelow();
		}
		else if( view->GetID() == "SwapAbove" )
		{
			std::cout<<"INFO: AssembleActivity request to swap with object above"<<std::endl;
			_totemController->ReorderSelectedObject(true);
			RebuildTrees();
		}
		else if( view->GetID() == "SwapBelow" )
		{
			std::cout<<"INFO: AssembleActivity request to swap with object below"<<std::endl;
			_totemController->ReorderSelectedObject(false);
			RebuildTrees();
		}
		else if( view->GetID() == "NudgeUp" || view->GetID() == "NudgeZpositive" )
		{
			std::cout<<"INFO: AssembleActivity request to nudge object upward"<<std::endl;
			_totemController->MoveSelectedObject(0.0f,0.0f,_nudgeStep);
			RebuildTrees(true);
		}
		else if( view->GetID() == "NudgeDown" || view->GetID() == "NudgeZnegative" )
		{
			std::cout<<"INFO: AssembleActivity request to nudge object downward"<<std::endl;
			_totemController->MoveSelectedObject(0.0f,0.0f,-_nudgeStep);
			RebuildTrees(true);
		}
		else if( view->GetID() == "NudgeXpositive" )
		{
			std::cout<<"INFO: AssembleActivity request to nudge object on positive X"<<std::endl;
			_totemController->MoveSelectedObject(_nudgeStep,0.0f,0.0f);
			RebuildTrees(true);
		}
		else if( view->GetID() == "NudgeXnegative" )
		{
			std::cout<<"INFO: AssembleActivity request to nudge object on negative X"<<std::endl;
			_totemController->MoveSelectedObject(-_nudgeStep,0.0f,0.0f);
			RebuildTrees(true);
		}
		else if( view->GetID() == "NudgeYpositive" )
		{
			std::cout<<"INFO: AssembleActivity request to nudge object on positive X"<<std::endl;
			_totemController->MoveSelectedObject(0.0f,_nudgeStep,0.0f);
			RebuildTrees(true);
		}
		else if( view->GetID() == "NudgeYnegative" )
		{
			std::cout<<"INFO: AssembleActivity request to nudge object on negative X"<<std::endl;
			_totemController->MoveSelectedObject(0.0f,-_nudgeStep,0.0f);
			RebuildTrees(true);
		}
		else if( view->GetID() == "NudgeReset" )
		{
			std::cout<<"INFO: AssembleActivity request to reset nudge"<<std::endl;
			_totemController->ResetMoveSelected();
			RebuildTrees(true);
		}
		else if( view->GetID() == "RotateReset" )
		{
			ResetRotation();
		}
		else if( view->GetID() == "BackButton" )
		{
			_totemController->ShowSelection(true);
			Finish();
		}
	}
	// Update our views
	UpdateViews();
}

void NudgeActivity::OnActivityResult(ShivaGUI::Bundle *data)
{
	RebuildTrees();
}


void NudgeActivity::UpdateViews()
{
	for( std::vector< std::pair<VolView*,ShivaGUI::GUIController*> >::iterator it = _VolViews.begin(); it != _VolViews.end(); ++it )
	{
		(*it).first->AddWorldRotationOffsetDegs(_rotationX,_rotationY,_rotationZ);
	}
	_rotationX = _rotationY = _rotationZ = 0.0f;

}

void NudgeActivity::ResetRotation()
{
	for( std::vector< std::pair<VolView*,ShivaGUI::GUIController*> >::iterator it = _VolViews.begin(); it != _VolViews.end(); ++it )
	{
		(*it).first->ResetWorldRotation();
	}
	_rotationX = _rotationY = _rotationZ = 0.0f;
}

void NudgeActivity::RebuildTrees(bool justparams)
{
	for( std::vector< std::pair<VolView*,ShivaGUI::GUIController*> >::iterator it = _VolViews.begin(); it != _VolViews.end(); ++it )
	{
		(*it).second->MakeCurrent();
		if( justparams )
		{
			(*it).first->RefreshTree();
			//(*it).first->RefreshTreeParams();
		}
		else
		{
			(*it).first->RefreshTree();
		}
	}
}


void NudgeActivity::InitIOWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data)
{
	// Here we are going to initialise an I/O window, which will have a couple of buttons

	// Register our UtilityEventHandlers with the GUIController.
	guiController->RegisterListener(_buttonHandler,"buttonHandler");

	// The layout xml is where the widget hierarchy is specified
	// This function will load the file and expand the hierarchy ready for display on screen
	guiController->LoadContentView( "NudgeIO.xml" );

	// Retrieve primitive list
	VolView *volView = dynamic_cast<VolView*>( guiController->GetResources()->GetViewFromID("MainVolView") );
	if( volView != NULL )
	{
		// For the assemble activity we want to allow the user to click to select primitives
		volView->AllowObjectClickSelection(true);
		_VolViews.push_back( std::pair<VolView*,ShivaGUI::GUIController*>(volView,guiController) );
		if( _setObjectColour )
		{
			volView->SetObjectColour(_objectColourR, _objectColourG, _objectColourB);
		}
	}

	UpdateViews();
}

void NudgeActivity::InitOutputWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data)
{
	// This window will be used for output only, so we don't need to register the listeners as there are no buttons

	// Just load the layout xml
	// Note that this is a different xml file to the IO window, so it will show different things (i.e. no buttons)
	guiController->LoadContentView( "VolViewOutput.xml" );


	VolView *volView = dynamic_cast<VolView*>( guiController->GetResources()->GetViewFromID("MainVolView") );
	if( volView != NULL )
	{
		_VolViews.push_back( std::pair<VolView*,ShivaGUI::GUIController*>(volView,guiController) );
		if( _setObjectColour )
		{
			volView->SetObjectColour(_objectColourR, _objectColourG, _objectColourB);
		}
	}
	UpdateViews();
}
