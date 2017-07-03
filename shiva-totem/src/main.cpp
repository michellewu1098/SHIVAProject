///-----------------------------------------------------------------------------------------------
/// \file main.cpp
/// \brief This is a simple demo showing how to use the SHIVA GUI System and the Custom Layout
/// \author Leigh McLoughlin
/// \date Jul 2, 2012
/// \version 1.0
///-----------------------------------------------------------------------------------------------

/*
	TODO:

	[x] - Fix rotations for mouse
	[x] - Fix camera position for adding / removing objects with rotating
	[x] - Fix nudging after adding new object / generally changing object list
	[x] - Scan support for ListView
	[x] - Fix bug with TextButton deflate to xml
	[x] - Fix bug with VolView deflate to xml
	[x] - Focus selection support for ListView
	[x] - Eyegaze hack support for ListView
	[x] - Scan order of buttons
	[x] - Selection order of buttons
	[ ] - Save to Volfile
	   [ ] - Fix transforms
	   [ ] - Spheres seem to have different function
	   [ ] - Cylinders seem to have different function
	   [ ] - Torus orientation
	[x] - ListView / DataAdapter / ShivaModelManager - automatically create primitives based on models index.xml file
	[ ] - Colour highlight of selected object
	[ ] - Blend amount buttons
	[ ] - Limit number of objects on tree
	[ ] - Limit offset to bounding box
	[ ] - Reset rotation button
	
	[ ] - Bar and base a different colour

	[ ] - Input screen: all controls
	[ ] - Output screen: just model - linked to rotation and model 
	[ ] - ListView / DataAdapter / ShivaModelManager - load vol file models based on index.xml file
	
	[ ] - Scrollcontainer needs to scroll up/down when scan or selection goes beyond visible items
	[ ] - Fix memory leaks!


	// Longer term:
	[ ] - Eyegaze rest: a button enables / disables all other cells but that one
	[ ] - Third screen: rotation and model view

*/

//#define _CRTDBG_MAP_ALLOC
//#define _CRTDBG_MAP_ALLOC_NEW
//#include <stdlib.h>
//#include <crtdbg.h>
//
//#ifdef _DEBUG
//   #ifndef DBG_NEW
//      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//      #define new DBG_NEW
//   #endif
//#endif  // _DEBUG

#include <iostream>
#include <boost/program_options.hpp>

#include "GUIManager.h"

#include "AssembleActivity.h"
#include "DrillActivity.h"
#include "EditMenuActivity.h"
#include "BlendAdjustActivity.h"
#include "RotateObjectActivity.h"
#include "UniformScaleActivity.h"
#include "NudgeActivity.h"
#include "PrintActivity.h"
#include "System/Activities/CreateNewProfileActivity.h"

#include "ShivaModelManager.h"
#include "Totem/TotemController.h"
#include "VolView.h"
#include "CommandManager.h"

#include "VolumeTree/VolumeTree.h"
#include "VolumeTree/Leaves/SphereNode.h"
#include "VolumeTree/Leaves/CylinderNode.h"
#include "VolumeTree/Leaves/ConeNode.h"
#include "VolumeTree/Leaves/CubeNode.h"
#include "VolumeTree/Leaves/TorusNode.h"
#include "VolumeTree/Leaves/VolCacheNode.h"
#include "VolumeTree/Nodes/CSG.h"
#include "VolumeTree/Nodes/TransformNode.h"

#ifdef WIN32
	#ifdef CreateWindow
		#undef CreateWindow
	#endif // CreateWindow
#endif // WIN32

//----------------------------------------------------------------------------------
/// \brief This will be used for storing our program options, which may also be loaded by command-line
//----------------------------------------------------------------------------------
struct ProgramOptions
{
	std::string profileDirectory;
	std::string profileName;
};
//----------------------------------------------------------------------------------
/// \brief Forward declaration of a function that will sort out our command-line options
//----------------------------------------------------------------------------------

bool GetOptions( ProgramOptions *_options, int _argc, char **_argv );

