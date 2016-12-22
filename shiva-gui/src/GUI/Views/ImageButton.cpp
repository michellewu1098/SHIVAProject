#include "GUI/Views/ImageButton.h"
#include "GUI/Drawables/NinePatch.h"
#include "GUI/Drawables/BitmapDrawable.h"
#include "GUIManager.h"
#include "ResourceManager.h"

//////////////////////////////////////////////////////////////////////////
#include <GL/GLee.h>
#include <iostream>
#include <cmath>

#ifndef PI
#define PI 3.141592653589793238462643383279502884
#endif

ShivaGUI::ImageButton::ImageButton()
{
	_centreX = 0;
	_centreY = 0;
	_sizeX = 0;
	_sizeY = 0;
	_stateListDrawable = NULL;
	_generalDrawable = NULL;
	_contentGenDrawable = NULL;
	_contentStateListDrawable = NULL;
	_focussed = false;
	_pressed = false;
	_active = true;
	_hover = false;
	_clickListener = NULL;
	_genDrawableFromTheme = _contentDrawableFromTheme = false;

	_audio_pressStart = NULL;
	_audio_releaseStop = false;
	_audio_hoverEnter = NULL;

	_usingEyeGaze = false;
	_hasGaze = false;
	_gazeDwellTimer = 0.0f;
	_gazeDwellToSelectTime = 3.0f;
	_gazeMaxWanderTime = 0.5f;
	_gazeWanderTimer = 0.0f;
	_gazeIsPressing = false;
	_gazePressTime = 0.5f;
	_gazeDwellCircleSize = 30.0f;
	_gazeRepeat = false;
	_gazeIsRestButton = false;
	_gazeRestButtonToggle = true;
	_gazeRestIssueEvent = false;

	_gazeRepeatFromTheme = false;

	_setContentScaleUp = _setContentAspectRatio = -1;

	_useToggleActive = false;
	_toggleActiveState = false;
}

ShivaGUI::ImageButton::~ImageButton()
{
	delete _stateListDrawable;
	delete _generalDrawable;
	delete _contentGenDrawable;
	delete _contentStateListDrawable;
	delete _audio_pressStart;
	delete _audio_hoverEnter;
}

void ShivaGUI::ImageButton::Update(float deltaTs, GUIController *guiController)
{
	if( _gazeRestIssueEvent )
	{
		InternalEvent *currentEvent = NULL;
		if( _gazeRestButtonToggle )
		{
			currentEvent = new InternalEvent( InternalEvent::EventType::EYEGAZE_ENABLE );
		}
		else
		{
			currentEvent = new InternalEvent( InternalEvent::EventType::EYEGAZE_DISABLE );
		}
		guiController->IssueEvent( currentEvent );
		delete currentEvent;
	}
	if( _usingEyeGaze )
	{
		if( _hasGaze )
		{
			_gazeDwellTimer += deltaTs;

			if( (_gazeDwellTimer > _gazeDwellToSelectTime) )
			{
				if( _gazeIsPressing )
				{
					if( _gazeDwellTimer > (_gazeDwellToSelectTime + _gazePressTime) )
					{
						// deselect the button (activates its callback on deselect)
						SetSelect(false);
						//_hasGaze = false;
						if( _gazeRepeat )
						{
							_gazeDwellTimer = 0.0f;
							_gazeIsPressing = false;
						}
					}
				}
				else
				{
					// select the button
					SetSelect(true);
					_gazeIsPressing = true;
				}
			}
		}
		else
		{
			if( _gazeWanderTimer < _gazeMaxWanderTime )
			{
				_gazeWanderTimer += deltaTs;
			}
		}
	}
}

void ShivaGUI::ImageButton::Draw()
{
	View::Draw();

	if( _visible )
	{
		if( _stateListDrawable != NULL )
			_stateListDrawable->Draw();
		else if( _generalDrawable != NULL )
			_generalDrawable->Draw();

		if( _contentStateListDrawable != NULL )
			_contentStateListDrawable->Draw();
		else if( _contentGenDrawable != NULL )
			_contentGenDrawable->Draw();

		if( _usingEyeGaze )
		{
			DrawGazeIndicator();
		}
	}
}

