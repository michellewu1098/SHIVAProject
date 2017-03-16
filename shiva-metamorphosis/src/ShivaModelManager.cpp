#include "ShivaModelManager.h"
#include "Utility/tinyxml.h"

 #include <iostream>
 #include <sstream>

/// Initialise statics
ShivaModelManager *ShivaModelManager::_instance = NULL;

ShivaModelManager* ShivaModelManager::Init( std::string indexFile )
{
	_instance = new ShivaModelManager(indexFile);
	return _instance;
}




ShivaModelManager::ShivaModelManager(std::string indexFile)
{
	// need to populate the model indices data
	TiXmlDocument doc( indexFile );
	if( doc.LoadFile() )
	{
		TiXmlElement *rootElement = doc.RootElement();

		if( rootElement != NULL )
		{
			if( rootElement->ValueStr() == "ModelIndexFile" )
			{
				for( TiXmlNode *indexChildNode = rootElement->FirstChild(); indexChildNode != NULL; indexChildNode = indexChildNode->NextSibling() )
				{
					if( indexChildNode->ValueStr() == "Model" )
					{
						std::string ID;
						std::map<std::string,std::string> *currentData = new std::map<std::string,std::string>;

						if( indexChildNode->Type() == TiXmlNode::TINYXML_ELEMENT )
						{
							TiXmlElement *xmlElement = indexChildNode->ToElement();
							for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
							{
								std::string attributeName = currentAttribute->Name();
								std::string attributeValue = currentAttribute->Value();
								if( attributeName == "ID" || attributeName == "id" )
									ID = attributeValue;
								else
									(*currentData)[attributeName] = attributeValue;
							}
						}
						else
							std::cerr<<"WARNING: ShivaModelManager parsing xml file: "<<indexFile<<" strange error: model node is not an element, cannot retrieve attributes"<<std::endl;


						if( !ID.empty() )
						{
							_modelIndices[ID] = currentData;
						}
						else
						{
							std::cerr<<"WARNING: ShivaModelManager parsing xml file: "<<indexFile<<" cannot store Model entry with no ID"<<std::endl;
							delete currentData;
						}


					}
				}
			}
			else
				std::cerr<<"WARNING: ShivaModelManager root element is not of value 'ModelIndexFile' in xml file: "<<indexFile<<std::endl;
		}
		else
			std::cerr<<"WARNING: ShivaModelManager cannot retrieve root element from xml file: "<<indexFile<<std::endl;

	}
}

std::string ShivaModelManager::GetAttribute(std::string modelID, std::string attribute)
{
	if( _modelIndices.find(modelID) != _modelIndices.end() )
	{
		std::map<std::string,std::string> *modelData = (_modelIndices[modelID]);

		if( modelData->find(attribute) != modelData->end() )
		{
			return (*modelData)[attribute];
		}
	}
	return "";
}

/*
std::string ShivaModelManager::GetThumbnailFilename(std::string index)
{
	if( _modelIndices.find(index) != _modelIndices.end() )
	{
		std::map<std::string,std::string> *modelData = &(_modelIndices[index]);

		if( modelData->find("thumbnail_file") != modelData->end() )
		{
			return (*modelData)["thumbnail_file"];
		}
	}
	return "";
}

std::string ShivaModelManager::GetBDFFilename(std::string index)
{

	return "";
}
*/
