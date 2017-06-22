#include "GUI/Views/ImageTextButton.h"
#include "GUI/Drawables/BitmapDrawable.h"
#include "GUI/Drawables/LayeredImageDrawable.h"

//----------------------------------------------------------------------------------

ShivaGUI::ImageTextButton::ImageTextButton()
{
	m_fontSize = 12;
	m_fontColour = 0;
	m_textAlignment = Left;
	m_fontNameFromTheme = m_fontSizeFromTheme = m_fontColourFromTheme = m_textAlignFromTheme = m_iconPercentSizeFromTheme = m_textPercentSizeFromTheme = false;

	m_stateListDrawable = NULL;
	m_generalDrawable = NULL;
	m_contentGenDrawable = NULL;
	m_contentStateListDrawable = NULL;
	m_focussed = false;
	m_pressed = false;
	m_active = true;
	m_hover = false;
	m_clickListener = NULL;
	m_genDrawableFromTheme = m_contentDrawableFromTheme = false;
	
	m_setContentScaleUp = m_setContentAspectRatio = -1;
}

//----------------------------------------------------------------------------------

ShivaGUI::ImageTextButton::~ImageTextButton()
{
	delete m_stateListDrawable;
	delete m_generalDrawable;
	delete m_contentGenDrawable;
	delete m_contentStateListDrawable;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageTextButton::SetText( std::string _text, ResourceManager* _resources )
{
	m_textBody = _text;
	if( !m_textBody.empty() )
		_resources->SetTextInfo( m_textBody, m_fontName, m_fontSize, m_fontColour, m_textAlignment );
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageTextButton::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "ImageTextButton_";
	
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
			else {
				std::cerr << "WARNING: TextButton::InflateLayoutParams attribute text_size does not have expected value type (int)" << std::endl;
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
			else if( pos == std::string( "bottom" ) || pos == std::string( "Bottom" ) || pos == std::string( "below" ) || pos == std::string( "Below" ) ) {
				m_iconPosition = Bottom;
				_resources->SetIconBelowText();
			}
		}
		else if( ( std::string( "icon_percentSize" ) == currentAttribute->Name() ) || ( _themePrefix + "icon_percentSize" == currentAttribute->Name() ) )
		{
			double value = 0.0;

			m_iconPercentSizeFromTheme = ( _themePrefix + "icon_percentSize" == currentAttribute->Name() );

			if( currentAttribute->QueryDoubleValue( &value ) == TIXML_SUCCESS ) {
				m_iconPercentSize = ( float )value;
				_resources->SetIconPercentSize( ( float )value );
			}
			else {
				std::cerr << "WARNING: ImageTextButton::InflateLayoutParams attribute icon_percentSize does not have expected value type (double)" << std::endl;
			}
		}
		else if( ( std::string( "text_percentSize" ) == currentAttribute->Name() ) || ( _themePrefix + "text_percentSize" == currentAttribute->Name() ) )
		{
			double value = 0.0;

			m_textPercentSizeFromTheme = ( _themePrefix + "text_percentSize" == currentAttribute->Name() );

			if( currentAttribute->QueryDoubleValue( &value ) == TIXML_SUCCESS ) {
				m_textPercentSize = value;
				_resources->SetTextPercentSize( ( float )value );
			}
			else {
				std::cerr << "WARNING: ImageTextButton::InflateLayoutParams attribute text_percentSize does not have expected value type (double)" << std::endl;
			}
		}
	}

	if( !m_textBody.empty() )
		_resources->SetTextInfo( m_textBody, m_fontName, m_fontSize, m_fontColour, m_textAlignment );
	
	ImageButton::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );

	LayeredImageDrawable* image = dynamic_cast< LayeredImageDrawable* >( m_contentGenDrawable );
	if( image != NULL )
	{
		image->AddTextLayer( m_fontColour );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageTextButton::SetContent( Drawable *_drawable )
{
	m_contentStateListDrawable = dynamic_cast< StateListDrawable* >( _drawable );
	if( m_contentStateListDrawable == NULL )
		m_contentGenDrawable = _drawable;

	BitmapDrawable *bitmapContent = dynamic_cast< BitmapDrawable* >( m_contentGenDrawable );
	if( bitmapContent != NULL )
	{
		if( m_setContentScaleUp >= 0 )
			bitmapContent->SetScaleup( m_setContentScaleUp != 0 );
		
		if( m_setContentAspectRatio >= 0 )
			bitmapContent->SetScaleKeepAspectRatio( m_setContentAspectRatio != 0 );
	}

	LayeredImageDrawable *layeredContent = dynamic_cast< LayeredImageDrawable* >( m_contentGenDrawable );
	if( m_contentGenDrawable != NULL )
	{
		layeredContent->AddTextLayer( m_fontColour ); 
	}
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::ImageTextButton::Deflate( ResourceManager *_resources )
{
	TiXmlElement *xmlNode = ImageButton::Deflate( _resources );
	xmlNode->SetValue( "ImageTextButton" );

	// We only need to output parameters which were *not* given to us via a theme

	if( !m_genDrawableFromTheme )
	{
		std::string srcFilename;
		if( m_stateListDrawable != NULL )
		{
			srcFilename = m_stateListDrawable->GetFilename();
		}
		else if( m_generalDrawable != NULL )
		{
			srcFilename = m_generalDrawable->GetFilename();
		}

		// The drawable might have been generated rather than loaded from file, so only output if we have a filename to give
		if( !srcFilename.empty() )
		{
			xmlNode->SetAttribute( "drawable", srcFilename );
		}
	}

	if( !m_contentDrawableFromTheme )
	{
		std::string srcFilename;
		if( m_contentStateListDrawable != NULL )
		{
			srcFilename = m_contentStateListDrawable->GetFilename();
		}
		else if( m_contentGenDrawable != NULL )
		{
			srcFilename = m_contentGenDrawable->GetFilename();
		}

		// The source might have been generated rather than loaded from file, so only output if we have a filename to give
		if( !srcFilename.empty() )
		{
			std::cout << "INFO: ImageTextButton deflate: filename contains: " << srcFilename << std::endl;
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

	if( !m_iconPercentSizeFromTheme )
		xmlNode->SetAttribute( "icon_percentSize", m_iconPercentSize );

	if( !m_textPercentSizeFromTheme )
		xmlNode->SetAttribute( "text_percentSize", m_textPercentSize );

	return xmlNode;
}

//----------------------------------------------------------------------------------