void ShivaGUI::ImageButton::Inflate(TiXmlElement *xmlElement, ResourceManager *resources, std::string themePrefix, bool rootNode )
{
	if( themePrefix.empty() )
		themePrefix = "ImageButton_";
	View::Inflate(xmlElement,resources,themePrefix,rootNode);

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( (std::string("drawable") == currentAttribute->Name()) || (themePrefix+"drawable" == currentAttribute->Name())  )
		{
			_genDrawableFromTheme = (themePrefix+"drawable" == currentAttribute->Name());

			std::string resourceName( resources->GetInflationAttribute( currentAttribute->Value() ) );

			// Retrieve resource and store it as correct type
			Drawable *drawable = resources->GetDrawable( resourceName );
			_stateListDrawable = dynamic_cast<StateListDrawable*>(drawable);
			if( _stateListDrawable == NULL )
				_generalDrawable = drawable;
		}
		else if( (std::string("src") == currentAttribute->Name()) || (themePrefix+"src" == currentAttribute->Name()) )
		{
			_contentDrawableFromTheme = (themePrefix+"src" == currentAttribute->Name());

			std::string resourceName( resources->GetInflationAttribute( currentAttribute->Value() ) );

			// Retrieve resource and store it as correct type
			Drawable *drawable = resources->GetDrawable( resourceName );
			SetContent(drawable);
		}
		else if( (std::string("scaleContent") == currentAttribute->Name()) || (themePrefix+"scaleContent" == currentAttribute->Name()) )
		{
			std::string value = currentAttribute->Value();
			_setContentScaleUp = (value == "true" || value == "1" || value == "yes");

			if( _contentGenDrawable != NULL )
			{
				BitmapDrawable *bitmapContent = dynamic_cast<BitmapDrawable*>(_contentGenDrawable);
				if( bitmapContent != NULL )
				{

					bitmapContent->SetScaleup( (value == "true" || value == "1" || value == "yes") );
				}
			}
		}
		else if( (std::string("keepContentAspectRatio") == currentAttribute->Name()) || (themePrefix+"keepContentAspectRatio" == currentAttribute->Name()) )
		{
			std::string value = currentAttribute->Value();
			_setContentAspectRatio = (value == "true" || value == "1" || value == "yes");

			if( _contentGenDrawable != NULL )
			{
				BitmapDrawable *bitmapContent = dynamic_cast<BitmapDrawable*>(_contentGenDrawable);
				if( bitmapContent != NULL )
				{

					bitmapContent->SetScaleKeepAspectRatio( (value == "true" || value == "1" || value == "yes") );
				}
			}
		}
		else if( std::string("onClick") == currentAttribute->Name() )
		{
			_clickListenerName = currentAttribute->Value();

			// Retrieve listener
			ViewEventListener *listener = resources->GetListener(_clickListenerName);
			if( listener != NULL )
				_clickListener = listener;
		}
		else if( (std::string("audio_pressStart") == currentAttribute->Name()) || (themePrefix+"audio_pressStart" == currentAttribute->Name()) )
		{
			std::string clipName = resources->GetInflationAttribute( currentAttribute->Value() );
			_audio_pressStart = resources->GetAudioClip(clipName);
		}
		else if( (std::string("audio_releaseStop") == currentAttribute->Name()) || (themePrefix+"audio_releaseStop" == currentAttribute->Name()) )
		{
			std::string value = currentAttribute->Value();
			if( value == "true" || value == "1" || value == "yes" )
			{
				_audio_releaseStop = true;
			}
			else
			{
				_audio_releaseStop = false;
			}
		}
		else if( (std::string("toggle_button") == currentAttribute->Name()) )
		{
			std::string value = currentAttribute->Value();
			_useToggleActive = ( value == "true" || value == "1" || value == "yes" );
		}
		else if( (std::string("audio_hoverEnter") == currentAttribute->Name()) || (themePrefix+"audio_hoverEnter" == currentAttribute->Name()) )
		{
			std::string clipName = resources->GetInflationAttribute( currentAttribute->Value() );
			_audio_hoverEnter = resources->GetAudioClip(clipName);
		}
		else if( (std::string("eyeGaze_enable") == currentAttribute->Name()) || (themePrefix+"eyeGaze_enable" == currentAttribute->Name()) )
		{
			std::string value = currentAttribute->Value();
			if( value == "true" || value == "1" || value == "yes" )
			{
				_usingEyeGaze = true;
			}
			else
			{
				_usingEyeGaze = false;
				
				if( _gazeIsRestButton )
				{
					// Hide and deactivate button if it's a rest area
					_active = _visible = false;
				}
			}
		}
		else if( (std::string("eyeGaze_DwellToSelectTime") == currentAttribute->Name()) || (themePrefix+"eyeGaze_DwellToSelectTime" == currentAttribute->Name()) )
		{
			_gazeDwellToSelectTime = (float) currentAttribute->DoubleValue();
		}
		else if( (std::string("eyeGaze_WanderTime") == currentAttribute->Name()) || (themePrefix+"eyeGaze_WanderTime" == currentAttribute->Name()) )
		{
			_gazeMaxWanderTime = (float) currentAttribute->DoubleValue();
		}
		else if( (std::string("eyeGaze_PressTime") == currentAttribute->Name()) || (themePrefix+"eyeGaze_PressTime" == currentAttribute->Name()) )
		{
			_gazePressTime = (float) currentAttribute->DoubleValue();
		}
		else if( (std::string("eyeGaze_CircleSize") == currentAttribute->Name()) || (themePrefix+"eyeGaze_CircleSize" == currentAttribute->Name()) )
		{
			_gazeDwellCircleSize = (float) currentAttribute->DoubleValue();
		}
		else if( (std::string("eyeGaze_RestButton") == currentAttribute->Name()) )
		{
			std::string value = currentAttribute->Value();
			if( value == "true" || value == "1" || value == "yes" )
			{
				_gazeIsRestButton = true;
				if( !_usingEyeGaze )
				{
					// Hide and deactivate button if it's a rest area
					_active = _visible = false;
				}
			}
			else
			{
				_gazeIsRestButton = false;
			}
		}
		else if( (std::string("eyeGaze_Repeat") == currentAttribute->Name()) || (themePrefix+"eyeGaze_Repeat" == currentAttribute->Name()) )
		{
			_gazeRepeatFromTheme = themePrefix+"eyeGaze_Repeat" == currentAttribute->Name();
			std::string value = currentAttribute->Value();
			if( value == "true" || value == "1" || value == "yes" )
			{
				_gazeRepeat = true;
			}
			else
			{
				_gazeRepeat = false;
			}
		}
		
	}
}

