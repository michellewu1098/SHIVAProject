
#include "RotateObjectActivity.h"

#include "System/SharedPreferences.h"
#include "GUIManager.h"




void RotateObjectActivity::OnCreate(ShivaGUI::Bundle *data)
{
	// This is like our constructor
	// We use it to initialise our variables and load the layouts to window

	_totemController = Totem::Controller::GetInstance();
	_totemController->ShowSelection(true);
	
	_buttonHandler = new UtilityEventHandler(this);

	_rotationStepsize = (3.14159265358979323846f/24.0f); // TODO: put in setting
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
		
		_rotationStepsize = prefs->GetFloat("RotateObjectStepRadians",_rotationStepsize);
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

void RotateObjectActivity::OnDestroy()
{
	// This is like our destructor, so delete our data etc
	delete _buttonHandler;
}


void RotateObjectActivity::UtilityEventReceived(UtilityEventHandler *handler, ShivaGUI::View *view)
{
	// This function is called when an event is received
	// the handler we are given should be one of those we registered
	// The view we are given should be the View that generated the event (e.g. the button)
	// Here we'll change the text that is displayed based on which button is pressed
	if( handler == _buttonHandler )
	{
		if( view->GetID() == "RotateLeft" )
		{
			_rotationZ -= _rotationStepsize;
		}
		else if( view->GetID() == "RotateRight" )
		{
			_rotationZ += _rotationStepsize;
		}
		else if( view->GetID() == "RotateUp" )
		{
			_rotationX -= _rotationStepsize;
		}
		else if( view->GetID() == "RotateDown" )
		{
			_rotationX += _rotationStepsize;
		}
		else if( view->GetID() == "RotateYUp" )
		{
			_rotationY -= _rotationStepsize;
		}
		else if( view->GetID() == "RotateYDown" )
		{
			_rotationY += _rotationStepsize;
		}
		else if( view->GetID() == "RotateZUp" )
		{
			_rotationZ -= _rotationStepsize;
		}
		else if( view->GetID() == "RotateZDown" )
		{
			_rotationZ += _rotationStepsize;
		}
		else if( view->GetID() == "RotateReset" )
		{
			ResetRotation();
		}
		else if( view->GetID() == "SelectAbove" )
		{
			_totemController->SelectObjectAbove();
		}
		else if( view->GetID() == "SelectBelow" )
		{
			_totemController->SelectObjectBelow();
		}
		else if( view->GetID() == "BackButton" )
		{
			_totemController->ShowSelection(true);
			Finish();
		}

		// Update our views
		UpdateViews();
		RebuildTrees(true);
	}

}

void RotateObjectActivity::OnActivityResult(ShivaGUI::Bundle *data)
{
	RebuildTrees();
}


void RotateObjectActivity::UpdateViews()
{
/*	for( std::vector< std::pair<VolView*,ShivaGUI::GUIController*> >::iterator it = _VolViews.begin(); it != _VolViews.end(); ++it )
	{
		(*it).first->AddWorldRotationOffsetDegs(_rotationX,_rotationY,_rotationZ);
	}
*/
	if( _totemController != NULL )
	{
		Totem::Object *currentObject = _totemController->GetSelected();
		if( currentObject != NULL )
		{
			std::cout<<"INFO: adding object rotation: "<<_rotationX<<" "<<_rotationY<<" "<<_rotationZ<<std::endl;
			currentObject->AddRotation(_rotationX,_rotationY,_rotationZ);
		}
	}
	_rotationX = _rotationY = _rotationZ = 0.0f;

}

void RotateObjectActivity::ResetRotation()
{
	for( std::vector< std::pair<VolView*,ShivaGUI::GUIController*> >::iterator it = _VolViews.begin(); it != _VolViews.end(); ++it )
	{
		(*it).first->ResetWorldRotation();
	}
	_rotationX = _rotationY = _rotationZ = 0.0f;
}

void RotateObjectActivity::RebuildTrees(bool justparams)
{
	for( std::vector< std::pair<VolView*,ShivaGUI::GUIController*> >::iterator it = _VolViews.begin(); it != _VolViews.end(); ++it )
	{
		(*it).second->MakeCurrent();
		if( justparams )
		{
			//(*it).first->RefreshTreeParams();
			(*it).first->RefreshTree();
		}
		else
		{
			(*it).first->RefreshTree();
		}
	}
}


void RotateObjectActivity::InitIOWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data)
{
	// Here we are going to initialise an I/O window, which will have a couple of buttons

	// Register our UtilityEventHandlers with the GUIController.
	guiController->RegisterListener(_buttonHandler,"buttonHandler");

	// The layout xml is where the widget hierarchy is specified
	// This function will load the file and expand the hierarchy ready for display on screen
	guiController->LoadContentView( "RotateObjectIO.xml" );

	VolView *volView = dynamic_cast<VolView*>( guiController->GetResources()->GetViewFromID("MainVolView") );
	if( volView != NULL )
	{
		_VolViews.push_back( std::pair<VolView*,ShivaGUI::GUIController*>(volView,guiController) );
		volView->AllowObjectClickSelection(true);
		if( _setObjectColour )
		{
			volView->SetObjectColour(_objectColourR, _objectColourG, _objectColourB);
		}
	}

	UpdateViews();
}

void RotateObjectActivity::InitOutputWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data)
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
