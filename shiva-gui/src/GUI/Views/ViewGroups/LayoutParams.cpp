#include "GUI/Views/ViewGroups/LayoutParams.h"

#include "ResourceManager.h"
#include "Utility/tinyxml.h"

ShivaGUI::LayoutParams::LayoutParams()
{
	_usesPercent = _usesConst = false;
	_heightPercent = _widthPercent = 0.0f;
	_heightConst = _widthConst = FILL_PARENT;
	_paddingLeft = _paddingRight = _paddingTop = _paddingBottom = 0;
	_wrapWidth = _wrapHeight = 0;
}

ShivaGUI::LayoutParams::LayoutParams(float width, float height)
{
	_usesPercent = true;
	_widthPercent = width;
	_heightPercent = height;
	_usesConst = false;
	_heightConst = _widthConst = FILL_PARENT;
	_paddingLeft = _paddingRight = _paddingTop = _paddingBottom = 0;
}

ShivaGUI::LayoutParams::LayoutParams(FillType width, FillType height)
{
	_usesPercent = false;
	_heightPercent = _widthPercent = 0.0f;
	_usesConst = true;
	_heightConst = height;
	_widthConst = width;
	_paddingLeft = _paddingRight = _paddingTop = _paddingBottom = 0;
}

ShivaGUI::LayoutParams::~LayoutParams()
{

}

void ShivaGUI::LayoutParams::Deflate(TiXmlElement *xmlNode, ResourceManager *resources)
{
	if( _heightConst == FILL_PARENT )
		xmlNode->SetAttribute("layout_height","FILL_PARENT");
	else if( _heightConst == WRAP_CONTENT )
		xmlNode->SetAttribute("layout_height","WRAP_CONTENT");
	else if( _heightConst == PERCENTAGE_PARENT )
	{
		xmlNode->SetDoubleAttribute("layout_height",_heightPercent);
		//xmlNode->SetDoubleAttribute("layout_height_percent",_heightPercent);
	}

	if( _widthConst == FILL_PARENT )
		xmlNode->SetAttribute("layout_width","FILL_PARENT");
	else if( _widthConst == WRAP_CONTENT )
		xmlNode->SetAttribute("layout_width","WRAP_CONTENT");
	else if( _widthConst == PERCENTAGE_PARENT )
	{
		xmlNode->SetDoubleAttribute("layout_width",_widthPercent);
		//xmlNode->SetDoubleAttribute("layout_Width_percent",_widthPercent);
	}

	if( (_paddingLeft == _paddingRight) && (_paddingLeft == _paddingTop) && (_paddingLeft == _paddingBottom) && (_paddingLeft > 0) )
	{
		xmlNode->SetAttribute("padding",_paddingLeft);
	}
	else
	{
		if( _paddingLeft > 0 )
			xmlNode->SetAttribute("padding_left",_paddingLeft);
		if( _paddingRight > 0 )
			xmlNode->SetAttribute("padding_right",_paddingRight);
		if( _paddingTop > 0 )
			xmlNode->SetAttribute("padding_top",_paddingTop);
		if( _paddingBottom > 0 )
			xmlNode->SetAttribute("padding_bottom",_paddingBottom);
	}

}
