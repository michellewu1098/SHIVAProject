///-----------------------------------------------------------------------------------------------
/// \file DataProvider.h
/// \brief This is a 'model' for a Model-View-Adapter pattern that is designed to work with a DataAdapter and a View of some sort
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_GUI_VIEWS_ADAPTERVIEWS_DATAPROVIDER__
#define __SHIVA_GUI_GUI_VIEWS_ADAPTERVIEWS_DATAPROVIDER__

#include <string>
#include <boost/any.hpp>

namespace ShivaGUI
{
	class DataProvider
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Tests whether an attribute exists for an entry
		/// \param [in] _dataEntry
		/// \param [in] _attribute
		//----------------------------------------------------------------------------------
		virtual bool QueryAttribute( int _dataEntry, std::string _attribute ) = 0;
		//----------------------------------------------------------------------------------
		/// \brief Retrieves an attribute from an entry
		/// \param [in] _dataEntry
		/// \param [in] _attribute
		//----------------------------------------------------------------------------------
		virtual std::string GetAttributeString( int _dataEntry, std::string _attribute ) = 0;
		//----------------------------------------------------------------------------------
		/// \brief Retrieves the number of entries in this Provider
		//----------------------------------------------------------------------------------
		virtual int GetNumEntries() = 0;
		//----------------------------------------------------------------------------------

	};
}

#endif
