#include "GUI/Views/ImageButton.h"
#include "GUI/Drawables/NinePatch.h"
#include "GUI/Drawables/BitmapDrawable.h"
#include "GUIManager.h"
#include "ResourceManager.h"

#include <cmath>

#ifndef PI
#define PI 3.141592653589793238462643383279502884
#endif

//----------------------------------------------------------------------------------

ShivaGUI::ImageButton::ImageButton()
{
	m_centreX = 0;
	m_centreY = 0;
	m_sizeX = 0;
	m_sizeY = 0;
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

	m_audio_pressStart = NULL;
	m_audio_releaseStop = false;
	m_audio_hoverEnter = NULL;

	m_usingEyeGaze = false;
	m_hasGaze = false;
	m_gazeDwellTimer = 0.0f;
	m_gazeDwellToSelectTime = 3.0f;
	m_gazeMaxWanderTime = 0.5f;
	m_gazeWanderTimer = 0.0f;
	m_gazeIsPressing = false;
	m_gazePressTime = 0.5f;
	m_gazeDwellCircleSize = 30.0f;
	m_gazeRepeat = false;
	m_gazeIsRestButton = false;
	m_gazeRestButtonToggle = true;
	m_gazeRestIssueEvent = false;

	m_gazeRepeatFromTheme = false;

	m_setContentScaleUp = m_setContentAspectRatio = -1;

	m_useToggleActive = false;
	m_toggleActiveState = false;

	glGenVertexArrays( 1, &m_gazeIndicatorVAO );

	m_gazeIndicatorShader = new Utility::GPUProgram();
	m_gazeIndicatorShader->Create( "Resources/Shaders/Crosshair", Utility::GPUProgram::VERTEX_AND_FRAGMENT );

	m_nVertices = 0;

}

//----------------------------------------------------------------------------------

