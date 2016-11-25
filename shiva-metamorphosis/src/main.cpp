

#include "GUIManager.h"
#include "ResourceManager.h"

#include "SDFView.h"
#include "ShivaModelManager.h"
#include "ModelChooser.h"
#include "ColourChooser.h"
#include "RotationChooser.h"
#include "ShivaMetamorphosis.h"

//////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <boost/program_options.hpp>

//////////////////////////////////////////////////////////////////////////


// This will be used for storing our program options, which may also be loaded by command-line
struct program_options
{
	std::string profileDirectory;
	std::string profileName;
};

// Forward declaration of a function that will sort out our command-line options
bool get_options(program_options *options, int argc, char **argv);



int main(int argc, char **argv)
{
	// Set our program options from command line
	program_options options;
	if( !get_options(&options,argc,argv) )
		return 0;

	ShivaModelManager::Init("Resources/Models/index.xml");

	ShivaGUI::GUIManager *mainGUIManager = new ShivaGUI::GUIManager("Metamorphosis Prototype 1b", "metamorph1b");

	mainGUIManager->RegisterActivityCreator("ShivaMetamorphosis",ShivaMetamorphosis::Factory);
	mainGUIManager->RegisterActivityCreator("ModelChooser",ModelChooser::Factory);
	mainGUIManager->RegisterActivityCreator("ColourChooser",ColourChooser::Factory);
	mainGUIManager->RegisterActivityCreator("RotationChooser",RotationChooser::Factory);
	mainGUIManager->RegisterViewCreator("SDFView",SDFView::Factory);

	mainGUIManager->StartWithProfileChooser("ModelChooser",options.profileDirectory,options.profileName);

/*
	ShivaGUI::GUIManager *mainGUIManager = new ShivaGUI::GUIManager("Metamorphosis Prototype 1b", "metamorph1b");

	mainGUIManager->SetProfileDirectory("/home/leigh/workspace/shiva-prototype1b/Profiles");

	mainGUIManager->LoadProfile("testProf01");

	if( !mainGUIManager->CreateWindowsFromProfile() )
	{
		std::cerr<<"INFO: User profile does not specify any windows, creating one default window"<<std::endl;
		mainGUIManager->CreateWindow(640,480,100,100);
	}

	//mainGUIManager->CreateWindow(640,480,760,100);

	ShivaModelManager::Init("Resources/Models/index.xml");

	mainGUIManager->RegisterActivityCreator("ShivaMetamorphosis",ShivaMetamorphosis::Factory);
	mainGUIManager->RegisterActivityCreator("ModelChooser",ModelChooser::Factory);
	mainGUIManager->RegisterActivityCreator("ColourChooser",ColourChooser::Factory);
	mainGUIManager->RegisterActivityCreator("RotationChooser",RotationChooser::Factory);
	mainGUIManager->RegisterViewCreator("SDFView",SDFView::Factory);

	mainGUIManager->StartActivity("ModelChooser",NULL);

	mainGUIManager->EnterMainLoop();
*/
	/*
	do
	{
		//float deltaTs = mainWindow->GetDeltaTs();

		//mainInputController->Process(deltaTs);
		mainGUIManager->Update(1.0f/50.0f);

		mainGUIManager->Draw();
	}
	while( !mainGUIManager->GetExitEvent() );
	*/

	return 0;

}

// This function will deal with our command line options, setting up the profile directory and profile name
bool get_options(program_options *options, int argc, char **argv)
{
	boost::program_options::options_description generic("Generic options");
	generic.add_options()
		("version,v", "print version string")
		("help,h", "produce help message");

	boost::program_options::options_description profile("Profile options");
	profile.add_options()
		("profile_directory,d",
		 boost::program_options::value<std::string>(&(options->profileDirectory))->default_value("Profiles"),
		 "profile directory")
		("profile_name,n",
		 boost::program_options::value<std::string>(&(options->profileName))->default_value(""),
		 "profile name");


	boost::program_options::options_description allOptions("Allowed options");
	allOptions.add(generic).add(profile);

	boost::program_options::variables_map variableMap;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, allOptions), variableMap);
	boost::program_options::notify(variableMap);

	// See what we have found in our command line options:
	if( variableMap.count("help") )
	{
		// This will print out a nicely formatted help text of all our options
	    std::cout << allOptions << std::endl;
	    // Return false to exit the program
	    return false;
	}
	else if( variableMap.count("version") )
	{
	    std::cout << "SHIVA Metamorphosis Exercise version 2.1" << std::endl;
	    // Return false to exit the program
	    return false;
	}

	return true;
}
