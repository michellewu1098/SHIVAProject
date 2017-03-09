///-----------------------------------------------------------------------------------------------
/// \file ShivaModelManager.h
/// \brief Responsible for managing the models (primitives)
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_MODELMANAGER__
#define __SHIVA_MODELMANAGER__

#include <map>
#include <string>
#include <iostream>
#include <sstream>

#include "GUI/Views/AdapterViews/DataProvider.h"
#include "Utility/tinyxml.h"


class ShivaModelManager : public ShivaGUI::DataProvider
{
public:

	//----------------------------------------------------------------------------------
	/// \brief Initiliase ModelManager
	/// \param [in] _indexFile 
	//----------------------------------------------------------------------------------
	static ShivaModelManager* Init( std::string _indexFile );
	//----------------------------------------------------------------------------------
	/// \brief Get an instance of the ModelManager
	/// \return m_instance
	//----------------------------------------------------------------------------------
	static ShivaModelManager* GetInstance() { return m_instance; }
	//----------------------------------------------------------------------------------
	/// \brief Query an attribute from an entry
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
	static ShivaModelManager *m_instance;
	//----------------------------------------------------------------------------------
	/// \brief Ctor
	/// \param [in] _indexFile
	//----------------------------------------------------------------------------------
	ShivaModelManager( std::string _indexFile );
	//----------------------------------------------------------------------------------
	/// \brief Each entry is accessed by it's index, then there are specific entries according to the type of data that is stored
	//----------------------------------------------------------------------------------
	std::map< std::string, std::map< std::string, std::string >* > m_modelIndices;
	//----------------------------------------------------------------------------------
};

#endif
