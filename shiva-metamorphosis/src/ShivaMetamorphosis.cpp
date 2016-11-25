
#include "ShivaMetamorphosis.h"
#include "ShivaModelManager.h"

#include "GUIManager.h"
#include <iostream>
#include <sstream>

void ShivaMetamorphosis::OnCreate(ShivaGUI::Bundle *data)
{

	_backButtonHandler = new UtilityEventHandler(this);
	_nextButtonHandler = new UtilityEventHandler(this);
	_morphSliderHandler = new UtilityEventHandler(this);
	_leftMorphButtonHandler = new UtilityEventHandler(this);
	_rightMorphButtonHandler = new UtilityEventHandler(this);

	_morphValue = 0.5f;
	_morphStepsize = 0.1f;

	ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();
	if( prefs != NULL )
	{
		_morphStepsize = prefs->GetFloat("MorphStepsize",_morphStepsize);
	}

	int numWindows = GetNumGUIControllers();

	for( int i = 0; i < numWindows; i++ )
	{
		ShivaGUI::GUIController *guiController = GetGUIController(i);
		ShivaGUI::Window::RequestedUse windowUse = guiController->GetRequestedUse();
		if( windowUse == ShivaGUI::Window::OUTPUT )
		{
			InitOutputWindow(guiController,data);
		}
		else
		{
			InitIOWindow(guiController,data);
		}
	}

	_startBundle = new ShivaGUI::Bundle(*data);

}

void ShivaMetamorphosis::OnDestroy()
{
	delete _backButtonHandler;
	delete _nextButtonHandler;
	delete _morphSliderHandler;
	delete _leftMorphButtonHandler;
	delete _rightMorphButtonHandler;
	delete _startBundle;
}


void ShivaMetamorphosis::UtilityEventReceived(UtilityEventHandler *handler, ShivaGUI::View *view)
{
	if( handler == _backButtonHandler )
	{
		Finish();
	}
	else if( handler == _nextButtonHandler )
	{
		_startBundle->PutFloat("MorphValue",_morphValue);
		GetGUIManager()->StartActivity("ColourChooser", new ShivaGUI::Bundle(*_startBundle) );
	}
	else if( handler == _morphSliderHandler )
	{
		ShivaGUI::Slider *morphSlider = dynamic_cast<ShivaGUI::Slider*>( view );
		_morphValue = morphSlider->GetHandlePosition();
		UpdateViews(morphSlider);
		/*
		if( morphSlider != NULL )
		{
			for( std::vector<SDFView*>::iterator it = _SDFViews.begin(); it != _SDFViews.end(); ++it )
			{
				(*it)->SetBlendParam( morphSlider->GetHandlePosition() );
			}
		}
		*/
	}
	else if( handler == _leftMorphButtonHandler )
	{
		_morphValue -= _morphStepsize;
		if( _morphValue < 0.0f )
			_morphValue = 0.0f;
		UpdateViews();
	}
	else if( handler == _rightMorphButtonHandler )
	{
		_morphValue += _morphStepsize;
		if( _morphValue > 1.0f )
			_morphValue = 1.0f;
		UpdateViews();
	}
}

void ShivaMetamorphosis::UpdateViews(ShivaGUI::Slider *issuingSlider)
{
	for( std::vector<SDFView*>::iterator it = _SDFViews.begin(); it != _SDFViews.end(); ++it )
	{
		(*it)->SetBlendParam( _morphValue );
	}

	for( std::vector<ShivaGUI::Slider*>::iterator it = _sliders.begin(); it != _sliders.end(); ++it )
	{
		if( (*it) != issuingSlider )
			(*it)->SetHandlePosition( _morphValue );
	}
}


void ShivaMetamorphosis::LoadSDFViewData(SDFView *sdfView,ShivaGUI::Bundle *data, ShivaGUI::GUIController *guiController)
{
	if( sdfView != NULL )
	{

		// Get options from profile
		ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();
		if( prefs != NULL )
		{
			sdfView->SetRotationSpeed( prefs->GetFloat("ModelRotationSpeed",0.0f) );
			sdfView->SetQuality( prefs->GetFloat("SDFQuality",32.0f) );
		}

		if( data != NULL && data->ContainsKey("ModelA") && data->ContainsKey("ModelB") )
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
		}
		else
			std::cerr<<"WARNING: ShivaMetamorphosis has not received model choices"<<std::endl;

	}
}



void ShivaMetamorphosis::InitIOWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data)
{
	guiController->RegisterListener(_backButtonHandler,"backButtonHandler");
	guiController->RegisterListener(_nextButtonHandler,"nextButtonHandler");
	guiController->RegisterListener(_morphSliderHandler,"morphSliderHandler");
	guiController->RegisterListener(_leftMorphButtonHandler,"morphAmountLeft");
	guiController->RegisterListener(_rightMorphButtonHandler,"morphAmountRight");

	guiController->LoadContentView( "Resources/Layout/MorphLayout.xml" );

	SDFView *currentSDFView = dynamic_cast<SDFView*>( guiController->GetResources()->GetViewFromID("mainSDFView") );
	if( currentSDFView != NULL )
		_SDFViews.push_back(currentSDFView);

	ShivaGUI::Slider *currentSlider = dynamic_cast<ShivaGUI::Slider*>( guiController->GetResources()->GetViewFromID("morphSlider") );
	if( currentSlider != NULL )
		_sliders.push_back(currentSlider);

	guiController->MakeCurrent();

	LoadSDFViewData(currentSDFView,data,guiController);
	UpdateViews();
}

void ShivaMetamorphosis::InitOutputWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data)
{
	guiController->RegisterListener(_morphSliderHandler,"morphSliderHandler");

	guiController->LoadContentView( "Resources/Layout/testLayout02.xml" );

	SDFView *currentSDFView = dynamic_cast<SDFView*>( guiController->GetResources()->GetViewFromID("mainSDFView") );
	if( currentSDFView != NULL )
		_SDFViews.push_back(currentSDFView);

	guiController->MakeCurrent();
	LoadSDFViewData(currentSDFView,data,guiController);
	UpdateViews();
}


