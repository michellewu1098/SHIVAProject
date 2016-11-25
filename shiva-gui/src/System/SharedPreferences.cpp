#include "System/SharedPreferences.h"

//////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>
#include <sstream>

ShivaGUI::SharedPreferences::SharedPreferences(std::string file)
{
	// Test to see if the file already exists
	std::ifstream testStream;
	testStream.open(file.c_str());
	testStream.close();
	if( testStream.fail() )
	{
		std::cout<<"Creating SharedPreferences file: "<<file<<std::endl;
		// File does not exist
		_prefsDoc = TiXmlDocument( file.c_str() );
		TiXmlElement *element = new TiXmlElement( "SharedPreferences" );
		_prefsDoc.LinkEndChild( element );
	}
	else
	{
		_prefsDoc.LoadFile( file.c_str() );
	}

}

ShivaGUI::SharedPreferences::~SharedPreferences()
{
}

void ShivaGUI::SharedPreferences::Save()
{
	if( !_prefsDoc.SaveFile() )
		std::cerr<<"WARNING: failed to save shared preferences to file"<<std::endl;
}

bool ShivaGUI::SharedPreferences::Contains(std::string key)
{
	return GetPrefNode(key) != NULL;
}

bool ShivaGUI::SharedPreferences::GetBoolean(std::string key, bool defaultValue)
{
	return GetValue(key,"bool",defaultValue);
}

float ShivaGUI::SharedPreferences::GetFloat(std::string key, float defaultValue)
{
	return GetValue(key,"float",defaultValue);
}

int ShivaGUI::SharedPreferences::GetInt(std::string key, int defaultValue)
{
	return GetValue(key,"int",defaultValue);
}

std::string ShivaGUI::SharedPreferences::GetString(std::string key, std::string defaultValue)
{
	return GetValue(key,"string",defaultValue);
}


template <class T> T ShivaGUI::SharedPreferences::GetValue(std::string key, std::string typeName, T defaultValue)
{
	TiXmlNode *prefNode = GetPrefNode(key);
	if( prefNode == NULL )
		return defaultValue;

	TiXmlElement *prefElement = prefNode->ToElement();
	if( prefElement == NULL )
	{
		std::cerr<<"WARNING: could not get preference for key: "<<key<<", retrieved xml node could not convert to element"<<std::endl;
		return defaultValue;
	}

	if( typeName != prefElement->Attribute("type") )
	{
		std::cerr<<"WARNING: could not get preference for key: "<<key<<", retrieved element is of wrong type"<<std::endl;
		return defaultValue;
	}

	std::stringstream stream( prefElement->GetText() );
	if( !stream )
		return defaultValue;
	T result = defaultValue;
	stream >> result;
	return result;
}


void ShivaGUI::SharedPreferences::SetBoolean(std::string key, bool value)
{
	SetValue(key,"bool",value);
}

void ShivaGUI::SharedPreferences::SetFloat(std::string key, float value)
{
	SetValue(key,"float",value);
}

void ShivaGUI::SharedPreferences::SetInt(std::string key, int value)
{
	SetValue(key,"int",value);
}

void ShivaGUI::SharedPreferences::SetString(std::string key, std::string value)
{
	SetValue(key,"string",value);
}

template <class T> void ShivaGUI::SharedPreferences::SetValue(std::string key, std::string typeName, T value)
{
	TiXmlNode *prefNode = GetPrefNode(key);
	TiXmlElement *prefElement = NULL;
	if( prefNode == NULL )
	{
		TiXmlElement *rootElement = _prefsDoc.RootElement();
		prefElement = new TiXmlElement(key);
		rootElement->LinkEndChild(prefElement);

		// Storing data type as attribute
		prefElement->SetAttribute("type",typeName);
	}
	else
	{
		prefElement = prefNode->ToElement();
		if( prefElement == NULL )
		{
			std::cerr<<"WARNING: could not set preference for key: "<<key<<", retrieved xml node could not convert to element"<<std::endl;
			return;
		}

		if( typeName != prefElement->Attribute("type") )
		{
			std::cerr<<"WARNING: could not set preference for key: "<<key<<", retrieved element is of wrong type"<<std::endl;
			return;
		}
	}

	std::stringstream stream;
	stream << value;
	TiXmlText *text = new TiXmlText( stream.str() );
	// Need to remove all previous children, otherwise our text will just be added to it
	prefElement->Clear();
	prefElement->LinkEndChild(text);
}


TiXmlNode* ShivaGUI::SharedPreferences::GetPrefNode(std::string key)
{
	TiXmlElement *rootElement = _prefsDoc.RootElement();

	if( rootElement != NULL )
	{
		if( rootElement->ValueStr() == "SharedPreferences" )
		{
			for( TiXmlNode *childNode = rootElement->FirstChild(); childNode != NULL; childNode = childNode->NextSibling() )
			{
				if( childNode->ValueStr() == key )
					return childNode;
			}
		}
		else
			std::cerr<<"WARNING: SharedPreferences root element is not of value 'SharedPreferences'"<<std::endl;
	}
	else
		std::cerr<<"WARNING: SharedPreferences Contains check cannot retrieve root element"<<std::endl;

	return NULL;
}
