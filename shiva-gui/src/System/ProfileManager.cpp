
#include "System/ProfileManager.h"

#include <iostream>
#include <boost/algorithm/string.hpp>

//////////////////////////////////////////////////////////////////////////

ShivaGUI::ProfileManager::ProfileManager( std::string authoringProgramName, std::string programOptionsDir )
{
	_author = authoringProgramName;
	_profilesDir = "Profiles";
	_prefsDir = programOptionsDir;
	// Add root element to the empty profile
	TiXmlElement *profileElement = new TiXmlElement( "Profile" );
	_profileDoc.LinkEndChild( profileElement );
	// Add a header and an options node
	profileElement->LinkEndChild( new TiXmlElement( "Header" ) );

	_optionsNode = _currentOptionsNode = new TiXmlElement( "Options" );
	profileElement->LinkEndChild( _optionsNode );

	_preferences = NULL;
	_optionIterator = NULL;
}

ShivaGUI::ProfileManager::~ProfileManager()
{
	// Deletes all sub-nodes
	_profileDoc.Clear();

	delete _preferences;
}

bool ShivaGUI::ProfileManager::SetDirectory( std::string profileDirectory )
{
	if( boost::filesystem::exists(profileDirectory) )
	{
		if( boost::filesystem::is_directory(profileDirectory) )
		{
			_profilesDir = profileDirectory;
			return true;
		}
		else
			std::cerr<< "WARNING: ShivaGUI::ProfileManager::SetDirectory, Path " <<profileDirectory<< " is not a directory!"<<std::endl;
	}
	else
		std::cerr<< "WARNING: ShivaGUI::ProfileManager::SetDirectory, Directory " <<profileDirectory<< " does not exist!"<<std::endl;

	return false;
}

std::string ShivaGUI::ProfileManager::GetDirectory()
{
	return _profilesDir.string();
}

bool ShivaGUI::ProfileManager::Load( std::string profileNameXML )
{
	delete _preferences;
	_preferences = NULL;
	_optionIterator = NULL;

	_profileName = profileNameXML;
	size_t lastdot = _profileName.find_last_of(".");
	if( lastdot != std::string::npos )
	{
		// chop off the ".xml" for the _profileName
		_profileName = _profileName.substr(0, lastdot);
	}
	else
	{
		// add a ".xml" for the profileName
		profileNameXML = std::string(profileNameXML + ".xml");
	}

	std::cout<<"INFO: ProfileManager attempting to load profile file: "<< profileNameXML<<std::endl;

	boost::filesystem::path profilePath(_profilesDir / profileNameXML);

	if( boost::filesystem::exists( profilePath ) )
	{
		if( boost::filesystem::is_regular_file( profilePath ) )
		{
			// This will clear all the existing data
			_profileDoc.LoadFile( profilePath.string() );
			_optionsNode = NULL;

			// Retrieve options node
			TiXmlElement *rootElement = _profileDoc.RootElement();

			if( rootElement != NULL )
			{
				// The boost iequals is a case-insensitive string comparison
				if( boost::algorithm::iequals(rootElement->ValueStr(), "profile") )
				{
					_optionsNode = _currentOptionsNode = GetSubElement(rootElement, "options" );

					if( _optionsNode == NULL )
					{
						std::cerr<<"WARNING: Profile Options node not found in file: "<< profilePath <<std::endl;
					}
					else
					{
						// We have loaded the xml document and it has a valid options node
						// With this limited checking, we will call it a valid profile
						// TODO: If we need more rigorous checking, it should be done here

						std::cerr<<"INFO: Loaded user profile: "<<profileNameXML<<std::endl;
						return true;
					}
				}
				else
					std::cerr<<"WARNING: Profile root element is not of value 'Profile' in file: "<< profilePath <<std::endl;
			}
			else
				std::cerr<<"WARNING: Profile Load cannot retrieve profile's root element from file: "<< profilePath <<std::endl;
		}
		else
			std::cerr<< "WARNING: ShivaGUI::ProfileManager::Load, " <<profilePath<< " is not a file!"<<std::endl;
	}
	else
		std::cerr<< "WARNING: ShivaGUI::ProfileManager::Load, file " <<profileNameXML<< " does not exist in profile directory: "<< _profilesDir <<std::endl;

	return false;
}

bool ShivaGUI::ProfileManager::Save( std::string profileNameXML )
{
	// Update header
	UpdateHeader();

	_profileName = profileNameXML;
	unsigned int lastdot = _profileName.find_last_of(".");
	if( lastdot != std::string::npos )
	{
		// chop off the ".xml" for the _profileName
		_profileName = _profileName.substr(0, lastdot);
	}
	else
	{
		// add a ".xml" for the profileName
		profileNameXML = profileNameXML + ".xml";
	}

	boost::filesystem::path profilePath(_profilesDir / profileNameXML);

	if( !_profileDoc.SaveFile( profilePath.string() ) )
	{
		std::cerr<<"WARNING: failed to save profile to file: "<< profilePath <<std::endl;
		return false;
	}

	return true;
}

