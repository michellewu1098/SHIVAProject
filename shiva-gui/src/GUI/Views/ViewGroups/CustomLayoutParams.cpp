#include "GUI/Views/ViewGroups/CustomLayoutParams.h"
#include "ResourceManager.h"

//----------------------------------------------------------------------------------

ShivaGUI::CustomLayoutParams::CustomLayoutParams()
{
	m_centreX = m_centreY = 0.5f;
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayoutParams::Deflate( TiXmlElement *_xmlNode, ResourceManager *_resources )
{
	LayoutParams::Deflate( _xmlNode, _resources );

	_xmlNode->SetDoubleAttribute( "layout_centreX", m_centreX );
	_xmlNode->SetDoubleAttribute( "layout_centreY", m_centreY );
}

//----------------------------------------------------------------------------------

float ShivaGUI::CustomLayoutParams::norm( float _value )
{
	if( _value < 0.0f )
		return 0.0f;
	if( _value > 1.0f )
		return 1.0f;
	return _value;
}

//----------------------------------------------------------------------------------
