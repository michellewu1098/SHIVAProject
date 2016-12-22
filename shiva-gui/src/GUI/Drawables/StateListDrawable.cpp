#include "GUI/Drawables/StateListDrawable.h"

//----------------------------------------------------------------------------------

ShivaGUI::StateListDrawable::StateListDrawable()
{
	_defaultDrawable = NULL;
	_currentState = Disabled;
}

//----------------------------------------------------------------------------------

ShivaGUI::StateListDrawable::~StateListDrawable()
{
	for( std::vector< std::pair< Drawable*, int > >::iterator it = _stateDrawables.begin(); it != _stateDrawables.end(); ++it )
		delete ( *it ).first;
	delete _defaultDrawable;
}

//----------------------------------------------------------------------------------

void ShivaGUI::StateListDrawable::Inflate( TiXmlElement *xmlElement, ResourceManager *resources )
{

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
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
	}

	for( TiXmlNode *child = xmlElement->FirstChild(); child != NULL; child = child->NextSibling() )
	{
		if( std::string( "Item" ) == child->Value() )
			ParseItem( child->ToElement(), resources );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::StateListDrawable::ParseItem( TiXmlElement *xmlElement, ResourceManager *resources )
{
	Drawable *drawable = NULL;
	int states = 0;

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( currentAttribute->Name() == std::string( "drawable" ) )
		{
			std::string filename( currentAttribute->Value() );

			drawable = resources->GetDrawable( filename );
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

void ShivaGUI::StateListDrawable::AddStateDrawable( Drawable *drawable, int states )
{
	if( states == 0 ) {
		_defaultDrawable = drawable;
	}
	else {
		_stateDrawables.push_back( std::pair< Drawable*, int >( drawable, states ) );
	}

	drawable->SetBounds( _boundsLeft, _boundsTop, _boundsRight, _boundsBottom );
}

//----------------------------------------------------------------------------------

ShivaGUI::Drawable* ShivaGUI::StateListDrawable::GetCurrentDrawable()
{
	// This looks through the list and uses the first drawable that matches the current state
	for( std::vector< std::pair< Drawable*, int > >::iterator it = _stateDrawables.begin(); it != _stateDrawables.end(); ++it )
	{
		if( ( *it ).second == _currentState )
		{
			// We have a match
			return ( *it ).first;
		}
	}
	return _defaultDrawable;
}

//----------------------------------------------------------------------------------

void ShivaGUI::StateListDrawable::Draw()
{
	// This looks through the list and uses the first drawable that matches the current state
	for( std::vector< std::pair< Drawable*, int > >::iterator it = _stateDrawables.begin(); it != _stateDrawables.end(); ++it )
	{
		if( ( *it ).second == _currentState )
		{
			// We have a match
			( *it ).first->Draw();
			return;
		}
	}
	if( _defaultDrawable != NULL )
		_defaultDrawable->Draw();
}

//----------------------------------------------------------------------------------

void ShivaGUI::StateListDrawable::GetContentBounds( float &left, float &top, float &right, float &bottom )
{
	Drawable *defaultDrawable = GetDefaultDrawable();
	if( defaultDrawable != NULL ) {
		return defaultDrawable->GetContentBounds( left, top, right, bottom );
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

int ShivaGUI::StateListDrawable::GetNativeWidthFromContent( int contentWidth )
{
	Drawable *current = GetCurrentDrawable();
	if( current != NULL ) {
		return current->GetNativeWidthFromContent( contentWidth );
	}
	return 0;
}

//----------------------------------------------------------------------------------

int ShivaGUI::StateListDrawable::GetNativeHeightFromContent( int contentHeight )
{
	Drawable *current = GetCurrentDrawable();
	if( current != NULL ) {
		return current->GetNativeHeightFromContent( contentHeight );
	}
	return 0;
}

//----------------------------------------------------------------------------------

void ShivaGUI::StateListDrawable::OnSetBounds( float left, float top, float right, float bottom, unsigned int gravity )
{
	for( std::vector<std::pair<Drawable*,int> >::iterator it = _stateDrawables.begin(); it != _stateDrawables.end(); ++it )
	{
		( *it ).first->SetBounds( left, top, right, bottom );
	}
	if( _defaultDrawable != NULL ) {
		_defaultDrawable->SetBounds( left, top, right, bottom );
	}
}

//----------------------------------------------------------------------------------