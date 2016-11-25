
#include "EditMenuActivity.h"

#include "System/SharedPreferences.h"
#include "GUIManager.h"

#include "Totem/Operations/TotemOpDrill.h"



void EditMenuActivity::OnCreate(ShivaGUI::Bundle *data)
{
	// This is like our constructor
	// We use it to initialise our variables and load the layouts to window

	_totemController = Totem::Controller::GetInstance();
	_totemController->ShowSelection(true);

	_selectionVolView = NULL;
	
	_buttonHandler = new UtilityEventHandler(this);
	_launchActivityHandler = new UtilityEventHandler(this);

	_rotationStepsize = 7.5f; // TODO: put in setting
	_rotationX = _rotationY = _rotationZ = 0.0f;

	_objectColourR = 0.88f;
	_objectColourG = 0.78f;
	_objectColourB = 0.54f;
	_setObjectColour = false;
	
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
	}


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

void EditMenuActivity::OnDestroy()
{
	// This is like our destructor, so delete our data etc
	delete _buttonHandler;
	delete _launchActivityHandler;
}


void EditMenuActivity::UtilityEventReceived(UtilityEventHandler *handler, ShivaGUI::View *view)
{
	// This function is called when an event is received
	// the handler we are given should be one of those we registered
	// The view we are given should be the View that generated the event (e.g. the button)
	// Here we'll change the text that is displayed based on which button is pressed
	if( handler == _buttonHandler )
	{
		if( view->GetID() == "RotateLeft" )
		{
			std::cout<<"INFO: DrillActivity request to rotate left"<<std::endl;
			_rotationZ -= _rotationStepsize;
		}
		else if( view->GetID() == "RotateRight" )
		{
			std::cout<<"INFO: DrillActivity request to rotate right"<<std::endl;
			_rotationZ += _rotationStepsize;
		}
		else if( view->GetID() == "RotateUp" )
		{
			std::cout<<"INFO: DrillActivity request to rotate up"<<std::endl;
			_rotationX -= _rotationStepsize;
		}
		else if( view->GetID() == "RotateDown" )
		{
			std::cout<<"INFO: DrillActivity request to rotate down"<<std::endl;
			_rotationX += _rotationStepsize;
		}
		else if( view->GetID() == "UndoButton" )
		{
			std::cout<<"INFO: DrillActivity request to undo operation"<<std::endl;
			_totemController->RemoveLastOperation();
			RebuildTrees();
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
	else if( handler == _launchActivityHandler )
	{
		if( view != NULL )
		{
			// Use the ID of the view as the activity name
			// If we say we expect a result we will be notified when it returns, so we can rebuild the trees
			GetGUIManager()->StartActivityForResult(view->GetID(),NULL);
		}
	}

	// Update our views
	UpdateViews();
}

void EditMenuActivity::OnActivityResult(ShivaGUI::Bundle *data)
{
	RebuildTrees();
}


void EditMenuActivity::UpdateViews()
{
	for( std::vector< std::pair<VolView*,ShivaGUI::GUIController*> >::iterator it = _VolViews.begin(); it != _VolViews.end(); ++it )
	{
		(*it).first->AddWorldRotationOffsetDegs(_rotationX,_rotationY,_rotationZ);
	}
	_rotationX = _rotationY = _rotationZ = 0.0f;

}

void EditMenuActivity::ResetRotation()
{
	for( std::vector< std::pair<VolView*,ShivaGUI::GUIController*> >::iterator it = _VolViews.begin(); it != _VolViews.end(); ++it )
	{
		(*it).first->ResetWorldRotation();
	}
	_rotationX = _rotationY = _rotationZ = 0.0f;
}

void EditMenuActivity::RebuildTrees()
{
	for( std::vector< std::pair<VolView*,ShivaGUI::GUIController*> >::iterator it = _VolViews.begin(); it != _VolViews.end(); ++it )
	{
		(*it).second->MakeCurrent();
		(*it).first->RefreshTree();
	}
}


void EditMenuActivity::InitIOWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data)
{
	// Here we are going to initialise an I/O window, which will have a couple of buttons

	// Register our UtilityEventHandlers with the GUIController.
	guiController->RegisterListener(_buttonHandler,"buttonHandler");
	guiController->RegisterListener(_launchActivityHandler,"launchActivityHandler");

	// The layout xml is where the widget hierarchy is specified
	// This function will load the file and expand the hierarchy ready for display on screen
	guiController->LoadContentView( "EditMenuIO.xml" );

	VolView *volView = dynamic_cast<VolView*>( guiController->GetResources()->GetViewFromID("MainVolView") );
	if( volView != NULL )
	{
		_VolViews.push_back( std::pair<VolView*,ShivaGUI::GUIController*>(volView,guiController) );
		if( _selectionVolView == NULL )
		{
			_selectionVolView = volView;
		}
		if( _setObjectColour )
		{
			volView->SetObjectColour(_objectColourR, _objectColourG, _objectColourB);
		}
	}

	UpdateViews();
}

void EditMenuActivity::InitOutputWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data)
{
	// This window will be used for output only, so we don't need to register the listeners as there are no buttons

	// Just load the layout xml
	// Note that this is a different xml file to the IO window, so it will show different things (i.e. no buttons)
	guiController->LoadContentView( "VolViewOutput.xml" );


	VolView *volView = dynamic_cast<VolView*>( guiController->GetResources()->GetViewFromID("MainVolView") );
	if( volView != NULL )
	{
		_VolViews.push_back( std::pair<VolView*,ShivaGUI::GUIController*>(volView,guiController) );
		if( _selectionVolView == NULL )
		{
			_selectionVolView = volView;
		}
		if( _setObjectColour )
		{
			volView->SetObjectColour(_objectColourR, _objectColourG, _objectColourB);
		}
	}

	UpdateViews();
}
