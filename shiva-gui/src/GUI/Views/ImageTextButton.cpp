#include "GUI/Views/ImageTextButton.h"
#include "GUI/Drawables/BitmapDrawable.h"
#include "GUI/Drawables/LayeredImageDrawable.h"

#include <GL/GLee.h>
#include <sstream>
#include <iostream>

//----------------------------------------------------------------------------------

ShivaGUI::ImageTextButton::ImageTextButton()
{
	_texID = 0;
	_fontSize = 12;
	_fontColour = 0;
	_textAlignment = Left;
	_fontNameFromTheme = _fontSizeFromTheme = _fontColourFromTheme = _textAlignFromTheme = false;

	_stateListDrawable = NULL;
	_generalDrawable = NULL;
	_contentGenDrawable = NULL;
	_contentStateListDrawable = NULL;
	_focussed = false;
	_pressed = false;
	_active = true;
	_hover = false;
	_clickListener = NULL;
	_genDrawableFromTheme = _contentDrawableFromTheme = false;
	
	_setContentScaleUp = _setContentAspectRatio = -1;
}

//----------------------------------------------------------------------------------

ShivaGUI::ImageTextButton::~ImageTextButton()
{
	// currently the Drawable does not delete the openGL texture (actually none of them are deleted, they're just cached)
	// but this texture will not be reused, so we need to delete it manually
	// TODO: fix this mess
	if( _texID != 0 )
		glDeleteTextures( 1, &_texID );

	delete _stateListDrawable;
	delete _generalDrawable;
	delete _contentGenDrawable;
	delete _contentStateListDrawable;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageTextButton::Inflate( TiXmlElement *xmlElement, ResourceManager *resources, std::string themePrefix, bool rootNode )
{
	if( themePrefix.empty() )
		themePrefix = "ImageTextButton_";
	
	
	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( std::string( "text" ) == currentAttribute->Name() )
		{
			_textBody = currentAttribute->Value();
		}
		else if( ( std::string( "font" ) == currentAttribute->Name() ) || ( themePrefix + "font" == currentAttribute->Name() ) )
		{
			_fontNameFromTheme = ( themePrefix + "font" == currentAttribute->Name() );
			_fontName = currentAttribute->Value();
		}
		else if( ( std::string( "text_size" ) == currentAttribute->Name() ) || ( themePrefix + "text_size" == currentAttribute->Name() ) )
		{
			int value = 0;
			_fontSizeFromTheme = ( themePrefix + "text_size" == currentAttribute->Name() );

			if( currentAttribute->QueryIntValue( &value ) == TIXML_SUCCESS ) {
				_fontSize = value;
			}
			else {
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


	ImageButton::Inflate( xmlElement, resources, themePrefix, rootNode );

	LayeredImageDrawable* image = dynamic_cast< LayeredImageDrawable* >( _contentGenDrawable );
	if( image != NULL )
	{
		image->SetTextParams( _textBody, _textAlignment, _fontName, _fontSize, _fontColour );
		image->addTextLayer( resources );
	}
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::ImageTextButton::Deflate( ResourceManager *resources )
{
	TiXmlElement *xmlNode = ImageButton::Deflate( resources );
	xmlNode->SetValue( "ImageTextButton" );

	// We only need to output parameters which were *not* given to us via a theme

	if( !_genDrawableFromTheme )
	{
		std::string srcFilename;
		if( _stateListDrawable != NULL )
		{
			srcFilename = _stateListDrawable->GetFilename();
		}
		else if( _generalDrawable != NULL )
		{
			srcFilename = _generalDrawable->GetFilename();
		}

		// The drawable might have been generated rather than loaded from file, so only output if we have a filename to give
		if( !srcFilename.empty() )
		{
			xmlNode->SetAttribute( "drawable", srcFilename );
		}
	}

	if( !_contentDrawableFromTheme )
	{
		std::string srcFilename;
		if( _contentStateListDrawable != NULL )
		{
			srcFilename = _contentStateListDrawable->GetFilename();
		}
		else if( _contentGenDrawable != NULL )
		{
			srcFilename = _contentGenDrawable->GetFilename();
		}

		// The source might have been generated rather than loaded from file, so only output if we have a filename to give
		if( !srcFilename.empty() )
		{
			std::cout << "INFO: ImageButton deflate: filename contains: " << srcFilename << std::endl;
			xmlNode->SetAttribute( "src", srcFilename );
		}
	}

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
		xmlNode->SetAttribute( "text_alignment", _textAlignment );

	return xmlNode;
}

//----------------------------------------------------------------------------------