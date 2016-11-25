
#include "Input/Scanners/Scanner.h"

#include <boost/algorithm/string.hpp>
#include <SDL.h>
#include <iostream>

#define __DEBUG_SCANNER__

//////////////////////////////////////////////////////////////////////////

void ShivaGUI::Scanner::DebugOutput(std::string value)
{
#ifdef __DEBUG_SCANNER__
	std::cerr<<value<<std::endl;
#endif
}
