#include "ResourceManager.h"
#include "GUIManager.h"
#include "System/Activity.h"
#include "GUI/Drawables/Drawable.h"
#include "GUI/Drawables/BitmapDrawable.h"
#include "GUI/Drawables/RectDrawable.h"
#include "GUI/Drawables/NinePatch.h"
#include "GUI/Drawables/StateListDrawable.h"
#include "GUI/Drawables/LayeredImageDrawable.h"
#include "GUI/Views/View.h"
#include "GUI/Views/ImageButton.h"
#include "GUI/Views/TextButton.h"
#include "GUI/Views/TextView.h"
#include "GUI/Views/ImageTextButton.h"
#include "GUI/Views/ViewGroups/LinearLayout.h"
#include "GUI/Views/ViewGroups/CustomLayout.h"
#include "GUI/Views/Slider.h"
#include "GUI/Views/ViewGroups/ScrollContainer.h"
#include "GUI/Views/ViewGroups/ViewGroup.h"
#include "GUI/Views/ColourSelector.h"
#include "GUI/Views/AdapterViews/ListView.h"
#include "GUI/Views/AdapterViews/LayoutAdapterView.h"

//----------------------------------------------------------------------------------

ShivaGUI::ResourceManager::ResourceManager( ShivaGUI::GUIManager *_guiManager, unsigned int _windowIndex )
{
	m_GUIManager = _guiManager;
	m_windowIndex = _windowIndex;
	m_mipmapImages = true;
	m_currentThemeNode = NULL;
	m_textTexName = 0;

	m_addTextSpace = false; 
	m_isTextToTexture = false;

	m_isRenderingText = false;

	m_projMatrix.identity();
	m_mvMatrix.identity();

	TTF_Init();
}

//----------------------------------------------------------------------------------

ShivaGUI::ResourceManager::~ResourceManager()
{ 
	// Clearing font cache
	std::map< std::string, TTF_Font* >::const_iterator fontIt;
	for( fontIt = m_fontcache.begin(); fontIt != m_fontcache.end(); fontIt++ )
	{
		TTF_CloseFont( fontIt->second );
	}
	m_fontcache.clear();
}

//----------------------------------------------------------------------------------

