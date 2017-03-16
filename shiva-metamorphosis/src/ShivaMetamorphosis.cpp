#include "ShivaMetamorphosis.h"
#include "ShivaModelManager.h"

#include "GUIManager.h"
#include <iostream>
#include <sstream>

//----------------------------------------------------------------------------------

void ShivaMetamorphosis::OnCreate( ShivaGUI::Bundle *_data )
{
	m_backButtonHandler = new UtilityEventHandler( this );
	m_nextButtonHandler = new UtilityEventHandler( this );
	m_morphSliderHandler = new UtilityEventHandler( this );
	m_leftMorphButtonHandler = new UtilityEventHandler( this );
	m_rightMorphButtonHandler = new UtilityEventHandler( this );

	m_morphValue = 0.5f;
	m_morphStepsize = 0.1f;

	ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();
	
	if( prefs != NULL )
	{
		m_morphStepsize = prefs->GetFloat( "MorphStepsize", m_morphStepsize );
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

	m_startBundle = new ShivaGUI::Bundle( *_data );

}

//----------------------------------------------------------------------------------

void ShivaMetamorphosis::OnDestroy()
{
	delete m_backButtonHandler;
	delete m_nextButtonHandler;
	delete m_morphSliderHandler;
	delete m_leftMorphButtonHandler;
	delete m_rightMorphButtonHandler;
	delete m_startBundle;
}

//----------------------------------------------------------------------------------

void ShivaMetamorphosis::UtilityEventReceived( UtilityEventHandler *_handler, ShivaGUI::View *_view )
{
	if( _handler == m_backButtonHandler )
	{
		Finish();
	}
	else if( _handler == m_nextButtonHandler )
	{
		m_startBundle->PutFloat( "MorphValue", m_morphValue );
		GetGUIManager()->StartActivity( "ColourChooser", new ShivaGUI::Bundle( *m_startBundle ) );
	}
	else if( _handler == m_morphSliderHandler )
	{
		ShivaGUI::Slider *morphSlider = dynamic_cast< ShivaGUI::Slider* >( _view );
		m_morphValue = morphSlider->GetHandlePosition();
		UpdateViews( morphSlider );
	}
	else if( _handler == m_leftMorphButtonHandler )
	{
		m_morphValue -= m_morphStepsize;
		if( m_morphValue < 0.0f )
			m_morphValue = 0.0f;
		UpdateViews();
	}
	else if( _handler == m_rightMorphButtonHandler )
	{
		m_morphValue += m_morphStepsize;
		if( m_morphValue > 1.0f )
			m_morphValue = 1.0f;
		UpdateViews();
	}
}

//----------------------------------------------------------------------------------

void ShivaMetamorphosis::UpdateViews( ShivaGUI::Slider *_issuingSlider )
{
	for( std::vector< SDFView* >::iterator it = m_SDFViews.begin(); it != m_SDFViews.end(); ++it )
	{
		( *it )->SetBlendParam( m_morphValue );
	}

	for( std::vector<ShivaGUI::Slider*>::iterator it = m_sliders.begin(); it != m_sliders.end(); ++it )
	{
		if( ( *it ) != _issuingSlider )
			( *it )->SetHandlePosition( m_morphValue );
	}
}

//----------------------------------------------------------------------------------

void ShivaMetamorphosis::LoadSDFViewData( SDFView *_sdfView, ShivaGUI::Bundle *_data, ShivaGUI::GUIController *_guiController )
{
	if( _sdfView != NULL )
	{
		// Get options from profile
		ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();
		if( prefs != NULL )
		{
			_sdfView->SetRotationSpeed( prefs->GetFloat( "ModelRotationSpeed", 0.0f ) );
			_sdfView->SetQuality( prefs->GetFloat( "SDFQuality", 32.0f ) );
		}

		if( _data != NULL && _data->ContainsKey( "ModelA" ) && _data->ContainsKey( "ModelB" ) )
		{
			int modelA = _data->GetInteger( "ModelA" );
			std::stringstream modelASS;
			modelASS << modelA;
			int modelB = _data->GetInteger( "ModelB" );
			std::stringstream modelBSS;
			modelBSS << modelB;
			std::string modelFilename = ShivaModelManager::GetInstance()->GetAttribute( modelASS.str(), "vol_file" );
			_sdfView->LoadVolumeFromFile( modelFilename, false, _guiController );
			modelFilename = ShivaModelManager::GetInstance()->GetAttribute( modelBSS.str(), "vol_file" );
			_sdfView->LoadVolumeFromFile( modelFilename, true, _guiController );
		}
		else
			std::cerr << "WARNING: ShivaMetamorphosis has not received model choices" << std::endl;
	}
}

//----------------------------------------------------------------------------------

void ShivaMetamorphosis::InitIOWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
{
	_guiController->RegisterListener( m_backButtonHandler, "backButtonHandler" );
	_guiController->RegisterListener( m_nextButtonHandler, "nextButtonHandler" );
	_guiController->RegisterListener( m_morphSliderHandler, "morphSliderHandler" );
	_guiController->RegisterListener( m_leftMorphButtonHandler, "morphAmountLeft" );
	_guiController->RegisterListener( m_rightMorphButtonHandler, "morphAmountRight" );

	_guiController->LoadContentView( "Resources/Layout/MorphLayout.xml" );

	SDFView *currentSDFView = dynamic_cast< SDFView* >( _guiController->GetResources()->GetViewFromID( "mainSDFView" ) );
	if( currentSDFView != NULL )
		m_SDFViews.push_back( currentSDFView );

	ShivaGUI::Slider *currentSlider = dynamic_cast< ShivaGUI::Slider* >( _guiController->GetResources()->GetViewFromID( "morphSlider" ) );
	if( currentSlider != NULL )
		m_sliders.push_back( currentSlider );

	_guiController->MakeCurrent();

	LoadSDFViewData( currentSDFView, _data, _guiController );
	UpdateViews();
}

//----------------------------------------------------------------------------------

void ShivaMetamorphosis::InitOutputWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
{
	_guiController->RegisterListener( m_morphSliderHandler, "morphSliderHandler" );

	_guiController->LoadContentView( "Resources/Layout/testLayout02.xml" );

	SDFView *currentSDFView = dynamic_cast< SDFView* >( _guiController->GetResources()->GetViewFromID( "mainSDFView" ) );
	if( currentSDFView != NULL )
		m_SDFViews.push_back( currentSDFView );

	_guiController->MakeCurrent();
	LoadSDFViewData( currentSDFView, _data, _guiController );
	UpdateViews();
}

//----------------------------------------------------------------------------------