TiXmlElement* ShivaGUI::ImageButton::Deflate(ResourceManager *resources)
{
	TiXmlElement *xmlNode = View::Deflate(resources);
	xmlNode->SetValue("ImageButton");

	// We only need to output parameters which were *not* given to us via a theme

	if( !_genDrawableFromTheme )
	{
		std::string srcFilename;
		if( _stateListDrawable != NULL )
		{
			srcFilename = _stateListDrawable->GetFilename();
		}
		else if( _generalDrawable != NULL )
		{
			srcFilename = _generalDrawable->GetFilename();
		}

		// The drawable might have been generated rather than loaded from file, so only output if we have a filename to give
		if( !srcFilename.empty() )
		{
			xmlNode->SetAttribute("drawable", srcFilename);
		}
	}

	if( !_contentDrawableFromTheme )
	{
		std::string srcFilename;
		if( _contentStateListDrawable != NULL )
		{
			srcFilename = _contentStateListDrawable->GetFilename();
		}
		else if( _contentGenDrawable != NULL )
		{
			srcFilename = _contentGenDrawable->GetFilename();
		}

		// The source might have been generated rather than loaded from file, so only output if we have a filename to give
		if( !srcFilename.empty() )
		{
			std::cout<<"INFO: ImageButton deflate: filename contains: "<<srcFilename<<std::endl;
			xmlNode->SetAttribute("src",srcFilename );
		}
	}
	
	if( _gazeIsRestButton )
	{
		xmlNode->SetAttribute("eyeGaze_RestButton", "true");
	}

	if( _gazeRepeat && !_gazeRepeatFromTheme )
	{
		xmlNode->SetAttribute("eyeGaze_Repeat", "true");
	}

	if( !_clickListenerName.empty() )
	{
		xmlNode->SetAttribute("onClick", _clickListenerName );
	}

	if( _useToggleActive )
	{
		xmlNode->SetAttribute("toggle_button", "true" );
	}

	return xmlNode;
}

void ShivaGUI::ImageButton::SetContent( Drawable *drawable )
{
	_contentStateListDrawable = dynamic_cast<StateListDrawable*>(drawable);
	if( _contentStateListDrawable == NULL )
		_contentGenDrawable = drawable;

	BitmapDrawable *bitmapContent = dynamic_cast<BitmapDrawable*>(_contentGenDrawable);
	if( bitmapContent != NULL )
	{
		if( _setContentScaleUp >= 0 )
			bitmapContent->SetScaleup( _setContentScaleUp != 0 );
		
		if( _setContentAspectRatio >= 0 )
			bitmapContent->SetScaleKeepAspectRatio( _setContentAspectRatio != 0 );
	}
}

