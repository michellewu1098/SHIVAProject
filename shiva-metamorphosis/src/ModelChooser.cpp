#include "ModelChooser.h"
#include "ShivaModelManager.h"
#include "GUIManager.h"
#include "GUI/Views/ImageButton.h"
#include "GUI/Views/TextView.h"
#include "GUI/Views/TextButton.h"
#include "GUI/Drawables/BitmapDrawable.h"

void ModelChooser::OnCreate(ShivaGUI::Bundle *data)
{
	// See if the data we have been given contains the model IDs
	_modelA = _modelB = -1;
	if( data != NULL )
	{
		if( data->ContainsKey("ModelA") )
		{
			_modelA = data->GetInteger("ModelA");
		}
		if( data->ContainsKey("ModelB") )
		{
			std::cerr<<"ERROR: ModelChooser::OnCreate, already selected ModelB"<<std::endl;
		}
	}

	// These handlers will be called when the buttons are pressed
	_backButtonHandler = new UtilityEventHandler(this);
	_modelSelectButtonHandler = new UtilityEventHandler(this);

	// Find out how many windows we have
	int numWindows = GetNumGUIControllers();
	for( int i = 0; i < numWindows; i++ )
	{
		// Load content for our windows, depending on how the profile says they should be used
		ShivaGUI::GUIController *guiController = GetGUIController(i);
		ShivaGUI::Window::RequestedUse windowUse = guiController->GetRequestedUse();
		if( windowUse == ShivaGUI::Window::OUTPUT )
		{
			InitOutputWindow(guiController);
		}
		else
		{
			InitIOWindow(guiController);
		}
	}

}

void ModelChooser::OnDestroy()
{
	delete _backButtonHandler;
	delete _modelSelectButtonHandler;
}


void ModelChooser::UtilityEventReceived( UtilityEventHandler *handler, ShivaGUI::View *view )
{
	if( handler == _backButtonHandler )
	{
		Finish();
	}
	else if( handler == _modelSelectButtonHandler )
	{

		ShivaGUI::Bundle *resultBundle = new ShivaGUI::Bundle();

		std::string modelIDstring = view->GetID();
		std::istringstream strStream(modelIDstring);
		int modelID = -1;
		if( !(strStream>>modelID) )
		{
			std::cerr<<"WARNING: ModelChooser::UtilityEventReceived no valid model selected, unsure what to do..."<<std::endl;
		}
		else
		{
			if( _modelA >= 0 )
			{
				resultBundle->PutInteger("ModelA",_modelA);
				resultBundle->PutInteger("ModelB",modelID);
				GetGUIManager()->StartActivity("ShivaMetamorphosis",resultBundle);
			}
			else
			{
				resultBundle->PutInteger("ModelA",modelID);
				GetGUIManager()->StartActivity("ModelChooser",resultBundle);
			}

		}
	}
}


void ModelChooser::InitIOWindow(ShivaGUI::GUIController *guiController)
{
	guiController->RegisterListener(_backButtonHandler,"backButtonHandler");
	guiController->RegisterListener(_modelSelectButtonHandler,"modelSelectButtonHandler");

	guiController->LoadContentView( "Resources/Layout/ModelChooser.xml" );

	ShivaGUI::TextView *titleView = dynamic_cast<ShivaGUI::TextView*>( guiController->GetResources()->GetViewFromID( "Title" ) );
	if( titleView != NULL )
	{
		if( _modelA >= 0 )
			titleView->SetText( "Choose Model B", guiController->GetResources() );
		else
			titleView->SetText( "Choose Model A", guiController->GetResources() );
	}

	// Only allow the back button to be visible when we're on the second choosing screen
	ShivaGUI::TextButton *backButton = dynamic_cast<ShivaGUI::TextButton*>( guiController->GetResources()->GetViewFromID( "BackButton" ) );
	if( titleView != NULL )
	{
		if( _modelA < 0 )
			backButton->SetVisibility(false);
	}


	// We have a set of 8 buttons and we need to give them images of the models that can be loaded
	for( unsigned int i = 0; i < 8; i ++ )
	{
		std::stringstream buttonID;
		buttonID<<i;
		std::string thumbnailFilename = ShivaModelManager::GetInstance()->GetAttribute(buttonID.str(),"thumbnail_file");

		ShivaGUI::ImageButton *currentButton = dynamic_cast<ShivaGUI::ImageButton*>( guiController->GetResources()->GetViewFromID( buttonID.str() ) );
		if( currentButton != NULL )
		{
			// Do it this way round so we can set the button to inactive if there is no thumbnail for it
			if( !thumbnailFilename.empty() )
			{
				ShivaGUI::BitmapDrawable *buttonDrawable = dynamic_cast<ShivaGUI::BitmapDrawable*>( currentButton->GetContent() );
				if( buttonDrawable != NULL )
				{
					unsigned int imageID = guiController->GetResources()->GetBitmap(thumbnailFilename);
					if( imageID != 0 )
					{
						buttonDrawable->SetTexID(imageID);
					}
					else
					{
						std::cerr<<"imageID 0"<<std::endl;
						currentButton->SetActive(false);
					}
				}
				else
				{
					std::cerr<<"buttonDrawable null"<<std::endl;
					currentButton->SetActive(false);
				}
			}
			else
			{
				std::cerr<<"tn empty"<<std::endl;
				currentButton->SetActive(false);
			}
		}
		else
			std::cerr<<"button null"<<std::endl;
	}
}

void ModelChooser::InitOutputWindow(ShivaGUI::GUIController *guiController)
{
	guiController->LoadContentView( "Resources/Layout/ModelChooserAltWin.xml" );

	ShivaGUI::TextView *titleView2 = dynamic_cast<ShivaGUI::TextView*>( guiController->GetResources()->GetViewFromID( "Title" ) );
	if( titleView2 != NULL )
	{
		if( _modelA >= 0 )
			titleView2->SetText( "Choose Model B", guiController->GetResources() );
		else
			titleView2->SetText( "Choose Model A", guiController->GetResources() );
	}
}
