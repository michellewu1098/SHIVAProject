///-----------------------------------------------------------------------------------------------
/// \file ResourceManager.h
/// \brief Manages Drawables, expands them from xml, loads bitmaps from file into OpenGL, converts text into a BitmapDrawable
/// and handles the sharing of OpenGL resources across Views and across Activities. 
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_RESOURCESYSTEM_RESOURCEMANAGER__
#define __SHIVA_RESOURCESYSTEM_RESOURCEMANAGER__

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <GL/glew.h>
#include <GL/glu.h>
#include <cml/cml.h>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

#include <vector>
#include <map>

#include "GUI/Definitions.h"
#include "System/ProfileManager.h"
#include "System/SharedPreferences.h"

namespace ShivaGUI
{
	// Forward declarations
	class Drawable;
	class GUIManager;
	class View;
	class ViewEventListener;
	class AudioManager;
	class AudioClip;

	/// These are per-window
	class ResourceManager
	{
	public:		
		
		//----------------------------------------------------------------------------------
		/// \brief Ctor 
		/// \param [in] _guiManager
		/// \param [in] _windowIndex
		//----------------------------------------------------------------------------------
		ResourceManager( GUIManager *_guiManager, unsigned int _windowIndex );
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~ResourceManager();
		//----------------------------------------------------------------------------------
		/// \brief Sets the ID for a View. This is now actually stored in the GUIController, this function will pass on the request to this
		/// This function is also in the ResourceManager because e.g. Views upon expansion are only given a ResourceManager, so it's easier to resolve this once
		/// \param [in] _view
		/// \param [in] _ID
		//----------------------------------------------------------------------------------
		void RegisterViewID( View*, std::string _ID );
		//----------------------------------------------------------------------------------
		/// \brief Retrieves a View from ID
		/// \param [in] _id View ID
		/// \return NULL if View is unknown
		//----------------------------------------------------------------------------------
		View* GetViewFromID( std::string _id );
		//----------------------------------------------------------------------------------
		/// \brief Set mipmap
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetMipmap( const bool &_value ) { m_mipmapImages = _value; }
		//----------------------------------------------------------------------------------

		// CREATION OF TEXTURES

