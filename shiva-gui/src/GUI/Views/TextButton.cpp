#include "GUI/Views/TextButton.h"
#include "GUI/Drawables/BitmapDrawable.h"

//----------------------------------------------------------------------------------

ShivaGUI::TextButton::TextButton()
{
	m_texID = 0;
	m_fontSize = 12;
	m_fontColour = 0;
	m_textAlignment = Left;
	m_fontNameFromTheme = m_fontSizeFromTheme = m_fontColourFromTheme = m_textAlignFromTheme = false;
}

//----------------------------------------------------------------------------------

ShivaGUI::TextButton::~TextButton()
{
	// currently the Drawable does not delete the openGL texture (actually none of them are deleted, they're just cached)
	// but this texture will not be reused, so we need to delete it manually
	// TODO: fix this mess
	if( m_texID != 0 )
		glDeleteTextures( 1, &m_texID );
}

//----------------------------------------------------------------------------------

void ShivaGUI::TextButton::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "TextButton_";
	ImageButton::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( std::string( "text" ) == currentAttribute->Name() )
		{
			m_textBody = currentAttribute->ValueStr();
		}
		else if( ( std::string( "font" ) == currentAttribute->Name() ) || ( _themePrefix + "font" == currentAttribute->Name() ) )
		{
			m_fontNameFromTheme = ( _themePrefix + "font" == currentAttribute->Name() );
			m_fontName = currentAttribute->Value();
		}
		else if( ( std::string( "text_size" ) == currentAttribute->Name() ) || ( _themePrefix + "text_size" == currentAttribute->Name() ) )
		{
			int value = 0;
			m_fontSizeFromTheme = ( _themePrefix + "text_size" == currentAttribute->Name() );

			if( currentAttribute->QueryIntValue( &value ) == TIXML_SUCCESS )
			{
				m_fontSize = value;
			}
			else
			{
				std::cerr << "WARNING: TextButton::InflateLayoutParams attribute text_size does not have expected value type (double)" << std::endl;
			}
		}
		else if( ( std::string( "text_colour" ) == currentAttribute->Name() ) || ( _themePrefix + "text_colour" == currentAttribute->Name() ) )
		{
			m_fontColourFromTheme = ( _themePrefix + "text_colour" == currentAttribute->Name() );

			std::string colourString( currentAttribute->Value() );
			std::stringstream colourStream;
			colourStream << std::hex << colourString;
			colourStream >> m_fontColour;
		}
		else if( ( std::string( "text_alignment" ) == currentAttribute->Name() ) || ( _themePrefix + "text_alignment" == currentAttribute->Name() ) ) 
		{
			m_textAlignFromTheme = ( _themePrefix + "text_alignment" == currentAttribute->Name() );

			std::string alignment = currentAttribute->ValueStr();

			if( alignment == std::string( "left" ) || alignment == std::string( "Left" ) ) {
				m_textAlignment = Left; 
			}
			else if( alignment == std::string( "centre" ) || alignment == std::string( "center" ) || alignment == std::string( "Centre" ) || alignment == std::string( "Center" ) ) {
				m_textAlignment = Centre;
			}
			else if( alignment == std::string( "right" ) || alignment == std::string( "Right" ) ) {
				m_textAlignment = Right;
			}
		}
	}

	// Need to do this here, after all the values have been read in
	BuildTextCache( _resources );
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::TextButton::Deflate( ResourceManager *_resources )
{
	TiXmlElement *xmlNode = ImageButton::Deflate( _resources );
	xmlNode->SetValue( "TextButton" );

	if( !m_textBody.empty() )
		xmlNode->SetAttribute( "text", m_textBody );

	if( !m_fontNameFromTheme )
		xmlNode->SetAttribute( "font", m_fontName );
	if( !m_fontSizeFromTheme )
		xmlNode->SetAttribute( "text_size", m_fontSize );
	if( !m_fontColourFromTheme )
	{
		std::string colourString;
		std::stringstream colourStream;
		colourStream << std::hex << m_fontColour;
		colourStream >> colourString;
		xmlNode->SetAttribute( "text_colour", colourString );
	}
	if( !m_textAlignFromTheme )
		xmlNode->SetAttribute( "text_alignment", m_textAlignment );

	return xmlNode;
}

//----------------------------------------------------------------------------------


void ShivaGUI::TextButton::BuildTextCache( ResourceManager *_resources )
{
	if( !m_textBody.empty() )
	{
		if( m_texID != 0 )
			glDeleteTextures( 1, &m_texID );
		m_texID = 0;
		//_texID = resources->GetSimpleText(_textBody, _fontName, _fontSize, _fontColour );
		m_texID = _resources->GetText( m_textBody, m_textAlignment, m_fontName, m_fontSize, m_fontColour );
		if( m_texID != 0 )
		{
			m_contentGenDrawable = _resources->CreateDrawable( "BitmapDrawable" );
			BitmapDrawable *bitmap = dynamic_cast< BitmapDrawable* >( m_contentGenDrawable );
			if( bitmap != NULL )
			{
				bitmap->SetTexID( m_texID );
			}
		}
		else
			std::cerr << "WARNING: TextButton text OpenGL texID == 0" << std::endl;
	}
}

//----------------------------------------------------------------------------------