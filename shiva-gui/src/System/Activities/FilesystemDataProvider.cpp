#include "System/Activities/FilesystemDataProvider.h"

//----------------------------------------------------------------------------------

ShivaGUI::FilesystemDataProvider::FilesystemDataProvider()
{
	m_showFiles = m_showDirectories = true;
}

//----------------------------------------------------------------------------------

void ShivaGUI::FilesystemDataProvider::SetCurrentDirectoryNew( std::string _dir )
{
	if( boost::filesystem::exists( _dir ) )
	{
		if( boost::filesystem::is_directory( _dir ) )
		{
			m_currentDir = _dir;
		}
		else
			std::cerr << "WARNING: ShivaGUI::FilesystemDataProvider::SetCurrentDirectory, Path " << _dir << " is not a directory!" << std::endl;
	}
	else
		std::cerr << "WARNING: ShivaGUI::FilesystemDataProvider::SetCurrentDirectory, Directory " << _dir << " does not exist!" << std::endl;
}

//----------------------------------------------------------------------------------

std::string ShivaGUI::FilesystemDataProvider::GetCurrentDirectory()
{
	return m_currentDir.string();
}

//----------------------------------------------------------------------------------

void ShivaGUI::FilesystemDataProvider::SetShowFiles( bool _value )
{
	m_showFiles = _value;
}

//----------------------------------------------------------------------------------

void ShivaGUI::FilesystemDataProvider::SetShowDirectories( bool _value )
{
	m_showDirectories = _value;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::FilesystemDataProvider::QueryAttribute( int _dataEntry, std::string _attribute )
{
	if( _dataEntry < GetNumEntries() && ( _attribute == "" ) )
	{
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------

std::string ShivaGUI::FilesystemDataProvider::GetAttributeString( int _dataEntry, std::string _attribute )
{
	int numEntries = -1;
	for( boost::filesystem::directory_iterator it( m_currentDir ); it != boost::filesystem::directory_iterator(); ++it )
	{
		if( m_showFiles && boost::filesystem::is_regular_file( ( *it ) ) )
		{
			++numEntries;
		}
		else if( m_showDirectories && boost::filesystem::is_directory( ( *it ) ) )
		{
			std::cout << "FilesystemDataProvider dir found at entry: " << _dataEntry << std::endl;
			++numEntries;
		}

		if( numEntries == _dataEntry )
		{
			if( _attribute == "FULL_FORMATTED" )
			{
				if( boost::filesystem::is_directory( ( *it ) ) )
				{
					std::cout << "FilesystemDataProvider providing attribute string for entry: " << _dataEntry << std::endl;
					return std::string( ( *it ).path().string() + " <dir>" );
				}
				return ( *it ).path().string();
			}
			else if( _attribute == "FILENAME" )
			{
				return ( *it ).path().filename().string();
			}
			else if( _attribute == "FILENAME_NO_EXTENSION" )
			{
				return ( *it ).path().stem().string();
			}
		}
	}
	return "";
}

//----------------------------------------------------------------------------------

int ShivaGUI::FilesystemDataProvider::GetNumEntries()
{
	int numEntries = 0;
	for( boost::filesystem::directory_iterator it( m_currentDir ); it != boost::filesystem::directory_iterator(); ++it )
	{
		if( m_showFiles && boost::filesystem::is_regular_file( ( *it ) ) )
		{
			++numEntries;
		}
		else if( m_showDirectories && boost::filesystem::is_directory( ( *it ) ) )
		{
			++numEntries;
		}
	}

	std::cout << "FilesystemDataProvider reports : " << numEntries << " entries" << std::endl;
	return numEntries;
}

//----------------------------------------------------------------------------------