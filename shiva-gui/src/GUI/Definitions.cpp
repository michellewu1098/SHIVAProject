#include "GUI/Definitions.h"

//----------------------------------------------------------------------------------

ShivaGUI::Definitions::FocusDirection ShivaGUI::Definitions::GetOppositeFocusDirection( FocusDirection _dir )
{
	if( _dir == Left )
		return Right;
	if( _dir == Right )
		return Left;
	if( _dir == Up )
		return Down;
	if( _dir == Down )
		return Up;
	return INVALID;
}

//----------------------------------------------------------------------------------