#include "System/Activities/CreateNewProfileActivity.h"
#include "System/ProfileManager.h"
#include "GUIManager.h"

//----------------------------------------------------------------------------------

void ShivaGUI::CreateNewProfileActivity::OnCreate( Bundle *_data )
{
	m_buttonHandler = new UtilityEventHandler( this );
	m_colourChangeHandler = new UtilityEventHandler( this );

	m_changedBackgroundColour = false;
	m_customDimensions = m_chosenTheme = false;

	// The data bundle should contain the name of the activity we need to start once the profile has been chosen
	// It may also contain information passed in, e.g. through the command line, about which profile to load
	
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	m_profile.LinkEndChild( decl );

	TiXmlElement* root = new TiXmlElement( "profile" );
	m_profile.LinkEndChild( root );

	TiXmlElement* head = new TiXmlElement( "header" );
	root->LinkEndChild( head );

	TiXmlElement* el = new TiXmlElement( "name" );

	m_saveDir = "Profiles/";
	m_saveName = "CustomProfile";
	
	bool fileFound = false;
	std::string fullFilename;
	std::string extension = ".xml";
	unsigned int i = 1;
	do
	{
		std::stringstream fileNum;
		fileNum<<i;
		fullFilename = m_saveDir + m_saveName + fileNum.str() + extension;
		// We want to find the highest number that *doesn't* exist
		fileFound = !boost::filesystem::exists( fullFilename );
		m_profileName = m_saveName + fileNum.str();
		i++;
	}
	while( (i < 10000) && !fileFound );

	if( fileFound )
	{
		if( !boost::filesystem::exists( m_saveDir ) )
		{
			boost::filesystem::create_directory( m_saveDir );
		}
		
		el->LinkEndChild( new TiXmlText( m_profileName ) );
	}

	head->LinkEndChild( el );

	el = new TiXmlElement( "author" );
	el->LinkEndChild( new TiXmlText( "CustomUser" ) );
	head->LinkEndChild( el );

	m_options = new TiXmlElement( "options" );
	root->LinkEndChild( m_options );

	LoadViews();
	UpdateViews();
}

//----------------------------------------------------------------------------------

void ShivaGUI::CreateNewProfileActivity::OnDestroy()
{
	delete m_buttonHandler;
}

//----------------------------------------------------------------------------------

void ShivaGUI::CreateNewProfileActivity::OnUpdate( float _deltaTs )
{
}

//----------------------------------------------------------------------------------

