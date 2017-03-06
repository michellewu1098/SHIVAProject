#include "Input/Scanners/Scanner.h"

#define __DEBUG_SCANNER__

//----------------------------------------------------------------------------------

void ShivaGUI::Scanner::DebugOutput( std::string _value )
{
#ifdef __DEBUG_SCANNER__
	std::cerr << _value << std::endl;
#endif
}

//----------------------------------------------------------------------------------
