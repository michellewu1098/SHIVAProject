#include "GUI/Views/ViewGroups/ViewGroup.h"

//////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <boost/algorithm/string/predicate.hpp>

ShivaGUI::ViewGroup::ViewGroup()
{
	_deflateChildren = true;
}


ShivaGUI::ViewGroup::~ViewGroup()
{

}

ShivaGUI::LayoutParams* ShivaGUI::ViewGroup::InflateLayoutParams( TiXmlElement *xmlElement, LayoutParams *params, std::string themePrefix )
{
	if( params == NULL )
	{
		std::cerr << "WARNING: ViewGroup::InflateLayoutParams given NULL input, creating base LayoutParams instance" << std::endl;
		params = new LayoutParams();
	}

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( ( std::string( "layout_width" ) == currentAttribute->Name() ) || ( themePrefix + "layout_width" == currentAttribute->Name() ) )
		{
			double value = 0.0;
			if( currentAttribute->QueryDoubleValue( &value ) == TIXML_SUCCESS )
			{
				params->SetWidthConst( LayoutParams::PERCENTAGE_PARENT );
				params->SetWidthPercent( ( float ) value );
			}
			else
			{
				std::string strValue( currentAttribute->Value() );
				if( boost::algorithm::iequals( strValue, "fill_parent" ) )
				{
					params->SetWidthConst( LayoutParams::FILL_PARENT );
				}
				else if( boost::algorithm::iequals( strValue, "wrap_content" ) )
				{
					params->SetWidthConst( LayoutParams::WRAP_CONTENT );
				}
				else
				{
					std::cerr << "WARNING: ViewGroup::InflateLayoutParams attribute layout_width does not have a recognised const value: " << strValue << std::endl;
				}
			}
		}
		else if( (std::string("layout_height") == currentAttribute->Name()) || (themePrefix+"layout_height" == currentAttribute->Name()) )
		{
			double value = 0.0;
			if( currentAttribute->QueryDoubleValue(&value) == TIXML_SUCCESS )
			{
				params->SetHeightConst(LayoutParams::PERCENTAGE_PARENT);
				params->SetHeightPercent((float) value);
			}
			else
			{
				std::string strValue( currentAttribute->Value() );
				if( boost::algorithm::iequals(strValue,"fill_parent") )
				{
					params->SetHeightConst(LayoutParams::FILL_PARENT);
				}
				else if( boost::algorithm::iequals(strValue,"wrap_content") )
				{
					params->SetHeightConst(LayoutParams::WRAP_CONTENT);
				}
				else
				{
					std::cerr<<"WARNING: ViewGroup::InflateLayoutParams attribute layout_height does not have a recognised const value: "<<strValue<<std::endl;
				}
			}
		}
		else if( (std::string("padding") == currentAttribute->Name()) || (themePrefix+"padding" == currentAttribute->Name()) )
		{
			int value = 0;

			if( currentAttribute->QueryIntValue(&value) == TIXML_SUCCESS )
			{
				params->SetPadding(value,value,value,value);
			}
			else
			{
				std::cerr<<"WARNING: ViewGroup::InflateLayoutParams attribute padding does not have expected value type (int)"<<std::endl;
			}
		}
		else if( (std::string("padding_left") == currentAttribute->Name()) || (themePrefix+"padding_left" == currentAttribute->Name()) )
		{
			int value = 0;

			if( currentAttribute->QueryIntValue(&value) == TIXML_SUCCESS )
			{
				params->SetPaddingLeft(value);
			}
			else
			{
				std::cerr<<"WARNING: ViewGroup::InflateLayoutParams attribute padding_left does not have expected value type (int)"<<std::endl;
			}
		}
		else if( (std::string("padding_right") == currentAttribute->Name()) || (themePrefix+"padding_right" == currentAttribute->Name()) )
		{
			int value = 0;

			if( currentAttribute->QueryIntValue(&value) == TIXML_SUCCESS )
			{
				params->SetPaddingRight(value);
			}
			else
			{
				std::cerr<<"WARNING: ViewGroup::InflateLayoutParams attribute padding_right does not have expected value type (int)"<<std::endl;
			}
		}
		else if( (std::string("padding_top") == currentAttribute->Name()) || (themePrefix+"padding_top" == currentAttribute->Name()) )
		{
			int value = 0;

			if( currentAttribute->QueryIntValue(&value) == TIXML_SUCCESS )
			{
				params->SetPaddingTop(value);
			}
			else
			{
				std::cerr<<"WARNING: ViewGroup::InflateLayoutParams attribute padding_top does not have expected value type (int)"<<std::endl;
			}
		}
		else if( (std::string("padding_bottom") == currentAttribute->Name()) || (themePrefix+"padding_bottom" == currentAttribute->Name()) )
		{
			int value = 0;

			if( currentAttribute->QueryIntValue(&value) == TIXML_SUCCESS )
			{
				params->SetPaddingBottom(value);
			}
			else
			{
				std::cerr<<"WARNING: ViewGroup::InflateLayoutParams attribute padding_bottom does not have expected value type (int)"<<std::endl;
			}
		}
	}

	return params;
}

