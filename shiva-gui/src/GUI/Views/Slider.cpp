#include "GUI/Views/Slider.h"
#include "GUIManager.h"
#include "ResourceManager.h"

//////////////////////////////////////////////////////////////////////////
#include <GL/GLee.h>
#include <iostream>


ShivaGUI::Slider::Slider()
{
	_handlePosition = 1.0f;
	_barDrawable = NULL;
	_handleGeneralDrawable = NULL;
	_handleStateListDrawable = NULL;
	_handleHeight = 100.0f;
	_handleWidth = 100.0f;
	_holdingHandle = false;
	_focussed = false;
	_active = true;
	_slideListener = NULL;
	_eventOnRelease = false;
	_barDrawableFromTheme = _handleDrawableFromTheme = _handleHeightFromTheme = _handleWidthFromTheme = false;
}

ShivaGUI::Slider::~Slider()
{
	delete _barDrawable;
	delete _handleGeneralDrawable;
	delete _handleStateListDrawable;
}

void ShivaGUI::Slider::Draw()
{
	View::Draw();

	if( _visible )
	{
		if( _barDrawable != NULL )
			_barDrawable->Draw();

		if( _handleStateListDrawable != NULL )
			_handleStateListDrawable->Draw();
		else if( _handleGeneralDrawable != NULL )
			_handleGeneralDrawable->Draw();
	}
}

void ShivaGUI::Slider::Inflate(TiXmlElement *xmlElement, ResourceManager *resources, std::string themePrefix, bool rootNode )
{
	if( themePrefix.empty() )
		themePrefix = "Slider_";
	View::Inflate(xmlElement,resources,themePrefix,rootNode);

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{

		if( (std::string("barDrawable") == currentAttribute->Name()) || (themePrefix+"barDrawable" == currentAttribute->Name()) )
		{
			_barDrawableFromTheme = (themePrefix+"barDrawable" == currentAttribute->Name());
			std::string resourceName( currentAttribute->Value() );

			// Retrieve resource and store it as correct type
			_barDrawable = resources->GetDrawable( resourceName );
		}
		else if( (std::string("handleDrawable") == currentAttribute->Name()) || (themePrefix+"handleDrawable" == currentAttribute->Name()) )
		{
			_handleDrawableFromTheme = (themePrefix+"handleDrawable" == currentAttribute->Name());
			std::string resourceName( currentAttribute->Value() );

			// Retrieve resource and store it as correct type
			Drawable *drawable = resources->GetDrawable( resourceName );
			_handleStateListDrawable = dynamic_cast<StateListDrawable*>(drawable);
			if( _handleStateListDrawable == NULL )
				_handleGeneralDrawable = drawable;
		}
		else if( (std::string("handleHeight") == currentAttribute->Name()) || (themePrefix+"handleHeight" == currentAttribute->Name()) )
		{
			_handleHeightFromTheme = (themePrefix+"handleHeight" == currentAttribute->Name());
			_handleHeight = (float) currentAttribute->DoubleValue();
		}
		else if( (std::string("handleWidth") == currentAttribute->Name()) || (themePrefix+"handleWidth" == currentAttribute->Name()) )
		{
			_handleWidthFromTheme = (themePrefix+"handleWidth" == currentAttribute->Name());
			_handleWidth = (float) currentAttribute->DoubleValue();
		}
		else if( std::string("handlePosition") == currentAttribute->Name() )
		{
			_handlePosition = (float) currentAttribute->DoubleValue();
			if( _handlePosition < 0.0f )
				_handlePosition = 0.0f;
			else if( _handlePosition > 1.0f)
				_handlePosition = 1.0f;
		}
		else if( std::string("onSlide") == currentAttribute->Name() )
		{
			_slideListenerName = currentAttribute->Value();

			// Retrieve listener
			ViewEventListener *listener = resources->GetListener(_slideListenerName);
			if( listener != NULL )
				_slideListener = listener;
		}
		else if( std::string("eventOnRelease") == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				_eventOnRelease = true;
			else if( value == "false" )
				_eventOnRelease = false;
		}

	}
}

TiXmlElement* ShivaGUI::Slider::Deflate(ResourceManager *resources)
{
	TiXmlElement *xmlNode = View::Deflate(resources);
	xmlNode->SetValue("Slider");


	if( !_barDrawableFromTheme )
	{
		if( _barDrawable != NULL )
			xmlNode->SetAttribute("barDrawable",_barDrawable->GetFilename() );
	}

	if( !_handleDrawableFromTheme )
	{
		if( _handleStateListDrawable != NULL )
			xmlNode->SetAttribute("handleDrawable",_handleStateListDrawable->GetFilename() );
		else if( _handleGeneralDrawable != NULL )
			xmlNode->SetAttribute("handleDrawable",_handleGeneralDrawable->GetFilename() );
	}

	if( !_handleHeightFromTheme )
		xmlNode->SetDoubleAttribute("handleHeight", _handleHeight );
	if( !_handleWidthFromTheme )
		xmlNode->SetDoubleAttribute("handleWidth", _handleWidth );
	xmlNode->SetDoubleAttribute("handlePosition", _handlePosition );

	if( !_slideListenerName.empty() )
		xmlNode->SetAttribute("onSlide", _slideListenerName );

	if( _eventOnRelease )
		xmlNode->SetAttribute("eventOnRelease","true");


	return xmlNode;
}


