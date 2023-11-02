#include "GUI/Views/ImageButton.h"
#include "GUI/Drawables/NinePatch.h"
#include "GUI/Drawables/BitmapDrawable.h"
#include "GUIManager.h"
#include "ResourceManager.h"

#include "System/SharedSettings.h"

#include <cmath>

#ifndef PI
#define PI 3.141592653589793238462643383279502884
#endif


//static bool pauseEyegaze = false; //true;  // Otherwise the profile screen shows the dwell marker!
//ShivaGUI::SharedPreferences:: *
	
//ShivaGUI::my_class *pauseEyegaze;

//ShivaGUI::Shared *shared;
//bool pauseEyegaze;// = *shared->IsEyegazePaused();// ShivaGUI::Shared::IsEyegazePaused();



// These are both global variables accessed by 'extern' in other .cpp files
// I know I shouldn't do this, but I don't know enough about to c++ to avoid it!
extern bool pauseEyegaze;  // Initially declared in AssembleActivity.cpp
extern bool eyeGazeProfile; // Initially declared in AssembleActivity.cpp

clock_t startTime = clock();  // This is a global variable - should get rid of this, but again, I'm not sure how?




// MM (Aug 2020) NOT FULLY IMPLEMENTED: This is an alternative approach to the global variables above
ShivaGUI::SharedSettings* params = new ShivaGUI::SharedSettings();	// This is 'global' to this code file - is this a bad thing to do - how do I avoid this??  
																// I did this to share the eyegaze on/off value across .cpp files (AssembleActivity and EditMenuActivity etc.).
																// I don't see this anywhere else in these code files, so it must be bad!?
																// I only want to examine the value once rather than create a value for every single button (pointless).



/*
ShivaGUI::Shared::Shared()
{
	bool x = IsEyegazePaused();
	SetEyegazePaused(true);
	bool y = IsEyegazePaused();
	SetEyegazePaused(false);
	bool z = IsEyegazePaused();

//    std::cout << "In foo::bar(), value is " << x << '\n';
}
*/

//ShivaGUI::Shared::Shared::SetEyeGazeActiveStatus(false);// shared->SetEyeGazeActiveStatus(false);
//----------------------------------------------------------------------------------

