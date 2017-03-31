#include "GUI/Views/Slider.h"
#include "GUIManager.h"
#include "ResourceManager.h"

//----------------------------------------------------------------------------------

ShivaGUI::Slider::Slider()
{
	m_handlePosition = 1.0f;
	m_barDrawable = NULL;
	m_handleGeneralDrawable = NULL;
	m_handleStateListDrawable = NULL;
	m_handleHeight = 100.0f;
	m_handleWidth = 100.0f;
	m_holdingHandle = false;
	m_focussed = false;
	m_active = true;
	m_slideListener = NULL;
	m_eventOnRelease = false;
	m_barDrawableFromTheme = m_handleDrawableFromTheme = m_handleHeightFromTheme = m_handleWidthFromTheme = false;
}

//----------------------------------------------------------------------------------

ShivaGUI::Slider::~Slider()
{
	delete m_barDrawable;
	delete m_handleGeneralDrawable;
	delete m_handleStateListDrawable;
}

//----------------------------------------------------------------------------------

void ShivaGUI::Slider::Draw()
{
	View::Draw();

	if( m_visible )
	{
		if( m_barDrawable != NULL )
			m_barDrawable->Draw();

		if( m_handleStateListDrawable != NULL )
			m_handleStateListDrawable->Draw();
		else if( m_handleGeneralDrawable != NULL )
			m_handleGeneralDrawable->Draw();
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::Slider::Draw( unsigned int _context )
{
	View::Draw( _context );

	if( m_visible )
	{
		if( m_barDrawable != NULL )
			m_barDrawable->Draw();

		if( m_handleStateListDrawable != NULL )
			m_handleStateListDrawable->Draw();
		else if( m_handleGeneralDrawable != NULL )
			m_handleGeneralDrawable->Draw();
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::Slider::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "Slider_";
	View::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{

		if( ( std::string( "barDrawable" ) == currentAttribute->Name() ) || ( _themePrefix + "barDrawable" == currentAttribute->Name() ) )
		{
			m_barDrawableFromTheme = ( _themePrefix + "barDrawable" == currentAttribute->Name() );
			std::string resourceName( currentAttribute->Value() );

			// Retrieve resource and store it as correct type
			m_barDrawable = _resources->GetDrawable( resourceName );
		}
		else if( ( std::string("handleDrawable") == currentAttribute->Name() ) || ( _themePrefix + "handleDrawable" == currentAttribute->Name() ) )
		{
			m_handleDrawableFromTheme = ( _themePrefix + "handleDrawable" == currentAttribute->Name() );
			std::string resourceName( currentAttribute->Value() );

			// Retrieve resource and store it as correct type
			Drawable *drawable = _resources->GetDrawable( resourceName );
			m_handleStateListDrawable = dynamic_cast< StateListDrawable* >( drawable );
			if( m_handleStateListDrawable == NULL )
				m_handleGeneralDrawable = drawable;
		}
		else if( ( std::string( "handleHeight" ) == currentAttribute->Name() ) || ( _themePrefix + "handleHeight" == currentAttribute->Name() ) )
		{
			m_handleHeightFromTheme = ( _themePrefix + "handleHeight" == currentAttribute->Name() );
			m_handleHeight = ( float )currentAttribute->DoubleValue();
		}
		else if( ( std::string( "handleWidth" ) == currentAttribute->Name() ) || ( _themePrefix + "handleWidth" == currentAttribute->Name() ) )
		{
			m_handleWidthFromTheme = ( _themePrefix + "handleWidth" == currentAttribute->Name() );
			m_handleWidth = ( float )currentAttribute->DoubleValue();
		}
		else if( std::string( "handlePosition" ) == currentAttribute->Name() )
		{
			m_handlePosition = ( float )currentAttribute->DoubleValue();
			if( m_handlePosition < 0.0f )
				m_handlePosition = 0.0f;
			else if( m_handlePosition > 1.0f )
				m_handlePosition = 1.0f;
		}
		else if( std::string( "onSlide" ) == currentAttribute->Name() )
		{
			m_slideListenerName = currentAttribute->Value();

			// Retrieve listener
			ViewEventListener *listener = _resources->GetListener( m_slideListenerName );
			if( listener != NULL )
				m_slideListener = listener;
		}
		else if( std::string( "eventOnRelease" ) == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				m_eventOnRelease = true;
			else if( value == "false" )
				m_eventOnRelease = false;
		}
	}
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::Slider::Deflate( ResourceManager *_resources )
{
	TiXmlElement *xmlNode = View::Deflate( _resources );
	xmlNode->SetValue( "Slider" );


	if( !m_barDrawableFromTheme )
	{
		if( m_barDrawable != NULL )
			xmlNode->SetAttribute( "barDrawable", m_barDrawable->GetFilename() );
	}

	if( !m_handleDrawableFromTheme )
	{
		if( m_handleStateListDrawable != NULL )
			xmlNode->SetAttribute( "handleDrawable", m_handleStateListDrawable->GetFilename() );
		else if( m_handleGeneralDrawable != NULL )
			xmlNode->SetAttribute( "handleDrawable", m_handleGeneralDrawable->GetFilename() );
	}

	if( !m_handleHeightFromTheme )
		xmlNode->SetDoubleAttribute( "handleHeight", m_handleHeight );
	if( !m_handleWidthFromTheme )
		xmlNode->SetDoubleAttribute( "handleWidth", m_handleWidth );
	xmlNode->SetDoubleAttribute( "handlePosition", m_handlePosition );

	if( !m_slideListenerName.empty() )
		xmlNode->SetAttribute( "onSlide", m_slideListenerName );

	if( m_eventOnRelease )
		xmlNode->SetAttribute( "eventOnRelease", "true" );

	return xmlNode;
}

//----------------------------------------------------------------------------------

void ShivaGUI::Slider::SetFocus( bool _value )
{
	if( m_active )
		m_focussed = _value;
	SetStateDrawable();
}

//----------------------------------------------------------------------------------

bool ShivaGUI::Slider::HandleEvent( InternalEvent *_currentEvent )
{
	if( !m_active || !m_visible )
		return false;

	if( _currentEvent->GetType() == InternalEvent::POSITIONAL_SELECT )
	{
		int mouseX, mouseY;
		_currentEvent->GetPosition(mouseX,mouseY);
		if( MouseHit( mouseX, mouseY ) == 1 )
		{
			//SetSelect(true);
			m_holdingHandle = true;
			SetStateDrawable();
			return true;
		}
	}
	else if( _currentEvent->GetType() == InternalEvent::POSITIONAL_DESELECT )
	{
		int mouseX, mouseY;
		_currentEvent->GetPosition( mouseX, mouseY );
		if( m_holdingHandle ) //MouseHit(mouseX,mouseY) )
		{
			//SetSelect(false);
			m_holdingHandle = false;
			SetStateDrawable();
			if( m_eventOnRelease && m_slideListener != NULL )
				m_slideListener->HandleEvent( this );

			return true;
		}
	}
	else if( _currentEvent->GetType() == InternalEvent::POSITIONAL_DRAG )
	{
		if( m_holdingHandle )
		{
			int mouseX, mouseY;
			_currentEvent->GetPosition( mouseX, mouseY );

			// Translate this and set it as the new position of the handle
			float widgetWidth = ( ( float )m_boundsRight ) - ( ( float )m_boundsLeft ) - m_handleWidth;

			float relativeMouseX = mouseX - m_boundsLeft - ( m_handleWidth / 2.0f );
			SetHandlePosition( relativeMouseX / widgetWidth );

			return true;
		}
	}
	/*
	else if( currentEvent->GetType() == InputSystem::KEYDOWN )
	{
		if( _focussed )
		{
			bool sliderMoved = false;
			// Left key
			if( currentEvent->GetKeycode() == 276 )
			{
				sliderMoved = true;
				SetHandlePosition(_handlePosition-0.1f);
			}
			// Right key
			else if( currentEvent->GetKeycode() == 275 )
			{
				sliderMoved = true;
				SetHandlePosition(_handlePosition+0.1f);
			}

			if( _eventOnRelease && sliderMoved )
			{
				SetStateDrawable();
				if( _eventOnRelease && _slideListener != NULL )
					_slideListener->HandleEvent(this);
				return true;
			}
		}
	}
	*/
	return false;
}

//----------------------------------------------------------------------------------

void ShivaGUI::Slider::SetHandlePosition( float _value )
{
	m_handlePosition = _value;

	if( m_handlePosition < 0.0f )
		m_handlePosition = 0.0f;
	else if( m_handlePosition > 1.0f )
		m_handlePosition = 1.0f;

	LayoutHandle();

	if( !m_eventOnRelease && m_slideListener != NULL )
		m_slideListener->HandleEvent(this);
}

//----------------------------------------------------------------------------------

int ShivaGUI::Slider::MouseHit( int _mouseX, int _mouseY )
{
	// Did it hit the thing as a whole?
	bool hitWidget = ( _mouseX > m_boundsLeft ) && ( _mouseX < m_boundsRight ) && ( _mouseY > m_boundsTop ) && ( _mouseY < m_boundsBottom );

	if( hitWidget )
	{
		// Did it hit the handle?
		if( ( _mouseX > m_handleLeft ) && ( _mouseX < m_handleRight ) && ( _mouseY > m_handleTop ) && ( _mouseY < m_handleBottom ) )
			return 1;

		// TODO: check hits on either side of handle
	}

	return 0;
}

//----------------------------------------------------------------------------------

void ShivaGUI::Slider::LayoutHandle()
{
	float widgetHeight = ( ( float )m_boundsBottom ) - ( ( float )m_boundsTop );
	float widgetWidth = ( ( float )m_boundsRight ) - ( ( float )m_boundsLeft );

	m_handleBottom = ( ( float )m_boundsBottom ) - ( widgetHeight / 2.0f ) + ( m_handleHeight / 2.0f );
	m_handleTop    = ( ( float )m_boundsBottom ) - ( widgetHeight / 2.0f ) - ( m_handleHeight / 2.0f );
	m_handleLeft   = ( ( float )m_boundsLeft ) + ( ( widgetWidth - m_handleWidth ) * m_handlePosition );
	m_handleRight  = ( ( float )m_boundsLeft ) + ( ( widgetWidth - m_handleWidth ) * m_handlePosition ) + ( m_handleWidth );
	
	if( m_handleGeneralDrawable != NULL )
		m_handleGeneralDrawable->SetBounds( m_handleLeft, m_handleTop, m_handleRight, m_handleBottom );
	if( m_handleStateListDrawable != NULL )
		m_handleStateListDrawable->SetBounds( m_handleLeft, m_handleTop, m_handleRight, m_handleBottom );
}

//----------------------------------------------------------------------------------

void ShivaGUI::Slider::Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	View::Layout( _left, _top, _right, _bottom, _windowWidth, _windowHeight );

	m_boundsLeft = _left;
	m_boundsRight = _right;
	m_boundsTop = _top;
	m_boundsBottom = _bottom;

	LayoutHandle();

	if( m_barDrawable != NULL )
		m_barDrawable->SetBounds( ( float )_left, ( float )_top, ( float )_right, ( float )_bottom );

	SetStateDrawable();
}

//----------------------------------------------------------------------------------

void ShivaGUI::Slider::SetStateDrawable()
{
	if( m_handleStateListDrawable != NULL )
	{
		if( m_active )
		{
			if( m_holdingHandle )
			{
				if( m_focussed )
				{
					m_handleStateListDrawable->SetCurrentState( StateListDrawable::Pressed | StateListDrawable::HasFocus );
				}
				else
				{
					m_handleStateListDrawable->SetCurrentState( StateListDrawable::Pressed );
				}
			}
			else
			{
				if( m_focussed )
				{
					m_handleStateListDrawable->SetCurrentState( StateListDrawable::HasFocus );
				}
				else
				{
					m_handleStateListDrawable->SetCurrentState( StateListDrawable::Active );
				}
			}
		}
		else
		{
			m_handleStateListDrawable->SetCurrentState( StateListDrawable::Disabled );
		}
	}
}

//----------------------------------------------------------------------------------