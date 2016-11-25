#include "GUI/Drawables/Drawable.h"

ShivaGUI::Drawable::Drawable()
{
	_boundsLeft = _boundsRight = _boundsTop = _boundsBottom = 0.0f;
}

ShivaGUI::Drawable::~Drawable()
{

}


void ShivaGUI::Drawable::SetBounds(float left, float top, float right, float bottom, unsigned int gravity)
{
	_boundsLeft = left;
	_boundsRight = right;
	_boundsTop = top;
	_boundsBottom = bottom;
	OnSetBounds(left,top,right,bottom,gravity);
}

void ShivaGUI::Drawable::GetContentBounds(float &left, float &top, float &right, float &bottom)
{
	left = _boundsLeft;
	top = _boundsTop;
	right = _boundsRight;
	bottom = _boundsBottom;
}
