#include "System/Bundle.h"

//////////////////////////////////////////////////////////////////////////


ShivaGUI::Bundle::Bundle()
{

}

ShivaGUI::Bundle::~Bundle()
{

}

bool ShivaGUI::Bundle::ContainsKey(std::string key)
{
	if( _integers.find(key) != _integers.end() )
		return true;
	if( _floats.find(key) != _floats.end() )
		return true;
	if( _strings.find(key) != _strings.end() )
		return true;

	return false;
}

void ShivaGUI::Bundle::PutInteger(std::string key, int value)
{
	_integers[key] = value;
}

int ShivaGUI::Bundle::GetInteger(std::string key)
{
	if( _integers.find(key) == _integers.end() )
		return false;
	return _integers[key];
}

void ShivaGUI::Bundle::PutFloat(std::string key, float value)
{
	_floats[key] = value;
}

float ShivaGUI::Bundle::GetFloat(std::string key)
{
	if( _floats.find(key) == _floats.end() )
		return 0.0f;
	return _floats[key];
}

void ShivaGUI::Bundle::PutString(std::string key, std::string value)
{
	_strings[key] = value;
}

std::string ShivaGUI::Bundle::GetString(std::string key)
{
	if( _strings.find(key) == _strings.end() )
		return "";
	return _strings[key];
}
