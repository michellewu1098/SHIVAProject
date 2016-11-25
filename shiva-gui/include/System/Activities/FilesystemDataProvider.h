
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_SYSTEM_ACTIVITIES_FILESYSTEMDATAPROVIDER__
#define __SHIVA_SYSTEM_ACTIVITIES_FILESYSTEMDATAPROVIDER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <map>
#include <string>
#include <boost/filesystem.hpp>

//////////////////////////////////////////////////////////////////////////

#include "GUI/Views/AdapterViews/DataProvider.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	/// This provides information about the filesystem in a 'browser'-like way
	class FilesystemDataProvider : public ShivaGUI::DataProvider
	{
	public:
		FilesystemDataProvider();

		// Browsing Commands
			void SetCurrentDirectory(std::string dir);
			std::string GetCurrentDirectory();

			/// For showing or hiding files
			void SetShowFiles(bool);

			/// For showing or hiding directories
			void SetShowDirectories(bool);


		// DataProvider interface:

			virtual bool QueryAttribute(int dataEntry, std::string attribute);

			/// Retrieves an attribute from an entry
			virtual std::string GetAttributeString(int dataEntry, std::string attribute);

			/// Retrieves the number of entries in this Provider
			virtual int GetNumEntries();

	protected:

		boost::filesystem::path _currentDir;

		bool _showFiles, _showDirectories;

	};

}

//////////////////////////////////////////////////////////////////////////
#endif
