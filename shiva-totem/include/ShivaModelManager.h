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
	/// \param [in] indexFile 
	//----------------------------------------------------------------------------------
	static ShivaModelManager* Init( std::string indexFile );
	//----------------------------------------------------------------------------------
	/// \brief Get an instance of the ModelManager
	/// \return _instance
	//----------------------------------------------------------------------------------
	static ShivaModelManager* GetInstance() { return _instance; }
	//----------------------------------------------------------------------------------
	/// \brief Query an attribute from an entry
	/// \param [in] dataEntry
	/// \param [in] attribute
	//----------------------------------------------------------------------------------
	virtual bool QueryAttribute( int dataEntry, std::string attribute );
	//----------------------------------------------------------------------------------
	/// \brief Retrieves an attribute from an entry
	/// \param [in] dataEntry
	/// \param [in] attribute
	//----------------------------------------------------------------------------------
	virtual std::string GetAttributeString( int dataEntry, std::string attribute );
	//----------------------------------------------------------------------------------
	/// \brief Retrieves the number of entries in this Provider
	//----------------------------------------------------------------------------------
	virtual int GetNumEntries();
	//----------------------------------------------------------------------------------

protected:

	//----------------------------------------------------------------------------------
	static ShivaModelManager *_instance;
	//----------------------------------------------------------------------------------
	/// \brief Ctor
	/// \param [in] indexFile
	//----------------------------------------------------------------------------------
	ShivaModelManager( std::string indexFile );
	//----------------------------------------------------------------------------------
	/// \brief Each entry is accessed by it's index, then there are specific entries according to the type of data that is stored
	//----------------------------------------------------------------------------------
	std::map< std::string, std::map< std::string, std::string >* > _modelIndices;
	//----------------------------------------------------------------------------------
};

#endif