int ShivaGUI::ImageButton::GetWrapWidth()
{
	int contentWidth = 0;

	if( _contentStateListDrawable != NULL )
	{
		contentWidth = _contentStateListDrawable->GetNativeWidth();
	}
	else if( _contentGenDrawable != NULL )
	{
		contentWidth = _contentGenDrawable->GetNativeWidth();
	}

	//std::cout<<"INFO: ImageButton::GetWrapWidth, contentWidth = "<<contentWidth<<std::endl;

	if( _stateListDrawable != NULL )
	{
		return _stateListDrawable->GetNativeWidthFromContent( contentWidth );
	}
	else if( _generalDrawable != NULL )
	{
		return _generalDrawable->GetNativeWidthFromContent( contentWidth );
	}

	return 0;
}

int ShivaGUI::ImageButton::GetWrapHeight()
{
	int contentHeight = 0;

	if( _contentStateListDrawable != NULL )
	{
		contentHeight = _contentStateListDrawable->GetNativeHeight();
	}
	else if( _contentGenDrawable != NULL )
	{
		contentHeight = _contentGenDrawable->GetNativeHeight();
	}

	//std::cout<<"INFO: ImageButton::GetWrapHeight, contentHeight = "<<contentHeight<<std::endl;

	if( _stateListDrawable != NULL )
	{
		return _stateListDrawable->GetNativeHeightFromContent( contentHeight );
	}
	else if( _generalDrawable != NULL )
	{
		return _generalDrawable->GetNativeHeightFromContent( contentHeight );
	}

	return 0;
}

/// For setting the focus of this View
/// If focussed, it is expected that the view will show this visually
/// and then allow other forms of input to be received, e.g. 'enter' button pressed etc
void ShivaGUI::ImageButton::SetFocus(bool value)
{
	if( _focussed == true && value == false )
	{
		_pressed = false;
	}
	_focussed = value;
	SetStateDrawable();
}

void ShivaGUI::ImageButton::SetSelect(bool value)
{
	if( _pressed == false && value == true )
	{
		// Press
		if( _audio_pressStart != NULL )
		{
			_audio_pressStart->Play();
		}
		if( _useToggleActive )
		{
			_toggleActiveState = !_toggleActiveState;
		}
	}
	if( _pressed == true && value == false )
	{
		// Release
		if( _clickListener != NULL ) {
			_clickListener->HandleEvent(this);
		}

		if( _gazeIsRestButton && _usingEyeGaze )
		{
			// Need to trigger an event to toggle eye-gaze
			_gazeRestIssueEvent = true;
			_gazeRestButtonToggle = !_gazeRestButtonToggle;
		}
		
		if( _audio_pressStart != NULL && _audio_releaseStop )
		{
			_audio_pressStart->Stop();
		}
	}
	_pressed = value;
	SetStateDrawable();
}

