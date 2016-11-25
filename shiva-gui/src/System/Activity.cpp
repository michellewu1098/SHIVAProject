#include "System/Activity.h"
#include "GUIManager.h"

//////////////////////////////////////////////////////////////////////////
#include <iostream>


ShivaGUI::Activity::Activity()
{
	_guiManager = NULL;
	_startedForResult = false;
	_numGUIControllers = 0;
	_GUIControllers = NULL;
}

ShivaGUI::Activity::~Activity()
{
	for( unsigned int i = 0; i < _numGUIControllers ; i++ )
	{
		delete _GUIControllers[i];
	}
	delete [] _GUIControllers;
}



void ShivaGUI::Activity::Create(GUIManager *guiManager, Bundle *savedState, bool startedForResult )
{
	_guiManager = guiManager;
	_startedForResult = startedForResult;
	OnCreate(savedState);
}

void ShivaGUI::Activity::Destroy()
{
	OnDestroy();
}

void ShivaGUI::Activity::Update(float deltaTs)
{
	if( _GUIControllers != NULL )
	{
		for( unsigned int i = 0; i < _numGUIControllers; i++ )
		{
			if( _GUIControllers[i] != NULL )
			{
				_GUIControllers[i]->Update(deltaTs);
			}
		}
	}
	OnUpdate(deltaTs);
}

void ShivaGUI::Activity::Draw()
{
	if( _GUIControllers != NULL )
	{
		for( unsigned int i = 0; i < _numGUIControllers; i++ )
		{
			if( _GUIControllers[i] != NULL )
			{
				_GUIControllers[i]->Draw();
			}
		}
	}
}

void ShivaGUI::Activity::Layout()
{
	if( _GUIControllers != NULL )
	{
		for( unsigned int i = 0; i < _numGUIControllers; i++ )
		{
			if( _GUIControllers[i] != NULL )
			{
				_GUIControllers[i]->Layout();
			}
		}
	}
}

void ShivaGUI::Activity::ReturnActivityResult(Bundle *data)
{
	OnActivityResult(data);
	// delete it here? - no, deleting it in GUIManager
}

void ShivaGUI::Activity::ConfigurationChanged()
{
	OnConfigurationChanged();
}


void ShivaGUI::Activity::Finish()
{
	_guiManager->FinishActivity();
}

void ShivaGUI::Activity::SetResult(Bundle *data)
{
	_guiManager->SetActivityResult(data);
}



void ShivaGUI::Activity::SetNumGUIControllers(unsigned int numControllers)
{
	// Delete existing controllers
	for( unsigned int i = 0; i < _numGUIControllers ; i++ )
	{
		delete _GUIControllers[i];
	}
	_numGUIControllers = numControllers;
	_GUIControllers = new GUIController *[numControllers];
	for( unsigned int i = 0; i < numControllers; i++ )
		_GUIControllers[i] = NULL;
}

void ShivaGUI::Activity::AddGUIController(GUIController *controller,unsigned int index)
{
	_GUIControllers[index] = controller;
}

unsigned int ShivaGUI::Activity::GetNumGUIControllers()
{
	return _numGUIControllers;
}

ShivaGUI::GUIController* ShivaGUI::Activity::GetGUIController(unsigned int index)
{
	if( index >= _numGUIControllers )
		return NULL;
	return _GUIControllers[index];
}


void ShivaGUI::Activity::IssueEvent(InternalEvent *currentEvent)
{
	if( currentEvent->GetType() == InternalEvent::QUIT )
		_guiManager->SetExitEvent();
	else if( currentEvent->GetType() == InternalEvent::BACK )
		OnBackPressed();

	if( _GUIControllers != NULL )
	{
		for( unsigned int i = 0; i < _numGUIControllers; i++ )
		{
			GUIController *currentController = _GUIControllers[i];
			if( currentController != NULL )
			{
				currentController->IssueEvent(currentEvent);
			}
		}
	}
}



ShivaGUI::Activity::UtilityEventHandler::UtilityEventHandler(Activity *parent)
{
	_parent = parent;
}

void ShivaGUI::Activity::UtilityEventHandler::HandleEvent(View *view)
{
	_parent->UtilityEventReceived(this,view);
}

