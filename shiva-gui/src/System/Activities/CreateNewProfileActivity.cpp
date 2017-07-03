#include "System/Activities/CreateNewProfileActivity.h"
#include "System/ProfileManager.h"
#include "GUIManager.h"

//----------------------------------------------------------------------------------

void ShivaGUI::CreateNewProfileActivity::OnCreate( Bundle *_data )
{
	m_buttonHandler = new UtilityEventHandler( this );

	// The data bundle should contain the name of the activity we need to start once the profile has been chosen
	// It may also contain information passed in, e.g. through the command line, about which profile to load
	
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	m_profile.LinkEndChild( decl );

	TiXmlElement* root = new TiXmlElement( "profile" );
	m_profile.LinkEndChild( root );

	TiXmlElement* head = new TiXmlElement( "header" );
	root->LinkEndChild( head );

	TiXmlElement* el = new TiXmlElement( "name" );
	el->LinkEndChild( new TiXmlText( "Shooot" ) );
	head->LinkEndChild( el );

	el = new TiXmlElement( "author" );
	el->LinkEndChild( new TiXmlText( "User" ) );
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
		else if( _view->GetID() == "TwoWindowsProfile" )
		{
			unsigned int numWindows = GetNumGUIControllers();
			for( unsigned int i = 0; i < numWindows; i++ )
			{
				ShivaGUI::GUIController *guiController = GetGUIController( i );
				guiController->LoadContentView( "TwoWindowsCreatorLayout.xml" );
			}

			UpdateViews();
		}
		else if( _view->GetID() == "HighVisTheme" )
		{
			TiXmlElement* theme = new TiXmlElement( "theme" );
			theme->LinkEndChild( new TiXmlText( "Resources/Themes/highvisTheme.xml" ) );
			m_options->LinkEndChild( theme );
		}
		else if( _view->GetID() == "EyeGazeTheme" )
		{
			TiXmlElement* theme = new TiXmlElement( "theme" );
			theme->LinkEndChild( new TiXmlText( "Resources/Themes/systemGaze.xml" ) );
			m_options->LinkEndChild( theme );
		}
		else if( _view->GetID() == "StandardTheme" )
		{
			TiXmlElement* theme = new TiXmlElement( "theme" );
			theme->LinkEndChild( new TiXmlText( "Resources/Themes/system.xml" ) );
			m_options->LinkEndChild( theme );
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
				TiXmlElement* theme = new TiXmlElement( "theme" );
				theme->LinkEndChild( new TiXmlText( filename ) );
				m_options->LinkEndChild( theme );
			}
		}
		else if( _view->GetID() == "SaveProfile" )
		{
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
				
			m_profile.SaveFile( "Profiles/CustomProfile.xml" );
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
		}

		guiController->LoadContentView( "ProfileCreatorLayout.xml" );
	}
}

//----------------------------------------------------------------------------------