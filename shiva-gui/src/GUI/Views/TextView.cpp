#include "GUI/Views/TextView.h"
#include "GUIManager.h"

//----------------------------------------------------------------------------------

ShivaGUI::TextView::TextView()
{
	m_texID = 0;
	m_fontSize = 12;
	m_fontColour = 0;
	m_textDrawable = NULL;

	m_textAlignment = Left;
	m_fontNameFromTheme = m_fontSizeFromTheme = m_fontColourFromTheme = m_textAlignFromTheme = false;
}

//----------------------------------------------------------------------------------

ShivaGUI::TextView::~TextView()
{
	if( m_texID != 0 )
		glDeleteTextures( 1, &m_texID );
	delete m_textDrawable;
}

//----------------------------------------------------------------------------------

void ShivaGUI::TextView::Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	View::Layout( _left, _top, _right, _bottom, _windowWidth, _windowHeight );

	if( m_textDrawable != NULL )
		m_textDrawable->SetBounds( ( float )_left, ( float )_top, ( float )_right, ( float )_bottom, m_internalGravity );
}

//----------------------------------------------------------------------------------

void ShivaGUI::TextView::Draw()
{
	View::Draw();
	if( m_visible && m_textDrawable )
		m_textDrawable->Draw();
}

//----------------------------------------------------------------------------------

void ShivaGUI::TextView::Inflate( TiXmlElement *_xmlElement, ShivaGUI::ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "TextView_";
	View::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );

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
			m_fontSizeFromTheme = ( _themePrefix + "text_size" == currentAttribute->Name() );
			int value = 0;

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

TiXmlElement* ShivaGUI::TextView::Deflate( ResourceManager *_resources )
{
	TiXmlElement *xmlNode = View::Deflate( _resources );
	xmlNode->SetValue( "TextView" );

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
		xmlNode->SetAttribute( "text_alignment", m_textAlignment ) ;

	return xmlNode;
}

//----------------------------------------------------------------------------------

void ShivaGUI::TextView::SetText( std::string _text, ShivaGUI::ResourceManager *_resources )
{
	m_textBody = _text;
	BuildTextCache( _resources );
}

//----------------------------------------------------------------------------------

int ShivaGUI::TextView::GetWrapWidth()
{
	if( m_textDrawable != NULL )
		return m_textDrawable->GetNativeWidth();
	return 0;
}

//----------------------------------------------------------------------------------

int ShivaGUI::TextView::GetWrapHeight()
{
	if( m_textDrawable != NULL )
		return m_textDrawable->GetNativeHeight();
	return 0;
}

//----------------------------------------------------------------------------------

void ShivaGUI::TextView::BuildTextCache( ShivaGUI::ResourceManager *_resources )
{
	if( !m_textBody.empty() && _resources != NULL )
	{
		if( m_texID != 0 )
			glDeleteTextures( 1, &m_texID );
		m_texID = 0;
		//_texID = resources->GetSimpleText(_textBody, _fontName, _fontSize, _fontColour );
		m_texID = _resources->GetText( m_textBody, m_textAlignment, m_fontName, m_fontSize, m_fontColour );
		if( m_texID != 0 )
		{
			m_textDrawable = dynamic_cast< BitmapDrawable* >( _resources->CreateDrawable( "BitmapDrawable" ) );
			if( m_textDrawable != NULL )
			{
				m_textDrawable->SetTexID( m_texID );

				LayoutParams *params = GetLayoutParams();
				if( params != NULL )
				{
					// Update our native sizes
					params->SetWrapWidth( m_textDrawable->GetNativeWidth() );
					params->SetWrapHeight( m_textDrawable->GetNativeHeight() );
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