bool ShivaGUI::ImageButton::HandleEvent(InternalEvent *currentEvent)
{
	if( _active && _visible )
	{
		if( currentEvent->GetType() == InternalEvent::EYEGAZE_ENABLE )
		{
			if( !_gazeIsRestButton )
			{
				_usingEyeGaze = true;
			}
		}
		if( currentEvent->GetType() == InternalEvent::EYEGAZE_DISABLE )
		{
			if( !_gazeIsRestButton )
			{
				_usingEyeGaze = false;
			}
		}
		if( (currentEvent->GetType() == InternalEvent::POSITIONAL_DRAG) && (!_pressed) && (!_hover) )
		{
			int mouseX,mouseY;
			currentEvent->GetPosition(mouseX,mouseY);
			if( MouseHit(mouseX,mouseY) )
			{
				// This is a hover enter
				_hover = true;
				return OnHoverEnter();
			}
		}
		if( (currentEvent->GetType() == InternalEvent::POSITIONAL_DRAG) && (!_pressed) && (_hover) )
		{
			int mouseX,mouseY;
			currentEvent->GetPosition(mouseX,mouseY);
			if( !MouseHit(mouseX,mouseY) )
			{
				// This is a hover exit
				_hover = false;
				return OnHoverExit();
			}
		}
		else if( currentEvent->GetType() == InternalEvent::POSITIONAL_SELECT )
		{
			int mouseX,mouseY;
			currentEvent->GetPosition(mouseX,mouseY);
			if( MouseHit(mouseX,mouseY) )
			{
				SetSelect(true);
				return true;
			}
		}
		else if( currentEvent->GetType() == InternalEvent::POSITIONAL_DESELECT )
		{
			//int mouseX,mouseY;
			//currentEvent->GetPosition(mouseX,mouseY);
			if( _pressed == true ) //MouseHit(mouseX,mouseY) )
			{
				SetSelect(false);
				return true;
			}
		}
		else if( currentEvent->GetType() == InternalEvent::FIRSTFOCUS && _isFirstFocus && !_focussed )
		{
			SetFocus(true);
			return true;
		}
		else if( currentEvent->GetType() == InternalEvent::NEXTFOCUS_RIGHT && _focussed )
		{
			View *nextFocus = GetNextFocus(Definitions::Right);
			if( nextFocus != NULL )
			{
				SetFocus(false);
				nextFocus->SetFocus(true);
			}
			return true;
		}
		else if( currentEvent->GetType() == InternalEvent::NEXTFOCUS_LEFT && _focussed )
		{
			View *nextFocus = GetNextFocus(Definitions::Left);
			if( nextFocus != NULL )
			{
				SetFocus(false);
				nextFocus->SetFocus(true);
			}
			return true;
		}
		else if( currentEvent->GetType() == InternalEvent::NEXTFOCUS_UP && _focussed )
		{
			View *nextFocus = GetNextFocus(Definitions::Up);
			if( nextFocus != NULL )
			{
				SetFocus(false);
				nextFocus->SetFocus(true);
			}
			return true;
		}
		else if( currentEvent->GetType() == InternalEvent::NEXTFOCUS_DOWN && _focussed )
		{
			View *nextFocus = GetNextFocus(Definitions::Down);
			if( nextFocus != NULL )
			{
				SetFocus(false);
				nextFocus->SetFocus(true);
			}
			return true;
		}
		else if( currentEvent->GetType() == InternalEvent::FOCUS_HIGHLIGHT && _focussed )
		{
			_pressed = true;
			SetStateDrawable();
			return true;
		}
		else if( currentEvent->GetType() == InternalEvent::FOCUS_SELECT_HOLD && _pressed )
		{
			if( _clickListener != NULL )
				_clickListener->HandleEvent(this);
			SetStateDrawable();
			return true;
		}
		else if( currentEvent->GetType() == InternalEvent::FOCUS_SELECT_RELEASE && _pressed )
		{
			_pressed = false;
			if( _clickListener != NULL )
				_clickListener->HandleEvent(this);
			SetStateDrawable();
			return true;
		}
		else if( currentEvent->GetType() == InternalEvent::FOCUS_DESELECT && (_pressed || _focussed) )
		{
			_pressed = false;
			_focussed = false;
			SetStateDrawable();
			return true;
		}
		else if( currentEvent->GetType() == InternalEvent::SCAN_FIRSTFOCUS && _isFirstScan && !_focussed )
		{
			SetFocus(true);
			return true;
		}
		else if( currentEvent->GetType() == InternalEvent::SCAN_NEXTFOCUS && _focussed )
		{
			View *nextFocus = GetNextScan(true);
			if( nextFocus != NULL )
			{
				SetFocus(false);
				nextFocus->SetFocus(true);
			}
			return true;
		}
		else if( currentEvent->GetType() == InternalEvent::SCAN_PREVFOCUS && _focussed )
		{
			View *nextFocus = GetNextScan(false);
			if( nextFocus != NULL )
			{
				SetFocus(false);
				nextFocus->SetFocus(true);
			}
			return true;
		}
		else if( currentEvent->GetType() == InternalEvent::SCAN_HIGHLIGHTFOCUS && _focussed )
		{
			SetSelect(true);
			return true;
		}
		else if( currentEvent->GetType() == InternalEvent::SCAN_SELECTFOCUS && _pressed )
		{
			SetSelect(false);
			return true;
		}
	}
	return false;
}


bool ShivaGUI::ImageButton::MouseHit(int mouseX, int mouseY)
{
	float halfSizeX = _sizeX/2.0f;
	float halfSizeY = _sizeY/2.0f;
	return (mouseX < _centreX+halfSizeX) && (mouseX > _centreX-halfSizeX)
		&& (mouseY < _centreY+halfSizeY) && (mouseY > _centreY-halfSizeY);
}

