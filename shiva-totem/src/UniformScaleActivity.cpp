
#include "UniformScaleActivity.h"

#include "System/SharedPreferences.h"
#include "GUIManager.h"




void UniformScaleActivity::OnCreate(ShivaGUI::Bundle *data)
{
	// This is like our constructor
	// We use it to initialise our variables and load the layouts to window

	_totemController = Totem::Controller::GetInstance();
	_totemController->ShowSelection(true);
	
	_buttonHandler = new UtilityEventHandler(this);

	_rotationStepsize = 7.5f;
	_rotationX = _rotationY = _rotationZ = 0.0f;

	_scaleStepsize = 0.1f;
	
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
		
		_rotationStepsize = prefs->GetFloat("RotationStepsize",_rotationStepsize);

		_scaleStepsize = prefs->GetFloat("ScaleStepsize",_scaleStepsize);
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

void UniformScaleActivity::OnDestroy()
{
	// This is like our destructor, so delete our data etc
	delete _buttonHandler;
}


void UniformScaleActivity::UtilityEventReceived(UtilityEventHandler *handler, ShivaGUI::View *view)
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
		else if( view->GetID() == "ScaleUp" )
		{
			if( _totemController != NULL )
			{
				Totem::Object *currentObject = _totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleX += _scaleStepsize;
					scaleY += _scaleStepsize;
					scaleZ += _scaleStepsize;
					if( scaleX > 2.0f )
						scaleX = 2.0f;
					if( scaleY > 2.0f )
						scaleY = 2.0f;
					if( scaleZ > 2.0f )
						scaleZ = 2.0f;
					std::cout<<"INFO: scaling up object to: "<< scaleX <<" "<< scaleY <<" "<< scaleZ <<std::endl;
					currentObject->SetScale(scaleX,scaleY,scaleZ);
				}
			}
		}
		else if( view->GetID() == "ScaleDown" )
		{
			if( _totemController != NULL )
			{
				Totem::Object *currentObject = _totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleX -= _scaleStepsize;
					scaleY -= _scaleStepsize;
					scaleZ -= _scaleStepsize;
					if( scaleX < 0.1f )
						scaleX = 0.1f;
					if( scaleY < 0.1f )
						scaleY = 0.1f;
					if( scaleZ < 0.1f )
						scaleZ = 0.1f;
					std::cout<<"INFO: scaling down object to: "<< scaleX <<" "<< scaleY <<" "<< scaleZ <<std::endl;
					currentObject->SetScale(scaleX,scaleY,scaleZ);
				}
			}
		}
		else if( view->GetID() == "ScaleXUp" )
		{
			if( _totemController != NULL )
			{
				Totem::Object *currentObject = _totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleX += _scaleStepsize;
					if( scaleX > 2.0f )
						scaleX = 2.0f;
					std::cout<<"INFO: scaling up object in X to: "<< scaleX <<std::endl;
					currentObject->SetScale(scaleX,scaleY,scaleZ);
				}
			}
		}
		else if( view->GetID() == "ScaleXDown" )
		{
			if( _totemController != NULL )
			{
				Totem::Object *currentObject = _totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleX -= _scaleStepsize;
					if( scaleX < 0.1f )
						scaleX = 0.1f;
					std::cout<<"INFO: scaling down object in X to: "<< scaleX <<std::endl;
					currentObject->SetScale(scaleX,scaleY,scaleZ);
				}
			}
		}
		else if( view->GetID() == "ScaleYUp" )
		{
			if( _totemController != NULL )
			{
				Totem::Object *currentObject = _totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleY += _scaleStepsize;
					if( scaleY > 2.0f )
						scaleY = 2.0f;
					std::cout<<"INFO: scaling up object in Y to: "<< scaleY <<std::endl;
					currentObject->SetScale(scaleX,scaleY,scaleZ);
				}
			}
		}
		else if( view->GetID() == "ScaleYDown" )
		{
			if( _totemController != NULL )
			{
				Totem::Object *currentObject = _totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleY -= _scaleStepsize;
					if( scaleY < 0.1f )
						scaleY = 0.1f;
					std::cout<<"INFO: scaling down object in Y to: "<< scaleY <<std::endl;
					currentObject->SetScale(scaleX,scaleY,scaleZ);
				}
			}
		}
		else if( view->GetID() == "ScaleZUp" )
		{
			if( _totemController != NULL )
			{
				Totem::Object *currentObject = _totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleZ += _scaleStepsize;
					if( scaleZ > 2.0f )
						scaleZ = 2.0f;
					std::cout<<"INFO: scaling up object in Z to: "<< scaleZ <<std::endl;
					currentObject->SetScale(scaleX,scaleY,scaleZ);
				}
			}
		}
		else if( view->GetID() == "ScaleZDown" )
		{
			if( _totemController != NULL )
			{
				Totem::Object *currentObject = _totemController->GetSelected();
				if( currentObject != NULL )
				{
					float scaleX = currentObject->GetScaleX();
					float scaleY = currentObject->GetScaleY();
					float scaleZ = currentObject->GetScaleZ();
					scaleZ -= _scaleStepsize;
					if( scaleZ < 0.1f )
						scaleZ = 0.1f;
					std::cout<<"INFO: scaling down object in Z to: "<< scaleZ <<std::endl;
					currentObject->SetScale(scaleX,scaleY,scaleZ);
				}
			}
		}
		else if( view->GetID() == "ScaleReset" )
		{
			if( _totemController != NULL )
			{
				Totem::Object *currentObject = _totemController->GetSelected();
				if( currentObject != NULL )
				{
					std::cout<<"INFO: scaling reset object to: 1"<<std::endl;
					currentObject->SetScale(1.0f,1.0f,1.0f);
				}
			}
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

void UniformScaleActivity::OnActivityResult(ShivaGUI::Bundle *data)
{
	RebuildTrees();
}


void UniformScaleActivity::UpdateViews()
{
	for( std::vector< std::pair<VolView*,ShivaGUI::GUIController*> >::iterator it = _VolViews.begin(); it != _VolViews.end(); ++it )
	{
		(*it).first->AddWorldRotationOffsetDegs(_rotationX,_rotationY,_rotationZ);
	}
	_rotationX = _rotationY = _rotationZ = 0.0f;

}

void UniformScaleActivity::ResetRotation()
{
	for( std::vector< std::pair<VolView*,ShivaGUI::GUIController*> >::iterator it = _VolViews.begin(); it != _VolViews.end(); ++it )
	{
		(*it).first->ResetWorldRotation();
	}
	_rotationX = _rotationY = _rotationZ = 0.0f;
}

void UniformScaleActivity::RebuildTrees(bool justparams)
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


void UniformScaleActivity::InitIOWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data)
{
	// Here we are going to initialise an I/O window, which will have a couple of buttons

	// Register our UtilityEventHandlers with the GUIController.
	guiController->RegisterListener(_buttonHandler,"buttonHandler");

	// The layout xml is where the widget hierarchy is specified
	// This function will load the file and expand the hierarchy ready for display on screen
	guiController->LoadContentView( "UniformScaleIO.xml" );

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

void UniformScaleActivity::InitOutputWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data)
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
