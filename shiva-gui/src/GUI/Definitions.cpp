
#include "GUI/Definitions.h"

ShivaGUI::Definitions::FocusDirection ShivaGUI::Definitions::GetOppositeFocusDirection(FocusDirection dir)
{
	if( dir == Left )
		return Right;
	if( dir == Right )
		return Left;
	if( dir == Up )
		return Down;
	if( dir == Down )
		return Up;
	return INVALID;
}
