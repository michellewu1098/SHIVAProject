
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_MODELMANAGER__
#define __SHIVA_MODELMANAGER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <map>
#include <string>

//////////////////////////////////////////////////////////////////////////

#include "GUI/Views/AdapterViews/DataProvider.h"

//////////////////////////////////////////////////////////////////////////


class ShivaModelManager : public ShivaGUI::DataProvider
{
public:

	static ShivaModelManager* Init( std::string indexFile );

	static ShivaModelManager* GetInstance() {return _instance;}


	virtual bool QueryAttribute(int dataEntry, std::string attribute);

	/// Retrieves an attribute from an entry
	virtual std::string GetAttributeString(int dataEntry, std::string attribute);

	/// Retrieves the number of entries in this Provider
	virtual int GetNumEntries();

protected:

	static ShivaModelManager *_instance;

	ShivaModelManager(std::string indexFile);

	/// Each entry is accessed by it's index, then there are specific entries according to the type of data that is stored
	std::map<std::string, std::map<std::string,std::string>* > _modelIndices;
};

//////////////////////////////////////////////////////////////////////////
#endif