void ShivaGUI::ResourceManager::RegisterViewID( View *_view, std::string _id )
{
	if( m_GUIManager != NULL )
	{
		Activity *currentActivity = m_GUIManager->GetCurrentActivity();
		if( currentActivity != NULL )
		{
			GUIController *currentGUIController = currentActivity->GetGUIController( m_windowIndex );
			if( currentGUIController != NULL )
			{
				currentGUIController->RegisterViewID( _view, _id );
			}
			else
				std::cerr << "ShivaGUI::ResourceManager::RegisterViewID failed to retrieve currentGUIController" << std::endl;
		}
		else
			std::cerr << "ShivaGUI::ResourceManager::RegisterViewID failed to retrieve currentActivity" << std::endl;
	}
	else
		std::cerr << "ShivaGUI::ResourceManager::RegisterViewID failed to retrieve _GUIManager" << std::endl;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ResourceManager::SetMatrices( const float &_width, const float &_height )
{
	m_projMatrix.identity();
	cml::matrix_orthographic_RH( m_projMatrix, 0.f, _width, _height, 0.f, -1.f, 1.f, cml::z_clip_neg_one );
	
	m_mvMatrix.identity();
}

//----------------------------------------------------------------------------------

ShivaGUI::View* ShivaGUI::ResourceManager::GetViewFromID( std::string _id )
{
	if( m_GUIManager != NULL )
	{
		Activity *currentActivity = m_GUIManager->GetCurrentActivity();
		if( currentActivity != NULL )
		{
			GUIController *currentGUIController = currentActivity->GetGUIController( m_windowIndex );
			if( currentGUIController != NULL )
			{
				return currentGUIController->GetViewFromID( _id );
			}
			else
				std::cerr << "ShivaGUI::ResourceManager::GetViewFromID failed to retrieve currentGUIController" << std::endl;
		}
		else
			std::cerr << "ShivaGUI::ResourceManager::GetViewFromID failed to retrieve currentActivity" << std::endl;
	}
	else
		std::cerr << "ShivaGUI::ResourceManager::GetViewFromID failed to retrieve _GUIManager" << std::endl;
	return NULL;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ResourceManager::ReloadTextures()
{
	for( std::map< std::string, unsigned int >::iterator it = m_glTextures.begin(); it != m_glTextures.end(); ++it )
	{
		bool mipmap = false;
		bool addAlpha = false;
		bool repeat = false;

		std::cout << "INFO: ResourceManager Reloading Image to OpenGL: " << it->first << std::endl;

		SDL_Surface *image;

		// Load the BMP file into a surface
		//image = SDL_LoadBMP(filename.c_str());

		image = IMG_Load( it->first.c_str() );

		if( image == NULL )
		{
			std::cerr << "WARNING: ResourceManager Couldn't load: " << it->first << " " << SDL_GetError() << std::endl;
		}

		SDLSurfaceToOpenGLID( image, it->second, mipmap, addAlpha, repeat );

		// Free the allocated BMP surface
		SDL_FreeSurface( image );
	}
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ResourceManager::GetManagedTexture( std::string _name, unsigned int *_texID )
{
	if( m_glExternalTextures.find( _name ) != m_glExternalTextures.end() )
	{
		*_texID = m_glExternalTextures[ _name ];
		return false;
	}

	glGenTextures( 1, _texID );
	m_glExternalTextures[ _name ] = *_texID;
	return true;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ResourceManager::ClearManagedTexture( std::string _name )
{
	if( m_glExternalTextures.find( _name ) != m_glExternalTextures.end() )
	{
		unsigned int texID = m_glExternalTextures[ _name ];

		glDeleteTextures( 1, &texID );
		m_glExternalTextures.erase( _name );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ResourceManager::ClearText()
{
	m_buttonText.clear();
}

//----------------------------------------------------------------------------------

void ShivaGUI::ResourceManager::SetTextInfo( const std::string &_text, const std::string &_fontName, const unsigned int &_fontSize, const unsigned int &_fontColour, const unsigned int &_textAlign )
{
	m_buttonText = _text;
	m_buttonFontName = _fontName;
	m_buttonFontSize = _fontSize;
	m_buttonFontColour = _fontColour;
	m_buttonTextAlign = _textAlign;

	if( !m_buttonText.empty() )
		m_isRenderingText = true;
}

//----------------------------------------------------------------------------------

SDL_Rect newSDL_Rect( int _xs, int _ys, int _dx, int _dy ) 
{
	SDL_Rect rc;
	rc.x = _xs; rc.y = _ys;
	rc.w = _dx; rc.h = _dy;

    return( rc );
}

//----------------------------------------------------------------------------------

unsigned int ShivaGUI::ResourceManager::GetBitmap( std::string _filename )
{
	if( m_glTextures.find( _filename ) != m_glTextures.end() )
	{
		return m_glTextures[ _filename ];
	}

	// I've had this function for so long I can't remember where it was from originally
	// I think it was based on SDL example usage code

	bool mipmap = false;
	bool addAlpha = true;
	bool repeat = false;

	std::cout << "INFO: ResourceManager Loading Image to OpenGL: " << _filename << std::endl;

	SDL_Surface *image;
	// Loading image as a SDL_Surface
	image = IMG_Load( _filename.c_str() );

	if( image == NULL ){
		std::cerr << "WARNING: ResourceManager Couldn't load: " << _filename << " " << SDL_GetError() << std::endl;
		return 0;
	}

	unsigned int texName;

	if( m_addTextSpace ) // For ImageTextButtons
	{
		// Creating the text surface from text body and attributes set for the font
		SDL_Surface* textSurf = GetTextSurface( m_buttonText, m_buttonTextAlign, m_buttonFontName, m_buttonFontSize, m_buttonFontColour );
		
		// At the moment, we're using the width of the previously loaded image as the "final" width of the image/icon+text that will be rendered
		int width = image->w;
		int height = image->h + textSurf->h + 10; // Adding some padding

		Uint32 rmask = 0x000000ff;
		Uint32 gmask = 0x0000ff00;
		Uint32 bmask = 0x00ff0000;
		Uint32 amask = 0xff000000;

		// Create the surface that will hold the final image scaled/resized based on the dimensions of the text
		SDL_Surface *finalImage = SDL_CreateRGBSurface( SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask );
		
		// If we assume that image will occupy 60% of total space, and text will occupy 40% of total space
		float imageH = height * 0.6f;
		float scalingFactor = imageH / ( float )image->h;
		float imageW = width * scalingFactor;

		SDL_SetSurfaceBlendMode( image, SDL_BLENDMODE_NONE );

		SDL_BlitScaled( image, NULL, finalImage, &( SDL_Rect )newSDL_Rect( ( ( image->w - ( int )imageW ) / 2 ), 5, ( int )imageW, ( int )imageH ) );
		
		if ( m_isTextToTexture ) // If we are rendering the text (which we will do only once)
		{
			// Now check if there is already a texture for the text (it shouldn't be there, but oh well)
			if( m_glTextures.find( std::string( "Text/" )  + _filename ) == m_glTextures.end() )
			{
				unsigned int textID;

				SDL_Surface *textImage = SDL_CreateRGBSurface( SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask );
				
				float textH = height * 0.4f;
				float textScalingFactor = textH / ( float )textSurf->h;
				float textW = textSurf->w * textScalingFactor;
		
				SDL_SetSurfaceBlendMode( textSurf, SDL_BLENDMODE_NONE );
		
				SDL_BlitScaled( textSurf, NULL, textImage, &( SDL_Rect )newSDL_Rect( ( ( image->w - ( int )textW ) / 2 ), ( int )imageH + 5, textSurf->w, textSurf->h ) );

				textID = SDLSurfaceToOpenGL( textImage, mipmap, addAlpha, repeat );

				SDL_FreeSurface( textImage );

				if( textID != 0 )
					m_glTextures[ std::string( "Text/" ) + _filename ] = textID;
			}
		}

		texName = SDLSurfaceToOpenGL( finalImage, mipmap, addAlpha, repeat );
		
		SDL_FreeSurface( finalImage );
		SDL_FreeSurface( textSurf );

		m_isRenderingText = false;
	}
	else
	{
		texName = SDLSurfaceToOpenGL( image, mipmap, addAlpha, repeat );
		// Free the allocated BMP surface
		SDL_FreeSurface( image );
	}

	if( texName != 0 )
		m_glTextures[ _filename ] = texName;
	
	return texName;
}

//----------------------------------------------------------------------------------

SDL_Surface* ShivaGUI::ResourceManager::GetTextSurface( std::string _text, unsigned int _alignment, std::string _fontfilename, unsigned int _fontsize, unsigned int _fontColour )
{
	// Load the font
	TTF_Font *font = LoadFont( _fontfilename, _fontsize );

	if( font == NULL ) {
		std::cerr << "WARNING: ResourceManager::GetSimpleText could not load font: " << _fontfilename << std::endl;
		return 0;
	}

	SDL_Color foregroundColour = { ( _fontColour & 0xFF000000 ) >> 24, ( _fontColour & 0x00FF0000 ) >> 16, ( _fontColour & 0x0000FF00 ) >> 8, ( _fontColour & 0x000000FF ) };

	SDL_Surface *finalSurface = NULL;
    Uint32 rmask, gmask, bmask, amask;

    // SDL interprets each pixel as a 32-bit number, so our masks must depend on the endianness (byte order) of the machine
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

	// Following code, most from https://www.gamedev.net/resources/_/technical/game-programming/sdl--fonts-part-2-printstrings-r1960

	int width = 0, height = 10;

	// Get line-skip value
	int lineHeight =  TTF_FontHeight( font );
	int lineSkip = TTF_FontLineSkip( font ) - ( lineHeight / 3 ); 
	std::vector< std::string > lines;

	// Break string into lines when it finds '\n'
	int n = 0;
	while( n != -1 ) {

		// Get until we find either \n or \0
		std::string subString;
		n = _text.find( '\n', 0 );
		subString = _text.substr( 0, n );

		if( n != -1 ) {
			
			_text = _text.substr( n + 1, -1 ); 
		}

		lines.push_back( subString );

		// Get size of rendered text
		int w = 0;
		TTF_SizeText( font, subString.c_str(), &w, &height );

		if( w > width ) {
			width = w;
		}
	}

	// Since width was determined earlier, get height
	height = ( lines.size() - 1 ) * lineSkip + height; 

	// Make surface to which blit the text
	finalSurface = SDL_CreateRGBSurface( SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask );

	if ( finalSurface == NULL) {
        SDL_Log( "SDL_CreateRGBSurface() failed: %s", SDL_GetError() );
        exit( 1 );
    }


	SDL_Surface *surf = NULL;
	
	// Blit text into final surface, taking into consideration "\n", perform alignment of text depending on user input 
	for( int i = ( lines.size() - 1 ); i >= 0; i-- )
	{
		surf = TTF_RenderText_Blended( font, lines[i].c_str(), foregroundColour );

		SDL_SetSurfaceBlendMode( surf, SDL_BLENDMODE_NONE );

		SDL_Rect layout;

		if( _alignment == ShivaGUI::TextButton::Left ) {

			layout = newSDL_Rect( 0, i * lineSkip, 0, 0 );
		}
		else if( _alignment == ShivaGUI::TextButton::Centre ) {
			int w = 0, h = 0; 
			TTF_SizeText( font, lines[ i ].c_str(), &w, &h );

			layout = newSDL_Rect( ( width - w ) / 2, i * lineSkip, 0, 0 );
		}
		else if( _alignment == ShivaGUI::TextButton::Right ) {
			int w = 0, h = 0; 
			TTF_SizeText( font, lines[ i ].c_str(), &w, &h );

			layout = newSDL_Rect( ( width - w ), i * lineSkip, 0, 0 );
		}

		if( SDL_BlitSurface( surf, NULL, finalSurface, &( SDL_Rect )layout ) != 0 )
		{
			std::cerr<< "ERROR: ResourceManager couldn't blit surface. " << SDL_GetError() << std::endl;
		}

		SDL_FreeSurface( surf );
	}

	return finalSurface;
}

//----------------------------------------------------------------------------------

unsigned int ShivaGUI::ResourceManager::GetText( std::string _text, unsigned int _alignment, std::string _fontfilename, unsigned int _fontsize, unsigned int _fontColour )
{
	return SDLSurfaceToOpenGL( GetTextSurface( _text, _alignment, _fontfilename, _fontsize, _fontColour ), false, false, false, true );
}

//----------------------------------------------------------------------------------

unsigned int ShivaGUI::ResourceManager::GetSimpleText( std::string _text, std::string _fontfilename, unsigned int _fontsize, unsigned int _fontColour )
{
	TTF_Font *font = LoadFont( _fontfilename, _fontsize );
	if( font == NULL )
	{
		std::cerr << "WARNING: ResourceManager::GetSimpleText could not load font: " << _fontfilename << std::endl;
		return 0;
	}

	SDL_Color foregroundColour = { ( _fontColour & 0xFF000000 ) >> 24, ( _fontColour & 0x00FF0000 ) >> 16, ( _fontColour & 0x0000FF00 ) >> 8, ( _fontColour & 0x000000FF ) };//{255,255,255,255};

	//std::cout<<"R = "<<(int)foregroundColour.r<<" G = "<<(int)foregroundColour.g<<" B = "<<(int)foregroundColour.b<<std::endl;

	SDL_Surface *surf = TTF_RenderText_Blended( font, _text.c_str(), foregroundColour ); //TTF_RenderText_Shaded( font, text.c_str(), foregroundColour, backgroundColour );

	return SDLSurfaceToOpenGL( surf, false, false, false, true );
}


//----------------------------------------------------------------------------------

TTF_Font* ShivaGUI::ResourceManager::LoadFont( std::string _filename, unsigned int _size )
{
	if( _filename.empty() )
		_filename = m_defaultFont;
	std::stringstream key;
	key << _filename << _size;

	std::map< std::string, TTF_Font* >::iterator fontIt = m_fontcache.find( key.str() );
	if( fontIt != m_fontcache.end() )
	{
		return fontIt->second;
	}
	TTF_Font *currentFont = TTF_OpenFont( _filename.c_str(), _size );

	if( currentFont != NULL )
		m_fontcache[ key.str() ] = currentFont;

	return currentFont;
}

//----------------------------------------------------------------------------------

ShivaGUI::Drawable* ShivaGUI::ResourceManager::GetDrawable( std::string _filename )
{
	// If the first character is an '@' we will assume it's a constant for replacement
	if( _filename.at( 0 ) == '@' )
		_filename = GetInflationAttribute( _filename );


	if( _filename.empty() )
	{
		std::cerr << "WARNING: ResourceManager::GetDrawable given empty filename" << std::endl;
		return NULL;
	}

	if( _filename.compare( _filename.size() - 4, 4, ".png" ) == 0 )
	{
		// We are being asked to load a png file
		Drawable *output = new BitmapDrawable( GetBitmap( std::string( "Resources/Drawables/" ) + _filename ) );
		output->SetFilename( _filename );
		return output;
	}
	else if( _filename.compare( _filename.size() - 4, 4, ".xml" ) == 0 )
	{
		// We are being asked to load a drawable from xml file

		TiXmlDocument doc( ( std::string( "Resources/Drawables/" ) + _filename ).c_str() );

		if( doc.LoadFile() )
		{
			Drawable *output = ParseDrawableElement( &doc );
			output->SetFilename( _filename );
			return output;
		}
		else
		{
			std::cerr << "WARNING: ResourceManager::GetDrawable Could not load drawable from xml: " << _filename << std::endl;
			return NULL;
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------------

ShivaGUI::Drawable* ShivaGUI::ResourceManager::CreateDrawable( std::string _drawableName )
{
	if( _drawableName == "BitmapDrawable" )
	{
		return new BitmapDrawable();
	}
	else if( _drawableName == "StateListDrawable" )
	{
		return new StateListDrawable();
	}
	else if( _drawableName == "NinePatch" )
	{
		return new NinePatch();
	}
	else if( _drawableName == "RectDrawable" )
	{
		return new RectDrawable();
	}
	else if( _drawableName == "LayeredImageDrawable" )
	{
		if( m_isRenderingText ) {
			LayeredImageDrawable* drawable = new LayeredImageDrawable();
			drawable->IsRenderingText();
			return drawable;
		}
		else {
			return new LayeredImageDrawable();
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------------

ShivaGUI::View* ShivaGUI::ResourceManager::CreateView( std::string _viewName )
{
	if( _viewName == "View" )
	{
		return new View();
	}
	if( _viewName == "ImageButton" )
	{
		return new ImageButton();
	}
	if( _viewName == "ImageTextButton" )
	{
		return new ImageTextButton();
	}
	if( _viewName == "TextButton" )
	{
		return new TextButton();
	}
	if( _viewName == "LinearLayout" )
	{
		return new LinearLayout();
	}
	if( _viewName == "Slider" )
	{
		return new Slider();
	}
	if( _viewName == "ColourSelector" )
	{
		return new ColourSelector();
	}
	if( _viewName == "TextView" )
	{
		return new TextView();
	}
	if( _viewName == "CustomLayout" )
	{
		return new CustomLayout();
	}
	if( _viewName == "ScrollContainer" )
	{
		return new ScrollContainer();
	}
	if( _viewName == "ListView" )
	{
		return new ListView();
	}
	if( _viewName == "LayoutAdapterView" )
	{
		return new LayoutAdapterView();
	}

	View *customView = m_GUIManager->CreateView( _viewName );
	if( customView != NULL ) {
		return customView;
	}

	std::cerr << "WARNING: ResourceManager::GetView could not create View of unknown type: " << _viewName << std::endl;
	return NULL;
}

//----------------------------------------------------------------------------------

ShivaGUI::AudioClip* ShivaGUI::ResourceManager::GetAudioClip( std::string _filename )
{
	return m_GUIManager->GetAudioManager()->GetSample( _filename );
}

//----------------------------------------------------------------------------------

ShivaGUI::View* ShivaGUI::ResourceManager::GetLayout( std::string _filenameInput )
{
	// Strip off any leading path stuff initially:
	boost::filesystem::path filename( _filenameInput );
	filename = filename.filename();

	// First, see if a version of the layout exists in the options directory:
	boost::filesystem::path optionsPath( m_GUIManager->GetProfileOptionsDir() );
	optionsPath = optionsPath / "Layout" / filename;

	TiXmlDocument doc;
	boost::filesystem::path pathToLoad;
	if( boost::filesystem::exists( optionsPath ) )
	{
		pathToLoad = optionsPath;
	}
	else
	{
		// Version in main resources dir
		pathToLoad = "Resources/Layout" / filename;
	}

	if( doc.LoadFile( pathToLoad.string() ) )
	{
		View *rootView = ParseLayoutElement( &doc, true );
		DoPostEvaluationLinks();
		return rootView;
	}
	else
	{
		std::cerr << "WARNING: ResourceManager::GetLayout Could not load layout from xml: " << filename << std::endl;
		return NULL;
	}
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ResourceManager::OutputLayout( std::string _filename, View *_rootNode )
{
	if( _filename.empty() || _rootNode == NULL )
		return false;

	std::cout << "INFO: ResourceManager Deflating Layout to file: " << _filename << std::endl;

	TiXmlDocument doc( _filename.c_str() );

	doc.InsertEndChild( *_rootNode->Deflate( this ) );

	return doc.SaveFile();
}

//----------------------------------------------------------------------------------

std::string ShivaGUI::ResourceManager::GetInflationAttribute( std::string _originalAttrib )
{
	if( _originalAttrib.at( 0 ) == '@' )
	{
		//std::cout<<"*********INFO: Const replacement for: "<<originalAttrib<<std::endl;
		// Check against consts in Profile
		if( m_attribConstsProfile.find( _originalAttrib ) != m_attribConstsProfile.end() )
		{
			//std::cout << "INFO: Const replacement for: " << originalAttrib << " from Profile with: " << _attribConstsProfile[ originalAttrib ] << std::endl;
			return m_attribConstsProfile[ _originalAttrib ];
		}
		// Check against consts in Theme
		if( m_attribConstsTheme.find( _originalAttrib ) != m_attribConstsTheme.end() )
		{
			//std::cout << "INFO: Const replacement for: " << originalAttrib << " from Theme with: " << _attribConstsTheme[ originalAttrib ] << std::endl;
			return m_attribConstsTheme[ _originalAttrib ];
		}
	}
	return _originalAttrib;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ResourceManager::LoadProfileAttributeConsts( ProfileManager *_profileManager )
{
	m_attribConstsProfile.clear();

	/*
	xml for consts should look like this  (same format in profile and theme)

	<constGroup>
		<const>
			<key>@something</key>
			<value>somethingelse</value>
		</const>
		<const>
			<key>@something</key>
			<value>somethingelse</value>
		</const>
	</constGroup>

	*/

	_profileManager->EnterOptionNode( "constGroup" );
	int numConsts = _profileManager->ContainsOption( "const" );
	for( int i = 0; i < numConsts; i++ )
	{
		if( _profileManager->EnterOptionNode( "const", i ) )
		{
			// We can now query our const replacements
			std::string key = _profileManager->GetString( "key", "@INVALID" );
			std::string value = _profileManager->GetString( "value", "@INVALID" );

			m_attribConstsProfile[ key ] = value;

			std::cout << "INFO: LoadProfileAttributeConsts, key: " << key << " value: " << value << std::endl;

			_profileManager->ExitOptionNode();
		}
	}
	_profileManager->ExitOptionNode();
}

//----------------------------------------------------------------------------------

void ShivaGUI::ResourceManager::SetTheme( std::string _filename )
{
	m_attribConstsTheme.clear();

	if( _filename.empty() )
	{
		delete m_currentThemeNode;
		m_currentThemeNode = NULL;
		return;
	}
	TiXmlDocument doc( _filename.c_str() );

	if( doc.LoadFile() )
	{
		delete m_currentThemeNode;
		m_currentThemeNode = NULL;
		// Grab a copy of the root element node
		m_currentThemeNode = ( TiXmlElement* ) doc.RootElement()->Clone();

		// Load consts list
		/*
		xml for consts should look like this (same format in profile and theme)

		<constGroup>
			<const>
				<key>@something</key>
				<value>somethingelse</value>
			</const>
			<const>
				<key>@something</key>
				<value>somethingelse</value>
			</const>
		</constGroup>

		*/
		if( m_currentThemeNode != NULL )
		{
			TiXmlElement *constContainerNode = m_currentThemeNode->FirstChildElement( "constGroup" );
			if( constContainerNode != NULL )
			{
				TiXmlNode *child = 0;
				while( child = constContainerNode->IterateChildren( "const", child ) )
				{
					TiXmlElement *keyElement = child->FirstChildElement( "key" );
					if( keyElement != NULL )
					{
						TiXmlElement *valueElement = child->FirstChildElement( "value" );
						if( valueElement != NULL )
						{
							std::string key = keyElement->GetText();//Value();
							std::string value = valueElement->GetText();//Value();
							m_attribConstsTheme[ key ] = value;

							//std::cout << "INFO: Load Theme Attribute Consts, key: " << key << " value: " << value << std::endl;
						}
					}
				}

				/*
				for( TiXmlAttribute *currentAttribute = constNode->FirstChildElement("Const"); currentAttribute != NULL; currentAttribute = currentAttribute->NextSiblingElement() )
				{
					std::string key = currentAttribute->Name();
					std::string value = currentAttribute->Value();
					_attribConstsTheme[key] = value;
				}
				*/
			}
		}
	}
	else
	{
		std::cerr << "WARNING: ResourceManager::SetTheme Could not load theme from xml: " << _filename << std::endl;
	}
}

//----------------------------------------------------------------------------------

ShivaGUI::ViewEventListener* ShivaGUI::ResourceManager::GetListener( std::string _name )
{
	if( m_GUIManager != NULL )
	{
		Activity *currentActivity = m_GUIManager->GetCurrentActivity();
		if( currentActivity != NULL )
		{
			GUIController *currentGUIController = currentActivity->GetGUIController( m_windowIndex );
			if( currentGUIController != NULL )
			{
				return currentGUIController->GetListener( _name );
			}
		}
	}
	std::cerr << "ShivaGUI::ResourceManager::GetListener could not retrieve current GUIController" << std::endl;
	return NULL;
}

//----------------------------------------------------------------------------------

ShivaGUI::View* ShivaGUI::ResourceManager::ParseLayoutElement( TiXmlNode *_parentNode, bool _rootNode )
{
	// Useful tinyxml links:
	// http://www.grinninglizard.com/tinyxmldocs/index.html
	// http://www.grinninglizard.com/tinyxmldocs/tutorial0.html
	 
	int nodeType = _parentNode->Type();

	switch( nodeType )
	{
		case TiXmlNode::TINYXML_DOCUMENT:
			for( TiXmlNode *childNode = _parentNode->FirstChild(); childNode != NULL; childNode = childNode->NextSibling() )
			{
				// We return the first element that is a valid view
				View *result = ParseLayoutElement( childNode, true );
				if( result != NULL )
					return result;
			}
			break;

		case TiXmlNode::TINYXML_ELEMENT:
			{
				View *parentView = CreateView( _parentNode->Value() );
				if( parentView != NULL )
				{
					// If we have a themes file, we must send this for inflation before we send the actual Layout node
					// This sets the View to the 'defaults' provided by the theme,
					// but also gives our Layout a chance to override anything it wants
					if( m_currentThemeNode != NULL )
						parentView->Inflate( m_currentThemeNode, this, "", _rootNode );
					parentView->Inflate( _parentNode->ToElement(), this );

					// Only check for children if View is a ViewGroup
					ViewGroup *parentContainer = dynamic_cast<ViewGroup*>( parentView );
					if( parentContainer != NULL )
					{
						for( TiXmlNode *childNode = _parentNode->FirstChild(); childNode != NULL; childNode = childNode->NextSibling())
						{
							View *childView = ParseLayoutElement( childNode );
							if( childView != NULL )
							{
								LayoutParams *childLayoutParams;
								if( m_currentThemeNode != NULL )
								{
									childLayoutParams = parentContainer->InflateLayoutParams( m_currentThemeNode, NULL, childView->GetThemePrefix() );
									parentContainer->InflateLayoutParams( childNode->ToElement(), childLayoutParams );
								}
								else
								{
									childLayoutParams = parentContainer->InflateLayoutParams( childNode->ToElement() );
								}

								if( childLayoutParams != NULL )
								{
									childView->SetLayoutParams( childLayoutParams );
									parentContainer->AddView( childView, this );
								}
								else
								{
									std::cerr << "WARNING: ResourceManager::ParseLayoutElement could not create LayoutParams for child node" << std::endl;
								}

							}
							else
							{
								std::cerr << "WARNING: ResourceManager::ParseLayoutElement could not parse layout element of container node" << std::endl;
							}
						}
					}

					return parentView;
				}
				else
				{
					std::cerr << "WARNING: ResourceManager::ParseLayoutElement could not create View" << std::endl;
				}
			}
			break;
	}

	return NULL;
}

//----------------------------------------------------------------------------------

ShivaGUI::Drawable* ShivaGUI::ResourceManager::ParseDrawableElement( TiXmlNode *_parentNode )
{
	int nodeType = _parentNode->Type();

	switch( nodeType )
	{
		case TiXmlNode::TINYXML_DOCUMENT:
			for( TiXmlNode *childNode = _parentNode->FirstChild(); childNode != NULL; childNode = childNode->NextSibling() )
			{
				// We return the first element that is a valid view
				Drawable *result = ParseDrawableElement( childNode );
				if( result != NULL )
					return result;
			}
			break;

		case TiXmlNode::TINYXML_ELEMENT:
		{
			Drawable *drawable = CreateDrawable( _parentNode->Value() );
			if( drawable != NULL )
			{
				drawable->Inflate( _parentNode->ToElement(), this );

				return drawable;
			}
			else
			{
				std::cerr << "WARNING: ResourceManager::ParseDrawableElement could not create Drawable" << std::endl;
			}
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------------

unsigned int ShivaGUI::ResourceManager::SDLSurfaceToOpenGL( SDL_Surface *_image, bool _mipmap, bool _addAlpha, bool _repeat, bool _reverseOrder )
{
	unsigned int texName = 0;
	glGenTextures( 1, &texName );
	SDLSurfaceToOpenGLID( _image, texName, _mipmap, _addAlpha, _repeat, _reverseOrder );
	return texName;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ResourceManager::SDLSurfaceToOpenGLID( SDL_Surface *_image, unsigned int _OpenGLTexID, bool _mipmap, bool _addAlpha, bool _repeat, bool _reverseOrder )
{

	GLenum type = GL_RGBA;

	if( _image->format->BitsPerPixel == 32 )
	{
		type = _reverseOrder ? GL_BGRA : GL_RGBA;
	}
	else if( _image->format->BitsPerPixel == 24 )
	{
		type = _reverseOrder ? GL_BGR : GL_RGB;
	}
	else
	{
		std::cerr << "WARNING: ResourceManager::SDLSurfaceToOpenGLID Image is not RGB or RGBA" << std::endl;
		//SDL_FreeSurface(image);
	}

	if( _OpenGLTexID != 0 )
	{
		glBindTexture( GL_TEXTURE_2D, _OpenGLTexID );

		if( _repeat )
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		}
		else
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
		}

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		if( _mipmap )
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		}
		else
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

		if( _addAlpha )
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, _image->w, _image->h, 0, type, GL_UNSIGNED_BYTE, _image->pixels );
		else
			glTexImage2D( GL_TEXTURE_2D, 0, _image->format->BitsPerPixel/8, _image->w, _image->h, 0, type, GL_UNSIGNED_BYTE, _image->pixels );

		if( _mipmap )
			if( gluBuild2DMipmaps( GL_TEXTURE_2D, _image->format->BitsPerPixel/8, _image->w, _image->h, type, GL_UNSIGNED_BYTE, _image->pixels ) != 0 )
				std::cerr << "WARNING: mipmap generation error" << std::endl;


		//if( image ) {
		//	SDL_FreeSurface( image );
		//}

		glBindTexture( GL_TEXTURE_2D, 0 );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ResourceManager::RegisterPostInflationLink( View *_src, std::string _dstID, bool _scanForward )
{
	m_postInflationLinks.push_back( PostInflationLink( _src, _dstID, _scanForward ) );
}

//----------------------------------------------------------------------------------

void ShivaGUI::ResourceManager::RegisterPostInflationLink( View *_src, std::string _dstID, Definitions::FocusDirection _focusDir )
{
	m_postInflationLinks.push_back( PostInflationLink( _src, _dstID, _focusDir) );
}

//----------------------------------------------------------------------------------

void ShivaGUI::ResourceManager::DoPostEvaluationLinks()
{
	for( std::vector< PostInflationLink >::iterator it = m_postInflationLinks.begin(); it != m_postInflationLinks.end(); ++it )
	{
		View *dstView = GetViewFromID( it->dstID );
		if( dstView != NULL && it->src != NULL )
		{
			if( it->scan )
			{
				it->src->SetNextScan( dstView, it->scanForward );
				dstView->SetNextScan( it->src, !it->scanForward );
			}
			if( it->focus )
			{
				it->src->SetNextFocus( dstView, it->focusDir );
				dstView->SetNextFocus( it->src, Definitions::GetOppositeFocusDirection( it->focusDir ) );
			}
		}
		else
		{
			std::cerr << "WARNING: ResourceManager::DoPostEvaluationLinks() cannot resolve Views, dstID = " << it->dstID << std::endl;
		}
	}
	m_postInflationLinks.clear();
}

//----------------------------------------------------------------------------------