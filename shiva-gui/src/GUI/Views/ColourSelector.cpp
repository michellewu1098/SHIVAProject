#include "GUI/Views/ColourSelector.h"
#include "GUIManager.h"

//----------------------------------------------------------------------------------

ShivaGUI::ColourSelector::ColourSelector()
{
	m_selectorBoundsLeft = m_selectorBoundsRight = m_selectorBoundsTop = m_selectorBoundsBottom = 0;
	m_sampleBoundsLeft = m_sampleBoundsRight = m_sampleBoundsTop = m_sampleBoundsBottom = 0;

	m_colourSelectorProgram = new Utility::GPUProgram();
	m_colourSelectorProgram->Create( "Resources/Shaders/ColourSelector", Utility::GPUProgram::FRAGMENT );

	m_sampleR = 1.0f;
	m_sampleG = 1.0f;
	m_sampleB = 1.0f;

	m_holdingHandle = false;

	m_handleHalfSize = 10.0f;
	m_handlePropPosX = 0.5f;
	m_handlePropPosY = 1.0f;

	m_handleBoundsLeft = m_handleBoundsRight = m_handleBoundsTop = m_handleBoundsBottom = 0.0f;

	m_selectorDrawable = NULL;
	m_selectorStateListDrawable = NULL;

	m_colourListener = NULL;

	HSLtoRGB( m_handlePropPosX, 1.0f, m_handlePropPosY, m_sampleR, m_sampleG, m_sampleB );
	SetHandleActualPos();
}

//----------------------------------------------------------------------------------

