#include "System/Activities/CreateNewProfileActivity.h"
#include "System/ProfileManager.h"
#include "GUIManager.h"

//----------------------------------------------------------------------------------

void ShivaGUI::CreateNewProfileActivity::OnCreate( Bundle *_data )
{
	m_buttonHandler = new UtilityEventHandler( this );

	// The data bundle should contain the name of the activity we need to start once the profile has been chosen
	// It may also contain information passed in, e.g. through the command line, about which profile to load

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
		if( _view->GetID() == "Browse" )
		{
			// http://stackoverflow.com/questions/8520560/get-a-file-name-from-a-path
			char const* lFilterPatterns = "*.xml";
			char const* filename = tinyfd_openFileDialog( "Choose a Profile", "Profiles/", 1, &lFilterPatterns, NULL, 0 );

			if( !filename )
			{
				tinyfd_messageBox( "Error", "No file selected. Couldn't load profile.", "ok", "error", 1 );
			}
			else
			{
				#if defined _WIN32
					const char *lastSeparator = strrchr( filename, '\\' );
				#else
					const char *lastSeparator = strrchr( filename, '/' );
				#endif
				
				
				std::string profileName = std::string( lastSeparator ? lastSeparator + 1 : filename );

				size_t lastdot = profileName.find_last_of( "." );
				if( lastdot != std::string::npos )
				{	
					// chop off the ".xml" for the _profileName
					profileName = profileName.substr( 0, lastdot );
				}

				//m_chosenProfile = profileName;
				UpdateViews();
			}
		}
	}
}

//----------------------------------------------------------------------------------
