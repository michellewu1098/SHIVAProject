#include "System/SharedPreferences.h"

//----------------------------------------------------------------------------------

ShivaGUI::SharedPreferences::SharedPreferences( std::string _file )
{
	// Test to see if the file already exists
	std::ifstream testStream;
	testStream.open( _file.c_str() );
	testStream.close();

	if( testStream.fail() )
	{
		std::cout << "Creating SharedPreferences file: " << _file << std::endl;
		// File does not exist
		m_prefsDoc = TiXmlDocument( _file.c_str() );
		TiXmlElement *element = new TiXmlElement( "SharedPreferences" );
		m_prefsDoc.LinkEndChild( element );
	}
	else
	{
		m_prefsDoc.LoadFile( _file.c_str() );
	}
}

//----------------------------------------------------------------------------------

ShivaGUI::SharedPreferences::~SharedPreferences()
{ }

//----------------------------------------------------------------------------------

void ShivaGUI::SharedPreferences::Save()
{
	if( !m_prefsDoc.SaveFile() )
		std::cerr << "WARNING: failed to save shared preferences to file" << std::endl;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::SharedPreferences::Contains( std::string _key )
{
	return GetPrefNode( _key ) != NULL;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::SharedPreferences::GetBoolean( std::string _key, bool _defaultValue )
{
	return GetValue( _key, "bool", _defaultValue );
}

//----------------------------------------------------------------------------------

float ShivaGUI::SharedPreferences::GetFloat( std::string _key, float _defaultValue )
{
	return GetValue( _key, "float", _defaultValue );
}

//----------------------------------------------------------------------------------

int ShivaGUI::SharedPreferences::GetInt( std::string _key, int _defaultValue )
{
	return GetValue( _key, "int", _defaultValue );
}

//----------------------------------------------------------------------------------

std::string ShivaGUI::SharedPreferences::GetString( std::string _key, std::string _defaultValue )
{
	return GetValue( _key, "string", _defaultValue );
}

//----------------------------------------------------------------------------------

template < class T > T ShivaGUI::SharedPreferences::GetValue(std::string _key, std::string _typeName, T _defaultValue )
{
	TiXmlNode *prefNode = GetPrefNode( _key );
	if( prefNode == NULL )
		return _defaultValue;

	TiXmlElement *prefElement = prefNode->ToElement();
	if( prefElement == NULL )
	{
		std::cerr << "WARNING: could not get preference for key: " << _key << ", retrieved xml node could not convert to element" << std::endl;
		return _defaultValue;
	}

	if( _typeName != prefElement->Attribute( "type" ) )
	{
		std::cerr << "WARNING: could not get preference for key: " << _key << ", retrieved element is of wrong type" << std::endl;
		return _defaultValue;
	}

	std::stringstream stream( prefElement->GetText() );
	if( !stream )
		return _defaultValue;
	T result = _defaultValue;
	stream >> result;
	return result;
}

//----------------------------------------------------------------------------------


void ShivaGUI::SharedPreferences::SetBoolean( std::string _key, bool _value )
{
	SetValue( _key, "bool", _value);
}

//----------------------------------------------------------------------------------

void ShivaGUI::SharedPreferences::SetFloat( std::string _key, float _value )
{
	SetValue( _key, "float", _value );
}

//----------------------------------------------------------------------------------

void ShivaGUI::SharedPreferences::SetInt( std::string _key, int _value )
{
	SetValue( _key, "int", _value );
}

//----------------------------------------------------------------------------------

void ShivaGUI::SharedPreferences::SetString( std::string _key, std::string _value )
{
	SetValue( _key, "string", _value);
}

//----------------------------------------------------------------------------------

template < class T > void ShivaGUI::SharedPreferences::SetValue( std::string _key, std::string _typeName, T _value )
{
	TiXmlNode *prefNode = GetPrefNode( _key );
	TiXmlElement *prefElement = NULL;
	if( prefNode == NULL )
	{
		TiXmlElement *rootElement = m_prefsDoc.RootElement();
		prefElement = new TiXmlElement( _key );
		rootElement->LinkEndChild( prefElement );

		// Storing data type as attribute
		prefElement->SetAttribute( "type", _typeName );
	}
	else
	{
		prefElement = prefNode->ToElement();
		if( prefElement == NULL )
		{
			std::cerr << "WARNING: could not set preference for key: " << _key << ", retrieved xml node could not convert to element" << std::endl;
			return;
		}

		if( _typeName != prefElement->Attribute( "type" ) )
		{
			std::cerr << "WARNING: could not set preference for key: " << _key << ", retrieved element is of wrong type" << std::endl;
			return;
		}
	}

	std::stringstream stream;
	stream << _value;
	TiXmlText *text = new TiXmlText( stream.str() );
	// Need to remove all previous children, otherwise our text will just be added to it
	prefElement->Clear();
	prefElement->LinkEndChild( text );
}

//----------------------------------------------------------------------------------

TiXmlNode* ShivaGUI::SharedPreferences::GetPrefNode( std::string _key )
{
	TiXmlElement *rootElement = m_prefsDoc.RootElement();

	if( rootElement != NULL )
	{
		if( rootElement->ValueStr() == "SharedPreferences" )
		{
			for( TiXmlNode *childNode = rootElement->FirstChild(); childNode != NULL; childNode = childNode->NextSibling() )
			{
				if( childNode->ValueStr() == _key )
					return childNode;
			}
		}
		else
			std::cerr << "WARNING: SharedPreferences root element is not of value 'SharedPreferences'" << std::endl;
	}
	else
		std::cerr << "WARNING: SharedPreferences Contains check cannot retrieve root element" << std::endl;

	return NULL;
}

//----------------------------------------------------------------------------------