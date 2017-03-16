#include "RotationChooser.h"
#include "ShivaModelManager.h"
#include "GUIManager.h"
#include "GUI/Views/ImageButton.h"
#include "GUI/Views/TextView.h"
#include "GUI/Views/TextButton.h"
#include "GUI/Views/ColourSelector.h"
#include "GUI/Drawables/BitmapDrawable.h"

#include <boost/filesystem.hpp>

//----------------------------------------------------------------------------------

void RotationChooser::OnCreate( ShivaGUI::Bundle *_data )
{
	_backButtonHandler = new UtilityEventHandler( this );
	_leftButtonHandler = new UtilityEventHandler( this );
	_rightButtonHandler = new UtilityEventHandler( this );
	_upButtonHandler = new UtilityEventHandler( this );
	_downButtonHandler = new UtilityEventHandler( this );
	_saveButtonHandler = new UtilityEventHandler( this );

	_rotationStepsize = 7.5f;
	_rotationX = _rotationY = _rotationZ = 0.0f;

	_saveDir = "Savefiles/";
	_saveName = "Morph";
	ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();
	if( prefs != NULL )
	{
		_saveDir = prefs->GetString( "SaveDirectory", _saveDir );
		_saveName = prefs->GetString( "SaveFilename", _saveName );
	}

	int numWindows = GetNumGUIControllers();

	for( int i = 0; i < numWindows; i++ )
	{
		ShivaGUI::GUIController *guiController = GetGUIController( i );
		ShivaGUI::Window::RequestedUse windowUse = guiController->GetRequestedUse();
		if( windowUse == ShivaGUI::Window::OUTPUT )
		{
			InitOutputWindow( guiController, _data );
		}
		else
		{
			InitIOWindow( guiController, _data );
		}
	}
}

//----------------------------------------------------------------------------------

void RotationChooser::OnDestroy()
{
	delete _backButtonHandler;
	delete _leftButtonHandler;
	delete _rightButtonHandler;
	delete _upButtonHandler;
	delete _downButtonHandler;
	delete _saveButtonHandler;
}


void RotationChooser::UtilityEventReceived( UtilityEventHandler *handler, ShivaGUI::View *view )
{
	if( handler == _backButtonHandler )
	{
		Finish();
	}
	else if( handler == _leftButtonHandler )
	{
		_rotationZ -= _rotationStepsize;
		UpdateViews();
	}
	else if( handler == _rightButtonHandler )
	{
		_rotationZ += _rotationStepsize;
		UpdateViews();
	}
	else if( handler == _upButtonHandler )
	{
		_rotationX -= _rotationStepsize;
		UpdateViews();
	}
	else if( handler == _downButtonHandler )
	{
		_rotationX += _rotationStepsize;
		UpdateViews();
	}
	else if( handler == _saveButtonHandler )
	{
		// Just use the first SDFView to save - they should all have the same data
		// Would be nice if SDFViews actually shared resources :(
		SDFView *mainSDFView = _SDFViews.front();
		if( mainSDFView != NULL )
		{
			bool fileFound = false;
			std::string fullFilename;
			std::string extension = ".vol";
			unsigned int i = 1;
			do
			{
				std::stringstream fileNum;
				fileNum<<i;
				fullFilename = _saveDir + _saveName + fileNum.str() + extension;
				// We want to find the highest number that *doesn't* exist
				fileFound = !boost::filesystem::exists(fullFilename);
				i++;
			}
			while( (i < 10000) && !fileFound );

			if( fileFound )
			{
				if( !boost::filesystem::exists(_saveDir) )
				{
					boost::filesystem::create_directory(_saveDir);
				}
				mainSDFView->SaveVolumeToFile(fullFilename);
			}
			else
			{
				std::cerr<<"WARNING: Cannot save file. Try removing previous files, limit is 10000 files"<<std::endl;
			}
		}
	}
}

void RotationChooser::UpdateViews()
{
	for( std::vector<SDFView*>::iterator it = _SDFViews.begin(); it != _SDFViews.end(); ++it )
	{
		(*it)->AddWorldRotationOffsetDegs(_rotationX,_rotationY,_rotationZ);
	}
	_rotationX = _rotationY = _rotationZ = 0.0f;
}

void RotationChooser::LoadSDFViewData(SDFView *sdfView,ShivaGUI::Bundle *data, ShivaGUI::GUIController *guiController)
{
	if( sdfView != NULL )
	{
		sdfView->SetRotationSpeed( 0.0f );

		// Get options from profile
		ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();
		if( prefs != NULL )
		{
			sdfView->SetQuality( prefs->GetFloat("SDFQuality",32.0f) );
		}

		if( data != NULL
				&& data->ContainsKey("ModelA")
				&& data->ContainsKey("ModelB")
				&& data->ContainsKey("MorphValue")
				&& data->ContainsKey("ColourR")
				&& data->ContainsKey("ColourG")
				&& data->ContainsKey("ColourB") )
		{
			int modelA = data->GetInteger("ModelA");
			std::stringstream modelASS;
			modelASS << modelA;
			int modelB = data->GetInteger("ModelB");
			std::stringstream modelBSS;
			modelBSS << modelB;
			std::string modelFilename = ShivaModelManager::GetInstance()->GetAttribute(modelASS.str(),"vol_file");
			sdfView->LoadVolumeFromFile(modelFilename,false,guiController);
			modelFilename = ShivaModelManager::GetInstance()->GetAttribute(modelBSS.str(),"vol_file");
			sdfView->LoadVolumeFromFile(modelFilename,true,guiController);
			sdfView->SetBlendParam( data->GetFloat("MorphValue") );

			float R = data->GetFloat("ColourR");
			float G = data->GetFloat("ColourG");
			float B = data->GetFloat("ColourB");

			sdfView->SetColour(R,G,B);

			std::cout<<"INFO: RotationChooser, modelA = "<<modelA<<" modelB = "<<modelB<<" morph value = "<<data->GetFloat("MorphValue")<<std::endl;
		}
		else
			std::cerr<<"WARNING: RotationChooser has not received model choices"<<std::endl;
	}
}




void RotationChooser::InitIOWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data)
{
	guiController->RegisterListener(_backButtonHandler,"backButtonHandler");
	guiController->RegisterListener(_leftButtonHandler,"rotateLeft");
	guiController->RegisterListener(_rightButtonHandler,"rotateRight");
	guiController->RegisterListener(_upButtonHandler,"rotateUp");
	guiController->RegisterListener(_downButtonHandler,"rotateDown");
	guiController->RegisterListener(_saveButtonHandler,"saveButtonHandler");

	guiController->LoadContentView( "Resources/Layout/RotationChooser.xml" );

	SDFView *currentSDFView = dynamic_cast<SDFView*>( guiController->GetResources()->GetViewFromID("mainSDFView") );
	if( currentSDFView != NULL )
		_SDFViews.push_back(currentSDFView);

	guiController->MakeCurrent();

	LoadSDFViewData(currentSDFView,data,guiController);
	UpdateViews();
}

void RotationChooser::InitOutputWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data)
{
	guiController->LoadContentView( "Resources/Layout/testLayout02.xml" );

	SDFView *currentSDFView = dynamic_cast<SDFView*>( guiController->GetResources()->GetViewFromID("mainSDFView") );
	if( currentSDFView != NULL )
		_SDFViews.push_back(currentSDFView);

	guiController->MakeCurrent();
	LoadSDFViewData(currentSDFView,data,guiController);
	UpdateViews();
}
