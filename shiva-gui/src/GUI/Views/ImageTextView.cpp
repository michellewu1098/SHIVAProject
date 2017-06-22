#include "GUI/Views/ImageTextView.h"

//----------------------------------------------------------------------------------

ShivaGUI::ImageTextView::ImageTextView()
{
	m_fontSize = 12;
	m_fontColour = 0;
	m_textAlignment = Left;

	m_fontNameFromTheme = m_fontSizeFromTheme = m_fontColourFromTheme = m_textAlignFromTheme = false;

	m_contentGenDrawable = NULL;
	m_contentDrawableFromTheme = false;
}

//----------------------------------------------------------------------------------

ShivaGUI::ImageTextView::~ImageTextView()
{
	delete m_contentGenDrawable;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageTextView::Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	View::Layout( _left, _top, _right, _bottom, _windowWidth, _windowHeight );

	float contentBoundsLeft = ( float )_left, contentBoundsRight = ( float )_right, contentBoundsTop = ( float )_top, contentBoundsBottom = ( float )_bottom;

	if( m_contentGenDrawable != NULL )
		m_contentGenDrawable->SetBounds( contentBoundsLeft, contentBoundsTop, contentBoundsRight, contentBoundsBottom );
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageTextView::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "ImageTextView_";

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( std::string( "text" ) == currentAttribute->Name() )
		{
			m_textBody = currentAttribute->Value();
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

			if( currentAttribute->QueryIntValue( &value ) == TIXML_SUCCESS ) {
				m_fontSize = value;
			}
			else { std::cerr << "WARNING: TextButton::InflateLayoutParams attribute text_size does not have expected value type (double)" << std::endl; }
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
			else if( alignment == std::string( "right" ) || alignment == std::string( "Right" ) )
			{
				m_textAlignment = Right;
			}
		}
		else if( ( std::string( "icon_position" ) == currentAttribute->Name() ) || ( _themePrefix + "icon_position" == currentAttribute->Name() ) )
		{
			m_iconPositionFromTheme = ( _themePrefix + "icon_position" == currentAttribute->Name() );

			std::string pos = currentAttribute->ValueStr();

			if( pos == std::string( "left" ) || pos == std::string( "Left" ) ) {
				m_iconPosition = Left;
				_resources->SetIconOnLeft();
			}
			else if( pos == std::string( "top" ) || pos == std::string( "Top" ) || pos == std::string( "above" ) || pos == std::string( "Above" ) ) {
				m_iconPosition = Top;
				_resources->SetIconAboveText();
			}
			else if( pos == std::string( "bottom" ) || pos == std::string ( "Bottom" ) || pos == std::string( "below" ) || pos == std::string( "Below" ) )
			{
				m_iconPosition = Bottom;
				_resources->SetIconBelowText();
			}
		}

		if( !m_textBody.empty() )
		_resources->SetTextInfo( m_textBody, m_fontName, m_fontSize, m_fontColour, m_textAlignment );

		if( ( std::string( "src" ) == currentAttribute->Name() ) || ( _themePrefix + "src" == currentAttribute->Name() ) )
		{
			//m_contentDrawableFromTheme = ( _themePrefix + "src" == currentAttribute->Value() );
			std::string resourceName( _resources->GetInflationAttribute( currentAttribute->Value() ) );

			Drawable* drawable = _resources->GetDrawable( resourceName );
			SetContent( drawable );
		}
		else if( ( std::string( "scaleContent" ) == currentAttribute->Name() ) || ( _themePrefix + "scaleContent" == currentAttribute->Name() ) )
		{
			std::string value = currentAttribute->Value();
			m_setContentScaleUp = ( value == "true" || value == "1" || value == "yes" );

			if( m_contentGenDrawable != NULL )
			{
				BitmapDrawable *bitmapContent = dynamic_cast< BitmapDrawable* >( m_contentGenDrawable );
				if( bitmapContent != NULL )
				{
					bitmapContent->SetScaleup( ( value == "true" || value == "1" || value == "yes" ) );
				}
			}
		}
		else if( ( std::string( "keepContentAspectRatio" ) == currentAttribute->Name() ) || ( _themePrefix + "keepContentAspectRatio" == currentAttribute->Name() ) )
		{
			std::string value = currentAttribute->Value();
			m_setContentAspectRatio = ( value == "true" || value == "1" || value == "yes" );

			if( m_contentGenDrawable != NULL )
			{
				BitmapDrawable *bitmapContent = dynamic_cast< BitmapDrawable* >( m_contentGenDrawable );
				if( bitmapContent != NULL )
				{
					bitmapContent->SetScaleKeepAspectRatio( ( value == "true" || value == "1" || value == "yes" ) );
				}
			}
		}
	}

	View::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );

	LayeredImageDrawable* image = dynamic_cast< LayeredImageDrawable* >( m_contentGenDrawable );
	if( image != NULL )
	{
		image->AddTextLayer( m_fontColour );
	}
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::ImageTextView::Deflate( ResourceManager* _resources )
{
	TiXmlElement* xmlNode = View::Deflate( _resources );
	xmlNode->SetValue( "ImageTextView" );

	// We only need to output parameters which were *not* given to usa via a theme

	if( !m_contentDrawableFromTheme )
	{
		std::string srcFilename;
		
		if( m_contentGenDrawable != NULL )
		{
			srcFilename = m_contentGenDrawable->GetFilename();
		}

		// The source might have been generated rather than loaded from file, so only output if we have a filename to give
		if( !srcFilename.empty() )
		{
			std::cout << "INFO: ImageTextView deflate: filename contains: " << srcFilename << std::endl;
			xmlNode->SetAttribute( "src", srcFilename );
		}
	}

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

void ShivaGUI::ImageTextView::SetContent( Drawable* _drawable )
{
	m_contentGenDrawable = _drawable;

	BitmapDrawable *bitmapContent = dynamic_cast< BitmapDrawable* >( m_contentGenDrawable );
	if( bitmapContent != NULL )
	{
		//if( m_setContentScaleUp >= 0 )
		//	bitmapContent->SetScaleup( m_setContentScaleup != 0 );

		//if( m_setContentAspectRation >= 0 )
		//	bitmapContent->SetScaleKeepAspectRatio( m_setContentAspectRatio != 0 );
	}

	LayeredImageDrawable *layeredContent = dynamic_cast< LayeredImageDrawable* >( m_contentGenDrawable );
	if( layeredContent != NULL )
	{
		layeredContent->AddTextLayer( m_fontColour );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageTextView::SetText( std::string _text, ResourceManager* _resources )
{
	m_textBody = _text;
	if( !m_textBody.empty() )
	{
		_resources->SetTextInfo( m_textBody, m_fontName, m_fontSize, m_fontColour, m_textAlignment );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageTextView::Draw()
{
	View::Draw();

	if( m_visible )
	{
		if( m_contentGenDrawable != NULL )
		{
			m_contentGenDrawable->Draw();
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageTextView::Draw( unsigned int _context )
{
	View::Draw( _context );

	if( m_visible )
	{
		if( m_contentGenDrawable != NULL )
		{
			m_contentGenDrawable->Draw();
		}
	}
}

//----------------------------------------------------------------------------------

int ShivaGUI::ImageTextView::GetWrapHeight()
{
	if( m_contentGenDrawable != NULL )
	{
		return m_contentGenDrawable->GetNativeHeight();
	}

	return 0;
}

//----------------------------------------------------------------------------------

int ShivaGUI::ImageTextView::GetWrapWidth()
{
	if( m_contentGenDrawable != NULL )
	{
		return m_contentGenDrawable->GetNativeWidth();
	}
	
	return 0;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageTextView::BuildTextCache( ShivaGUI::ResourceManager *_resources )
{

}

//----------------------------------------------------------------------------------