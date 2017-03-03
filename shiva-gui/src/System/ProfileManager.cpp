
#include "System/ProfileManager.h"

//----------------------------------------------------------------------------------

ShivaGUI::ProfileManager::ProfileManager( std::string _authoringProgramName, std::string _programOptionsDir )
{
	m_author = _authoringProgramName;
	m_profilesDir = "Profiles";
	m_prefsDir = _programOptionsDir;
	// Add root element to the empty profile
	TiXmlElement *profileElement = new TiXmlElement( "Profile" );
	m_profileDoc.LinkEndChild( profileElement );
	// Add a header and an options node
	profileElement->LinkEndChild( new TiXmlElement( "Header" ) );

	m_optionsNode = m_currentOptionsNode = new TiXmlElement( "Options" );
	profileElement->LinkEndChild( m_optionsNode );

	m_preferences = NULL;
	m_optionIterator = NULL;
}

//----------------------------------------------------------------------------------

ShivaGUI::ProfileManager::~ProfileManager()
{
	// Deletes all sub-nodes
	m_profileDoc.Clear();

	delete m_preferences;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ProfileManager::SetDirectory( std::string _profileDirectory )
{
	if( boost::filesystem::exists( _profileDirectory ) )
	{
		if( boost::filesystem::is_directory( _profileDirectory ) )
		{
			m_profilesDir = _profileDirectory;
			return true;
		}
		else
			std::cerr << "WARNING: ShivaGUI::ProfileManager::SetDirectory, Path " << _profileDirectory << " is not a directory!" << std::endl;
	}
	else
		std::cerr << "WARNING: ShivaGUI::ProfileManager::SetDirectory, Directory " << _profileDirectory << " does not exist!" << std::endl;

	return false;
}

//----------------------------------------------------------------------------------

std::string ShivaGUI::ProfileManager::GetDirectory()
{
	return m_profilesDir.string();
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ProfileManager::Load( std::string _profileNameXML )
{
	delete m_preferences;
	m_preferences = NULL;
	m_optionIterator = NULL;

	m_profileName = _profileNameXML;
	size_t lastdot = m_profileName.find_last_of( "." );
	if( lastdot != std::string::npos )
	{
		// chop off the ".xml" for the _profileName
		m_profileName = m_profileName.substr( 0, lastdot );
	}
	else
	{
		// add a ".xml" for the profileName
		_profileNameXML = std::string( _profileNameXML + ".xml" );
	}

	std::cout << "INFO: ProfileManager attempting to load profile file: " << _profileNameXML << std::endl;

	boost::filesystem::path profilePath( m_profilesDir / _profileNameXML );

	if( boost::filesystem::exists( profilePath ) )
	{
		if( boost::filesystem::is_regular_file( profilePath ) )
		{
			// This will clear all the existing data
			m_profileDoc.LoadFile( profilePath.string() );
			m_optionsNode = NULL;

			// Retrieve options node
			TiXmlElement *rootElement = m_profileDoc.RootElement();

			if( rootElement != NULL )
			{
				// The boost iequals is a case-insensitive string comparison
				if( boost::algorithm::iequals( rootElement->ValueStr(), "profile" ) )
				{
					m_optionsNode = m_currentOptionsNode = GetSubElement( rootElement, "options" );

					if( m_optionsNode == NULL )
					{
						std::cerr << "WARNING: Profile Options node not found in file: " << profilePath << std::endl;
					}
					else
					{
						// We have loaded the xml document and it has a valid options node
						// With this limited checking, we will call it a valid profile
						// TODO: If we need more rigorous checking, it should be done here

						std::cerr << "INFO: Loaded user profile: " << _profileNameXML << std::endl;
						return true;
					}
				}
				else
					std::cerr << "WARNING: Profile root element is not of value 'Profile' in file: " << profilePath << std::endl;
			}
			else
				std::cerr << "WARNING: Profile Load cannot retrieve profile's root element from file: "<< profilePath << std::endl;
		}
		else
			std::cerr << "WARNING: ShivaGUI::ProfileManager::Load, " << profilePath << " is not a file!" << std::endl;
	}
	else
		std::cerr << "WARNING: ShivaGUI::ProfileManager::Load, file " << _profileNameXML << " does not exist in profile directory: " << m_profilesDir << std::endl;

	return false;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ProfileManager::Save( std::string _profileNameXML )
{
	// Update header
	UpdateHeader();

	m_profileName = _profileNameXML;
	unsigned int lastdot = m_profileName.find_last_of( "." );
	if( lastdot != std::string::npos )
	{
		// chop off the ".xml" for the _profileName
		m_profileName = m_profileName.substr( 0, lastdot );
	}
	else
	{
		// add a ".xml" for the profileName
		_profileNameXML = _profileNameXML + ".xml";
	}

	boost::filesystem::path profilePath( m_profilesDir / _profileNameXML );

	if( !m_profileDoc.SaveFile( profilePath.string() ) )
	{
		std::cerr << "WARNING: failed to save profile to file: " << profilePath << std::endl;
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ProfileManager::Copy( std::string _profileName, std::string _newProfileName )
{
	if( _profileName.empty() )
	{
		std::cerr << "WARNING: ProfileManager asked to copy profile but given empty profile name" << std::endl;
		return false;
	}
	if( _newProfileName.empty() )
	{
		std::cerr << "WARNING: ProfileManager asked to copy profile but given empty destination profile name" << std::endl;
		return false;
	}
	if( !boost::filesystem::exists( m_profilesDir ) || !boost::filesystem::is_directory( m_profilesDir ) )
	{
		std::cerr << "WARNING: ProfileManager asked to copy profile but no valid profile directory:" << m_profilesDir.string() << std::endl;
		return false;
	}

	try
	{
		// Copy profile xml
		boost::filesystem::copy_file( ( m_profilesDir / _profileName ).replace_extension( ".xml" ), ( m_profilesDir / _newProfileName ).replace_extension( ".xml" ) );

		// Copy profile directory if it exists
		if( boost::filesystem::exists( m_profilesDir / _profileName ) && boost::filesystem::is_directory( m_profilesDir / _profileName ) )
		{
			boost::filesystem3::copy_directory( m_profilesDir / _profileName, m_profilesDir / _newProfileName );
		}
	}
	catch( boost::filesystem::filesystem_error &ErrorCode )
	{
		// TODO: properly report on error code
		std::cerr << "WARNING: ProfileManager::Copy threw a boost::filesystem::filesystem_error after copy_file: " << ErrorCode.code().message() << std::endl;
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------

std::string ShivaGUI::ProfileManager::GetCurrentOptionsDir()
{
	return ( m_profilesDir / m_profileName / m_prefsDir ).string();
}

//----------------------------------------------------------------------------------

std::string ShivaGUI::ProfileManager::GetCurrentProfileFileName()
{
	return m_profileName;
}

//----------------------------------------------------------------------------------

ShivaGUI::SharedPreferences* ShivaGUI::ProfileManager::GetProgSpecificOptions()
{
	if( m_profilesDir.empty() )
	{
		std::cerr << "WARNING: ProfileManager::GetProgSpecificOptions() working with empty profile directory" << std::endl;
		return NULL;
	}
	// Don't need to test _profilesDir for validity, should have been done when it was set - if it's changed since then, tough luck!

	if( m_profileName.empty() )
	{
		std::cerr << "WARNING: ProfileManager::GetProgSpecificOptions() working with empty profile name" << std::endl;
		return NULL;
	}

	// If the directory does not exist, need to create it

	boost::filesystem::path prefsPath( m_profilesDir / m_profileName / m_prefsDir );

	if( !boost::filesystem::exists( prefsPath ) )
	{
		boost::filesystem::create_directories( prefsPath );
	}

	prefsPath = prefsPath / "prefs.xml";

	m_preferences = new SharedPreferences( prefsPath.string() );

	return m_preferences;
}

//----------------------------------------------------------------------------------

int ShivaGUI::ProfileManager::ContainsOption( std::string _key )
{
	int i = 0;
	for( TiXmlNode *childNode = m_currentOptionsNode->FirstChild(); childNode != NULL; childNode = childNode->NextSibling() )
	{
		if( boost::algorithm::iequals( childNode->ValueStr(), _key ) )
		{
			i++;
		}
	}

	return i;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ProfileManager::EnterOptionNode( std::string _key, int _index )
{
	int i = 0;
	for( TiXmlNode *childNode = m_currentOptionsNode->FirstChild(); childNode != NULL; childNode = childNode->NextSibling() )
	{
		if( boost::algorithm::iequals(childNode->ValueStr(), _key) )
		{
			if( i == _index )
			{
				m_currentOptionsNode = childNode->ToElement();
				return true;
			}
			i++;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------

// Leaves a sub-node in a hierarchical option, going up the tree to the parent
// You cannot go higher than the options level, e.g. you cannot go to the parent and view the header

void ShivaGUI::ProfileManager::ExitOptionNode()
{
	if( m_currentOptionsNode != m_optionsNode )
		m_currentOptionsNode = m_currentOptionsNode->Parent()->ToElement();
}

//----------------------------------------------------------------------------------

void ShivaGUI::ProfileManager::InitIterator()
{
	if( m_currentOptionsNode != NULL )
		m_optionIterator = m_currentOptionsNode->FirstChild();
	else
		m_optionIterator = NULL;
}

//----------------------------------------------------------------------------------

std::string ShivaGUI::ProfileManager::GetNextKey( int &_index )
{
	if( m_currentOptionsNode != NULL && m_optionIterator != NULL )
	{
		std::string retValue = m_optionIterator->ValueStr();
		m_optionIterator = m_optionIterator->NextSibling();

		_index = 0;
		for( TiXmlNode *childNode = m_currentOptionsNode->FirstChild(); childNode != NULL && childNode != m_currentOptionsNode; childNode = childNode->NextSibling() )
		{
			if( boost::algorithm::iequals( childNode->ValueStr(), retValue ) )
			{
				++_index;
			}
		}

		return retValue;
	}
	_index = -1;
	return "END";
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ProfileManager::GetBoolean( std::string _key, bool _defaultValue, int _index )
{
	return GetOption( _key, _defaultValue, _index ); 
}
//----------------------------------------------------------------------------------

float ShivaGUI::ProfileManager::GetFloat( std::string _key, float _defaultValue, int _index )	
{
	return GetOption( _key, _defaultValue, _index ); 
}

//----------------------------------------------------------------------------------

int ShivaGUI::ProfileManager::GetInt( std::string _key, int _defaultValue, int _index )					
{
	return GetOption( _key, _defaultValue, _index ); 
}

//----------------------------------------------------------------------------------

std::string ShivaGUI::ProfileManager::GetString( std::string _key, std::string _defaultValue, int _index )	
{
	return GetOption( _key, _defaultValue, _index ); 
}

//----------------------------------------------------------------------------------

void ShivaGUI::ProfileManager::SetBoolean( std::string _key, bool _value )						
{
	SetOption( _key, _value );
}

//----------------------------------------------------------------------------------

void ShivaGUI::ProfileManager::SetFloat( std::string _key, float _value )
{
	SetOption( _key, _value );
}

//----------------------------------------------------------------------------------

void ShivaGUI::ProfileManager::SetInt( std::string _key, int _value )								
{
	SetOption( _key, _value );
}

//----------------------------------------------------------------------------------

void ShivaGUI::ProfileManager::SetString( std::string _key, std::string _value )					
{
	SetOption( _key, _value );
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ProfileManager::UpdateHeader()
{
	// Retrieve header root

	TiXmlElement *rootElement = m_profileDoc.RootElement();

	if( rootElement != NULL )
	{
		// The boost iequals is a case-insensitive string comparison
		if( boost::algorithm::iequals( rootElement->ValueStr(), "Profile" ) )
		{
			TiXmlElement *headerNode = GetSubElement( rootElement, "Header" );

			if( headerNode == NULL )
			{
				// Need to add a header
				headerNode = new TiXmlElement( "Header" );
				rootElement->LinkEndChild( headerNode );
			}

			// Populate header with info
				// Name
				AddSubElementText( headerNode, "Name", m_name );
				// Authoring program
				AddSubElementText( headerNode, "Author", m_author );


		}
		else
			std::cerr << "WARNING: Profile UpdateHeader() root element is not of value 'Profile'" << std::endl;
	}
	else
		std::cerr << "WARNING: Profile UpdateHeader() cannot retrieve profile's root element" << std::endl;

	return false;
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::ProfileManager::GetSubElement( TiXmlElement *_parent, std::string _value, int _index )
{
	if( _parent == NULL )
	{
		// useful for debugging
		std::cerr << "WARNING: ShivaGUI::ProfileManager::GetSubElement() given NULL parent node" << std::endl;
		return NULL;
	}

	int i = 0;
	for( TiXmlNode *childNode = _parent->FirstChild(); childNode != NULL; childNode = childNode->NextSibling() )
	{
		if( boost::algorithm::iequals( childNode->ValueStr(), _value ) )
		{
			if( i == _index )
				return childNode->ToElement();
			else
				++i;
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------------

TiXmlText* ShivaGUI::ProfileManager::GetSubText( TiXmlElement *_parent )
{
	if( _parent == NULL )
	{
		// useful for debugging
		std::cerr << "WARNING: ShivaGUI::ProfileManager::GetSubText() given NULL parent node" << std::endl;
		return NULL;
	}

	for( TiXmlNode *childNode = _parent->FirstChild(); childNode != NULL; childNode = childNode->NextSibling() )
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

//----------------------------------------------------------------------------------

void ShivaGUI::ProfileManager::AddSubElementText( TiXmlElement *_parent, std::string _value, std::string _text )
{
	TiXmlElement *childNode = GetSubElement( _parent, _value );
	if( childNode == NULL )
	{
		childNode = new TiXmlElement( _value );
		_parent->LinkEndChild( childNode );
	}
	TiXmlText *textNode = GetSubText( childNode );
	if( textNode == NULL )
	{
		textNode = new TiXmlText( _text );
		childNode->LinkEndChild( textNode );
	}
	else
	{
		TiXmlText *newText = new TiXmlText( _text );
		childNode->ReplaceChild( textNode, *newText );
		// The replace function will delete the old node
	}
}

//----------------------------------------------------------------------------------

template < class T > T ShivaGUI::ProfileManager::GetOption( std::string _key, T _defaultValue, int _index )
{
	TiXmlElement *optionElement = GetSubElement( m_currentOptionsNode, _key, _index );

	if( optionElement == NULL )
		return _defaultValue;

	std::stringstream stream( optionElement->GetText() );
	if( !stream )
		return _defaultValue;
	T result = _defaultValue;
	stream >> std::boolalpha >> result;
	return result;
}

//----------------------------------------------------------------------------------

template <class T> void ShivaGUI::ProfileManager::SetOption( std::string _key, T _value )
{
	std::stringstream streamer;
	streamer << std::boolalpha << _value;
	AddSubElementText( m_currentOptionsNode, _key, streamer.str() );
}

//----------------------------------------------------------------------------------