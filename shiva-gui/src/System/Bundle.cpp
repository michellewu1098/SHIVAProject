#include "System/Bundle.h"

//----------------------------------------------------------------------------------

ShivaGUI::Bundle::Bundle()
{ }

//----------------------------------------------------------------------------------

ShivaGUI::Bundle::~Bundle()
{ }

//----------------------------------------------------------------------------------

bool ShivaGUI::Bundle::ContainsKey( std::string _key )
{
	if( m_integers.find( _key ) != m_integers.end() )
		return true;
	if( m_floats.find( _key ) != m_floats.end() )
		return true;
	if( m_strings.find( _key ) != m_strings.end() )
		return true;

	return false;
}

//----------------------------------------------------------------------------------

void ShivaGUI::Bundle::PutInteger( std::string _key, int _value )
{
	m_integers[ _key ] = _value;
}

//----------------------------------------------------------------------------------

int ShivaGUI::Bundle::GetInteger( std::string _key )
{
	if( m_integers.find( _key ) == m_integers.end() )
		return false;
	return m_integers[ _key ];
}

//----------------------------------------------------------------------------------

void ShivaGUI::Bundle::PutFloat( std::string _key, float _value )
{
	m_floats[ _key ] = _value;
}

//----------------------------------------------------------------------------------

float ShivaGUI::Bundle::GetFloat( std::string _key )
{
	if( m_floats.find( _key ) == m_floats.end() )
		return 0.0f;
	return m_floats[ _key ];
}

//----------------------------------------------------------------------------------

void ShivaGUI::Bundle::PutString( std::string _key, std::string _value )
{
	m_strings[ _key ] = _value;
}

//----------------------------------------------------------------------------------

std::string ShivaGUI::Bundle::GetString( std::string _key )
{
	if( m_strings.find( _key ) == m_strings.end() )
		return "";
	return m_strings[ _key ];
}

//----------------------------------------------------------------------------------