#include "GUI/Views/TextView.h"
#include "GUI/Drawables/Drawable.h"
#include "GUIManager.h"

#include <GL/GLee.h>
#include <sstream>
#include <iostream>

//----------------------------------------------------------------------------------

ShivaGUI::TextView::TextView()
{
	_texID = 0;
	_fontSize = 12;
	_fontColour = 0;
	_textDrawable = NULL;

	_textAlignment = Left;
	_fontNameFromTheme = _fontSizeFromTheme = _fontColourFromTheme = _textAlignFromTheme = false;
}

//----------------------------------------------------------------------------------

ShivaGUI::TextView::~TextView()
{
	if( _texID != 0 )
		glDeleteTextures( 1, &_texID );
	delete _textDrawable;
}

//----------------------------------------------------------------------------------

void ShivaGUI::TextView::Layout( int left, int top, int right, int bottom, int windowWidth, int windowHeight )
{
	View::Layout( left, top, right, bottom, windowWidth, windowHeight );

	if( _textDrawable != NULL )
		_textDrawable->SetBounds( left, top, right, bottom, _internalGravity );
}

//----------------------------------------------------------------------------------

void ShivaGUI::TextView::Draw()
{
	View::Draw();
	if( _visible && _textDrawable )
		_textDrawable->Draw();
}

//----------------------------------------------------------------------------------

void ShivaGUI::TextView::Inflate(TiXmlElement *xmlElement, ShivaGUI::ResourceManager *resources, std::string themePrefix, bool rootNode )
{
	if( themePrefix.empty() )
		themePrefix = "TextView_";
	View::Inflate( xmlElement, resources, themePrefix, rootNode );

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( std::string( "text" ) == currentAttribute->Name() )
		{
			_textBody = currentAttribute->ValueStr();
		}
		else if( ( std::string( "font" ) == currentAttribute->Name() ) || ( themePrefix + "font" == currentAttribute->Name() ) )
		{
			_fontNameFromTheme = ( themePrefix + "font" == currentAttribute->Name() );
			_fontName = currentAttribute->Value();
		}
		else if( ( std::string( "text_size" ) == currentAttribute->Name() ) || ( themePrefix + "text_size" == currentAttribute->Name() ) )
		{
			_fontSizeFromTheme = ( themePrefix + "text_size" == currentAttribute->Name() );
			int value = 0;

			if( currentAttribute->QueryIntValue(&value) == TIXML_SUCCESS )
			{
				_fontSize = value;
			}
			else
			{
				std::cerr << "WARNING: TextButton::InflateLayoutParams attribute text_size does not have expected value type (double)" << std::endl;
			}
		}
		else if( ( std::string( "text_colour" ) == currentAttribute->Name() ) || ( themePrefix + "text_colour" == currentAttribute->Name() ) )
		{
			_fontColourFromTheme = ( themePrefix + "text_colour" == currentAttribute->Name() );

			std::string colourString( currentAttribute->Value() );
			std::stringstream colourStream;
			colourStream << std::hex << colourString;
			colourStream >> _fontColour;
		}
		else if( ( std::string( "text_alignment" ) == currentAttribute->Name() ) || ( themePrefix + "text_alignment" == currentAttribute->Name() ) ) 
		{
			_textAlignFromTheme = ( themePrefix + "text_alignment" == currentAttribute->Name() );
			
			std::string alignment = currentAttribute->ValueStr();

			if( alignment == std::string( "left" ) || alignment == std::string( "Left" ) ) {
				_textAlignment = Left; 
			}
			else if( alignment == std::string( "centre" ) || alignment == std::string( "center" ) || alignment == std::string( "Centre" ) || alignment == std::string( "Center" ) ) {
				_textAlignment = Centre;
			}
			else if( alignment == std::string( "right" ) || alignment == std::string( "Right" ) ) {
				_textAlignment = Right;
			}
		}
	}

	// Need to do this here, after all the values have been read in
	BuildTextCache( resources );
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::TextView::Deflate( ResourceManager *resources )
{
	TiXmlElement *xmlNode = View::Deflate( resources );
	xmlNode->SetValue( "TextView" );

	if( !_textBody.empty() )
		xmlNode->SetAttribute( "text", _textBody );

	if( !_fontNameFromTheme )
		xmlNode->SetAttribute( "font", _fontName );
	if( !_fontSizeFromTheme )
		xmlNode->SetAttribute( "text_size", _fontSize );
	if( !_fontColourFromTheme )
	{
		std::string colourString;
		std::stringstream colourStream;
		colourStream << std::hex << _fontColour;
		colourStream >> colourString;
		xmlNode->SetAttribute( "text_colour", colourString );
	}
	if( !_textAlignFromTheme )
		xmlNode->SetAttribute( "text_alignment", _textAlignment ) ;

	return xmlNode;
}

//----------------------------------------------------------------------------------

void ShivaGUI::TextView::SetText( std::string text, ShivaGUI::ResourceManager *resources )
{
	_textBody = text;
	BuildTextCache( resources );
}

//----------------------------------------------------------------------------------

int ShivaGUI::TextView::GetWrapWidth()
{
	if( _textDrawable != NULL )
		return _textDrawable->GetNativeWidth();
	return 0;
}

//----------------------------------------------------------------------------------

int ShivaGUI::TextView::GetWrapHeight()
{
	if( _textDrawable != NULL )
		return _textDrawable->GetNativeHeight();
	return 0;
}

//----------------------------------------------------------------------------------

void ShivaGUI::TextView::BuildTextCache( ShivaGUI::ResourceManager *resources )
{
	if( !_textBody.empty() && resources != NULL )
	{
		if( _texID != 0 )
			glDeleteTextures( 1, &_texID );
		_texID = 0;
		//_texID = resources->GetSimpleText(_textBody, _fontName, _fontSize, _fontColour );
		_texID = resources->GetText( _textBody, _textAlignment, _fontName, _fontSize, _fontColour );
		if( _texID != 0 )
		{
			_textDrawable = dynamic_cast< BitmapDrawable* >( resources->CreateDrawable( "BitmapDrawable" ) );
			if( _textDrawable != NULL )
			{
				_textDrawable->SetTexID( _texID );

				LayoutParams *params = GetLayoutParams();
				if( params != NULL )
				{
					// Update our native sizes
					params->SetWrapWidth( _textDrawable->GetNativeWidth() );
					params->SetWrapHeight( _textDrawable->GetNativeHeight() );
				}
			}
			else
				std::cerr << "WARNING: TextView::Inflate, Resource system created Drawable of wrong type!" << std::endl;
		}
		else
			std::cerr << "WARNING: TextButton text OpenGL texID == 0" << std::endl;
	}
}

//----------------------------------------------------------------------------------