ShivaGUI::ImageButton::ImageButton()
{
//	my_class classy = my_class();
//	classy->
// = ShivaGUI::my_class::my_class(1);// = new ShivaGUI::my_class;
//	classy->

//pauseEyegaze = new my_class(true);
//classy->set_value(false);

//pauseEyegaze = shared->IsEyegazePaused();

/*
ShivaGUI::Shared *shared = new ShivaGUI::Shared();

bool xx = shared->EyeGazeEnabled();
shared->SetEyeGazeActiveStatus(true);

bool yy = shared->EyeGazeEnabled();

shared->SetEyeGazeActiveStatus(false);

bool zz = shared->EyeGazeEnabled();
*/

	m_left = 0;//m_centreX - halfSizeX;
	m_right = 0;//m_centreX + halfSizeX;
	m_top = 0;//m_centreY - halfSizeY;
	m_bottom = 0;//m_centreY + halfSizeY;
	

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

	m_gazeColour.resize( 4 );
	m_gazeColour[ 0 ] = 0.f;
	m_gazeColour[ 1 ] = 1.f;
	m_gazeColour[ 2 ] = 0.5f;
	m_gazeColour[ 3 ] = 1.f;

	m_gazeRepeatFromTheme = false;

	m_setContentScaleUp = m_setContentAspectRatio = -1;

	m_useToggleActive = false;
	m_toggleActiveState = false;

	glGenVertexArrays( 1, &m_gazeIndicatorVAO );

	m_gazeIndicatorShader = new Utility::GPUProgram();
	m_gazeIndicatorShader->Create( "Resources/Shaders/Crosshair", Utility::GPUProgram::VERTEX_AND_FRAGMENT );
	glBindAttribLocation( m_gazeIndicatorShader->GetProgramID(), 0, "vPosition" );
	m_gazeIndicatorShader->LinkProgram();

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

	delete m_background;
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
						//m_hasGaze = false;
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

void ShivaGUI::ImageButton::Draw( unsigned int _context )
{
	View::Draw( _context );

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
	#ifdef _DEBUG
	std::cout << "INFO: _resources->GetButtonName(): " << _resources->GetButtonName() << std::endl;
	#endif

	if( _themePrefix.empty() )
		_themePrefix = "ImageButton_";
	View::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );

	// Searches through e.g. \shiva-totem\vs\shiva-totem\Resources\Themes\system.xml
	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( ( std::string( "drawable" ) == currentAttribute->Name() ) || ( _themePrefix + "drawable" == currentAttribute->Name() ) )
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
		//	eyeGazeProfile = true;
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
		else if( ( std::string( "eyeGaze_Colour" ) == currentAttribute->Name() ) || ( _themePrefix + "eyeGaze_Colour" == currentAttribute->Name() ) )
		{
			unsigned int gazeColour;

			std::string colourString( currentAttribute->Value() );
			std::stringstream colourStream;
			colourStream << std::hex << colourString;
			colourStream >> gazeColour;

			m_gazeColour[ 0 ] = ( ( gazeColour & 0xFF000000 ) >> 24 ) / 255.f;
			m_gazeColour[ 1 ] = ( ( gazeColour & 0x00FF0000 ) >> 16 ) / 255.f;
			m_gazeColour[ 2 ] = ( ( gazeColour & 0x0000FF00 ) >> 8 ) / 255.f;
			m_gazeColour[ 3 ] = ( gazeColour & 0x000000FF ) / 255.f;
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



    if( m_usingEyeGaze )
	{
		xmlNode->SetAttribute( "eyeGaze_enable", "true" );
	}
	else
	{
		xmlNode->SetAttribute( "eyeGaze_enable", "false" );
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
//			InternalEvent *currentEvent = NULL;
//			currentEvent = new InternalEvent( InternalEvent::EYEGAZE_DISABLE );
//			m_gazeRestIssueEvent = true;
//			m_gazeRestButtonToggle = true;
//			m_usingEyeGaze = false;
//			m_gazeRestIssueEvent = true;
//			m_gazeRestButtonToggle = false;//!m_gazeRestButtonToggle;


	std::string buttonID = ImageButton::GetID();

	// std::string img = ImageButton::m_imageName;

	if( m_pressed == false && _value == true )
	{
		// Press
		if( m_audio_pressStart != NULL )
		{
			// *** NEW April 2020: Auditory feedback for individual buttons
			// We look for a .wav file in the shiva-totem\vs\shiva-totem\Resources\Audio folder that matches the ID of the button + .wav 
			// extension.  If one cannot be found, we use the default sound.

			// Part of this is a proper bodge, particularly the bit for primitives: they don't seem to have anything that identifies them 
			// when clicked apart from m_contentGenDrawable->GetFilename() which returns m_filename which contains something like 
			// 'Models/colprim_cone.xml'
			
			// ImageButton *IB = this;
			// std::string imageName = ImageButton::GetImageName();
			// m_contentGenDrawable->m_filename
			// std::string imageText = ImageButton::GetImageText();
			// std::string textButtonID = TextButton
			// ShivaGUI::View *_view;


			std::string buttonIDWithPath = "Resources/Audio/" + buttonID;

			std::string soundFile = buttonIDWithPath + ".wav";

			if (soundFile == "Resources/Audio/imagebutton.wav")
			{
			   std::string fn = m_contentGenDrawable->GetFilename();
			   boost::erase_all(fn, "Models/colprim_"); // "Models/colprim_cone.xml"

				if (fn == "sphere.xml")
					soundFile = "Resources/Audio/Sphere.wav";
				
				else if ( fn == "cone.xml")
					soundFile = "Resources/Audio/Cone.wav";
				
				else if ( fn == "cylinder.xml")
					soundFile = "Resources/Audio/Cylinder.wav";

				else if ( fn == "cube.xml")
					soundFile = "Resources/Audio/Cube.wav";
	
				else if (fn == "rect.xml")
					soundFile = "Resources/Audio/Cuboid.wav";		
			}

			// Command *nextFocus = GetPrimType();  //GetNextFocus( Definitions::Right );
			// std::string w = 
			// View:: SetID("Cone");//GetID();// SetID(addObjCmd->GetPrimType());
			// std::vector< std::pair< ShivaGUI::AdapterView*, ShivaGUI::GUIController* > >::iterator it = m_listViews.begin();
			// dataEntryIndex = ( *it ).first->GetDataIndex( _view );

			if( !boost::filesystem::exists( soundFile ) )
				// The button has no matching sound file
				soundFile = "Resources/Audio/Default.wav";

			// Assemble the audio clip to play - an AudioManager and Mix_Chunk are needed to build an AudioClip to play
			AudioManager *audMan = new AudioManager();
			Mix_Chunk *chunk = new Mix_Chunk;
			
			AudioClip *audClip = new AudioClip( audMan, chunk, soundFile, false);
			audClip = audMan->GetSample(soundFile);

			m_audio_pressStart = audClip;

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

		/*
		if (m_usingEyeGaze)
		{
//			m_usingEyeGaze = false;
			m_gazeRestIssueEvent = true;
			m_gazeRestButtonToggle = true;
	}
	*/

		if (m_usingEyeGaze && !pauseEyegaze)
		{
//			if (buttonID == "BackButton") 
//				pauseEyegaze = true;
//				m_gazeDwellTimer = 0.0f;

			if ( (buttonID == "EditMenuActivity") 
				|| (buttonID == "NudgeActivity") 
				|| (buttonID == "UniformScaleActivity") 
				|| (buttonID == "RotateObjectActivity")
				|| (buttonID == "DrillActivity")
				|| (buttonID == "BlendAdjustActivity") 
				|| (buttonID == "BackButton") )
			{
				// Pause when changing screens to avoid selecting something before the page has loaded
				//pauseEyegaze->set_value(true);
				eyeGazeProfile = true; // Global variable!
				pauseEyegaze = true;
				startTime = clock();

//				m_gazeDwellTimer = 0.0f;

//				SetSelect(false);
				m_gazeWanderTimer = 0.0f;
				//m_gazeDwellTimer = 0.0f;
			}
			//else
//				m_gazeWanderTimer = 1.0f;
//				shared->SetEyegazePaused(true);//pauseEyegaze->set_value(true);
//				pauseEyegaze = true;
		}

/*			m_usingEyeGaze = false;
		else
			m_usingEyeGaze = true;*/



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
//	if (m_usingEyeGaze && m_gazeRestButtonToggle)

		// Pause a little while between page changes to prevent accidental selections
		if (eyeGazeProfile)
		{
//		if (!pauseEyegaze)
//			m_usingEyeGaze = true;
			if (pauseEyegaze)
			{
				// *** IMPORTANT ***
				// Have a look at whether eye gaze is paused using OO approach
				// params->SetEyegazePaused(false);
				bool eyeGazePaused = params->IsEyegazePaused();

				double timeSinceEyeGazePaused = 0.0f;

				clock_t timeNow = clock(); // This is a global variable - should get rid of this

				timeSinceEyeGazePaused = (timeNow - startTime) / CLOCKS_PER_SEC;

				const double pauseTime = 1.0f;

				if (timeSinceEyeGazePaused  > pauseTime)
				{
					pauseEyegaze = false;
//					m_usingEyeGaze = true;
//					m_gazeDwellTimer = 0.0f;
				}
			}
			else
			{
//			if( !pauseEyegaze && m_usingEyeGaze )
//			{
/*
				if( m_gazeWanderTimer >= m_gazeMaxWanderTime )
				{
					m_gazeDwellTimer = 0.0f;
					m_gazeIsPressing = false;
				}
//				m_gazeWanderTimer = 0.0f;
				m_hasGaze = true;
//			}

		//}
*/
	//			m_usingEyeGaze = true;
			}
		}


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
		else
		{
			if( ( _currentEvent->GetType() == InternalEvent::POSITIONAL_DRAG ) && ( !m_pressed ) && ( !m_hover ) )
			{
				int mouseX, mouseY;
				_currentEvent->GetPosition( mouseX, mouseY );

			//if (pauseEyegaze)
/*			
			if( MouseOver( mouseX, mouseY ) )
			{
				m_hover = true;

				//if (mouseX > ShivaGUI::ImageButton::
				return OnHoverOver();
			}
*/
				if ( !pauseEyegaze && m_usingEyeGaze )
				{
					if( MouseHit( mouseX, mouseY ) )
					{
						// This is a hover enter
						m_hover = true;
						return OnHoverEnter();
					}
				}
			}

			if ( _currentEvent->GetType() == InternalEvent::POSITIONAL_DRAG )
			{
				int mouseX, mouseY;
				_currentEvent->GetPosition( mouseX, mouseY );

			//if (pauseEyegaze)
				if( MouseOver( mouseX, mouseY ) )
				{
					std::string buttonID = ImageButton::GetID();

// IMPORTANT NOTE: If this code is built and run in debug mode the dwell marker will appear stuttery if the mouse is continually moved over a button.
//				   I think this is becase it generates many calls to the onHover event and has to put the debug text into the console window.
//					When built as a release version it seems to be OK.

#ifdef _DEBUG
	std::cout << "\nbuttonID: " << buttonID << std::endl;
#endif

#ifdef _DEBUG
	std::cout << "mouseX: " << mouseX << std::endl;
	std::cout << "m_left: " << m_left << ", m_right: " << m_right << std::endl;
	std::cout << "mouseY: " << mouseY << std::endl;
	std::cout << "m_top: " << m_top << ", m_bottom: " << m_bottom << std::endl;
#endif

#ifdef _DEBUG
	std::cout << "OnHoverOver" << std::endl;
#endif
					// This is a hover over
					m_hover = true;
					return OnHoverOver();
				}			
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
#ifdef _DEBUG
				std::cout << "INFO: Next scan item is: " << nextFocus->GetID() << std::endl;
#endif
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

	m_left = m_centreX - halfSizeX;
	m_right = m_centreX + halfSizeX;
	m_top = m_centreY - halfSizeY;
	m_bottom = m_centreY + halfSizeY;

	return ( _mouseX < m_centreX + halfSizeX ) && ( _mouseX > m_centreX - halfSizeX )
		&& ( _mouseY < m_centreY + halfSizeY ) && ( _mouseY > m_centreY - halfSizeY );
}

//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------

bool ShivaGUI::ImageButton::MouseOver( int _mouseX, int _mouseY )
{
//	float halfSizeX = m_sizeX / 2.0f;
//	float halfSizeY = m_sizeY / 2.0f;
	
	return ( _mouseX >= m_left ) && ( _mouseX <= m_right )
		&& ( _mouseY >= m_top ) && ( _mouseY <= m_bottom );
//	return ( _mouseX >= m_centreX - halfSizeX ) && ( _mouseX <= m_centreX + halfSizeX )
//		&& ( _mouseY >= m_centreY - halfSizeY ) && ( _mouseY <= m_centreY + halfSizeY );
}

//----------------------------------------------------------------------------------


void ShivaGUI::ImageButton::SetStateDrawable()
{
	// m_stateListDrawable->SetCurrentState( StateListDrawable::ToggleActive );
	// return;

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

	//std::cout << "INFO: ImageButton::Layout width = " << m_sizeX << " height = " << m_sizeY << std::endl;

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


#ifdef _DEBUG
				std::cout << "OnHoverEnter" << std::endl;
#endif
	// Eye gaze is paused temporarily because we are moving from one screen to another
//	if (m_usingEyeGaze && m_gazeRestButtonToggle)
//	if ( m_gazeRestButtonToggle)
	{
//		if (pauseEyegaze)
//			m_usingEyeGaze = false;
//		else
//			m_usingEyeGaze = true;
	}
//		if (!pauseEyegaze && (m_usingEyeGaze == false))
//			m_usingEyeGaze = true;

	//std::cout<<"image button hover enter"<<std::endl;

	// Play the cell 'focus' sound effect
	if( m_audio_hoverEnter != NULL )
		m_audio_hoverEnter->Play();


	if( !pauseEyegaze && m_usingEyeGaze )
	//if( m_usingEyeGaze )
	{

//		if (m_gazeWanderTimer != NULL)
//		{
		m_hasGaze = true;
		if( m_gazeWanderTimer >= m_gazeMaxWanderTime )
		{
			// Wander time has been exceeded - reset it and the dwell timer
//  		m_gazeWanderTimer = 0.0f;
			m_gazeDwellTimer = 0.0f;
//			m_gazeIsPressing = false;
		}
		m_gazeWanderTimer = 0.0f;
//		}

	}

//	if( !pauseEyegaze && m_usingEyeGaze )
	{
//		if( m_gazeWanderTimer >= m_gazeMaxWanderTime )
		{
//			m_gazeDwellTimer = 0.0f;
//			m_gazeIsPressing = false;
//			m_gazeWanderTimer = 0.0f;
		}

//		m_hasGaze = true;
	}
	
	return false;
}


bool ShivaGUI::ImageButton::OnHoverOver()
{
	// Eye gaze is paused temporarily because we are moving from one screen to another
//	if (m_usingEyeGaze && m_gazeRestButtonToggle)
//	if ( m_gazeRestButtonToggle)
//	{
//		if (pauseEyegaze)
//			m_usingEyeGaze = false;
//		else
//			m_usingEyeGaze = true;
//	}
//		if (!pauseEyegaze && (m_usingEyeGaze == false))
//			m_usingEyeGaze = true;

	//std::cout<<"image button hover enter"<<std::endl;
//	if( m_audio_hoverEnter != NULL )
//		m_audio_hoverEnter->Play();

	if( !pauseEyegaze && m_usingEyeGaze )//&& m_gazeIsPressing )
	{
		// Eye gaze isn't paused
//		if( m_gazeWanderTimer >= m_gazeMaxWanderTime )
//		{
//			m_gazeDwellTimer = 0.0f;
//			m_gazeIsPressing = false;
//		}


//		if( m_gazeWanderTimer >= m_gazeMaxWanderTime )
//		{
			// Wander time has been exceeded - reset it and the dwell timer
//	    	m_gazeWanderTimer = 0.0f;
//			m_gazeDwellTimer = 0.0f;
//			m_gazeIsPressing = false;

//		}

		m_hasGaze = true;

//		SetSelect(true);
		if ( m_gazeDwellTimer > m_gazeDwellToSelectTime)
		{
			if (!m_gazeIsPressing)
			{
			// Selection time is complete

			m_gazeIsPressing = true;
			m_gazeDwellTimer = 0.0f;
			}
			else
				SetSelect(false);
//				m_gazeWanderTimer = 0.0f;
//			    m_hasGaze = true;
//				m_gazeIsPressing = false;
		}
		else
		{
			m_gazeIsPressing = false;
//			m_hasGaze = true;
		}
	}
//	else
//		m_gazeIsPressing = false;

	return false;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ImageButton::OnHoverExit()
{
#ifdef _DEBUG
				std::cout << "OnHoverExit" << std::endl;
#endif
	if( m_usingEyeGaze )
	{
	  if (m_gazeIsPressing)
		m_gazeDwellTimer = 0.0f;
//		m_gazeIsPressing = false;
//		m_hover = false;

		m_hasGaze = false;
//		m_gazeWanderTimer = NULL;
//		m_gazeIsPressing = false;
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

		//glUniform3f( glGetUniformLocation( m_gazeIndicatorShader->GetProgramID(), "u_Colour" ), 0.0f, 1.0f, 0.5f );
		glUniform3f( glGetUniformLocation( m_gazeIndicatorShader->GetProgramID(), "u_Colour" ), m_gazeColour[ 0 ], m_gazeColour[ 1 ], m_gazeColour[ 2 ] );
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