
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_MODELMANAGER__
#define __SHIVA_MODELMANAGER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <map>
#include <string>

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////


class ShivaModelManager
{
public:

	static ShivaModelManager* Init( std::string indexFile );

	static ShivaModelManager* GetInstance() {return _instance;}

	/// This allows access to the raw data stored for the models
	/// These are simply 'attribute, value' pairs of strings
	std::string GetAttribute(std::string modelID, std::string attribute);

	/// Returns the thumbnail image as a BitmapDrawable for a given model
	//BitmapDrawable* GetThumbnail(std::string modelID);

	/// Returns the BDF filename for a given model
	//std::string GetBDFFilename(std::string modelID);

protected:

	static ShivaModelManager *_instance;

	ShivaModelManager(std::string indexFile);

	/// Each entry is accessed by it's index, then there are specific entries according to the type of data that is stored
	std::map<std::string, std::map<std::string,std::string>* > _modelIndices;
};

//////////////////////////////////////////////////////////////////////////
#endif