ShivaGUI::ColourSelector::~ColourSelector()
{
	delete m_colourSelectorProgram;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	m_selectorBoundsLeft = ( float )_left + m_handleHalfSize;
	m_selectorBoundsRight = ( float )_right - m_handleHalfSize;
	m_selectorBoundsTop = ( float )_top + 30.f;
	m_selectorBoundsBottom = ( float )_bottom - m_handleHalfSize;

	m_sampleBoundsLeft = ( float )_left + m_handleHalfSize;
	m_sampleBoundsRight = ( float )_right - m_handleHalfSize;
	m_sampleBoundsTop = ( float )_top;
	m_sampleBoundsBottom = ( float )_top + 25.f;

	SetHandleActualPos();
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::Draw()
{
	// Bind Shaders
	m_colourSelectorProgram->Bind();

		glColor3f( 1.0f, 1.0f, 1.0f );
		glBegin( GL_QUADS );
			glTexCoord2f( 0.0f, 0.0f );	glVertex2f( m_selectorBoundsLeft, m_selectorBoundsTop );
			glTexCoord2f( 1.0f, 0.0f );	glVertex2f( m_selectorBoundsRight, m_selectorBoundsTop );
			glTexCoord2f( 1.0f, 1.0f );	glVertex2f( m_selectorBoundsRight, m_selectorBoundsBottom );
			glTexCoord2f( 0.0f, 1.0f );	glVertex2f( m_selectorBoundsLeft, m_selectorBoundsBottom );
		glEnd();

	m_colourSelectorProgram->Unbind();

	glColor3f( m_sampleR, m_sampleG, m_sampleB);
	glBegin( GL_QUADS );
		glTexCoord2f( 0.0f, 0.0f );	glVertex2f( m_sampleBoundsLeft, m_sampleBoundsTop );
		glTexCoord2f( 1.0f, 0.0f );	glVertex2f( m_sampleBoundsRight, m_sampleBoundsTop );
		glTexCoord2f( 1.0f, 1.0f );	glVertex2f( m_sampleBoundsRight, m_sampleBoundsBottom );
		glTexCoord2f( 0.0f, 1.0f );	glVertex2f( m_sampleBoundsLeft, m_sampleBoundsBottom );
	glEnd();

	/*
	glColor3f(1.0f,1.0f,1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);	glVertex2f(_handleBoundsLeft, _handleBoundsTop);
		glTexCoord2f(1.0f,0.0f);	glVertex2f(_handleBoundsRight, _handleBoundsTop);
		glTexCoord2f(1.0f,1.0f);	glVertex2f(_handleBoundsRight, _handleBoundsBottom);
		glTexCoord2f(0.0f,1.0f);	glVertex2f(_handleBoundsLeft, _handleBoundsBottom);
	glEnd();
	*/

	if( m_selectorStateListDrawable != NULL )
		m_selectorStateListDrawable->Draw();
	else if( m_selectorDrawable != NULL )
		m_selectorDrawable->Draw();
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "ColourSelector_";
	View::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( ( std::string( "selectorDrawable" ) == currentAttribute->Name() ) || ( _themePrefix + "selectorDrawable" == currentAttribute->Name() ) )
		{
			std::string resourceName( currentAttribute->Value() );

			// Retrieve resource and store it as correct type
			Drawable *drawable = _resources->GetDrawable( resourceName );
			m_selectorStateListDrawable = dynamic_cast< StateListDrawable* >( drawable );
			if( m_selectorStateListDrawable == NULL )
				m_selectorDrawable = drawable;
		}
		else if( std::string( "onColourChange" ) == currentAttribute->Name() )
		{
			m_colourListenerName = std::string( currentAttribute->Value() );

			// Retrieve listener
			ViewEventListener *listener = _resources->GetListener( m_colourListenerName );
			if( listener != NULL )
				m_colourListener = listener;
		}
	}
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::ColourSelector::Deflate( ResourceManager *_resources )
{
	TiXmlElement *xmlNode = View::Deflate( _resources );
	xmlNode->SetValue( "ColourSelector" );

	if( m_selectorStateListDrawable != NULL )
		xmlNode->SetAttribute( "selectorDrawable", m_selectorStateListDrawable->GetFilename() );
	else if( m_selectorDrawable != NULL )
		xmlNode->SetAttribute( "selectorDrawable", m_selectorDrawable->GetFilename() );

	if( !m_colourListenerName.empty() )
		xmlNode->SetAttribute( "onColourChange", m_colourListenerName );

	return xmlNode;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::SetFocus( bool )
{

}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::SetSelect( bool )
{

}

//----------------------------------------------------------------------------------

bool ShivaGUI::ColourSelector::HandleEvent( InternalEvent *_currentEvent )
{
	if( m_active )
	{

		if( _currentEvent->GetType() == InternalEvent::POSITIONAL_SELECT )
		{
			int mouseX, mouseY;
			_currentEvent->GetPosition( mouseX, mouseY );
			float posX, posY;
			if( MouseHitSelector( mouseX, mouseY, posX, posY ) )
			{
				m_handlePropPosX = posX;
				m_handlePropPosY = posY;
				HSLtoRGB( posX, 1.0f, posY, m_sampleR, m_sampleG, m_sampleB );
				SetHandleActualPos();
				m_holdingHandle = true;
				return true;
			}
		}
		if( m_holdingHandle && _currentEvent->GetType() == InternalEvent::POSITIONAL_DRAG )
		{
			int mouseX, mouseY;
			_currentEvent->GetPosition( mouseX, mouseY );
			float posX, posY;
			if( MouseHitSelector( mouseX, mouseY, posX, posY ) )
			{
				m_handlePropPosX = posX;
				m_handlePropPosY = posY;
				HSLtoRGB( posX, 1.0f, posY, m_sampleR, m_sampleG, m_sampleB );
				SetHandleActualPos();
				if( m_colourListener != NULL )
					m_colourListener->HandleEvent( this );
				return true;
			}
		}
		if( m_holdingHandle && _currentEvent->GetType() == InternalEvent::POSITIONAL_DESELECT )
		{
			m_holdingHandle = false;
			if( m_colourListener != NULL )
				m_colourListener->HandleEvent( this );
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::SetHandlePosition( float _x, float _y )
{
	m_handlePropPosX = _x;
	m_handlePropPosY = _y;
	HSLtoRGB( _x, 1.0f, _y, m_sampleR, m_sampleG, m_sampleB );
	SetHandleActualPos();
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ColourSelector::MouseHitSelector( int _mouseX, int _mouseY, float &_posX, float &_posY )
{
	if( m_holdingHandle || ( _mouseX > m_selectorBoundsLeft && _mouseX < m_selectorBoundsRight
		&& _mouseY > m_selectorBoundsTop && _mouseY < m_selectorBoundsBottom ) )
	{
		float widgetWidth = m_selectorBoundsRight - m_selectorBoundsLeft;
		float widgetHeight = m_selectorBoundsBottom - m_selectorBoundsTop;
		_posX = ( float )( _mouseX - m_selectorBoundsLeft ) / widgetWidth;
		_posY = ( float )( _mouseY - m_selectorBoundsTop ) / widgetHeight;

		if( _posX < 0.0f )
			_posX = 0.0f;
		else if( _posX > 1.0f )
			_posX = 1.0f;

		if( _posY < 0.0f )
			_posY = 0.0f;
		else if( _posY > 1.0f )
			_posY = 1.0f;

		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::HSLtoRGB( float _H, float _S, float _L, float &_R, float &_G, float &_B )
{
	/// This code is kinda from Bob Powell (http://www.bobpowell.net/RGBHSB.htm)
	/// But he got it from Foley and van Dam
	/// See also http://130.113.54.154/~monger/hsl-rgb.html
	/// (it's the exact same thing, variables same names etc)

	float temp2 = ( ( _L <= 0.5f ) ? _L * ( 1.0f + _S ) : _L + _S - ( _L * _S ) );
	float temp1 = 2.0f * _L - temp2;

	float temp3[ 3 ];
	temp3[ 0 ] = _H + 1.0f / 3.0f;
	temp3[ 1 ] = _H;
	temp3[ 2 ] = _H - 1.0f / 3.0f;

	float colour[ 3 ];
	colour[ 0 ] = 0.0f;
	colour[ 1 ] = 0.0f;
	colour[ 2 ] = 0.0f;

	for( int i = 0; i < 3; i++ )
	{
		if( temp3[ i ] < 0 )
			temp3[ i ] += 1.0f;
		else if( temp3[ i ] > 1 )
			temp3[ i ] -= 1.0f;

		if( 6.0f * temp3[ i ] < 1.0f )
			colour[ i ] = temp1 + ( temp2 - temp1 ) * temp3[ i ] * 6.0f;
		else if( 2.0f * temp3[ i ] < 1.0f )
			colour[ i ] = temp2;
		else if( 3.0f * temp3[ i ] < 2.0f )
			colour[ i ] = ( temp1 + ( temp2 - temp1 ) * ( ( 2.0f / 3.0f ) - temp3[ i ] ) * 6.0f );
		else
			colour[ i ] = temp1;
	}

	_R = colour[ 0 ];
	_G = colour[ 1 ];
	_B = colour[ 2 ];
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::SetHandleActualPos()
{
	float widgetWidth = m_selectorBoundsRight - m_selectorBoundsLeft;
	float widgetHeight = m_selectorBoundsBottom - m_selectorBoundsTop;
	float posX = ( m_handlePropPosX * widgetWidth) + m_selectorBoundsLeft;
	float posY = ( m_handlePropPosY * widgetHeight) + m_selectorBoundsTop;
	m_handleBoundsLeft = posX - m_handleHalfSize;
	m_handleBoundsRight = posX + m_handleHalfSize;
	m_handleBoundsTop = posY - m_handleHalfSize;
	m_handleBoundsBottom = posY + m_handleHalfSize;


	if( m_selectorStateListDrawable != NULL )
		m_selectorStateListDrawable->SetBounds( m_handleBoundsLeft, m_handleBoundsTop, m_handleBoundsRight, m_handleBoundsBottom );
	else if( m_selectorDrawable != NULL )
		m_selectorDrawable->SetBounds( m_handleBoundsLeft, m_handleBoundsTop, m_handleBoundsRight, m_handleBoundsBottom );
}

//----------------------------------------------------------------------------------

