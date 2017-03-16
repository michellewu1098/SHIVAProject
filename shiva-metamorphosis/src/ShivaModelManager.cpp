#include "ShivaModelManager.h"

//----------------------------------------------------------------------------------
/// Initialise statics
ShivaModelManager *ShivaModelManager::m_instance = NULL;

//----------------------------------------------------------------------------------

ShivaModelManager* ShivaModelManager::Init( std::string _indexFile )
{
	m_instance = new ShivaModelManager( _indexFile );
	return m_instance;
}

//----------------------------------------------------------------------------------

ShivaModelManager::ShivaModelManager( std::string _indexFile )
{
	// need to populate the model indices data
	TiXmlDocument doc( _indexFile );
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
						std::map< std::string, std::string > *currentData = new std::map< std::string, std::string >;

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
									( *currentData )[ attributeName ] = attributeValue;
							}
						}
						else
							std::cerr << "WARNING: ShivaModelManager parsing xml file: " << _indexFile << " strange error: model node is not an element, cannot retrieve attributes" << std::endl;


						if( !ID.empty() )
						{
							m_modelIndices[ ID ] = currentData;
						}
						else
						{
							std::cerr << "WARNING: ShivaModelManager parsing xml file: " << _indexFile << " cannot store Model entry with no ID" << std::endl;
							delete currentData;
						}
					}
				}
			}
			else
				std::cerr << "WARNING: ShivaModelManager root element is not of value 'ModelIndexFile' in xml file: " << _indexFile << std::endl;
		}
		else
			std::cerr << "WARNING: ShivaModelManager cannot retrieve root element from xml file: " << _indexFile << std::endl;
	}
}

//----------------------------------------------------------------------------------

std::string ShivaModelManager::GetAttribute( std::string _modelID, std::string _attribute )
{
	if( m_modelIndices.find( _modelID ) != m_modelIndices.end() )
	{
		std::map< std::string, std::string > *modelData = ( m_modelIndices[ _modelID ] );

		if( modelData->find( _attribute ) != modelData->end() )
		{
			return ( *modelData )[ _attribute ];
		}
	}
	return "";
}

//----------------------------------------------------------------------------------

/*
std::string ShivaModelManager::GetThumbnailFilename( std::string _index )
{
	if( m_modelIndices.find( _index ) != m_modelIndices.end() )
	{
		std::map< std::string, std::string > *modelData = &( m_modelIndices[ _index ] );

		if( modelData->find( "thumbnail_file" ) != modelData->end() )
		{
			return ( *modelData )[ "thumbnail_file" ];
		}
	}
	return "";
}

//----------------------------------------------------------------------------------

std::string ShivaModelManager::GetBDFFilename(std::string index)
{

	return "";
}
*/

//----------------------------------------------------------------------------------