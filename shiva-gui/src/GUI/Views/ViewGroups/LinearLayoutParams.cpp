#include "GUI/Views/ViewGroups/LinearLayoutParams.h"

//----------------------------------------------------------------------------------

ShivaGUI::LinearLayoutParams::LinearLayoutParams() : LayoutParams()
{
	m_weight = 1.0f;
}

//----------------------------------------------------------------------------------

ShivaGUI::LinearLayoutParams::LinearLayoutParams( float _weight) : LayoutParams()
{
	m_weight = _weight;
}

//----------------------------------------------------------------------------------