ShivaGUI::ImageButton::~ImageButton()
{
	delete m_stateListDrawable;
	delete m_generalDrawable;
	delete m_contentGenDrawable;
	delete m_contentStateListDrawable;
	delete m_audio_pressStart;
	delete m_audio_hoverEnter;
	delete m_gazeIndicatorShader;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageButton::Update( float _deltaTs, GUIController *_guiController )
{
	if( m_gazeRestIssueEvent )
	{
		InternalEvent *currentEvent = NULL;
		if( m_gazeRestButtonToggle )
		{
			currentEvent = new InternalEvent( InternalEvent::EYEGAZE_ENABLE );
		}
		else
		{
			currentEvent = new InternalEvent( InternalEvent::EYEGAZE_DISABLE );
		}
		_guiController->IssueEvent( currentEvent );
		delete currentEvent;
	}
	if( m_usingEyeGaze )
	{
		if( m_hasGaze )
		{
			m_gazeDwellTimer += _deltaTs;

			if( ( m_gazeDwellTimer > m_gazeDwellToSelectTime ) )
			{
				if( m_gazeIsPressing )
				{
					if( m_gazeDwellTimer > ( m_gazeDwellToSelectTime + m_gazePressTime ) )
					{
						// deselect the button (activates its callback on deselect)
						SetSelect( false );
						//_hasGaze = false;
						if( m_gazeRepeat )
						{
							m_gazeDwellTimer = 0.0f;
							m_gazeIsPressing = false;
						}
					}
				}
				else
				{
					// select the button
					SetSelect( true );
					m_gazeIsPressing = true;
				}
			}
		}
		else
		{
			if( m_gazeWanderTimer < m_gazeMaxWanderTime )
			{
				m_gazeWanderTimer += _deltaTs;
			}
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageButton::Draw()
{
	View::Draw();

	if( m_visible )
	{
		if( m_stateListDrawable != NULL )
			m_stateListDrawable->Draw();
		else if( m_generalDrawable != NULL )
			m_generalDrawable->Draw();

		if( m_contentStateListDrawable != NULL )
			m_contentStateListDrawable->Draw();
		else if( m_contentGenDrawable != NULL )
			m_contentGenDrawable->Draw();

		if( m_usingEyeGaze )
		{
			DrawGazeIndicator();
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageButton::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "ImageButton_";
	View::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( ( std::string( "drawable" ) == currentAttribute->Name() ) || ( _themePrefix + "drawable" == currentAttribute->Name() )  )
		{
			m_genDrawableFromTheme = ( _themePrefix + "drawable" == currentAttribute->Name() );

			std::string resourceName( _resources->GetInflationAttribute( currentAttribute->Value() ) );

			// Retrieve resource and store it as correct type
			Drawable *drawable = _resources->GetDrawable( resourceName );
			m_stateListDrawable = dynamic_cast< StateListDrawable* >( drawable );
			if( m_stateListDrawable == NULL )
				m_generalDrawable = drawable;
		}
		else if( ( std::string( "src" ) == currentAttribute->Name() ) || ( _themePrefix + "src" == currentAttribute->Name() ) )
		{
			m_contentDrawableFromTheme = ( _themePrefix + "src" == currentAttribute->Name() );

			std::string resourceName( _resources->GetInflationAttribute( currentAttribute->Value() ) );

			//// Retrieve resource and store it as correct type
			Drawable *drawable = _resources->GetDrawable( resourceName );
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
		else if( std::string( "onClick" ) == currentAttribute->Name() )
		{
			m_clickListenerName = currentAttribute->Value();

			// Retrieve listener
			ViewEventListener *listener = _resources->GetListener( m_clickListenerName );
			if( listener != NULL )
				m_clickListener = listener;
		}
		else if( ( std::string( "audio_pressStart" ) == currentAttribute->Name() ) || ( _themePrefix + "audio_pressStart" == currentAttribute->Name() ) )
		{
			std::string clipName = _resources->GetInflationAttribute( currentAttribute->Value() );
			m_audio_pressStart = _resources->GetAudioClip( clipName );
		}
		else if( ( std::string( "audio_releaseStop" ) == currentAttribute->Name() ) || ( _themePrefix + "audio_releaseStop" == currentAttribute->Name() ) )
		{
			std::string value = currentAttribute->Value();
			if( value == "true" || value == "1" || value == "yes" )
			{
				m_audio_releaseStop = true;
			}
			else
			{
				m_audio_releaseStop = false;
			}
		}
		else if( ( std::string( "toggle_button" ) == currentAttribute->Name() ) )
		{
			std::string value = currentAttribute->Value();
			m_useToggleActive = ( value == "true" || value == "1" || value == "yes" );
		}
		else if( ( std::string( "audio_hoverEnter" ) == currentAttribute->Name() ) || ( _themePrefix + "audio_hoverEnter" == currentAttribute->Name() ) )
		{
			std::string clipName = _resources->GetInflationAttribute( currentAttribute->Value() );
			m_audio_hoverEnter = _resources->GetAudioClip( clipName );
		}
		else if( ( std::string( "eyeGaze_enable" ) == currentAttribute->Name() ) || ( _themePrefix + "eyeGaze_enable" == currentAttribute->Name() ) )
		{
			std::string value = currentAttribute->Value();
			if( value == "true" || value == "1" || value == "yes" )
			{
				m_usingEyeGaze = true;
			}
			else
			{
				m_usingEyeGaze = false;
				
				if( m_gazeIsRestButton )
				{
					// Hide and deactivate button if it's a rest area
					m_active = m_visible = false;
				}
			}
		}
		else if( ( std::string( "eyeGaze_DwellToSelectTime" ) == currentAttribute->Name() ) || ( _themePrefix + "eyeGaze_DwellToSelectTime" == currentAttribute->Name() ) )
		{
			m_gazeDwellToSelectTime = ( float )currentAttribute->DoubleValue();
		}
		else if( ( std::string( "eyeGaze_WanderTime" ) == currentAttribute->Name() ) || ( _themePrefix + "eyeGaze_WanderTime" == currentAttribute->Name() ) )
		{
			m_gazeMaxWanderTime = ( float ) currentAttribute->DoubleValue();
		}
		else if( ( std::string( "eyeGaze_PressTime" ) == currentAttribute->Name() ) || ( _themePrefix + "eyeGaze_PressTime" == currentAttribute->Name() ) )
		{
			m_gazePressTime = (float) currentAttribute->DoubleValue();
		}
		else if( ( std::string( "eyeGaze_CircleSize" ) == currentAttribute->Name() ) || ( _themePrefix + "eyeGaze_CircleSize" == currentAttribute->Name() ) )
		{
			m_gazeDwellCircleSize = ( float ) currentAttribute->DoubleValue();
		}
		else if( ( std::string( "eyeGaze_RestButton" ) == currentAttribute->Name() ) )
		{
			std::string value = currentAttribute->Value();
			if( value == "true" || value == "1" || value == "yes" )
			{
				m_gazeIsRestButton = true;
				if( !m_usingEyeGaze )
				{
					// Hide and deactivate button if it's a rest area
					m_active = m_visible = false;
				}
			}
			else
			{
				m_gazeIsRestButton = false;
			}
		}
		else if( ( std::string( "eyeGaze_Repeat" ) == currentAttribute->Name() ) || ( _themePrefix + "eyeGaze_Repeat" == currentAttribute->Name() ) )
		{
			m_gazeRepeatFromTheme = _themePrefix + "eyeGaze_Repeat" == currentAttribute->Name();
			std::string value = currentAttribute->Value();
			if( value == "true" || value == "1" || value == "yes" )
			{
				m_gazeRepeat = true;
			}
			else
			{
				m_gazeRepeat = false;
			}
		}
		
	}
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::ImageButton::Deflate( ResourceManager *_resources )
{
	TiXmlElement *xmlNode = View::Deflate( _resources );
	xmlNode->SetValue( "ImageButton" );

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
			std::cout << "INFO: ImageButton deflate: filename contains: " << srcFilename << std::endl;
			xmlNode->SetAttribute( "src", srcFilename );
		}
	}
	
	if( m_gazeIsRestButton )
	{
		xmlNode->SetAttribute( "eyeGaze_RestButton", "true" );
	}

	if( m_gazeRepeat && !m_gazeRepeatFromTheme )
	{
		xmlNode->SetAttribute( "eyeGaze_Repeat", "true" );
	}

	if( !m_clickListenerName.empty() )
	{
		xmlNode->SetAttribute( "onClick", m_clickListenerName );
	}

	if( m_useToggleActive )
	{
		xmlNode->SetAttribute( "toggle_button", "true" );
	}

	return xmlNode;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageButton::SetContent( Drawable *_drawable )
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
}

//----------------------------------------------------------------------------------

int ShivaGUI::ImageButton::GetWrapWidth()
{
	int contentWidth = 0;

	if( m_contentStateListDrawable != NULL )
	{
		contentWidth = m_contentStateListDrawable->GetNativeWidth();
	}
	else if( m_contentGenDrawable != NULL )
	{
		contentWidth = m_contentGenDrawable->GetNativeWidth();
	}

	//std::cout<<"INFO: ImageButton::GetWrapWidth, contentWidth = "<<contentWidth<<std::endl;

	if( m_stateListDrawable != NULL )
	{
		return m_stateListDrawable->GetNativeWidthFromContent( contentWidth );
	}
	else if( m_generalDrawable != NULL )
	{
		return m_generalDrawable->GetNativeWidthFromContent( contentWidth );
	}

	return 0;
}

//----------------------------------------------------------------------------------

int ShivaGUI::ImageButton::GetWrapHeight()
{
	int contentHeight = 0;

	if( m_contentStateListDrawable != NULL )
	{
		contentHeight = m_contentStateListDrawable->GetNativeHeight();
	}
	else if( m_contentGenDrawable != NULL )
	{
		contentHeight = m_contentGenDrawable->GetNativeHeight();
	}

	//std::cout<<"INFO: ImageButton::GetWrapHeight, contentHeight = "<<contentHeight<<std::endl;

	if( m_stateListDrawable != NULL )
	{
		return m_stateListDrawable->GetNativeHeightFromContent( contentHeight );
	}
	else if( m_generalDrawable != NULL )
	{
		return m_generalDrawable->GetNativeHeightFromContent( contentHeight );
	}

	return 0;
}

//----------------------------------------------------------------------------------

/// For setting the focus of this View
/// If focussed, it is expected that the view will show this visually
/// and then allow other forms of input to be received, e.g. 'enter' button pressed etc

void ShivaGUI::ImageButton::SetFocus( bool _value )
{
	if( m_focussed == true && _value == false )
	{
		m_pressed = false;
	}
	m_focussed = _value;
	SetStateDrawable();
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageButton::SetSelect( bool _value )
{
	if( m_pressed == false && _value == true )
	{
		// Press
		if( m_audio_pressStart != NULL )
		{
			m_audio_pressStart->Play();
		}
		if( m_useToggleActive )
		{
			m_toggleActiveState = !m_toggleActiveState;
		}
	}
	if( m_pressed == true && _value == false )
	{
		// Release
		if( m_clickListener != NULL ) {
			m_clickListener->HandleEvent( this );
		}

		if( m_gazeIsRestButton && m_usingEyeGaze )
		{
			// Need to trigger an event to toggle eye-gaze
			m_gazeRestIssueEvent = true;
			m_gazeRestButtonToggle = !m_gazeRestButtonToggle;
		}
		
		if( m_audio_pressStart != NULL && m_audio_releaseStop )
		{
			m_audio_pressStart->Stop();
		}
	}
	m_pressed = _value;
	SetStateDrawable();
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ImageButton::HandleEvent( InternalEvent *_currentEvent )
{
	if( m_active && m_visible )
	{
		if( _currentEvent->GetType() == InternalEvent::EYEGAZE_ENABLE )
		{
			if( !m_gazeIsRestButton )
			{
				m_usingEyeGaze = true;
			}
		}
		if( _currentEvent->GetType() == InternalEvent::EYEGAZE_DISABLE )
		{
			if( !m_gazeIsRestButton )
			{
				m_usingEyeGaze = false;
			}
		}
		if( ( _currentEvent->GetType() == InternalEvent::POSITIONAL_DRAG ) && ( !m_pressed ) && ( !m_hover ) )
		{
			int mouseX, mouseY;
			_currentEvent->GetPosition( mouseX, mouseY );
			if( MouseHit( mouseX, mouseY ) )
			{
				// This is a hover enter
				m_hover = true;
				return OnHoverEnter();
			}
		}
		if( ( _currentEvent->GetType() == InternalEvent::POSITIONAL_DRAG ) && ( !m_pressed ) && ( m_hover ) )
		{
			int mouseX, mouseY;
			_currentEvent->GetPosition( mouseX, mouseY );
			if( !MouseHit( mouseX, mouseY ) )
			{
				// This is a hover exit
				m_hover = false;
				return OnHoverExit();
			}
		}
		else if( _currentEvent->GetType() == InternalEvent::POSITIONAL_SELECT )
		{
			int mouseX, mouseY;
			_currentEvent->GetPosition( mouseX, mouseY );
			if( MouseHit( mouseX, mouseY ) )
			{
				SetSelect( true );
				return true;
			}
		}
		else if( _currentEvent->GetType() == InternalEvent::POSITIONAL_DESELECT )
		{
			//int mouseX,mouseY;
			//_currentEvent->GetPosition(mouseX,mouseY);
			if( m_pressed == true ) //MouseHit(mouseX,mouseY) )
			{
				SetSelect( false );
				return true;
			}
		}
		else if( _currentEvent->GetType() == InternalEvent::FIRSTFOCUS && m_isFirstFocus && !m_focussed )
		{
			SetFocus( true );
			return true;
		}
		else if( _currentEvent->GetType() == InternalEvent::NEXTFOCUS_RIGHT && m_focussed )
		{
			View *nextFocus = GetNextFocus( Definitions::Right );
			if( nextFocus != NULL )
			{
				SetFocus( false );
				nextFocus->SetFocus( true );
			}
			return true;
		}
		else if( _currentEvent->GetType() == InternalEvent::NEXTFOCUS_LEFT && m_focussed )
		{
			View *nextFocus = GetNextFocus( Definitions::Left );
			if( nextFocus != NULL )
			{
				SetFocus( false );
				nextFocus->SetFocus( true );
			}
			return true;
		}
		else if( _currentEvent->GetType() == InternalEvent::NEXTFOCUS_UP && m_focussed )
		{
			View *nextFocus = GetNextFocus( Definitions::Up );
			if( nextFocus != NULL )
			{
				SetFocus( false );
				nextFocus->SetFocus( true );
			}
			return true;
		}
		else if( _currentEvent->GetType() == InternalEvent::NEXTFOCUS_DOWN && m_focussed )
		{
			View *nextFocus = GetNextFocus( Definitions::Down );
			if( nextFocus != NULL )
			{
				SetFocus( false );
				nextFocus->SetFocus( true );
			}
			return true;
		}
		else if( _currentEvent->GetType() == InternalEvent::FOCUS_HIGHLIGHT && m_focussed )
		{
			m_pressed = true;
			SetStateDrawable();
			return true;
		}
		else if( _currentEvent->GetType() == InternalEvent::FOCUS_SELECT_HOLD && m_pressed )
		{
			if( m_clickListener != NULL )
				m_clickListener->HandleEvent( this );
			SetStateDrawable();
			return true;
		}
		else if( _currentEvent->GetType() == InternalEvent::FOCUS_SELECT_RELEASE && m_pressed )
		{
			m_pressed = false;
			if( m_clickListener != NULL )
				m_clickListener->HandleEvent( this );
			SetStateDrawable();
			return true;
		}
		else if( _currentEvent->GetType() == InternalEvent::FOCUS_DESELECT && ( m_pressed || m_focussed ) )
		{
			m_pressed = false;
			m_focussed = false;
			SetStateDrawable();
			return true;
		}
		else if( _currentEvent->GetType() == InternalEvent::SCAN_FIRSTFOCUS && m_isFirstScan && !m_focussed )
		{
			SetFocus( true );
			return true;
		}
		else if( _currentEvent->GetType() == InternalEvent::SCAN_NEXTFOCUS && m_focussed )
		{
			View *nextFocus = GetNextScan( true );
			if( nextFocus != NULL )
			{
				SetFocus( false );
				nextFocus->SetFocus( true );
			}
			return true;
		}
		else if( _currentEvent->GetType() == InternalEvent::SCAN_PREVFOCUS && m_focussed )
		{
			View *nextFocus = GetNextScan( false );
			if( nextFocus != NULL )
			{
				SetFocus( false );
				nextFocus->SetFocus( true );
			}
			return true;
		}
		else if( _currentEvent->GetType() == InternalEvent::SCAN_HIGHLIGHTFOCUS && m_focussed )
		{
			SetSelect( true );
			return true;
		}
		else if( _currentEvent->GetType() == InternalEvent::SCAN_SELECTFOCUS && m_pressed )
		{
			SetSelect( false );
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ImageButton::MouseHit( int _mouseX, int _mouseY )
{
	float halfSizeX = m_sizeX / 2.0f;
	float halfSizeY = m_sizeY / 2.0f;
	return ( _mouseX < m_centreX + halfSizeX ) && ( _mouseX > m_centreX - halfSizeX )
		&& ( _mouseY < m_centreY + halfSizeY ) && ( _mouseY > m_centreY - halfSizeY );
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageButton::SetStateDrawable()
{
	if( m_stateListDrawable != NULL )
	{
		if( m_active )
		{
			if( m_pressed )
			{
				if( m_focussed )
				{
					m_stateListDrawable->SetCurrentState( StateListDrawable::Pressed | StateListDrawable::HasFocus );
				}
				else
				{
					m_stateListDrawable->SetCurrentState( StateListDrawable::Pressed );
				}
			}
			else
			{
				if( m_focussed )
				{
					m_stateListDrawable->SetCurrentState( StateListDrawable::HasFocus );
				}
				else
				{
					//_stateListDrawable->SetCurrentState(StateListDrawable::Active);
					
					if( ( m_gazeIsRestButton && !m_gazeRestButtonToggle ) || m_toggleActiveState )
					{
						m_stateListDrawable->SetCurrentState( StateListDrawable::ToggleActive );
					}
					else
					{
						m_stateListDrawable->SetCurrentState( StateListDrawable::Active );
					}
				}
			}
		}
		else
		{
			m_stateListDrawable->SetCurrentState( StateListDrawable::Disabled );
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageButton::Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	View::Layout( _left, _top, _right, _bottom, _windowWidth, _windowHeight );

	float contentBoundsLeft = ( float )_left, contentBoundsRight = ( float )_right, contentBoundsTop = ( float )_top, contentBoundsBottom = ( float )_bottom;

	m_sizeX = _right - _left;
	m_sizeY = _bottom - _top;

	//std::cout<<"INFO: ImageButton::Layout width = "<<_sizeX<<" height = "<<_sizeY<<std::endl;

	m_centreX = ( int ) ( _left + ( m_sizeX / 2.0f ) );
	m_centreY = ( int ) ( _top + ( m_sizeY / 2.0f ) );
	if( m_generalDrawable != NULL )
	{
		m_generalDrawable->SetBounds( ( float )_left, ( float )_top, ( float )_right, ( float )_bottom );
		m_generalDrawable->GetContentBounds( contentBoundsLeft, contentBoundsTop, contentBoundsRight, contentBoundsBottom );
	}
	else if( m_stateListDrawable != NULL )
	{
		// First, set the bounds of the statelist
		m_stateListDrawable->SetBounds( ( float )_left, ( float )_top, ( float )_right, ( float )_bottom );
		m_stateListDrawable->GetContentBounds( contentBoundsLeft, contentBoundsTop, contentBoundsRight, contentBoundsBottom );

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

	if( m_contentGenDrawable != NULL )
		m_contentGenDrawable->SetBounds( contentBoundsLeft, contentBoundsTop, contentBoundsRight, contentBoundsBottom );
	if( m_contentStateListDrawable != NULL )
		m_contentStateListDrawable->SetBounds( contentBoundsLeft, contentBoundsTop, contentBoundsRight, contentBoundsBottom );

	SetStateDrawable();
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ImageButton::OnHoverEnter()
{
	//std::cout<<"image button hover enter"<<std::endl;
	if( m_audio_hoverEnter != NULL )
		m_audio_hoverEnter->Play();
	if( m_usingEyeGaze )
	{
		if( m_gazeWanderTimer >= m_gazeMaxWanderTime )
		{
			m_gazeDwellTimer = 0.0f;
			m_gazeIsPressing = false;
		}
		m_gazeWanderTimer = 0.0f;
		m_hasGaze = true;
	}
	return false;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ImageButton::OnHoverExit()
{
	if( m_usingEyeGaze )
	{
		m_hasGaze = false;
	}
	return false;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageButton::LoadMatricesToShader( GLuint _shaderID, cml::matrix44f_c _proj, cml::matrix44f_c _mv )
{
	// ModelView matrix
	GLint mvLoc = glGetUniformLocation( _shaderID, "u_ModelViewMatrix" );
	if( mvLoc != -1 ) { glUniformMatrix4fv( mvLoc, 1, GL_FALSE, _mv.data() ); }
	else { std::cerr << "u_ModelViewMatrix not found in shader " << _shaderID << std::endl; }


	// Projection matrix	
	GLint pLoc = glGetUniformLocation( _shaderID, "u_ProjectionMatrix" );
	if( pLoc != -1 ) { glUniformMatrix4fv( pLoc, 1, GL_FALSE, _proj.data() ); }
	else { std::cerr << "u_ProjectionMatrix not found in shader " << _shaderID << std::endl; }
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageButton::DrawGazeIndicator()
{
	if( m_hasGaze )
	{
		cml::matrix44f_c proj, mv;
		proj.identity();

		int viewport[4]; // Shouldn't really do this, but temporarily it's fine
		glGetIntegerv( GL_VIEWPORT, viewport );

		cml::matrix_orthographic_RH( proj, 0.f, ( float )viewport[ 2 ], ( float )viewport[ 3 ], 0.f, -1.f, 1.f, cml::z_clip_neg_one );
		
		mv.identity();

		m_gazeIndicatorShader->Bind();

		glUniform3f( glGetUniformLocation( m_gazeIndicatorShader->GetProgramID(), "u_Colour" ), 0.0f, 1.0f, 0.5f );
		LoadMatricesToShader( m_gazeIndicatorShader->GetProgramID(), proj, mv );

		// Create vao and vbo for drawing gaze indicator
		BuildGazeIndicatorVBOs();

		glBindVertexArray( m_gazeIndicatorVAO );
			glDrawArrays( GL_TRIANGLE_FAN, 0, m_nVertices );
		glBindVertexArray( 0 );
		
		m_gazeIndicatorShader->Unbind();
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ImageButton::BuildGazeIndicatorVBOs()
{
	float centreX = ( ( float )m_layoutBoundsLeft + m_layoutBoundsRight ) / 2.0f;
	float centreY = ( ( float )m_layoutBoundsTop + m_layoutBoundsBottom ) / 2.0f;

	std::vector< float > verts;
	verts.push_back( centreX );
	verts.push_back( centreY );

	for( float i = 0; i < std::min( 1.0f, ( m_gazeDwellTimer / m_gazeDwellToSelectTime ) ); i += 0.01f )
	{
		float angle = 2.0f * ( float )PI * i;
		verts.push_back( centreX + ( cos( angle ) * m_gazeDwellCircleSize ) );
		verts.push_back( centreY + ( sin( angle ) * m_gazeDwellCircleSize ) );
	}

	GLuint vertVBO;
	glGenBuffers( 1, &vertVBO );

	glBindBuffer( GL_ARRAY_BUFFER, vertVBO );
	glBufferData( GL_ARRAY_BUFFER, verts.size() * sizeof( float ), &verts[ 0 ], GL_STATIC_DRAW );

	glBindVertexArray( m_gazeIndicatorVAO );

	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, vertVBO );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, ( GLfloat* )NULL );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	m_nVertices = verts.size() / 2;
}

//----------------------------------------------------------------------------------