void ShivaGUI::ImageButton::SetStateDrawable()
{
	if( _stateListDrawable != NULL )
	{
		if( _active )
		{
			if( _pressed )
			{
				if( _focussed )
				{
					_stateListDrawable->SetCurrentState( StateListDrawable::Pressed | StateListDrawable::HasFocus );
				}
				else
				{
					_stateListDrawable->SetCurrentState(StateListDrawable::Pressed);
				}
			}
			else
			{
				if( _focussed )
				{
					_stateListDrawable->SetCurrentState(StateListDrawable::HasFocus);
				}
				else
				{
					//_stateListDrawable->SetCurrentState(StateListDrawable::Active);
					
					if( (_gazeIsRestButton && !_gazeRestButtonToggle) || _toggleActiveState )
					{
						_stateListDrawable->SetCurrentState(StateListDrawable::ToggleActive);
					}
					else
					{
						_stateListDrawable->SetCurrentState(StateListDrawable::Active);
					}
				}
			}
		}
		else
		{
			_stateListDrawable->SetCurrentState(StateListDrawable::Disabled);
		}
	}
}

void ShivaGUI::ImageButton::Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight)
{
	View::Layout(left,top,right,bottom,windowWidth,windowHeight);

	float contentBoundsLeft = (float) left, contentBoundsRight = (float) right, contentBoundsTop = (float) top, contentBoundsBottom = (float) bottom;

	_sizeX = right-left;
	_sizeY = bottom - top;

	//std::cout<<"INFO: ImageButton::Layout width = "<<_sizeX<<" height = "<<_sizeY<<std::endl;

	_centreX = (int) (left + (_sizeX/2.0f));
	_centreY = (int) (top + (_sizeY/2.0f));
	if( _generalDrawable != NULL )
	{
		_generalDrawable->SetBounds((float)left,(float)top,(float)right,(float)bottom);
		_generalDrawable->GetContentBounds(contentBoundsLeft,contentBoundsTop,contentBoundsRight,contentBoundsBottom);
	}
	else if( _stateListDrawable != NULL )
	{
		// First, set the bounds of the statelist
		_stateListDrawable->SetBounds((float)left,(float)top,(float)right,(float)bottom);
		_stateListDrawable->GetContentBounds(contentBoundsLeft,contentBoundsTop,contentBoundsRight,contentBoundsBottom);

		/*
		// The statelist will work out how big its content should be, so retrieve that:
		NinePatch *ninePatch = dynamic_cast<NinePatch*>(_stateListDrawable->GetDefaultDrawable());
		if( ninePatch != NULL )
			ninePatch->GetContentBounds(contentBoundsLeft,contentBoundsTop,contentBoundsRight,contentBoundsBottom);
		else
		{

		}
		*/
	}

	if( _contentGenDrawable != NULL )
		_contentGenDrawable->SetBounds(contentBoundsLeft,contentBoundsTop,contentBoundsRight,contentBoundsBottom);
	if( _contentStateListDrawable != NULL )
		_contentStateListDrawable->SetBounds(contentBoundsLeft,contentBoundsTop,contentBoundsRight,contentBoundsBottom);

	SetStateDrawable();
}


bool ShivaGUI::ImageButton::OnHoverEnter()
{
	//std::cout<<"image button hover enter"<<std::endl;
	if( _audio_hoverEnter != NULL )
		_audio_hoverEnter->Play();
	if( _usingEyeGaze )
	{
		if( _gazeWanderTimer >= _gazeMaxWanderTime )
		{
			_gazeDwellTimer = 0.0f;
			_gazeIsPressing = false;
		}
		_gazeWanderTimer = 0.0f;
		_hasGaze = true;
	}
	return false;
}

bool ShivaGUI::ImageButton::OnHoverExit()
{
	//std::cout<<"image button hover exit"<<std::endl;
	if( _usingEyeGaze )
	{
		_hasGaze = false;
		//_gazeWanderTimer = 0.0f;
	}
	return false;
}

void ShivaGUI::ImageButton::DrawGazeIndicator()
{
	if( _hasGaze )
	{
		glBegin(GL_POLYGON);
			glColor4f(0.0f,1.0f,0.0f,1.0f);

			float centreX = ((float)_layoutBoundsLeft+_layoutBoundsRight)/2.0f;
			float centreY = ((float)_layoutBoundsTop+_layoutBoundsBottom)/2.0f;

			glVertex2f( centreX , centreY );

			for( float i = 0; i < min(1.0f,(_gazeDwellTimer/_gazeDwellToSelectTime)); i+=0.01f )
			{
				float angle = 2.0f * (float)PI * i;
				glVertex2f( centreX + (cos(angle)*_gazeDwellCircleSize), centreY + (sin(angle)*_gazeDwellCircleSize) );
			}

		glEnd();
	}
}
