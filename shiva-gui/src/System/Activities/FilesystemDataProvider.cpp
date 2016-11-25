
#include <iostream>
#include "System/Activities/FilesystemDataProvider.h"

ShivaGUI::FilesystemDataProvider::FilesystemDataProvider()
{
	_showFiles = _showDirectories = true;
}

void ShivaGUI::FilesystemDataProvider::SetCurrentDirectory(std::string dir)
{
	if( boost::filesystem::exists(dir) )
	{
		if( boost::filesystem::is_directory(dir) )
		{
			_currentDir = dir;
		}
		else
			std::cerr<< "WARNING: ShivaGUI::FilesystemDataProvider::SetCurrentDirectory, Path " <<dir<< " is not a directory!"<<std::endl;
	}
	else
		std::cerr<< "WARNING: ShivaGUI::FilesystemDataProvider::SetCurrentDirectory, Directory " <<dir<< " does not exist!"<<std::endl;
}

std::string ShivaGUI::FilesystemDataProvider::GetCurrentDirectory()
{
	return _currentDir.string();
}

void ShivaGUI::FilesystemDataProvider::SetShowFiles(bool value)
{
	_showFiles = value;
}

void ShivaGUI::FilesystemDataProvider::SetShowDirectories(bool value)
{
	_showDirectories = value;
}

bool ShivaGUI::FilesystemDataProvider::QueryAttribute(int dataEntry, std::string attribute)
{
	if( dataEntry < GetNumEntries() && (
		attribute == ""
		) )
	{
		return true;
	}
	return false;
}

std::string ShivaGUI::FilesystemDataProvider::GetAttributeString(int dataEntry, std::string attribute)
{
	int numEntries = -1;
	for( boost::filesystem::directory_iterator it(_currentDir); it != boost::filesystem::directory_iterator(); ++it )
	{
		if( _showFiles && boost::filesystem::is_regular_file( (*it) ) )
		{
			++numEntries;
		}
		else if( _showDirectories && boost::filesystem::is_directory( (*it) ) )
		{
			std::cout<<"FilesystemDataProvider dir found at entry: "<<dataEntry<<std::endl;
			++numEntries;
		}

		if( numEntries == dataEntry )
		{
			if( attribute == "FULL_FORMATTED" )
			{
				if( boost::filesystem::is_directory( (*it) ) )
				{
					std::cout<<"FilesystemDataProvider providing attribute string for entry: "<<dataEntry<<std::endl;
					return std::string( (*it).path().string() + " <dir>" );
				}
				return (*it).path().string();
			}
			else if( attribute == "FILENAME" )
			{
				return (*it).path().filename().string();
			}
			else if( attribute == "FILENAME_NO_EXTENSION" )
			{
				return (*it).path().stem().string();
			}
		}
	}
	return "";
}

int ShivaGUI::FilesystemDataProvider::GetNumEntries()
{
	int numEntries = 0;
	for( boost::filesystem::directory_iterator it(_currentDir); it != boost::filesystem::directory_iterator(); ++it )
	{
		if( _showFiles && boost::filesystem::is_regular_file( (*it) ) )
		{
			++numEntries;
		}
		else if( _showDirectories && boost::filesystem::is_directory( (*it) ) )
		{
			++numEntries;
		}
	}

	std::cout<<"FilesystemDataProvider reports : "<<numEntries<<" entries"<<std::endl;
	return numEntries;
}
