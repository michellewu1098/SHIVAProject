#include "ColourChooser.h"
#include "ShivaModelManager.h"
#include "GUIManager.h"
#include "GUI/Views/ImageButton.h"
#include "GUI/Views/TextView.h"
#include "GUI/Views/TextButton.h"
#include "GUI/Views/ColourSelector.h"
#include "GUI/Drawables/BitmapDrawable.h"

void ColourChooser::OnCreate(ShivaGUI::Bundle *data)
{
	_backButtonHandler = new UtilityEventHandler(this);
	_nextButtonHandler = new UtilityEventHandler(this);
	_leftColButtonHandler = new UtilityEventHandler(this);
	_rightColButtonHandler = new UtilityEventHandler(this);
	_colourChangeHandler = new UtilityEventHandler(this);

	_colourX = _colourY = 0.5f;
	_colourStepsize = 0.05f;

	ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();
	if( prefs != NULL )
	{
		_colourStepsize = prefs->GetFloat("ColourStepsize",_colourStepsize);
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

void ColourChooser::OnDestroy()
{
	delete _backButtonHandler;
	delete _nextButtonHandler;
	delete _leftColButtonHandler;
	delete _rightColButtonHandler;
	delete _colourChangeHandler;
}

void ColourChooser::UtilityEventReceived( UtilityEventHandler *handler, ShivaGUI::View *view )
{
	if( handler == _backButtonHandler )
	{
		Finish();
	}
	if( handler == _nextButtonHandler )
	{
		ShivaGUI::ColourSelector *firstSelector = _selectors.front();
		if( firstSelector != NULL )
		{
			float R, G, B;
			firstSelector->GetColour(R,G,B);
			_startBundle->PutFloat("ColourR",R);
			_startBundle->PutFloat("ColourG",G);
			_startBundle->PutFloat("ColourB",B);
		}
		GetGUIManager()->StartActivity("RotationChooser", new ShivaGUI::Bundle(*_startBundle) );
	}
	if( handler == _colourChangeHandler )
	{
		ShivaGUI::ColourSelector *selector = dynamic_cast<ShivaGUI::ColourSelector*>( view );
		if( selector != NULL )
		{
			selector->GetHandlePosition(_colourX, _colourY);
			UpdateViews();
		}
	}
	if( handler == _leftColButtonHandler )
	{
		_colourX -= _colourStepsize;
		if( _colourX < 0.0f )
			_colourX = 0.0f;
		UpdateViews();
	}
	if( handler == _rightColButtonHandler )
	{
		_colourX += _colourStepsize;
		if( _colourX > 1.0f )
			_colourX = 1.0f;
		UpdateViews();
	}
}

void ColourChooser::UpdateViews(ShivaGUI::ColourSelector *issuingSelector)
{
	float R = 1.0f, G = 1.0f, B = 1.0f;
	if( issuingSelector != NULL )
		issuingSelector->GetColour(R,G,B);

	for( std::vector<ShivaGUI::ColourSelector*>::iterator it = _selectors.begin(); it != _selectors.end(); ++it )
	{
		if( (*it) != issuingSelector )
		{
			(*it)->SetHandlePosition(_colourX,_colourY);
			(*it)->GetColour(R,G,B);
		}
	}

	for( std::vector<SDFView*>::iterator it = _SDFViews.begin(); it != _SDFViews.end(); ++it )
	{
		(*it)->SetColour(R,G,B);
	}
}

void ColourChooser::LoadSDFViewData(SDFView *sdfView,ShivaGUI::Bundle *data, ShivaGUI::GUIController *guiController)
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

		if( data != NULL && data->ContainsKey("ModelA") && data->ContainsKey("ModelB") && data->ContainsKey("MorphValue") )
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
			std::cout<<"INFO: ColourChooser, modelA = "<<modelA<<" modelB = "<<modelB<<" morph value = "<<data->GetFloat("MorphValue")<<std::endl;
		}
		else
			std::cerr<<"WARNING: ShivaMetamorphosis has not received model choices"<<std::endl;

	}
}


void ColourChooser::InitIOWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data)
{
	guiController->RegisterListener(_backButtonHandler,"backButtonHandler");
	guiController->RegisterListener(_nextButtonHandler,"nextButtonHandler");
	guiController->RegisterListener(_leftColButtonHandler,"colourLeft");
	guiController->RegisterListener(_rightColButtonHandler,"colourRight");
	guiController->RegisterListener(_colourChangeHandler,"colourChangeHandler");

	guiController->LoadContentView( "Resources/Layout/ColourChooser.xml" );

	SDFView *currentSDFView = dynamic_cast<SDFView*>( guiController->GetResources()->GetViewFromID("mainSDFView") );
	if( currentSDFView != NULL )
		_SDFViews.push_back(currentSDFView);

	ShivaGUI::ColourSelector *currentSelector = dynamic_cast<ShivaGUI::ColourSelector*>( guiController->GetResources()->GetViewFromID("ColourSelector") );
	if( currentSelector != NULL )
		_selectors.push_back(currentSelector);

	guiController->MakeCurrent();

	LoadSDFViewData(currentSDFView,data,guiController);
	UpdateViews();
}

void ColourChooser::InitOutputWindow(ShivaGUI::GUIController *guiController, ShivaGUI::Bundle *data)
{
	guiController->LoadContentView( "Resources/Layout/testLayout02.xml" );

	SDFView *currentSDFView = dynamic_cast<SDFView*>( guiController->GetResources()->GetViewFromID("mainSDFView") );
	if( currentSDFView != NULL )
		_SDFViews.push_back(currentSDFView);

	guiController->MakeCurrent();
	LoadSDFViewData(currentSDFView,data,guiController);
	UpdateViews();
}