bool ShivaGUI::ProfileManager::Copy( std::string profileName, std::string newProfileName )
{
	if( profileName.empty() )
	{
		std::cerr<<"WARNING: ProfileManager asked to copy profile but given empty profile name"<<std::endl;
		return false;
	}
	if( newProfileName.empty() )
	{
		std::cerr<<"WARNING: ProfileManager asked to copy profile but given empty destination profile name"<<std::endl;
		return false;
	}
	if( !boost::filesystem::exists(_profilesDir) || !boost::filesystem::is_directory(_profilesDir) )
	{
		std::cerr<<"WARNING: ProfileManager asked to copy profile but no valid profile directory:" << _profilesDir.string() <<std::endl;
		return false;
	}

	try
	{
		// Copy profile xml
		boost::filesystem::copy_file( (_profilesDir / profileName).replace_extension(".xml"),(_profilesDir / newProfileName).replace_extension(".xml"));

		// Copy profile directory if it exists
		if( boost::filesystem::exists(_profilesDir / profileName) && boost::filesystem::is_directory(_profilesDir / profileName) )
		{
			boost::filesystem3::copy_directory(_profilesDir / profileName, _profilesDir / newProfileName);
		}
	}
	catch( boost::filesystem::filesystem_error &ErrorCode )
	{
		// TODO: properly report on error code
		std::cerr<<"WARNING: ProfileManager::Copy threw a boost::filesystem::filesystem_error after copy_file"<<std::endl;
		return false;
	}

	return true;
}

std::string ShivaGUI::ProfileManager::GetCurrentOptionsDir()
{
	return (_profilesDir / _profileName / _prefsDir).string();
}

std::string ShivaGUI::ProfileManager::GetCurrentProfileFileName()
{
	return _profileName;
}

ShivaGUI::SharedPreferences* ShivaGUI::ProfileManager::GetProgSpecificOptions()
{
	if( _profilesDir.empty() )
	{
		std::cerr<<"WARNING: ProfileManager::GetProgSpecificOptions() working with empty profile directory"<<std::endl;
		return NULL;
	}
	// Don't need to test _profilesDir for validity, should have been done when it was set - if it's changed since then, tough luck!

	if( _profileName.empty() )
	{
		std::cerr<<"WARNING: ProfileManager::GetProgSpecificOptions() working with empty profile name"<<std::endl;
		return NULL;
	}

	// If the directory does not exist, need to create it

	boost::filesystem::path prefsPath( _profilesDir / _profileName / _prefsDir );

	if( !boost::filesystem::exists(prefsPath) )
	{
		boost::filesystem::create_directories(prefsPath);
	}

	prefsPath = prefsPath / "prefs.xml";

	_preferences = new SharedPreferences( prefsPath.string() );

	return _preferences;
}

int ShivaGUI::ProfileManager::ContainsOption( std::string key )
{
	int i = 0;
	for( TiXmlNode *childNode = _currentOptionsNode->FirstChild(); childNode != NULL; childNode = childNode->NextSibling() )
	{
		if( boost::algorithm::iequals(childNode->ValueStr(), key) )
		{
			i++;
		}
	}

	return i;
}

bool ShivaGUI::ProfileManager::EnterOptionNode( std::string key, int index )
{
	int i = 0;
	for( TiXmlNode *childNode = _currentOptionsNode->FirstChild(); childNode != NULL; childNode = childNode->NextSibling() )
	{
		if( boost::algorithm::iequals(childNode->ValueStr(), key) )
		{
			if( i == index )
			{
				_currentOptionsNode = childNode->ToElement();
				return true;
			}
			i++;
		}
	}
	return false;
}

/// Leaves a sub-node in a hierarchical option, going up the tree to the parent
/// You cannot go higher than the options level, e.g. you cannot go to the parent and view the header
void ShivaGUI::ProfileManager::ExitOptionNode()
{
	if( _currentOptionsNode != _optionsNode )
		_currentOptionsNode = _currentOptionsNode->Parent()->ToElement();
}


void ShivaGUI::ProfileManager::InitIterator()
{
	if( _currentOptionsNode != NULL )
		_optionIterator = _currentOptionsNode->FirstChild();
	else
		_optionIterator = NULL;
}

std::string ShivaGUI::ProfileManager::GetNextKey(int &index)
{
	if( _currentOptionsNode != NULL && _optionIterator != NULL )
	{
		std::string retValue = _optionIterator->ValueStr();
		_optionIterator = _optionIterator->NextSibling();

		index = 0;
		for( TiXmlNode *childNode = _currentOptionsNode->FirstChild(); childNode != NULL && childNode != _currentOptionsNode; childNode = childNode->NextSibling() )
		{
			if( boost::algorithm::iequals(childNode->ValueStr(), retValue) )
			{
				++index;
			}
		}

		return retValue;
	}
	index = -1;
	return "END";
}


