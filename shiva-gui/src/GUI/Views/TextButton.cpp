#include "GUI/Views/TextButton.h"
#include "GUI/Drawables/BitmapDrawable.h"

//////////////////////////////////////////////////////////////////////////
#include <GL/GLee.h>
#include <sstream>
#include <iostream>


ShivaGUI::TextButton::TextButton()
{
	_texID = 0;
	_fontSize = 12;
	_fontColour = 0;
	_fontNameFromTheme = _fontSizeFromTheme = _fontColourFromTheme = false;
}

ShivaGUI::TextButton::~TextButton()
{
	// currently the Drawable does not delete the openGL texture (actually none of them are deleted, they're just cached)
	// but this texture will not be reused, so we need to delete it manually
	// TODO: fix this mess
	if( _texID != 0 )
		glDeleteTextures(1,&_texID);
}

void ShivaGUI::TextButton::Inflate(TiXmlElement *xmlElement, ResourceManager *resources, std::string themePrefix, bool rootNode )
{
	if( themePrefix.empty() )
		themePrefix = "TextButton_";
	ImageButton::Inflate(xmlElement,resources,themePrefix,rootNode);


	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( std::string("text") == currentAttribute->Name() )
		{
			_textBody = currentAttribute->ValueStr();
		}
		else if( (std::string("font") == currentAttribute->Name()) || (themePrefix+"font" == currentAttribute->Name()) )
		{
			_fontNameFromTheme = (themePrefix+"font" == currentAttribute->Name());
			_fontName = currentAttribute->Value();
		}
		else if( (std::string("text_size") == currentAttribute->Name()) || (themePrefix+"text_size" == currentAttribute->Name()) )
		{
			int value = 0;
			_fontSizeFromTheme = (themePrefix+"text_size" == currentAttribute->Name());

			if( currentAttribute->QueryIntValue(&value) == TIXML_SUCCESS )
			{
				_fontSize = value;
			}
			else
			{
				std::cerr<<"WARNING: TextButton::InflateLayoutParams attribute text_size does not have expected value type (double)"<<std::endl;
			}
		}
		else if( (std::string("text_colour") == currentAttribute->Name()) || (themePrefix+"text_colour" == currentAttribute->Name()) )
		{
			_fontColourFromTheme = (themePrefix+"text_colour" == currentAttribute->Name());

			std::string colourString( currentAttribute->Value() );
			std::stringstream colourStream;
			colourStream << std::hex << colourString;
			colourStream >> _fontColour;
		}
	}

	// Need to do this here, after all the values have been read in
	BuildTextCache(resources);
}

TiXmlElement* ShivaGUI::TextButton::Deflate(ResourceManager *resources)
{
	TiXmlElement *xmlNode = ImageButton::Deflate(resources);
	xmlNode->SetValue("TextButton");

	if( !_textBody.empty() )
		xmlNode->SetAttribute("text",_textBody );

	if( !_fontNameFromTheme )
		xmlNode->SetAttribute("font",_fontName );
	if( !_fontSizeFromTheme )
		xmlNode->SetAttribute("text_size",_fontSize );
	if( !_fontColourFromTheme )
	{
		std::string colourString;
		std::stringstream colourStream;
		colourStream << std::hex << _fontColour;
		colourStream >> colourString;
		xmlNode->SetAttribute("text_colour",colourString );
	}

	return xmlNode;
}


void ShivaGUI::TextButton::BuildTextCache(ResourceManager *resources)
{
	if( !_textBody.empty() )
	{
		if( _texID != 0 )
			glDeleteTextures(1,&_texID);
		_texID = 0;
		_texID = resources->GetSimpleText(_textBody, _fontName, _fontSize, _fontColour );
		if( _texID != 0 )
		{
			_contentGenDrawable = resources->CreateDrawable("BitmapDrawable");
			BitmapDrawable *bitmap = dynamic_cast<BitmapDrawable*>(_contentGenDrawable);
			if( bitmap != NULL )
			{
				bitmap->SetTexID(_texID);
			}
		}
		else
			std::cerr<<"WARNING: TextButton text OpenGL texID == 0"<<std::endl;
	}
}
