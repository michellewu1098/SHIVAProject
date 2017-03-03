///-----------------------------------------------------------------------------------------------
/// \file FilesystemDataProvider.h
/// \brief  This provides information about the filesystem in a 'browser'-like way
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_SYSTEM_ACTIVITIES_FILESYSTEMDATAPROVIDER__
#define __SHIVA_SYSTEM_ACTIVITIES_FILESYSTEMDATAPROVIDER__


#include <iostream>
#include <map>
#include <boost/filesystem.hpp>

#include "GUI/Views/AdapterViews/DataProvider.h"

namespace ShivaGUI
{
	class FilesystemDataProvider : public ShivaGUI::DataProvider
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		FilesystemDataProvider();
		//----------------------------------------------------------------------------------
		
		// BROWSING COMMANDS
		
		//----------------------------------------------------------------------------------
		/// \brief Set current directory
		/// \param [in] _dir
		//----------------------------------------------------------------------------------
		void SetCurrentDirectoryNew( std::string _dir );
		//----------------------------------------------------------------------------------
		/// \brief Returns current directory
		//----------------------------------------------------------------------------------
		std::string GetCurrentDirectory();
		//----------------------------------------------------------------------------------
		/// \brief For showing or hiding files
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetShowFiles( bool );
		//----------------------------------------------------------------------------------
		/// \brief For showing or hiding directories
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetShowDirectories( bool );
		//----------------------------------------------------------------------------------

		// DATAPROVIDER INTERFACE

		//----------------------------------------------------------------------------------
		/// \brief Query attribute
		/// \param [in] _dataEntry
		/// \param [in] _attribute
		//----------------------------------------------------------------------------------
		virtual bool QueryAttribute( int _dataEntry, std::string _attribute );
		//----------------------------------------------------------------------------------
		/// \brief Retrieves an attribute from an entry
		/// \param [in] _dataEntry
		/// \param [in] _attribute
		//----------------------------------------------------------------------------------
		virtual std::string GetAttributeString( int _dataEntry, std::string _attribute );
		//----------------------------------------------------------------------------------
		/// \brief Retrieves the number of entries in this Provider
		//----------------------------------------------------------------------------------
		virtual int GetNumEntries();
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Current directory
		//----------------------------------------------------------------------------------
		boost::filesystem::path m_currentDir;
		//----------------------------------------------------------------------------------
		/// \brief Whether to show files
		//----------------------------------------------------------------------------------
		bool m_showFiles;
		//----------------------------------------------------------------------------------
		/// \brief Whether to show directories
		//----------------------------------------------------------------------------------
		bool m_showDirectories;
		//----------------------------------------------------------------------------------

	};
}

#endif
