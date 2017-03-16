#include "ColourChooser.h"
#include "ShivaModelManager.h"
#include "GUIManager.h"
#include "GUI/Views/ImageButton.h"
#include "GUI/Views/TextView.h"
#include "GUI/Views/TextButton.h"
#include "GUI/Views/ColourSelector.h"
#include "GUI/Drawables/BitmapDrawable.h"

//----------------------------------------------------------------------------------

void ColourChooser::OnCreate( ShivaGUI::Bundle *_data )
{
	m_backButtonHandler = new UtilityEventHandler( this );
	m_nextButtonHandler = new UtilityEventHandler( this );
	m_leftColButtonHandler = new UtilityEventHandler( this );
	m_rightColButtonHandler = new UtilityEventHandler( this );
	m_colourChangeHandler = new UtilityEventHandler( this );

	m_colourX = m_colourY = 0.5f;
	m_colourStepsize = 0.05f;

	ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();
	if( prefs != NULL )
	{
		m_colourStepsize = prefs->GetFloat( "ColourStepsize", m_colourStepsize );
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

void ColourChooser::OnDestroy()
{
	delete m_backButtonHandler;
	delete m_nextButtonHandler;
	delete m_leftColButtonHandler;
	delete m_rightColButtonHandler;
	delete m_colourChangeHandler;
}

//----------------------------------------------------------------------------------

void ColourChooser::UtilityEventReceived( UtilityEventHandler *_handler, ShivaGUI::View *_view )
{
	if( _handler == m_backButtonHandler )
	{
		Finish();
	}
	if( _handler == m_nextButtonHandler )
	{
		ShivaGUI::ColourSelector *firstSelector = m_selectors.front();
		if( firstSelector != NULL )
		{
			float R, G, B;
			firstSelector->GetColour( R, G, B );
			m_startBundle->PutFloat( "ColourR", R );
			m_startBundle->PutFloat( "ColourG", G );
			m_startBundle->PutFloat( "ColourB", B );
		}
		GetGUIManager()->StartActivity( "RotationChooser", new ShivaGUI::Bundle( *m_startBundle ) );
	}
	if( _handler == m_colourChangeHandler )
	{
		ShivaGUI::ColourSelector *selector = dynamic_cast< ShivaGUI::ColourSelector* >( _view );
		if( selector != NULL )
		{
			selector->GetHandlePosition( m_colourX, m_colourY );
			UpdateViews();
		}
	}
	if( _handler == m_leftColButtonHandler )
	{
		m_colourX -= m_colourStepsize;
		if( m_colourX < 0.0f )
			m_colourX = 0.0f;
		UpdateViews();
	}
	if( _handler == m_rightColButtonHandler )
	{
		m_colourX += m_colourStepsize;
		if( m_colourX > 1.0f )
			m_colourX = 1.0f;
		UpdateViews();
	}
}

//----------------------------------------------------------------------------------

void ColourChooser::UpdateViews( ShivaGUI::ColourSelector *_issuingSelector )
{
	float R = 1.0f, G = 1.0f, B = 1.0f;
	if( _issuingSelector != NULL )
		_issuingSelector->GetColour( R, G, B );

	for( std::vector< ShivaGUI::ColourSelector* >::iterator it = m_selectors.begin(); it != m_selectors.end(); ++it )
	{
		if( ( *it ) != _issuingSelector )
		{
			( *it )->SetHandlePosition( m_colourX, m_colourY );
			( *it )->GetColour( R, G, B );
		}
	}

	for( std::vector< SDFView* >::iterator it = m_SDFViews.begin(); it != m_SDFViews.end(); ++it )
	{
		( *it )->SetColour( R, G, B );
	}
}

//----------------------------------------------------------------------------------

void ColourChooser::LoadSDFViewData( SDFView *_sdfView, ShivaGUI::Bundle *_data, ShivaGUI::GUIController *_guiController )
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

		if( _data != NULL && _data->ContainsKey( "ModelA" ) && _data->ContainsKey( "ModelB" ) && _data->ContainsKey( "MorphValue" ) )
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
			_sdfView->SetBlendParam( _data->GetFloat( "MorphValue" ) );
			
			std::cout << "INFO: ColourChooser, modelA = " << modelA << " modelB = " << modelB << " morph value = " << _data->GetFloat( "MorphValue" ) << std::endl;
		}
		else
			std::cerr << "WARNING: ShivaMetamorphosis has not received model choices" << std::endl;
	}
}

//----------------------------------------------------------------------------------

void ColourChooser::InitIOWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
{
	_guiController->RegisterListener( m_backButtonHandler, "backButtonHandler" );
	_guiController->RegisterListener( m_nextButtonHandler, "nextButtonHandler" );
	_guiController->RegisterListener( m_leftColButtonHandler, "colourLeft" );
	_guiController->RegisterListener( m_rightColButtonHandler, "colourRight" );
	_guiController->RegisterListener( m_colourChangeHandler, "colourChangeHandler" );

	_guiController->LoadContentView( "Resources/Layout/ColourChooser.xml" );

	SDFView *currentSDFView = dynamic_cast< SDFView* >( _guiController->GetResources()->GetViewFromID( "mainSDFView" ) );
	if( currentSDFView != NULL )
		m_SDFViews.push_back( currentSDFView );

	ShivaGUI::ColourSelector *currentSelector = dynamic_cast< ShivaGUI::ColourSelector* >( _guiController->GetResources()->GetViewFromID( "ColourSelector" ) );
	if( currentSelector != NULL )
		m_selectors.push_back( currentSelector );

	_guiController->MakeCurrent();

	LoadSDFViewData( currentSDFView, _data, _guiController );
	UpdateViews();
}

//----------------------------------------------------------------------------------

void ColourChooser::InitOutputWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
{
	_guiController->LoadContentView( "Resources/Layout/testLayout02.xml" );

	SDFView *currentSDFView = dynamic_cast< SDFView* >( _guiController->GetResources()->GetViewFromID( "mainSDFView" ) );
	if( currentSDFView != NULL )
		m_SDFViews.push_back( currentSDFView );

	_guiController->MakeCurrent();
	LoadSDFViewData( currentSDFView, _data, _guiController );
	UpdateViews();
}

//----------------------------------------------------------------------------------