		//----------------------------------------------------------------------------------
		/// \brief Loads an image (.png) from file
		/// \param [in] filename
		/// \return An OpenGL texture ID for the bitmap
		//----------------------------------------------------------------------------------
		//unsigned int GetBitmap( std::string _filename );
		//----------------------------------------------------------------------------------
		/// \brief Loads an image (.png) from file
		/// \param [in] _filename
		/// \param [in] _isWithText
		/// \return An OpenGL texture ID for the bitmap
		//----------------------------------------------------------------------------------
		unsigned int GetBitmap( std::string _filename, bool _isWithText );
		//----------------------------------------------------------------------------------
		/// \brief Creates text texture and returns it
		/// \param [in] _text Text content
		/// \param [in] _alignment Text alignment (left | centre | right)
		/// \param [in] _fontFilename Font name
		/// \param [in] _fontSize Font size
		/// \param [in] _fontColour Font colour
		//----------------------------------------------------------------------------------
		unsigned int GetText( std::string _text, unsigned int _alignment, std::string _fontFilename, unsigned int _fontSize, unsigned int _fontColour = 0 );
		//----------------------------------------------------------------------------------
		/// \brief Creates a text surface 
		/// \param [in] _text Text content
		/// \param [in] _alignment Text alignment (left | centre | right)
		/// \param [in] _fontFilename Font name
		/// \param [in] _fontSize Font size
		/// \param [in] _fontColour Font colour
		//----------------------------------------------------------------------------------
		SDL_Surface* GetTextSurface( std::string _text, unsigned int _alignment, std::string _fontFilename, unsigned int _fontSize, unsigned int _fontColour );
		//----------------------------------------------------------------------------------
		/// \brief Renders a simple text string to OpenGL texture, returning the OpenGL texture ID
		/// If the fontfilename is empty, it will attempt to use a default font
		/// \param [in] _text Text content
		/// \param [in] _fontFilename Font name
		/// \param [in] _fontSize Font size
		/// \param [in] _fontColour Font colour
		//----------------------------------------------------------------------------------
		unsigned int GetSimpleText( std::string _text, std::string _fontFilename, unsigned int _fontSize, unsigned int _fontColour = 0 );
		//----------------------------------------------------------------------------------
		/// \brief Set the info related to the text. If _text is not empty, it will set the
		/// rendering of the text to true so to create a LayeredImageDrawable with space for
		/// both icon and text
		/// \param [in] _text Text content
		/// \param [in] _fontName Font name
		/// \param [in] _fontSize Font size
		/// \param [in] _fontColour Font colour
		/// \param [in] _textAlign Text alignment (left | centre | right)
		//----------------------------------------------------------------------------------
		void SetTextInfo( const std::string &_text, const std::string &_fontName, const unsigned int &_fontSize, const unsigned int &_fontColour, const unsigned int &_textAlign );
		//----------------------------------------------------------------------------------
		/// \brief Used to clear the content of buttonText to render additional layers of LayeredImageDrawable
		//----------------------------------------------------------------------------------
		void ClearText() { m_buttonText.clear(); }
		//----------------------------------------------------------------------------------
		/// \brief Set flag of icon on side of text 
		/// \param [in] _value True or false
		//----------------------------------------------------------------------------------
		void SetIconOnLeft() { m_iconOnLeft = true; m_iconAbove = false; m_iconBelow = false; }
		//----------------------------------------------------------------------------------
		/// \brief Set flag of icon above text 
		/// \param [in] _value True or false
		//----------------------------------------------------------------------------------
		void SetIconAboveText() { m_iconAbove = true; m_iconOnLeft = false; m_iconBelow = false; }
		//----------------------------------------------------------------------------------
		/// \brief Set flag of icon below text 
		/// \param [in] _value True or false
		//----------------------------------------------------------------------------------
		void SetIconBelowText() { m_iconBelow = true; m_iconOnLeft = false; m_iconAbove = false; }
		//----------------------------------------------------------------------------------
		/// \brief Parse layout preferences from prefs set for each activity
		/// \param [in] _prefs Preferences
		//----------------------------------------------------------------------------------
		//void SetLayoutPrefs( SharedPreferences* _prefs );
		//----------------------------------------------------------------------------------
		/// \brief Set % of space occupied by icon in an image + text button/view
		/// \param [in] _value Percentage between 0.0 and 1.0
		//----------------------------------------------------------------------------------
		void SetIconPercentSize( const float &_value ) { m_iconPercentSize = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Set % of space occupied by text in an image + text button/view
		/// \param [in] _value Percentage between 0.0 and 1.0
		//----------------------------------------------------------------------------------
		void SetTextPercentSize( const float &_value ) { m_textPercentSize = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Get % of space occupied by icon in an image + text button/view
		/// \return m_iconPercentSize
		//----------------------------------------------------------------------------------
		float GetIconPercentSize() const { return m_iconPercentSize; }
		//----------------------------------------------------------------------------------
		/// \brief Get % of space occupied by text in an image + text button/view
		/// \return m_textPercentSize
		//----------------------------------------------------------------------------------
		float GetTextPercentSize() const { return m_textPercentSize; }
		//----------------------------------------------------------------------------------
		/// \brief Sets the default font
		/// \param [in] _filename Font name
		//----------------------------------------------------------------------------------
		void SetDefaultFont( std::string _filename ) { m_defaultFont = _filename; }
		//----------------------------------------------------------------------------------
		/// \brief Returns texture
		/// \param [in] _texName Texture name
		//----------------------------------------------------------------------------------
		unsigned int GetTexture( std::string _texName ) { return m_glTextures[ _texName ]; }
		//----------------------------------------------------------------------------------
		/// \brief Used to set whether we're rendering text to a texture or not
		/// \param [in] _value True or false
		//----------------------------------------------------------------------------------
		void RenderTextToTexture( const bool &_value ) { m_renderTextToTexture = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Returns if we're rendering text to a texture
		/// \return m_renderTextToTexture
		//----------------------------------------------------------------------------------
		bool IsRenderingText() const { return m_renderTextToTexture; }
		//----------------------------------------------------------------------------------

		// MANAGING TEXTURES

		//----------------------------------------------------------------------------------
		/// \brief Used when the OpenGL context has been invalidated and we need to reload all textures (Note that this cannot reload)
		//----------------------------------------------------------------------------------
		void ReloadTextures();
		//----------------------------------------------------------------------------------
		/// \brief Convenience function if you want to use an OpenGL texture across multiple Views or Activities
		/// \param [in] _name Texture name
		/// \param [in] _texID Texture ID
		/// \return TRUE if this is a new texture
		//----------------------------------------------------------------------------------
		bool GetManagedTexture( std::string _name, unsigned int *_texID );
		//----------------------------------------------------------------------------------
		/// \brief Remove texture from OpenGL
		/// \param [in] _name Texture name
		//----------------------------------------------------------------------------------
		void ClearManagedTexture( std::string _name );
		//----------------------------------------------------------------------------------


		//----------------------------------------------------------------------------------
		/// \brief Loads a Drawable from resource file. Most drawables are xml files
		/// If the filename given is a bitmap, this function will wrap it in a BitmapDrawable
		/// \param [in] _filename
		//----------------------------------------------------------------------------------
		Drawable* GetDrawable( std::string _filename );
		//----------------------------------------------------------------------------------
		/// \brief Factory method for creating a Drawable, mainly for use by xml parsing
		/// \param [in] _drawableName
		//----------------------------------------------------------------------------------
		Drawable* CreateDrawable( std::string _drawableName );
		//----------------------------------------------------------------------------------
		/// \brief Loads a layout hierarchy from an xml file. It will first look for it in the active Profile directory, then the normal Layout directory
		/// \param [in] _filename
		//----------------------------------------------------------------------------------
		View* GetLayout( std::string _filename );
		//----------------------------------------------------------------------------------
		/// \brief For collapsing a View hierarchy to xml
		/// \param [in] _filename
		/// \param [in] _rootNode
		//----------------------------------------------------------------------------------
		bool OutputLayout( std::string _filename, View *_rootNode );
		//----------------------------------------------------------------------------------
		/// \brief Factory method for creating a View, mainly for use by xml parsing
		/// \param [in] _viewName
		//----------------------------------------------------------------------------------
		View* CreateView( std::string _viewName );
		//----------------------------------------------------------------------------------
		/// \brief Get audio clip
		/// \param [in] _filename Audio clip file name
		//----------------------------------------------------------------------------------
		AudioClip* GetAudioClip( std::string _filename );
		//----------------------------------------------------------------------------------
		/// \brief Attributes are retrieved during inflation which can refer to constants
		/// This function retrieves constant values, or returns the original attribute if no replacement is required
		/// Constant names are expected to start with the character '@'
		/// Constants in Profiles override those in Themes
		/// Views should use this function whenever they read properties from XML
		/// Drawables should also use this function when reading properties from XML
		/// \param [in] _originalAttrib
		//----------------------------------------------------------------------------------
		std::string GetInflationAttribute( std::string _originalAttrib );
		//----------------------------------------------------------------------------------
		/// \brief Load profile attribute constants
		/// \param [in] _profileManager Profile Manager
		//----------------------------------------------------------------------------------
		void LoadProfileAttributeConsts( ProfileManager* _profileManager );
		//----------------------------------------------------------------------------------
		/// \brief Sets the current theme
		/// Note that it does not apply it to existing Layouts / Views - to do this you should restart the Activity or application
		/// \param [in] _filename 
		//----------------------------------------------------------------------------------
		void SetTheme( std::string _filename );
		//----------------------------------------------------------------------------------
		/// \brief Retrieves the Listener from the current Activity's connected GUIController
		/// \param [in] _name
		//----------------------------------------------------------------------------------
		ViewEventListener* GetListener( std::string _name );
		//----------------------------------------------------------------------------------
		/// \brief During the Layout inflation process, Views often want connections between each other for scan and focus links
		/// However, not all Views will have been loaded yet, so their IDs will be unknown
		/// If that is the case, this function can be used to queue up a link for evaluation after the Layout has been fully loaded
		/// \param [in] _src
		/// \param [in] _dstID
		/// \param [in] _scanForward
		//----------------------------------------------------------------------------------
		void RegisterPostInflationLink( View *_src, std::string _dstID, bool _scanForward );
		//----------------------------------------------------------------------------------
		/// \param [in] _src
		/// \param [in] _dstID
		/// \param [in] _focusDir
		//----------------------------------------------------------------------------------
		void RegisterPostInflationLink( View *_src, std::string _dstID, Definitions::FocusDirection _focusDir );
		//----------------------------------------------------------------------------------
		//----------------------------------------------------------------------------------
		/// \brief Get button name
		//----------------------------------------------------------------------------------
		std::string GetButtonName() const { return m_buttonText; }

	protected:

		//----------------------------------------------------------------------------------
		/// \brief GUI Manager instance
		//----------------------------------------------------------------------------------
		GUIManager *m_GUIManager;
		//----------------------------------------------------------------------------------
		/// \brief Windox index
		//----------------------------------------------------------------------------------
		unsigned int m_windowIndex;
		//----------------------------------------------------------------------------------
		/// \brief Controls whether mipmaps are automatically generated for loaded images (Default value is true)
		//----------------------------------------------------------------------------------
		bool m_mipmapImages;
		//----------------------------------------------------------------------------------
		/// \brief Recursively parses an xml layout tree
		/// \param [in] xmlNode
		/// \param [in] rootNode
		//----------------------------------------------------------------------------------
		View* ParseLayoutElement( TiXmlNode*, bool rootNode = false );
		//----------------------------------------------------------------------------------
		/// \brief Parses an xml drawable tree
		/// \param [in] xmlNode
		//----------------------------------------------------------------------------------
		Drawable* ParseDrawableElement( TiXmlNode* );
		//----------------------------------------------------------------------------------
		// TODO: properly share openGL texIDs, using smart pointers, so can delete ones no longer in use
		//----------------------------------------------------------------------------------
		/// \brief Textures
		//----------------------------------------------------------------------------------
		std::map< std::string, unsigned int > m_glTextures;
		//----------------------------------------------------------------------------------
		/// \brief External textures
		//----------------------------------------------------------------------------------
		std::map< std::string, unsigned int > m_glExternalTextures;
		//----------------------------------------------------------------------------------
		/// \brief Constants for use when looking up xml attributes from Theme
		//----------------------------------------------------------------------------------
		std::map< std::string, std::string > m_attribConstsTheme;
		//----------------------------------------------------------------------------------
		/// \brief Constants for use when looking up xml attributes from Profile
		//----------------------------------------------------------------------------------
		std::map< std::string, std::string > m_attribConstsProfile;
		//----------------------------------------------------------------------------------
		/// \brief Current theme node
		//----------------------------------------------------------------------------------
		TiXmlElement *m_currentThemeNode;
		//----------------------------------------------------------------------------------
		/// \brief Utility function for converting an SDL surface to an OpenGL texture
		/// \param [in] image
		/// \param [in] mipmap
		/// \param [in] addAlpha
		/// \param [in] repeat
		/// \param [in] reverseOrder
		/// \note This function does not free the SDL surface
		//----------------------------------------------------------------------------------
		unsigned int SDLSurfaceToOpenGL( SDL_Surface *image, bool mipmap = false, bool addAlpha = false, bool repeat = false, bool reverseOrder = false );
		//----------------------------------------------------------------------------------
		/// \brief Utility function for putting an SDL Surface in a specific OpenGL texture ID
		/// \param [in] image
		/// \param [in] OpenGLTexID
		/// \param [in] mipmap
		/// \param [in] addAlpha
		/// \param [in] repeat
		/// \param [in] reverseOrder
		//----------------------------------------------------------------------------------
		void SDLSurfaceToOpenGLID( SDL_Surface *image, unsigned int OpenGLTexID, bool mipmap = false, bool addAlpha = false, bool repeat = false, bool reverseOrder = false );
		//----------------------------------------------------------------------------------
		/// \brief For storing loaded fonts
		/// \note The size is also included in the string key
		//----------------------------------------------------------------------------------
		std::map< std::string, TTF_Font* > m_fontcache;
		//----------------------------------------------------------------------------------
		/// \brief For loading a font
		//----------------------------------------------------------------------------------
		TTF_Font* LoadFont( std::string filename, unsigned int size );
		//----------------------------------------------------------------------------------
		/// \brief Default font
		//----------------------------------------------------------------------------------
		std::string m_defaultFont;
		//----------------------------------------------------------------------------------
		
		// TEXT/FONT INFO FROM IMAGETEXTBUTTON

		//----------------------------------------------------------------------------------
		/// \brief Text to be displayed on button
		//----------------------------------------------------------------------------------
		std::string m_buttonText;
		//----------------------------------------------------------------------------------
		/// \brief Font used to render text on button
		//----------------------------------------------------------------------------------
		std::string m_buttonFontName;
		//----------------------------------------------------------------------------------
		/// \brief Font size used to render text on button
		//----------------------------------------------------------------------------------
		unsigned int m_buttonFontSize;
		//----------------------------------------------------------------------------------
		/// \brief Font colour used to render text on button
		//----------------------------------------------------------------------------------
		unsigned int m_buttonFontColour;
		//----------------------------------------------------------------------------------
		/// \brief Text alignment (left | centre | right)
		//----------------------------------------------------------------------------------
		unsigned int m_buttonTextAlign;
		//----------------------------------------------------------------------------------
		/// \brief Flag used to check if icon is on left side of text in final texture
		//----------------------------------------------------------------------------------
		bool m_iconOnLeft;
		//----------------------------------------------------------------------------------
		/// \brief Flag used to check if icon is above text in final texture
		//----------------------------------------------------------------------------------
		bool m_iconAbove;
		//----------------------------------------------------------------------------------
		/// \brief Flag used to check if icon is below text in final texture
		//----------------------------------------------------------------------------------
		bool m_iconBelow;
		//----------------------------------------------------------------------------------
		/// \brief Flag used to check if it is rendering text
		//----------------------------------------------------------------------------------
		bool m_isRenderingText;
		//----------------------------------------------------------------------------------
		/// \brief Flag used to render the text onto a texture only once
		//----------------------------------------------------------------------------------
		bool m_renderTextToTexture;
		//----------------------------------------------------------------------------------
		/// \brief % of button space that should be occupied by icon
		//----------------------------------------------------------------------------------
		float m_iconPercentSize;
		//----------------------------------------------------------------------------------
		/// \brief Default icon % size
		//----------------------------------------------------------------------------------
		float m_defaultIconPercentSize;
		//----------------------------------------------------------------------------------
		/// \brief % of button space that should be occupied by text
		//----------------------------------------------------------------------------------
		float m_textPercentSize;
		//----------------------------------------------------------------------------------
		/// \brief Default text % size
		//----------------------------------------------------------------------------------
		float m_defaultTextPercentSize;
		//----------------------------------------------------------------------------------
		/// \brief During inflation of Layout links need to be made between Views for scanning and selection order purposes
		/// The Views they need to link to may not have been created yet, so we save the requests using instances of this sub-class and deal with them after inflation
		//----------------------------------------------------------------------------------
		class PostInflationLink
		{
		public:

			//----------------------------------------------------------------------------------
			/// \brief Ctor 
			/// \param [in] inSrc
			/// \param [in] inDstID
			/// \param [in] inScanForward
			//----------------------------------------------------------------------------------
			PostInflationLink( View *inSrc, std::string inDstID, bool inScanForward ) : m_src( inSrc ), m_dstID( inDstID ), m_scan( true ), m_scanForward( inScanForward ), m_focus( false ), m_focusDir( Definitions::INVALID ) {}
			//----------------------------------------------------------------------------------
			/// \brief Ctor 
			/// \param [in] inSrc
			/// \param [in] inDstID
			/// \param [in] inFocusDir
			//----------------------------------------------------------------------------------
			PostInflationLink( View *inSrc, std::string inDstID, Definitions::FocusDirection inFocusDir ) : m_src( inSrc ), m_dstID( inDstID ), m_scan( false ), m_scanForward( false ), m_focus( true ), m_focusDir( inFocusDir ) {}
			//----------------------------------------------------------------------------------
			/// \brief Sourcs
			//----------------------------------------------------------------------------------
			View *m_src;
			//----------------------------------------------------------------------------------
			/// \brief Destination ID
			//----------------------------------------------------------------------------------
			std::string m_dstID;
			//----------------------------------------------------------------------------------
			/// \brief Scan?
			//----------------------------------------------------------------------------------
			bool m_scan;
			//----------------------------------------------------------------------------------
			/// \brief Scan forward?
			//----------------------------------------------------------------------------------
			bool m_scanForward;
			//----------------------------------------------------------------------------------
			/// \brief Focus?
			//----------------------------------------------------------------------------------
			bool m_focus;
			//----------------------------------------------------------------------------------
			/// \brief Focus direction
			//----------------------------------------------------------------------------------
			Definitions::FocusDirection m_focusDir;
			//----------------------------------------------------------------------------------
		};

		//----------------------------------------------------------------------------------
		/// \brief Post inflations links
		//----------------------------------------------------------------------------------
		std::vector< PostInflationLink > m_postInflationLinks;
		//----------------------------------------------------------------------------------
		/// \brief Do post evaluation links
		//----------------------------------------------------------------------------------
		void DoPostEvaluationLinks();
		//----------------------------------------------------------------------------------
	};
}

#endif
