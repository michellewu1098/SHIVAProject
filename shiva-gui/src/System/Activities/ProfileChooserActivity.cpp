
#include <boost/filesystem.hpp>

#include "System/Activities/ProfileChooserActivity.h"
#include "System/ProfileManager.h"
#include "GUIManager.h"


void ShivaGUI::ProfileChooserActivity::OnCreate(Bundle *data)
{
	_startNextActivity = false;


	_startHandler = new UtilityEventHandler(this);
	_chooseProfileListHandler = new UtilityEventHandler(this);
	_copyProfileHandler =  new UtilityEventHandler(this);
	_deleteProfileHandler = new UtilityEventHandler(this);

	_profileFilesProvider = new FilesystemDataProvider();
	_profileFilesProvider->SetShowDirectories(false);


	// Create and configure our list adapter
	_profileFilesAdapter =  new ShivaGUI::DataAdapter();
	_profileFilesAdapter->SetDataProvider( _profileFilesProvider );

	// create mapping:
	std::string fromAttrib("FILENAME_NO_EXTENSION"); // This must correspond to what we want out of the DataProvider
	std::string toView("textbutton");		// This must be the View ID in the layout xml that we want to map the above data to

	_profileFilesAdapter->SetMapping(&fromAttrib,&toView,1);
	_profileFilesAdapter->SetLayoutFile("ProfileChooserEntry.xml");	// This is the layout xml that specifies the single entry for the ListView



	// The data bundle should contain the name of the activity we need to start once the profile has been chosen
	// It may also contain information passed in, e.g. through the command line, about which profile to load
	if( data != NULL )
	{
		if( data->ContainsKey("StartActivity") )
		{
			_startActivity = data->GetString("StartActivity");
		}
		if( data->ContainsKey("ProfileDirectory") )
		{
			if( !GetGUIManager()->SetProfileDirectory( data->GetString("ProfileDirectory") ) )
				std::cerr<<"WARNING: Could not set profile directory to: "<<data->GetString("ProfileDirectory")<<std::endl;
		}
		if( data->ContainsKey("LoadProfile") && !_startActivity.empty() )
		{
			// We are given a profile to load, probably from commandline, to skip this ProfileChooserActivity and start the next one already
			_chosenProfile = data->GetString("LoadProfile");

			StartNextActivity();
		}
	}

	//_profileFilesProvider->SetCurrentDirectory( GetGUIManager()->GetProfileDir() );
	_profileFilesProvider->SetCurrentDirectoryNew( GetGUIManager()->GetProfileDir() );

	LoadViews();
	UpdateViews();
}

void ShivaGUI::ProfileChooserActivity::OnConfigurationChanged()
{
	_currentProfileDisplays.clear();
	_currentProfDirDisplays.clear();
	LoadViews();
}

void ShivaGUI::ProfileChooserActivity::OnDestroy()
{
	delete _startHandler;
}

void ShivaGUI::ProfileChooserActivity::OnUpdate(float deltaTs)
{
	// Need to make sure the command queue is empty as bootstrapping takes a while
	if( _startNextActivity && GetGUIManager()->CommandQueueEmpty() )
	{
		std::cout<<"ProfileChooserActivity::OnUpdate trying to start activity: "<< _startActivity<<std::endl;
		_startNextActivity = false;
		GetGUIManager()->StartActivity(_startActivity);
	}
}

void ShivaGUI::ProfileChooserActivity::UtilityEventReceived(UtilityEventHandler *handler, ShivaGUI::View *view)
{
	if( handler == _startHandler )
	{
		StartNextActivity();
	}
	else if( handler == _chooseProfileListHandler )
	{
		// We have received a request to select a profile
		// We don't know which button it came from,
		// but the text of the button will be the profile name that we need

		TextButton *_pressedButton = dynamic_cast<ShivaGUI::TextButton*>(view);
		if( _pressedButton != NULL )
		{
			_chosenProfile = _pressedButton->GetText();
			std::cout<<"INFO: ProfileChooserActivity request to change to profile: "<<_pressedButton->GetText()<<std::endl;

			UpdateViews();
		}
	}
	else if( handler == _copyProfileHandler )
	{
		if( !_chosenProfile.empty() )
		{
			GetGUIManager()->GetProfileManager()->Copy(_chosenProfile,_chosenProfile+"_copy");

			//std::string profileDirectory = GetGUIManager()->GetProfileDir() + "/";
			//_profileFilesProvider->CopyFile(_chosenProfile,_chosenProfile+"_copy");
			//boost::filesystem3::copy_file(profileDirectory+_chosenProfile+".xml",profileDirectory+_chosenProfile+"_copy.xml");

			for( std::vector< std::pair<ShivaGUI::ListView*,unsigned int> >::iterator it = _profileFileListDisplays.begin(); it != _profileFileListDisplays.end(); ++it )
				(*it).first->RefreshFromSource();

			UpdateViews();
		}
	}
}

