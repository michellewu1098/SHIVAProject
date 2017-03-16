#include "ModelChooser.h"
#include "ShivaModelManager.h"
#include "GUIManager.h"
#include "GUI/Views/ImageButton.h"
#include "GUI/Views/TextView.h"
#include "GUI/Views/TextButton.h"
#include "GUI/Drawables/BitmapDrawable.h"

//----------------------------------------------------------------------------------

void ModelChooser::OnCreate( ShivaGUI::Bundle *_data )
{
	// See if the data we have been given contains the model IDs
	m_modelA = m_modelB = -1;
	if( _data != NULL )
	{
		if( _data->ContainsKey( "ModelA" ) )
		{
			m_modelA = _data->GetInteger( "ModelA" );
		}
		if( _data->ContainsKey( "ModelB" ) )
		{
			std::cerr << "ERROR: ModelChooser::OnCreate, already selected ModelB" << std::endl;
		}
	}

	// These handlers will be called when the buttons are pressed
	m_backButtonHandler = new UtilityEventHandler( this );
	m_modelSelectButtonHandler = new UtilityEventHandler( this );

	// Find out how many windows we have
	int numWindows = GetNumGUIControllers();
	for( int i = 0; i < numWindows; i++ )
	{
		// Load content for our windows, depending on how the profile says they should be used
		ShivaGUI::GUIController *guiController = GetGUIController( i );
		ShivaGUI::Window::RequestedUse windowUse = guiController->GetRequestedUse();
		if( windowUse == ShivaGUI::Window::OUTPUT )
		{
			InitOutputWindow( guiController );
		}
		else
		{
			InitIOWindow( guiController );
		}
	}
}

//----------------------------------------------------------------------------------

void ModelChooser::OnDestroy()
{
	delete m_backButtonHandler;
	delete m_modelSelectButtonHandler;
}

//----------------------------------------------------------------------------------

void ModelChooser::UtilityEventReceived( UtilityEventHandler *_handler, ShivaGUI::View *_view )
{
	if( _handler == m_backButtonHandler )
	{
		Finish();
	}
	else if( _handler == m_modelSelectButtonHandler )
	{

		ShivaGUI::Bundle *resultBundle = new ShivaGUI::Bundle();

		std::string modelIDstring = _view->GetID();
		std::istringstream strStream( modelIDstring );
		int modelID = -1;
		if( !( strStream>>modelID ) )
		{
			std::cerr << "WARNING: ModelChooser::UtilityEventReceived no valid model selected, unsure what to do..." << std::endl;
		}
		else
		{
			if( m_modelA >= 0 )
			{
				resultBundle->PutInteger( "ModelA", m_modelA );
				resultBundle->PutInteger( "ModelB", modelID );
				GetGUIManager()->StartActivity( "ShivaMetamorphosis", resultBundle );
			}
			else
			{
				resultBundle->PutInteger( "ModelA", modelID );
				GetGUIManager()->StartActivity( "ModelChooser", resultBundle );
			}
		}
	}
}

//----------------------------------------------------------------------------------

void ModelChooser::InitIOWindow( ShivaGUI::GUIController *_guiController )
{
	_guiController->RegisterListener( m_backButtonHandler, "backButtonHandler" );
	_guiController->RegisterListener( m_modelSelectButtonHandler, "modelSelectButtonHandler" );

	_guiController->LoadContentView( "Resources/Layout/ModelChooser.xml" );

	ShivaGUI::TextView *titleView = dynamic_cast< ShivaGUI::TextView* >( _guiController->GetResources()->GetViewFromID( "Title" ) );
	if( titleView != NULL )
	{
		if( m_modelA >= 0 )
			titleView->SetText( "Choose Model B", _guiController->GetResources() );
		else
			titleView->SetText( "Choose Model A", _guiController->GetResources() );
	}

	// Only allow the back button to be visible when we're on the second choosing screen
	ShivaGUI::TextButton *backButton = dynamic_cast< ShivaGUI::TextButton* >( _guiController->GetResources()->GetViewFromID( "BackButton" ) );
	if( titleView != NULL )
	{
		if( m_modelA < 0 )
			backButton->SetVisibility(false);
	}


	// We have a set of 8 buttons and we need to give them images of the models that can be loaded
	for( unsigned int i = 0; i < 8; i ++ )
	{
		std::stringstream buttonID;
		buttonID<<i;
		std::string thumbnailFilename = ShivaModelManager::GetInstance()->GetAttribute(buttonID.str(), "thumbnail_file" );

		ShivaGUI::ImageButton *currentButton = dynamic_cast< ShivaGUI::ImageButton* >( _guiController->GetResources()->GetViewFromID( buttonID.str() ) );
		if( currentButton != NULL )
		{
			// Do it this way round so we can set the button to inactive if there is no thumbnail for it
			if( !thumbnailFilename.empty() )
			{
				ShivaGUI::BitmapDrawable *buttonDrawable = dynamic_cast< ShivaGUI::BitmapDrawable* >( currentButton->GetContent() );
				if( buttonDrawable != NULL )
				{
					unsigned int imageID = _guiController->GetResources()->GetBitmap( thumbnailFilename );
					if( imageID != 0 )
					{
						buttonDrawable->SetTexID( imageID );
					}
					else
					{
						std::cerr << "imageID 0" << std::endl;
						currentButton->SetActive( false );
					}
				}
				else
				{
					std::cerr << "buttonDrawable null" << std::endl;
					currentButton->SetActive( false );
				}
			}
			else
			{
				std::cerr << "tn empty" << std::endl;
				currentButton->SetActive( false );
			}
		}
		else
			std::cerr << "button null" << std::endl;
	}
}

//----------------------------------------------------------------------------------

void ModelChooser::InitOutputWindow( ShivaGUI::GUIController *_guiController )
{
	_guiController->LoadContentView( "Resources/Layout/ModelChooserAltWin.xml" );

	ShivaGUI::TextView *titleView2 = dynamic_cast< ShivaGUI::TextView* >( _guiController->GetResources()->GetViewFromID( "Title" ) );
	if( titleView2 != NULL )
	{
		if( m_modelA >= 0 )
			titleView2->SetText( "Choose Model B", _guiController->GetResources() );
		else
			titleView2->SetText( "Choose Model A", _guiController->GetResources() );
	}
}

//----------------------------------------------------------------------------------