//----------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// Set our program options from command line
	ProgramOptions options;
	if( !GetOptions( &options, argc, argv ) )
		return 0;


	ShivaModelManager *modelManager = ShivaModelManager::Init( "Resources/Models/index.xml" );
	ShivaGUI::GUIManager *mainGUIManager = new ShivaGUI::GUIManager( "SHIVA Totem Prototype 1b", "totem1b" );

	Totem::Controller *totemController = Totem::Controller::Init();
	Totem::CommandManager *commandManager = Totem::CommandManager::Init();
	VolumeTree::Node *currentModelNode = NULL;

	if( modelManager != NULL )
	{
		int numModels = modelManager->GetNumEntries();
		std::cout << "INFO: ModelManager reports " << numModels << " models" << std::endl;
		totemController->SetNumPrimitives( numModels );

		for( int i = 0; i < numModels; i++ )
		{
			currentModelNode = NULL;

			if( modelManager->QueryAttribute( i, "primitive" ) )
			{
				// Model is a primitive
				std::string primType = modelManager->GetAttributeString( i, "primitive" );

				if( primType == "Sphere" )
				{
					currentModelNode = new VolumeTree::SphereNode( 0.25f, 0.25f, 0.25f );
				}
				else if( primType == "Cone" )
				{
					currentModelNode = new VolumeTree::ConeNode( 0.5f, 0.25f );
				}
				else if( primType == "Cylinder" )
				{
					currentModelNode = new VolumeTree::CylinderNode( 0.5f, 0.25f, 0.25f );
				}
				else if( primType == "Cube" )
				{
					currentModelNode = new VolumeTree::CubeNode( 0.5f );
				}
				else if( primType == "Torus" )
				{
					currentModelNode = new VolumeTree::TorusNode( 0.25, 0.02f );
				}
				else if( primType == "Box" )
				{
					currentModelNode = new VolumeTree::CubeNode( 1.0f, 0.5f, 0.5f );
				}
				else
				{
					std::cerr << "WARNING: ModelManager reports for entry " << i << " unknown primitive type: " << primType << std::endl;
				}
			}


			if( currentModelNode != NULL )
			{
				totemController->SetPrimitiveNode( i, currentModelNode );
			}
			else
			{
				std::cerr << "WARNING: ModelManager does not give usable model" << std::endl;
			}
		}
	}

	mainGUIManager->RegisterActivityCreator( "AssembleActivity", AssembleActivity::Factory );
	mainGUIManager->RegisterActivityCreator( "DrillActivity", DrillActivity::Factory );
	mainGUIManager->RegisterActivityCreator( "EditMenuActivity", EditMenuActivity::Factory );
	mainGUIManager->RegisterActivityCreator( "BlendAdjustActivity", BlendAdjustActivity::Factory );
	mainGUIManager->RegisterActivityCreator( "RotateObjectActivity", RotateObjectActivity::Factory );
	mainGUIManager->RegisterActivityCreator( "UniformScaleActivity", UniformScaleActivity::Factory );
	mainGUIManager->RegisterActivityCreator( "NudgeActivity", NudgeActivity::Factory );
	mainGUIManager->RegisterActivityCreator( "PrintActivity", PrintActivity::Factory );
	mainGUIManager->RegisterActivityCreator( "NewProfileActivity", ShivaGUI::CreateNewProfileActivity::Factory );
	mainGUIManager->RegisterViewCreator( "VolView", VolView::Factory );

	mainGUIManager->StartWithProfileChooser( "AssembleActivity", options.profileDirectory, options.profileName );

	Totem::Controller::UnInit();
	Totem::CommandManager::UnInit();

	delete currentModelNode;
	currentModelNode = NULL;
	delete mainGUIManager;

	//_CrtDumpMemoryLeaks();
	return 0;
}

//----------------------------------------------------------------------------------

// brief This function will deal with our command line options, setting up the profile directory and profile name

bool GetOptions( ProgramOptions *_options, int _argc, char **_argv )
{
	boost::program_options::options_description generic( "Generic options" );
	generic.add_options()
		( "version,v", "print version string" )
		( "help,h", "produce help message" );

	boost::program_options::options_description profile( "Profile options" );
	profile.add_options()
		( "profile_directory,d",
		 boost::program_options::value< std::string >( &( _options->profileDirectory ) )->default_value( "Profiles" ),
		 "profile directory" )
		( "profile_name,n",
		 boost::program_options::value< std::string >( &( _options->profileName ) )->default_value( "" ),
		 "profile name" );


	boost::program_options::options_description allOptions( "Allowed options" );
	allOptions.add( generic ).add( profile );

	boost::program_options::variables_map variableMap;
	boost::program_options::store( boost::program_options::parse_command_line( _argc, _argv, allOptions ), variableMap );
	boost::program_options::notify( variableMap );

	// See what we have found in our command line options:
	if( variableMap.count( "help" ) )
	{
		// This will print out a nicely formatted help text of all our options
	    std::cout << allOptions << std::endl;
	    // Return false to exit the program
	    return false;
	}
	else if( variableMap.count( "version" ) )
	{
	    std::cout << "SHIVA Demo 1.0 - simple GUI System demo" << std::endl;
	    // Return false to exit the program
	    return false;
	}

	return true;
}

//----------------------------------------------------------------------------------