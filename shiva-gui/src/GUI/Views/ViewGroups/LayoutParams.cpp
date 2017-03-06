#include "GUI/Views/ViewGroups/LayoutParams.h"
#include "ResourceManager.h"

//----------------------------------------------------------------------------------

ShivaGUI::LayoutParams::LayoutParams()
{
	m_usesPercent = m_usesConst = false;
	m_heightPercent = m_widthPercent = 0.0f;
	m_heightConst = m_widthConst = FILL_PARENT;
	m_paddingLeft = m_paddingRight = m_paddingTop = m_paddingBottom = 0;
	m_wrapWidth = m_wrapHeight = 0;
}

//----------------------------------------------------------------------------------

ShivaGUI::LayoutParams::LayoutParams( float _width, float _height )
{
	m_usesPercent = true;
	m_widthPercent = _width;
	m_heightPercent = _height;
	m_usesConst = false;
	m_heightConst = m_widthConst = FILL_PARENT;
	m_paddingLeft = m_paddingRight = m_paddingTop = m_paddingBottom = 0;
}

//----------------------------------------------------------------------------------

ShivaGUI::LayoutParams::LayoutParams( FillType _width, FillType _height )
{
	m_usesPercent = false;
	m_heightPercent = m_widthPercent = 0.0f;
	m_usesConst = true;
	m_heightConst = _height;
	m_widthConst = _width;
	m_paddingLeft = m_paddingRight = m_paddingTop = m_paddingBottom = 0;
}

//----------------------------------------------------------------------------------

ShivaGUI::LayoutParams::~LayoutParams()
{

}

//----------------------------------------------------------------------------------

void ShivaGUI::LayoutParams::Deflate( TiXmlElement *_xmlNode, ResourceManager *_resources )
{
	if( m_heightConst == FILL_PARENT )
		_xmlNode->SetAttribute( "layout_height", "FILL_PARENT" );
	else if( m_heightConst == WRAP_CONTENT )
		_xmlNode->SetAttribute( "layout_height", "WRAP_CONTENT" );
	else if( m_heightConst == PERCENTAGE_PARENT )
	{
		_xmlNode->SetDoubleAttribute( "layout_height", m_heightPercent );
		//xmlNode->SetDoubleAttribute("layout_height_percent",_heightPercent);
	}

	if( m_widthConst == FILL_PARENT )
		_xmlNode->SetAttribute( "layout_width", "FILL_PARENT" );
	else if( m_widthConst == WRAP_CONTENT )
		_xmlNode->SetAttribute( "layout_width", "WRAP_CONTENT" );
	else if( m_widthConst == PERCENTAGE_PARENT )
	{
		_xmlNode->SetDoubleAttribute( "layout_width", m_widthPercent );
		//xmlNode->SetDoubleAttribute("layout_Width_percent",_widthPercent);
	}

	if( ( m_paddingLeft == m_paddingRight ) && ( m_paddingLeft == m_paddingTop ) && ( m_paddingLeft == m_paddingBottom ) && ( m_paddingLeft > 0 ) )
	{
		_xmlNode->SetAttribute( "padding", m_paddingLeft );
	}
	else
	{
		if( m_paddingLeft > 0 )
			_xmlNode->SetAttribute( "padding_left", m_paddingLeft );
		if( m_paddingRight > 0 )
			_xmlNode->SetAttribute( "padding_right", m_paddingRight );
		if( m_paddingTop > 0 )
			_xmlNode->SetAttribute( "padding_top", m_paddingTop );
		if( m_paddingBottom > 0 )
			_xmlNode->SetAttribute( "padding_bottom", m_paddingBottom );
	}
}

//----------------------------------------------------------------------------------
