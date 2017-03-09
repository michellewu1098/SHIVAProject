#include "GUI/Views/ViewGroups/ViewGroup.h"

//----------------------------------------------------------------------------------

ShivaGUI::ViewGroup::ViewGroup()
{
	m_deflateChildren = true;
}

//----------------------------------------------------------------------------------

ShivaGUI::ViewGroup::~ViewGroup()
{

}

//----------------------------------------------------------------------------------

ShivaGUI::LayoutParams* ShivaGUI::ViewGroup::InflateLayoutParams( TiXmlElement *_xmlElement, LayoutParams *_params, std::string _themePrefix )
{
	if( _params == NULL )
	{
		std::cerr << "WARNING: ViewGroup::InflateLayoutParams given NULL input, creating base LayoutParams instance" << std::endl;
		_params = new LayoutParams();
	}

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( ( std::string( "layout_width" ) == currentAttribute->Name() ) || ( _themePrefix + "layout_width" == currentAttribute->Name() ) )
		{
			double value = 0.0;
			if( currentAttribute->QueryDoubleValue( &value ) == TIXML_SUCCESS )
			{
				_params->SetWidthConst( LayoutParams::PERCENTAGE_PARENT );
				_params->SetWidthPercent( ( float ) value );
			}
			else
			{
				std::string strValue( currentAttribute->Value() );
				if( boost::algorithm::iequals( strValue, "fill_parent" ) )
				{
					_params->SetWidthConst( LayoutParams::FILL_PARENT );
				}
				else if( boost::algorithm::iequals( strValue, "wrap_content" ) )
				{
					_params->SetWidthConst( LayoutParams::WRAP_CONTENT );
				}
				else
				{
					std::cerr << "WARNING: ViewGroup::InflateLayoutParams attribute layout_width does not have a recognised const value: " << strValue << std::endl;
				}
			}
		}
		else if( ( std::string( "layout_height" ) == currentAttribute->Name() ) || ( _themePrefix + "layout_height" == currentAttribute->Name() ) )
		{
			double value = 0.0;
			if( currentAttribute->QueryDoubleValue( &value ) == TIXML_SUCCESS )
			{
				_params->SetHeightConst( LayoutParams::PERCENTAGE_PARENT );
				_params->SetHeightPercent( ( float )value );
			}
			else
			{
				std::string strValue( currentAttribute->Value() );
				if( boost::algorithm::iequals( strValue, "fill_parent" ) )
				{
					_params->SetHeightConst( LayoutParams::FILL_PARENT );
				}
				else if( boost::algorithm::iequals( strValue, "wrap_content" ) )
				{
					_params->SetHeightConst( LayoutParams::WRAP_CONTENT );
				}
				else
				{
					std::cerr << "WARNING: ViewGroup::InflateLayoutParams attribute layout_height does not have a recognised const value: " << strValue << std::endl;
				}
			}
		}
		else if( ( std::string( "padding" ) == currentAttribute->Name() ) || ( _themePrefix + "padding" == currentAttribute->Name() ) )
		{
			int value = 0;

			if( currentAttribute->QueryIntValue( &value ) == TIXML_SUCCESS )
			{
				_params->SetPadding( value, value, value, value );
			}
			else
			{
				std::cerr << "WARNING: ViewGroup::InflateLayoutParams attribute padding does not have expected value type (int)" << std::endl;
			}
		}
		else if( ( std::string( "padding_left" ) == currentAttribute->Name() ) || ( _themePrefix + "padding_left" == currentAttribute->Name() ) )
		{
			int value = 0;

			if( currentAttribute->QueryIntValue( &value ) == TIXML_SUCCESS )
			{
				_params->SetPaddingLeft( value );
			}
			else
			{
				std::cerr << "WARNING: ViewGroup::InflateLayoutParams attribute padding_left does not have expected value type (int)" << std::endl;
			}
		}
		else if( ( std::string( "padding_right" ) == currentAttribute->Name() ) || ( _themePrefix + "padding_right" == currentAttribute->Name() ) )
		{
			int value = 0;

			if( currentAttribute->QueryIntValue( &value ) == TIXML_SUCCESS )
			{
				_params->SetPaddingRight( value );
			}
			else
			{
				std::cerr << "WARNING: ViewGroup::InflateLayoutParams attribute padding_right does not have expected value type (int)" << std::endl;
			}
		}
		else if( ( std::string( "padding_top" ) == currentAttribute->Name() ) || ( _themePrefix + "padding_top" == currentAttribute->Name() ) )
		{
			int value = 0;

			if( currentAttribute->QueryIntValue( &value ) == TIXML_SUCCESS )
			{
				_params->SetPaddingTop( value );
			}
			else
			{
				std::cerr << "WARNING: ViewGroup::InflateLayoutParams attribute padding_top does not have expected value type (int)" << std::endl;
			}
		}
		else if( ( std::string( "padding_bottom" ) == currentAttribute->Name() ) || ( _themePrefix + "padding_bottom" == currentAttribute->Name() ) )
		{
			int value = 0;

			if( currentAttribute->QueryIntValue( &value ) == TIXML_SUCCESS )
			{
				_params->SetPaddingBottom( value );
			}
			else
			{
				std::cerr << "WARNING: ViewGroup::InflateLayoutParams attribute padding_bottom does not have expected value type (int)" << std::endl;
			}
		}
		else if( ( std::string( "height_constrained" ) == currentAttribute->Name() ) || ( _themePrefix + "height_constrained" == currentAttribute->Name() ) )
		{
			std::string value = currentAttribute->Value();
			if( value == "true" || value == "1" || value == "yes" )
			{
				_params->SetHeightConstrained( true );
			}
			else
			{
				_params->SetHeightConstrained( false );
			}
		}
	}

	return _params;
}

//----------------------------------------------------------------------------------
