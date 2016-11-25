
#include "GUI/Views/ViewGroups/CustomLayoutParams.h"

#include "ResourceManager.h"
#include "Utility/tinyxml.h"

ShivaGUI::CustomLayoutParams::CustomLayoutParams()
{
	_centreX = _centreY = 0.5f;
}


void ShivaGUI::CustomLayoutParams::Deflate(TiXmlElement *xmlNode, ResourceManager *resources)
{
	LayoutParams::Deflate(xmlNode,resources);

	xmlNode->SetDoubleAttribute("layout_centreX",_centreX);
	xmlNode->SetDoubleAttribute("layout_centreY",_centreY);
}

float ShivaGUI::CustomLayoutParams::norm(float value)
{
	if( value < 0.0f )
		return 0.0f;
	if( value > 1.0f )
		return 1.0f;
	return value;
}
