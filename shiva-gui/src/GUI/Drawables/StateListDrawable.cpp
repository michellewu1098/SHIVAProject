#include "GUI/Drawables/StateListDrawable.h"

//----------------------------------------------------------------------------------

ShivaGUI::StateListDrawable::StateListDrawable()
{
	m_defaultDrawable = NULL;
	m_currentState = Disabled;
}

//----------------------------------------------------------------------------------

ShivaGUI::StateListDrawable::~StateListDrawable()
{
	for( std::vector< std::pair< Drawable*, int > >::iterator it = m_stateDrawables.begin(); it != m_stateDrawables.end(); ++it )
		delete ( *it ).first;
	delete m_defaultDrawable;
}

//----------------------------------------------------------------------------------

void ShivaGUI::StateListDrawable::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources )
{

	//for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	//{
		// Currently no attributes to sort out
		/*
		if( currentAttribute->Name() == std::string("orientation") )
		{
			std::string orientation( currentAttribute->Value() );
			if( orientation == "horizontal" )
			{
				_verticalOrientation = false;
			}
			else if( orientation == "vertical" )
			{
				_verticalOrientation = true;
			}
			else
			{
				std::cerr<<"WARNING: LinearLayout::Inflate unknown orientation: "<<orientation<<std::endl;
			}
		}
		*/
	//}

	for( TiXmlNode *child = _xmlElement->FirstChild(); child != NULL; child = child->NextSibling() )
	{
		if( std::string( "Item" ) == child->Value() )
			ParseItem( child->ToElement(), _resources );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::StateListDrawable::ParseItem( TiXmlElement *_xmlElement, ResourceManager *_resources )
{
	Drawable *drawable = NULL;
	int states = 0;

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( currentAttribute->Name() == std::string( "drawable" ) )
		{
			std::string filename( currentAttribute->Value() );

			drawable = _resources->GetDrawable( filename );
		}
		else if( currentAttribute->Name() == std::string( "state_focus" ) )
		{
			states = states | HasFocus;
		}
		else if( currentAttribute->Name() == std::string( "state_pressed" ) )
		{
			states = states | Pressed;
		}
		else if( currentAttribute->Name() == std::string( "state_active" ) )
		{
			states = states | Active;
		}
		else if( currentAttribute->Name() == std::string( "state_toggleactive" ) )
		{
			states = states | ToggleActive;
		}
		else if( currentAttribute->Name() == std::string( "state_disabled" ) )
		{
			states = Disabled;
		}
	}

	if( drawable != NULL )
	{
		AddStateDrawable( drawable, states );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::StateListDrawable::AddStateDrawable( Drawable *_drawable, int _states )
{
	if( _states == 0 ) {
		m_defaultDrawable = _drawable;
	}
	else {
		m_stateDrawables.push_back( std::pair< Drawable*, int >( _drawable, _states ) );
	}

	_drawable->SetBounds( m_boundsLeft, m_boundsTop, m_boundsRight, m_boundsBottom );
}

//----------------------------------------------------------------------------------

ShivaGUI::Drawable* ShivaGUI::StateListDrawable::GetCurrentDrawable()
{
	// This looks through the list and uses the first drawable that matches the current state
	for( std::vector< std::pair< Drawable*, int > >::iterator it = m_stateDrawables.begin(); it != m_stateDrawables.end(); ++it )
	{
		if( ( *it ).second == m_currentState )
		{
			// We have a match
			return ( *it ).first;
		}
	}
	return m_defaultDrawable;
}

//----------------------------------------------------------------------------------

void ShivaGUI::StateListDrawable::Draw()
{
	// This looks through the list and uses the first drawable that matches the current state
	for( std::vector< std::pair< Drawable*, int > >::iterator it = m_stateDrawables.begin(); it != m_stateDrawables.end(); ++it )
	{
		if( ( *it ).second == m_currentState )
		{
			// We have a match
			( *it ).first->Draw();
			return;
		}
	}
	if( m_defaultDrawable != NULL )
		m_defaultDrawable->Draw();
}

//----------------------------------------------------------------------------------

void ShivaGUI::StateListDrawable::GetContentBounds( float &_left, float &_top, float &_right, float &_bottom )
{
	Drawable *defaultDrawable = GetDefaultDrawable();
	if( defaultDrawable != NULL ) {
		return defaultDrawable->GetContentBounds( _left, _top, _right, _bottom );
	}
}

//----------------------------------------------------------------------------------

int ShivaGUI::StateListDrawable::GetNativeWidth()
{
	Drawable *current = GetCurrentDrawable();
	if( current != NULL ) {
		return current->GetNativeWidth();
	}
	return 0;
}

//----------------------------------------------------------------------------------

int ShivaGUI::StateListDrawable::GetNativeHeight()
{
	Drawable *current = GetCurrentDrawable();
	if( current != NULL ) {
		return current->GetNativeHeight();
	}
	return 0;
}

//----------------------------------------------------------------------------------

int ShivaGUI::StateListDrawable::GetNativeWidthFromContent( int _contentWidth )
{
	Drawable *current = GetCurrentDrawable();
	if( current != NULL ) {
		return current->GetNativeWidthFromContent( _contentWidth );
	}
	return 0;
}

//----------------------------------------------------------------------------------

int ShivaGUI::StateListDrawable::GetNativeHeightFromContent( int _contentHeight )
{
	Drawable *current = GetCurrentDrawable();
	if( current != NULL ) {
		return current->GetNativeHeightFromContent( _contentHeight );
	}
	return 0;
}

//----------------------------------------------------------------------------------

void ShivaGUI::StateListDrawable::OnSetBounds( float _left, float _top, float _right, float _bottom, unsigned int _gravity )
{
	for( std::vector<std::pair< Drawable*, int > >::iterator it = m_stateDrawables.begin(); it != m_stateDrawables.end(); ++it )
	{
		( *it ).first->SetBounds( _left, _top, _right, _bottom );
	}
	if( m_defaultDrawable != NULL ) {
		m_defaultDrawable->SetBounds( _left, _top, _right, _bottom );
	}
}

//----------------------------------------------------------------------------------