bool ShivaGUI::ProfileManager::GetBoolean(std::string key, bool defaultValue, int index )				{return GetOption(key,defaultValue,index);}
float ShivaGUI::ProfileManager::GetFloat(std::string key, float defaultValue, int index )				{return GetOption(key,defaultValue,index);}
int ShivaGUI::ProfileManager::GetInt(std::string key, int defaultValue, int index )						{return GetOption(key,defaultValue,index);}
std::string ShivaGUI::ProfileManager::GetString(std::string key, std::string defaultValue, int index )	{return GetOption(key,defaultValue,index);}

void ShivaGUI::ProfileManager::SetBoolean(std::string key, bool value)						{SetOption(key,value);}
void ShivaGUI::ProfileManager::SetFloat(std::string key, float value)							{SetOption(key,value);}
void ShivaGUI::ProfileManager::SetInt(std::string key, int value)								{SetOption(key,value);}
void ShivaGUI::ProfileManager::SetString(std::string key, std::string value)					{SetOption(key,value);}





bool ShivaGUI::ProfileManager::UpdateHeader()
{
	// Retrieve header root

	TiXmlElement *rootElement = _profileDoc.RootElement();

	if( rootElement != NULL )
	{
		// The boost iequals is a case-insensitive string comparison
		if( boost::algorithm::iequals(rootElement->ValueStr(), "Profile") )
		{
			TiXmlElement *headerNode = GetSubElement(rootElement, "Header" );

			if( headerNode == NULL )
			{
				// Need to add a header
				headerNode = new TiXmlElement( "Header" );
				rootElement->LinkEndChild( headerNode );
			}

			// Populate header with info
				// Name
				AddSubElementText(headerNode,"Name",_name);
				// Authoring program
				AddSubElementText(headerNode,"Author",_author);


		}
		else
			std::cerr<<"WARNING: Profile UpdateHeader() root element is not of value 'Profile'"<<std::endl;
	}
	else
		std::cerr<<"WARNING: Profile UpdateHeader() cannot retrieve profile's root element"<<std::endl;

	return false;
}

TiXmlElement* ShivaGUI::ProfileManager::GetSubElement(TiXmlElement *parent, std::string value, int index)
{
	if( parent == NULL )
	{
		// useful for debugging
		std::cerr<<"WARNING: ShivaGUI::ProfileManager::GetSubElement() given NULL parent node"<<std::endl;
		return NULL;
	}

	int i = 0;
	for( TiXmlNode *childNode = parent->FirstChild(); childNode != NULL; childNode = childNode->NextSibling() )
	{
		if( boost::algorithm::iequals(childNode->ValueStr(), value) )
		{
			if( i == index )
				return childNode->ToElement();
			else
				++i;
		}
	}
	return NULL;
}

TiXmlText* ShivaGUI::ProfileManager::GetSubText(TiXmlElement *parent)
{
	if( parent == NULL )
	{
		// useful for debugging
		std::cerr<<"WARNING: ShivaGUI::ProfileManager::GetSubText() given NULL parent node"<<std::endl;
		return NULL;
	}

	for( TiXmlNode *childNode = parent->FirstChild(); childNode != NULL; childNode = childNode->NextSibling() )
	{
		// Does a safe check and cast, returning NULL if not successful
		TiXmlText *textNode = childNode->ToText();
		if( textNode != NULL )
		{
			return textNode;
		}
	}
	return NULL;
}

void ShivaGUI::ProfileManager::AddSubElementText(TiXmlElement *parent, std::string value, std::string text)
{
	TiXmlElement *childNode = GetSubElement(parent, value );
	if( childNode == NULL )
	{
		childNode = new TiXmlElement( value );
		parent->LinkEndChild( childNode );
	}
	TiXmlText *textNode = GetSubText(childNode);
	if( textNode == NULL )
	{
		textNode = new TiXmlText( text );
		childNode->LinkEndChild( textNode );
	}
	else
	{
		TiXmlText *newText = new TiXmlText( text );
		childNode->ReplaceChild(textNode, *newText);
		// The replace function will delete the old node
	}
}


template <class T> T ShivaGUI::ProfileManager::GetOption(std::string key, T defaultValue, int index)
{
	TiXmlElement *optionElement = GetSubElement(_currentOptionsNode, key, index);

	if( optionElement == NULL )
		return defaultValue;

	std::stringstream stream( optionElement->GetText() );
	if( !stream )
		return defaultValue;
	T result = defaultValue;
	stream >> std::boolalpha >> result;
	return result;
}

template <class T> void ShivaGUI::ProfileManager::SetOption(std::string key, T value)
{
	std::stringstream streamer;
	streamer <<std::boolalpha<< value;
	AddSubElementText(_currentOptionsNode,key, streamer.str() );
}