void ShivaGUI::ProfileChooserActivity::UpdateViews()
{
	_profileFilesProvider->SetCurrentDirectoryNew( GetGUIManager()->GetProfileDir() );

	for( std::vector< std::pair<TextView*,unsigned int> >::iterator it = _currentProfileDisplays.begin(); it != _currentProfileDisplays.end(); ++it )
	{
		ShivaGUI::GUIController *guiController = GetGUIController(it->second);
		if( guiController != NULL )
			it->first->SetText(_chosenProfile, guiController->GetResources() );
	}

	for( std::vector< std::pair<TextView*,unsigned int> >::iterator it = _currentProfDirDisplays.begin(); it != _currentProfDirDisplays.end(); ++it )
	{
		ShivaGUI::GUIController *guiController = GetGUIController(it->second);
		if( guiController != NULL )
			it->first->SetText(GetGUIManager()->GetProfileDir(), guiController->GetResources() );
	}
	Layout();
}

void ShivaGUI::ProfileChooserActivity::StartNextActivity()
{
	// Load our selected profile
	GetGUIManager()->RequestBootstrapChangeProfile( _chosenProfile );
	_startNextActivity = true;
	//GetGUIManager()->StartActivity(_startActivity);
	/*
	if( )
	{
		// Start the next Activity
		//GetGUIManager()->StartActivity(_startActivity);
	}
	else
	{
		std::cerr<<"WARNING: ProfileChooserActivity: Could not load profile: "<<_chosenProfile<<" please select another"<<std::endl;
	}
	*/
}

void ShivaGUI::ProfileChooserActivity::LoadViews()
{

	unsigned int numWindows = GetNumGUIControllers();
	for( unsigned int i = 0; i < numWindows; i++ )
	{
		ShivaGUI::GUIController *guiController = GetGUIController(i);

		if( guiController->GetRequestedUse() != ShivaGUI::Window::OUTPUT )
		{
			guiController->RegisterListener(_startHandler,"StartActivityHandler");
			guiController->RegisterListener(_chooseProfileListHandler,"SelectProfileHandler");
			guiController->RegisterListener(_copyProfileHandler,"CopyProfileHandler");
			guiController->RegisterListener(_deleteProfileHandler,"DeleteProfileHandler");


			guiController->LoadContentView( "ProfileChooserLayout.xml" );

			TextView *currentProfileDisplayView = dynamic_cast<TextView*>( guiController->GetResources()->GetViewFromID("CurrentProfileDisplay") );
			if( currentProfileDisplayView != NULL )
			{
				_currentProfileDisplays.push_back( std::pair<TextView*,unsigned int>(currentProfileDisplayView,i) );
			}

			TextView *currentProfDirDisplayView = dynamic_cast<TextView*>( guiController->GetResources()->GetViewFromID("CurrentProfileDirectory") );
			if( currentProfDirDisplayView != NULL )
			{
				_currentProfDirDisplays.push_back( std::pair<TextView*,unsigned int>(currentProfDirDisplayView,i) );
			}

			// Retrieve primitive list
			ShivaGUI::ListView *profilesListView = dynamic_cast<ShivaGUI::ListView*>( guiController->GetResources()->GetViewFromID("ProfilesList") );
			if( profilesListView != NULL )
			{
				profilesListView->SetAdapter(_profileFilesAdapter);
				_profileFileListDisplays.push_back( std::pair<ShivaGUI::ListView*,unsigned int>(profilesListView,i) );
			}

		}
		else
		{
		}
	}

	//UpdateViews();
}

