#include "RotationChooser.h"
#include "ShivaModelManager.h"
#include "GUIManager.h"
#include "GUI/Views/ImageButton.h"
#include "GUI/Views/TextView.h"
#include "GUI/Views/TextButton.h"
#include "GUI/Views/ColourSelector.h"
#include "GUI/Drawables/BitmapDrawable.h"

//----------------------------------------------------------------------------------

void RotationChooser::OnCreate( ShivaGUI::Bundle *_data )
{
	m_backButtonHandler = new UtilityEventHandler( this );
	m_leftButtonHandler = new UtilityEventHandler( this );
	m_rightButtonHandler = new UtilityEventHandler( this );
	m_upButtonHandler = new UtilityEventHandler( this );
	m_downButtonHandler = new UtilityEventHandler( this );
	m_saveButtonHandler = new UtilityEventHandler( this );

	m_rotationStepsize = 7.5f;
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;

	m_saveDir = "Savefiles/";
	m_saveName = "Morph";

	ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();
	if( prefs != NULL )
	{
		m_saveDir = prefs->GetString( "SaveDirectory", m_saveDir );
		m_saveName = prefs->GetString( "SaveFilename", m_saveName );
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
	delete m_backButtonHandler;
	delete m_leftButtonHandler;
	delete m_rightButtonHandler;
	delete m_upButtonHandler;
	delete m_downButtonHandler;
	delete m_saveButtonHandler;
}

//----------------------------------------------------------------------------------

void RotationChooser::UtilityEventReceived( UtilityEventHandler *_handler, ShivaGUI::View *_view )
{
	if( _handler == m_backButtonHandler )
	{
		Finish();
	}
	else if( _handler == m_leftButtonHandler )
	{
		m_rotationZ -= m_rotationStepsize;
		UpdateViews();
	}
	else if( _handler == m_rightButtonHandler )
	{
		m_rotationZ += m_rotationStepsize;
		UpdateViews();
	}
	else if( _handler == m_upButtonHandler )
	{
		m_rotationX -= m_rotationStepsize;
		UpdateViews();
	}
	else if( _handler == m_downButtonHandler )
	{
		m_rotationX += m_rotationStepsize;
		UpdateViews();
	}
	else if( _handler == m_saveButtonHandler )
	{
		// Just use the first SDFView to save - they should all have the same data
		// Would be nice if SDFViews actually shared resources :(
		SDFView *mainSDFView = m_SDFViews.front();
		if( mainSDFView != NULL )
		{
			bool fileFound = false;
			std::string fullFilename;
			std::string extension = ".vol";
			unsigned int i = 1;
			do
			{
				std::stringstream fileNum;
				fileNum << i;
				fullFilename = m_saveDir + m_saveName + fileNum.str() + extension;
				// We want to find the highest number that *doesn't* exist
				fileFound = !boost::filesystem::exists( fullFilename );
				i++;
			}
			while( ( i < 10000 ) && !fileFound );

			if( fileFound )
			{
				if( !boost::filesystem::exists( m_saveDir ) )
				{
					boost::filesystem::create_directory( m_saveDir );
				}
				mainSDFView->SaveVolumeToFile( fullFilename );
			}
			else
			{
				std::cerr << "WARNING: Cannot save file. Try removing previous files, limit is 10000 files" << std::endl;
			}
		}
	}
}

//----------------------------------------------------------------------------------

void RotationChooser::UpdateViews()
{
	for( std::vector< SDFView* >::iterator it = m_SDFViews.begin(); it != m_SDFViews.end(); ++it )
	{
		( *it )->AddWorldRotationOffsetDegs( m_rotationX, m_rotationY, m_rotationZ );
	}
	m_rotationX = m_rotationY = m_rotationZ = 0.0f;
}

//----------------------------------------------------------------------------------

void RotationChooser::LoadSDFViewData( SDFView *_sdfView, ShivaGUI::Bundle *_data, ShivaGUI::GUIController *_guiController )
{
	if( _sdfView != NULL )
	{
		_sdfView->SetRotationSpeed( 0.0f );

		// Get options from profile
		ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();
		if( prefs != NULL )
		{
			_sdfView->SetQuality( prefs->GetFloat( "SDFQuality", 32.0f ) );
		}

		if( _data != NULL
				&& _data->ContainsKey( "ModelA" )
				&& _data->ContainsKey( "ModelB" )
				&& _data->ContainsKey( "MorphValue" )
				&& _data->ContainsKey( "ColourR" )
				&& _data->ContainsKey( "ColourG" )
				&& _data->ContainsKey( "ColourB" ) )
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
			_sdfView->SetBlendParam( _data->GetFloat( "MorphValue") );

			float R = _data->GetFloat( "ColourR" );
			float G = _data->GetFloat( "ColourG" );
			float B = _data->GetFloat( "ColourB" );

			_sdfView->SetColour( R, G, B );

			std::cout << "INFO: RotationChooser, modelA = " << modelA << " modelB = " << modelB << " morph value = " << _data->GetFloat( "MorphValue" ) << std::endl;
		}
		else
			std::cerr << "WARNING: RotationChooser has not received model choices" << std::endl;
	}
}

//----------------------------------------------------------------------------------

void RotationChooser::InitIOWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
{
	_guiController->RegisterListener( m_backButtonHandler, "backButtonHandler" );
	_guiController->RegisterListener( m_leftButtonHandler, "rotateLeft" );
	_guiController->RegisterListener( m_rightButtonHandler, "rotateRight" );
	_guiController->RegisterListener( m_upButtonHandler, "rotateUp" );
	_guiController->RegisterListener( m_downButtonHandler, "rotateDown" );
	_guiController->RegisterListener( m_saveButtonHandler, "saveButtonHandler" );

	_guiController->LoadContentView( "Resources/Layout/RotationChooser.xml" );

	SDFView *currentSDFView = dynamic_cast< SDFView* >( _guiController->GetResources()->GetViewFromID( "mainSDFView" ) );
	if( currentSDFView != NULL )
		m_SDFViews.push_back( currentSDFView );

	_guiController->MakeCurrent();

	LoadSDFViewData( currentSDFView, _data, _guiController );
	UpdateViews();
}

//----------------------------------------------------------------------------------

void RotationChooser::InitOutputWindow( ShivaGUI::GUIController *_guiController, ShivaGUI::Bundle *_data )
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