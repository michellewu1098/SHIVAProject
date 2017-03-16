#ifndef __SHIVA_MODELMANAGER__
#define __SHIVA_MODELMANAGER__

#include <map>
#include <string>
#include <iostream>
#include <sstream>

#include "Utility/tinyxml.h"

class ShivaModelManager
{
public:

	//----------------------------------------------------------------------------------
	/// \brief Initialise method
	/// \param [in] _indexFile
	//----------------------------------------------------------------------------------
	static ShivaModelManager* Init( std::string _indexFile );
	//----------------------------------------------------------------------------------
	/// \brief Returns Model Manager instance
	//----------------------------------------------------------------------------------
	static ShivaModelManager* GetInstance() { return m_instance; }
	//----------------------------------------------------------------------------------
	/// \brief This allows access to the raw data stored for the models
	/// These are simply 'attribute, value' pairs of strings
	/// \param [in] _modelID
	/// \param [in] _attribute
	//----------------------------------------------------------------------------------
	std::string GetAttribute( std::string _modelID, std::string _attribute );
	//----------------------------------------------------------------------------------
	/// \brief Returns the thumbnail image as a BitmapDrawable for a given model
	//----------------------------------------------------------------------------------
	//BitmapDrawable* GetThumbnail( std::string _modelID );
	//----------------------------------------------------------------------------------
	/// \brief Returns the BDF filename for a given model
	//----------------------------------------------------------------------------------
	//std::string GetBDFFilename( std::string _modelID );
	//----------------------------------------------------------------------------------

protected:

	//----------------------------------------------------------------------------------
	/// \brief Model manager instance
	//----------------------------------------------------------------------------------
	static ShivaModelManager *m_instance;
	//----------------------------------------------------------------------------------
	/// \brief Ctor
	//----------------------------------------------------------------------------------
	ShivaModelManager( std::string _indexFile );
	//----------------------------------------------------------------------------------
	/// \brief Each entry is accessed by it's index, then there are specific entries according to the type of data that is stored
	//----------------------------------------------------------------------------------
	std::map< std::string, std::map< std::string, std::string >* > m_modelIndices;
	//----------------------------------------------------------------------------------

};

#endif