void ShivaGUI::CreateNewProfileActivity::UtilityEventReceived( UtilityEventHandler *_handler, ShivaGUI::View *_view )
{
	if( _handler == m_buttonHandler )
	{
		if( _view->GetID() == "OneWindowProfile" )
		{
			unsigned int numWindows = GetNumGUIControllers();
			for( unsigned int i = 0; i < numWindows; i++ )
			{
				ShivaGUI::GUIController *guiController = GetGUIController( i );
				

				guiController->LoadContentView( "OneWindowCreatorLayout.xml" );
			}

			UpdateViews();
		}
		/*else if( _view->GetID() == "TwoWindowsProfile" )
		{
			unsigned int numWindows = GetNumGUIControllers();
			for( unsigned int i = 0; i < numWindows; i++ )
			{
				ShivaGUI::GUIController *guiController = GetGUIController( i );
				guiController->LoadContentView( "TwoWindowsCreatorLayout.xml" );
			}

			UpdateViews();
		}*/
		else if( _view->GetID() == "800x600Window" )
		{
			m_customDimensions = true;
			TiXmlElement* window = new TiXmlElement( "window" );
			TiXmlElement* width = new TiXmlElement( "width" );

			width->LinkEndChild( new TiXmlText( "800" ) );
			window->LinkEndChild( width );

			TiXmlElement* height = new TiXmlElement( "height" );
			height->LinkEndChild( new TiXmlText( "600" ) );
			window->LinkEndChild( height );

			m_options->LinkEndChild( window );

			_view->SetActive( false );
		}
		/*else if( _view->GetID() == "FullscreenWindow" ) 
		{
			TiXmlElement* window = new TiXmlElement( "window" );
			TiXmlElement* width = new TiXmlElement( "width" );

			width->LinkEndChild( new TiXmlText( "800" ) );
			window->LinkEndChild( width );

			TiXmlElement* height = new TiXmlElement( "height" );
			height->LinkEndChild( new TiXmlText( "600" ) );
			window->LinkEndChild( height );

			m_options->LinkEndChild( window );
		}*/
		else if( _view->GetID() == "HighVisTheme" )
		{
			if( !m_chosenTheme )
			{
				TiXmlElement* theme = new TiXmlElement( "theme" );
				theme->LinkEndChild( new TiXmlText( "Resources/Themes/highvisTheme.xml" ) );
				m_options->LinkEndChild( theme );
				_view->SetActive( false );

				m_chosenTheme = true;
			}
		}
		else if( _view->GetID() == "EyeGazeTheme" )
		{
			if( !m_chosenTheme )
			{
				TiXmlElement* theme = new TiXmlElement( "theme" );
				theme->LinkEndChild( new TiXmlText( "Resources/Themes/systemGaze.xml" ) );
				m_options->LinkEndChild( theme );
				_view->SetActive( false );

				m_chosenTheme = true;
			}
		}
		else if( _view->GetID() == "StandardTheme" )
		{
			if( !m_chosenTheme )
			{
				TiXmlElement* theme = new TiXmlElement( "theme" );
				theme->LinkEndChild( new TiXmlText( "Resources/Themes/system.xml" ) );
				m_options->LinkEndChild( theme );
				_view->SetActive( false );

				m_chosenTheme = true;
			}
		}
		else if( _view->GetID() == "BrowseTheme" )
		{
			char const* lFilterPatterns = "*.xml";
			char const* filename = tinyfd_openFileDialog( "Choose a Theme", "Resources/Themes", 1, &lFilterPatterns, NULL, 0 );

			if( !filename )
			{
				tinyfd_messageBox( "Error", "No file selected. Couldn't use theme.", "ok", "error", 1 );
			}
			else
			{
				if( !m_chosenTheme )
				{
					TiXmlElement* theme = new TiXmlElement( "theme" );
					theme->LinkEndChild( new TiXmlText( filename ) );
					m_options->LinkEndChild( theme );

					m_chosenTheme = true;
				}
			}
		}
		else if( _view->GetID() == "SaveProfile" )
		{
			if( !m_customDimensions )
			{
				TiXmlElement* window = new TiXmlElement( "window" );
				TiXmlElement* width = new TiXmlElement( "width" );

				width->LinkEndChild( new TiXmlText( "800" ) );
				window->LinkEndChild( width );

				TiXmlElement* height = new TiXmlElement( "height" );
				height->LinkEndChild( new TiXmlText( "600" ) );
				window->LinkEndChild( height );

				m_options->LinkEndChild( window );
			}

			if( !m_chosenTheme )
			{
				TiXmlElement* theme = new TiXmlElement( "theme" );
				theme->LinkEndChild( new TiXmlText( "Resources/Themes/system.xml" ) );
				m_options->LinkEndChild( theme );
			}

			if( m_changedBackgroundColour )
			{
				TiXmlElement* constGroup = new TiXmlElement( "constGroup" );
				TiXmlElement* el = new TiXmlElement( "const" );
				constGroup->LinkEndChild( el );

				TiXmlElement* keyValue = new TiXmlElement( "key" );
				keyValue->LinkEndChild( new TiXmlText( "@GradientBackgroundLeft" ) );
				el->LinkEndChild( keyValue );

				TiXmlElement* value = new TiXmlElement( "value" );
				value->LinkEndChild( new TiXmlText( m_colourR.str() + m_colourG.str() + m_colourB.str() + "ff" ) );
				el->LinkEndChild( value );


				el = new TiXmlElement( "const" );
				constGroup->LinkEndChild( el );

				keyValue = new TiXmlElement( "key" );
				keyValue->LinkEndChild( new TiXmlText( "@GradientBackgroundRight" ) );
				el->LinkEndChild( keyValue );

				value = new TiXmlElement( "value" );
				value->LinkEndChild( new TiXmlText( m_colourR.str() + m_colourG.str() + m_colourB.str() + "ff" ) );
				el->LinkEndChild( value );


				el = new TiXmlElement( "const" );
				constGroup->LinkEndChild( el );

				keyValue = new TiXmlElement( "key" );
				keyValue->LinkEndChild( new TiXmlText( "@GradientBackgroundTop" ) );
				el->LinkEndChild( keyValue );

				value = new TiXmlElement( "value" );
				value->LinkEndChild( new TiXmlText( m_colourR.str() + m_colourG.str() + m_colourB.str() + "ff" ) );
				el->LinkEndChild( value );


				el = new TiXmlElement( "const" );
				constGroup->LinkEndChild( el );

				keyValue = new TiXmlElement( "key" );
				keyValue->LinkEndChild( new TiXmlText( "@GradientBackgroundBottom" ) );
				el->LinkEndChild( keyValue );

				value = new TiXmlElement( "value" );
				value->LinkEndChild( new TiXmlText( m_colourR.str() + m_colourG.str() + m_colourB.str() + "ff" ) );
				el->LinkEndChild( value );

				m_options->LinkEndChild( constGroup );

				m_changedBackgroundColour = false;
			}

			TiXmlElement *keyInput = new TiXmlElement( "keyinput" );
			m_options->LinkEndChild( keyInput );

			TiXmlElement *map = new TiXmlElement( "map" );
			keyInput->LinkEndChild( map );
			
				TiXmlElement *debounce = new TiXmlElement( "debounce" );
				map->LinkEndChild( debounce );
					TiXmlElement *trailTime = new TiXmlElement( "trailTime" );
					trailTime->LinkEndChild( new TiXmlText( "1.0" ) );
					debounce->LinkEndChild( trailTime );

					TiXmlElement *suppressionTime = new TiXmlElement( "suppressionTime" );
					suppressionTime->LinkEndChild( new TiXmlText( "1.0" ) );
					debounce->LinkEndChild( suppressionTime );

					TiXmlElement *repeatEnable = new TiXmlElement( "repeatEnable" );
					repeatEnable->LinkEndChild( new TiXmlText( "false" ) );
					debounce->LinkEndChild( repeatEnable );

				TiXmlElement *key = new TiXmlElement( "key" );
				key->LinkEndChild( new TiXmlText( "41" ) );
				map->LinkEndChild( key );
					TiXmlElement *keyEvent = new TiXmlElement( "releaseEvent" );
					keyEvent->LinkEndChild( new TiXmlText( "BACK" ) );
					key->LinkEndChild( keyEvent );
				
				key = new TiXmlElement ( "key" );
				key->LinkEndChild( new TiXmlText( "44" ) );
				map->LinkEndChild( key );
					keyEvent = new TiXmlElement( "pressEvent" );
					keyEvent->LinkEndChild( new TiXmlText( "SCAN_HIGHLIGHT" ) );
					key->LinkEndChild( keyEvent );

					keyEvent = new TiXmlElement( "releaseEvent" );
					keyEvent->LinkEndChild( new TiXmlText( "SCAN_SELECT" ) );
					key->LinkEndChild( keyEvent );
				
				key = new TiXmlElement ( "key" );
				key->LinkEndChild( new TiXmlText( "62" ) );
				map->LinkEndChild( key );
					keyEvent = new TiXmlElement( "pressEvent" );
					keyEvent->LinkEndChild( new TiXmlText( "EDIT_CUSTOMLAYOUT" ) );
					key->LinkEndChild( keyEvent );

			map = new TiXmlElement( "map" );
			keyInput->LinkEndChild( map );
			
				debounce = new TiXmlElement( "debounce" );
				map->LinkEndChild( debounce );
					trailTime = new TiXmlElement( "trailTime" );
					trailTime->LinkEndChild( new TiXmlText( "0.1" ) );
					debounce->LinkEndChild( trailTime );

					repeatEnable = new TiXmlElement( "repeatEnable" );
					repeatEnable->LinkEndChild( new TiXmlText( "true" ) );
					debounce->LinkEndChild( repeatEnable );

					TiXmlElement *repeatDebounced = new TiXmlElement( "repeatDebounced" );
					repeatDebounced->LinkEndChild( new TiXmlText( "false" ) );
					debounce->LinkEndChild( repeatDebounced );

					TiXmlElement *repeatDelay = new TiXmlElement( "repeatDelay" );
					repeatDelay->LinkEndChild( new TiXmlText( "1.0" ) );
					debounce->LinkEndChild( repeatDelay );

					TiXmlElement *repeatRate = new TiXmlElement( "repeatRate" );
					repeatRate->LinkEndChild( new TiXmlText( "0.1" ) );
					debounce->LinkEndChild( repeatRate );

				key = new TiXmlElement( "key" );
				key->LinkEndChild( new TiXmlText( "40" ) );
				map->LinkEndChild( key );
					keyEvent = new TiXmlElement( "pressEvent" );
					keyEvent->LinkEndChild( new TiXmlText( "FOCUS_HIGHLIGHT" ) );
					key->LinkEndChild( keyEvent );

					keyEvent = new TiXmlElement( "repeatEvent" );
					keyEvent->LinkEndChild( new TiXmlText( "FOCUS_SELECT_HOLD" ) );
					key->LinkEndChild( keyEvent );

					keyEvent = new TiXmlElement( "releaseEvent" );
					keyEvent->LinkEndChild( new TiXmlText( "FOCUS_SELECT_RELEASE" ) );
					key->LinkEndChild( keyEvent );

			map = new TiXmlElement( "map" );
			keyInput->LinkEndChild( map );
			
				debounce = new TiXmlElement( "debounce" );
				map->LinkEndChild( debounce );
					trailTime = new TiXmlElement( "trailTime" );
					trailTime->LinkEndChild( new TiXmlText( "0.01" ) );
					debounce->LinkEndChild( trailTime );

					TiXmlElement *delayTime = new TiXmlElement( "delayTime" );
					delayTime->LinkEndChild( new TiXmlText( "0.01" ) );
					debounce->LinkEndChild( delayTime );

					suppressionTime = new TiXmlElement( "suppressionTime" );
					suppressionTime->LinkEndChild( new TiXmlText( "0.01" ) );
					debounce->LinkEndChild( suppressionTime );

					repeatEnable = new TiXmlElement( "repeatEnable" );
					repeatEnable->LinkEndChild( new TiXmlText( "true" ) );
					debounce->LinkEndChild( repeatEnable );

					repeatDebounced = new TiXmlElement( "repeatDebounced" );
					repeatDebounced->LinkEndChild( new TiXmlText( "false" ) );
					debounce->LinkEndChild( repeatDebounced );

					repeatDelay = new TiXmlElement( "repeatDelay" );
					repeatDelay->LinkEndChild( new TiXmlText( "1.0" ) );
					debounce->LinkEndChild( repeatDelay );

					repeatRate = new TiXmlElement( "repeatRate" );
					repeatRate->LinkEndChild( new TiXmlText( "0.1" ) );
					debounce->LinkEndChild( repeatRate );

				key = new TiXmlElement( "key" );
				key->LinkEndChild( new TiXmlText( "79" ) );
				map->LinkEndChild( key );
					keyEvent = new TiXmlElement( "pressEvent" );
					keyEvent->LinkEndChild( new TiXmlText( "NEXTFOCUS_RIGHT" ) );
					key->LinkEndChild( keyEvent );

					keyEvent = new TiXmlElement( "repeatEvent" );
					keyEvent->LinkEndChild( new TiXmlText( "NEXTFOCUS_RIGHT" ) );
					key->LinkEndChild( keyEvent );

				key = new TiXmlElement( "key" );
				key->LinkEndChild( new TiXmlText( "80" ) );
				map->LinkEndChild( key );
					keyEvent = new TiXmlElement( "pressEvent" );
					keyEvent->LinkEndChild( new TiXmlText( "NEXTFOCUS_LEFT" ) );
					key->LinkEndChild( keyEvent );

					keyEvent = new TiXmlElement( "repeatEvent" );
					keyEvent->LinkEndChild( new TiXmlText( "NEXTFOCUS_LEFT" ) );
					key->LinkEndChild( keyEvent );

				key = new TiXmlElement( "key" );
				key->LinkEndChild( new TiXmlText( "81" ) );
				map->LinkEndChild( key );
					keyEvent = new TiXmlElement( "pressEvent" );
					keyEvent->LinkEndChild( new TiXmlText( "NEXTFOCUS_DOWN" ) );
					key->LinkEndChild( keyEvent );

					keyEvent = new TiXmlElement( "repeatEvent" );
					keyEvent->LinkEndChild( new TiXmlText( "NEXTFOCUS_DOWN" ) );
					key->LinkEndChild( keyEvent );

				key = new TiXmlElement( "key" );
				key->LinkEndChild( new TiXmlText( "82" ) );
				map->LinkEndChild( key );
					keyEvent = new TiXmlElement( "pressEvent" );
					keyEvent->LinkEndChild( new TiXmlText( "NEXTFOCUS_UP" ) );
					key->LinkEndChild( keyEvent );

					keyEvent = new TiXmlElement( "repeatEvent" );
					keyEvent->LinkEndChild( new TiXmlText( "NEXTFOCUS_UP" ) );
					key->LinkEndChild( keyEvent );
				
			m_profile.SaveFile( "Profiles/" + m_profileName + ".xml" );

			Finish();
		}
	}
	else if( _handler == m_colourChangeHandler )
	{
		m_changedBackgroundColour = true; 

		ShivaGUI::ColourSelector *selector = dynamic_cast< ShivaGUI::ColourSelector* >( _view );
		if( selector != NULL )
		{
			m_colourR.str( std::string() );
			m_colourG.str( std::string() );
			m_colourB.str( std::string() );

			float colourX, colourY;
			selector->GetHandlePosition( colourX, colourY );
			
			float R = 1.0f, G = 1.0f, B = 1.0f;
			
			selector->SetHandlePosition( colourX, colourY );
			selector->GetColour( R, G, B );
			//std::cout << "Colour selected: R " << R << " G " << G << " B " << B << std::endl;
			//std::cout << "Colour selected: R " << R * 255 << " G " << G * 255 << " B " << B * 255 << std::endl;

			R *= 255.0f;
			G *= 255.0f;
			B *= 255.0f;

#ifdef _DEBUG
			std::cout << "In hex R " << std::hex << (int)R << std::endl;
			std::cout << "In hex G " << std::hex << (int)G << std::endl;
			std::cout << "In hex B " << std::hex << (int)B << std::endl;
#endif 

			if( R < 10 ) {
				m_colourR << "0" << std::hex << ( int )R;
			}
			else {
				m_colourR << std::hex << ( int )R;
			}

			if( G < 10 ) {
				m_colourG << "0" << std::hex << ( int )G;
			}
			else {
				m_colourG << std::hex << ( int )G;
			}

			if( B < 10 ) {
				m_colourB << "0" << std::hex << ( int )B;
			}
			else {
				m_colourB << std::hex << (int)B;
			}

#ifdef _DEBUG
			std::cout << "Final colour " << m_colourR.str() + m_colourG.str() + m_colourB.str() << std::endl; 
#endif
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::CreateNewProfileActivity::UpdateViews()
{
	Layout();
}

//----------------------------------------------------------------------------------

void ShivaGUI::CreateNewProfileActivity::LoadViews()
{
	unsigned int numWindows = GetNumGUIControllers();
	for( unsigned int i = 0; i < numWindows; i++ )
	{
		ShivaGUI::GUIController *guiController = GetGUIController( i );

		if( guiController->GetRequestedUse() != ShivaGUI::Window::OUTPUT )
		{
			guiController->RegisterListener( m_buttonHandler, "OneWindowHandler" );
			guiController->RegisterListener( m_buttonHandler, "TwoWindowsHandler" );
			guiController->RegisterListener( m_buttonHandler, "HighVisThemeHandler" );
			guiController->RegisterListener( m_buttonHandler, "EyeGazeThemeHandler" );
			guiController->RegisterListener( m_buttonHandler, "StandardThemeHandler" );
			guiController->RegisterListener( m_buttonHandler, "BrowseThemeHandler" );
			guiController->RegisterListener( m_buttonHandler, "SaveProfileHandler" );
			guiController->RegisterListener( m_buttonHandler, "WindowDimsHandler" );
			guiController->RegisterListener( m_colourChangeHandler, "ColourChangerHandler" );
		}

		guiController->LoadContentView( "ProfileCreatorLayout.xml" );

	}
}

//----------------------------------------------------------------------------------