
#include "GUI/GUIController.h"
#include "GUIManager.h"

//////////////////////////////////////////////////////////////////////////
#include <GL/GLee.h>
#include <SDL.h>
#include <iostream>

ShivaGUI::GUIController::GUIController(Window *inputWindow, ResourceManager *resources, GUIManager *guiman)
{
	_attachedWindow = inputWindow;
	_resourceManager = resources;
	_GUIManager = guiman;
	_contentView = NULL;
}

ShivaGUI::GUIController::~GUIController()
{
	delete _contentView;
}

void ShivaGUI::GUIController::RegisterViewID(ShivaGUI::View *view, std::string ID)
{
	if( _viewIDs.find(ID) != _viewIDs.end() )
		std::cout<<"WARNING: GUIController registering multiple Views of ID: "<<ID<<std::endl;
	_viewIDs[ID] = view;
}

ShivaGUI::View* ShivaGUI::GUIController::GetViewFromID(std::string ID)
{
	std::map<std::string,View*>::iterator it = _viewIDs.find(ID);
	if( it != _viewIDs.end() )
		return (*it).second;
	return NULL;
}

void ShivaGUI::GUIController::ChangeWindowSize(unsigned int width, unsigned int height)
{
	if( _attachedWindow != NULL )
		_attachedWindow->SetSize(width,height);
	Layout();
}

void ShivaGUI::GUIController::GetWindowSize(unsigned int &width, unsigned int &height)
{
	if( _attachedWindow != NULL )
		_attachedWindow->GetSize(width,height);
}

bool ShivaGUI::GUIController::GetFullScreen()
{
	if( _attachedWindow != NULL )
		return _attachedWindow->GetFullScreen();
	return false;
}

ShivaGUI::Window::RequestedUse ShivaGUI::GUIController::GetRequestedUse()
{
	if( _attachedWindow != NULL )
		return _attachedWindow->GetRequestedUse();
	return Window::ANYTHING;
}


void ShivaGUI::GUIController::RegisterListener(ShivaGUI::ViewEventListener *listener, std::string name)
{
	_eventListeners[name] = listener;
}

ShivaGUI::ViewEventListener* ShivaGUI::GUIController::GetListener(std::string name)
{
	return _eventListeners[name];
}




void ShivaGUI::GUIController::Layout()
{
	if( _attachedWindow != NULL && _contentView != NULL )
	{
		_attachedWindow->MakeCurrent();
		unsigned int resX, resY;
		_attachedWindow->GetSize(resX,resY);
		_contentView->Layout(0,0,resX,resY, resX,resY);
	}
}




bool ShivaGUI::GUIController::IssueEvent( InternalEvent *currentEvent )
{
	if( currentEvent->GetType() == InternalEvent::WINDOW_RESIZE )
		Layout();
	else if( currentEvent->GetType() == InternalEvent::QUIT )
		_GUIManager->SetExitEvent(); // This event should never be here, as it's not associated with a Window, but just in case
//	else if( currentEvent->GetType() == InternalEvent::BACK )
//		_GUIManager->GetCurrentActivity()->IssueEvent(currentEvent);

	if( _contentView != NULL ) {
		return _contentView->HandleEvent(currentEvent);
	}
	return false;
}




void ShivaGUI::GUIController::Update(float deltaTs)
{
	// Update content View
	if( _contentView != NULL )
		_contentView->Update(deltaTs,this);
}

void ShivaGUI::GUIController::Draw()
{
	if( _attachedWindow == NULL || _contentView == NULL )
		return;

	_attachedWindow->MakeCurrent();
	unsigned int resX, resY;
	_attachedWindow->GetSize(resX,resY);
	// Set up OpenGL
	// Remember that OpenGL uses lower-left corner as origin
	// Viewport function wants lower-left corner too
	glViewport(0,0,resX,resY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f,(float)resX,(float)resY,0.0f,-1.0f,1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	_contentView->Draw();

	_attachedWindow->SwapBuffers();
}

void ShivaGUI::GUIController::LoadContentView(std::string layoutName)
{
	_contentViewFilename = layoutName;
	_attachedWindow->MakeCurrent();
	_contentView = GetResources()->GetLayout(layoutName);
}

void ShivaGUI::GUIController::SaveLayoutToProfile()
{
	_GUIManager->SaveLayoutToProfile(_contentViewFilename, _contentView, _resourceManager);
}


