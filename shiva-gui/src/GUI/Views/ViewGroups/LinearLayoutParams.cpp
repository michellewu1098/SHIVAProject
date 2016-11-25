#include "GUI/Views/ViewGroups/LinearLayoutParams.h"

ShivaGUI::LinearLayoutParams::LinearLayoutParams() : LayoutParams()
{
	_weight = 1.0f;
}

ShivaGUI::LinearLayoutParams::LinearLayoutParams(float weight) : LayoutParams()
{
	_weight = weight;
}