void ShivaGUI::Slider::SetFocus(bool value)
{
	if( _active )
		_focussed = value;
	SetStateDrawable();
}

bool ShivaGUI::Slider::HandleEvent(InternalEvent *currentEvent)
{
	if( !_active || !_visible )
		return false;

	if( currentEvent->GetType() == InternalEvent::POSITIONAL_SELECT )
	{
		int mouseX,mouseY;
		currentEvent->GetPosition(mouseX,mouseY);
		if( MouseHit(mouseX,mouseY) == 1 )
		{
			//SetSelect(true);
			_holdingHandle = true;
			SetStateDrawable();
			return true;
		}
	}
	else if( currentEvent->GetType() == InternalEvent::POSITIONAL_DESELECT )
	{
		int mouseX,mouseY;
		currentEvent->GetPosition(mouseX,mouseY);
		if( _holdingHandle ) //MouseHit(mouseX,mouseY) )
		{
			//SetSelect(false);
			_holdingHandle = false;
			SetStateDrawable();
			if( _eventOnRelease && _slideListener != NULL )
				_slideListener->HandleEvent(this);

			return true;
		}
	}
	else if( currentEvent->GetType() == InternalEvent::POSITIONAL_DRAG )
	{
		if( _holdingHandle )
		{
			int mouseX,mouseY;
			currentEvent->GetPosition(mouseX,mouseY);

			// Translate this and set it as the new position of the handle
			float widgetWidth = ((float)_boundsRight) - ((float)_boundsLeft) - _handleWidth;

			float relativeMouseX = mouseX - _boundsLeft - (_handleWidth/2.0f);
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

void ShivaGUI::Slider::SetHandlePosition( float value )
{
	_handlePosition = value;

	if( _handlePosition < 0.0f )
		_handlePosition = 0.0f;
	else if( _handlePosition > 1.0f )
		_handlePosition = 1.0f;

	LayoutHandle();
	if( !_eventOnRelease && _slideListener != NULL )
		_slideListener->HandleEvent(this);
}

/// \return 0 on no hit, 1 on hit handle, 2 on hit bar left of handle, 3 on hit right bar
int ShivaGUI::Slider::MouseHit(int mouseX, int mouseY)
{
	// Did it hit the thing as a whole?
	bool hitWidget = (mouseX>_boundsLeft) && (mouseX<_boundsRight) && (mouseY>_boundsTop) && (mouseY<_boundsBottom);

	if( hitWidget )
	{
		// Did it hit the handle?
		if( (mouseX>_handleLeft) && (mouseX<_handleRight) && (mouseY>_handleTop) && (mouseY<_handleBottom) )
			return 1;

		// TODO: check hits on either side of handle
	}

	return 0;
}

void ShivaGUI::Slider::LayoutHandle()
{
	float widgetHeight = ((float)_boundsBottom) - ((float)_boundsTop);
	float widgetWidth = ((float)_boundsRight) - ((float)_boundsLeft);

	_handleBottom = ((float)_boundsBottom) - (widgetHeight/2.0f) + (_handleHeight/2.0f);
	_handleTop    = ((float)_boundsBottom) - (widgetHeight/2.0f) - (_handleHeight/2.0f);
	_handleLeft   = ((float)_boundsLeft) + ((widgetWidth - _handleWidth) * _handlePosition) ;
	_handleRight   = ((float)_boundsLeft) + ((widgetWidth - _handleWidth) * _handlePosition) + (_handleWidth);
	//_handleLeft   = ((float)_boundsLeft) + (widgetWidth * _barPosition) - (_handleWidth/2.0f);
	//_handleRight   = ((float)_boundsLeft) + (widgetWidth * _barPosition) + (_handleWidth/2.0f);

	if( _handleGeneralDrawable != NULL )
		_handleGeneralDrawable->SetBounds(_handleLeft,_handleTop,_handleRight,_handleBottom);
	if( _handleStateListDrawable != NULL )
		_handleStateListDrawable->SetBounds(_handleLeft,_handleTop,_handleRight,_handleBottom);
}

void ShivaGUI::Slider::Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight)
{
	View::Layout(left,top,right,bottom,windowWidth,windowHeight);

	_boundsLeft = left;
	_boundsRight = right;
	_boundsTop = top;
	_boundsBottom = bottom;

	LayoutHandle();

	if( _barDrawable != NULL )
		_barDrawable->SetBounds(left,top,right,bottom);

	SetStateDrawable();
}

void ShivaGUI::Slider::SetStateDrawable()
{
	if( _handleStateListDrawable != NULL )
	{
		if( _active )
		{
			if( _holdingHandle )
			{
				if( _focussed )
				{
					_handleStateListDrawable->SetCurrentState( StateListDrawable::Pressed | StateListDrawable::HasFocus );
				}
				else
				{
					_handleStateListDrawable->SetCurrentState(StateListDrawable::Pressed);
				}
			}
			else
			{
				if( _focussed )
				{
					_handleStateListDrawable->SetCurrentState(StateListDrawable::HasFocus);
				}
				else
				{
					_handleStateListDrawable->SetCurrentState(StateListDrawable::Active);
				}
			}
		}
		else
		{
			_handleStateListDrawable->SetCurrentState(StateListDrawable::Disabled);
		}
	}
}
