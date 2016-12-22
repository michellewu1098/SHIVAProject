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

#include <GL/GLee.h>
#include <GL/glu.h>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

#include <string>
#include <vector>
#include <map>

#include "GUI/Definitions.h"
#include "System/ProfileManager.h"
#include "System/SharedPreferences.h"
#include "Utility/tinyxml.h"

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
		/// \param [in] 
		/// \param [in] windowIndex
		//----------------------------------------------------------------------------------
		ResourceManager( GUIManager*, unsigned int windowIndex );
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~ResourceManager();
		//----------------------------------------------------------------------------------
		/// \brief Sets the ID for a View. This is now actually stored in the GUIController, this function will pass on the request to this
		/// This function is also in the ResourceManager because e.g. Views upon expansion are only given a ResourceManager, so it's easier to resolve this once
		/// \param [in] 
		/// \param [in] ID
		//----------------------------------------------------------------------------------
		void RegisterViewID( View*, std::string ID );
		//----------------------------------------------------------------------------------
		/// \brief Retrieves a View from ID
		/// \return NULL if View is unknown
		//----------------------------------------------------------------------------------
		View* GetViewFromID( std::string ID );
		//----------------------------------------------------------------------------------
		/// \brief Set mipmap
		//----------------------------------------------------------------------------------
		void SetMipmap( bool value ) { _mipmapImages = value; }
		//----------------------------------------------------------------------------------
		/// \brief Used when the OpenGL context has been invalidated and we need to reload all textures (Note that this cannot reload)
		//----------------------------------------------------------------------------------
		void ReloadTextures();
		//----------------------------------------------------------------------------------
		/// \brief Convenience function if you want to use an OpenGL texture across multiple Views or Activities
		/// \return TRUE if this is a new texture
		bool GetManagedTexture( std::string name, unsigned int *texID );
		//----------------------------------------------------------------------------------
		/// \brief Remove texture from OpenGL
		/// \param [in] name
		//----------------------------------------------------------------------------------
		void ClearManagedTexture( std::string name );
		//----------------------------------------------------------------------------------
		/// \brief Loads an image (.png) from file
		/// \param [in] filename
		/// \return An OpenGL texture ID for the bitmap
		//----------------------------------------------------------------------------------
		unsigned int GetBitmap( std::string filename );
		//----------------------------------------------------------------------------------
		/// \brief Renders a simple text string to OpenGL texture, returning the OpenGL texture ID
		/// If the fontfilename is empty, it will attempt to use a default font
		/// \param [in] text
		/// \param [in] fontfilename
		/// \param [in] fontsize
		/// \param [in] fontColour
		//----------------------------------------------------------------------------------
		unsigned int GetSimpleText( std::string text, std::string fontfilename, unsigned int fontsize, unsigned int fontColour = 0 );
		//----------------------------------------------------------------------------------
		/// \brief Loads a Drawable from resource file. Most drawables are xml files
		/// If the filename given is a bitmap, this function will wrap it in a BitmapDrawable
		/// \param [in] filename
		Drawable* GetDrawable( std::string filename );
		//----------------------------------------------------------------------------------
		/// \brief Loads a layout hierarchy from an xml file. It will first look for it in the active Profile directory, then the normal Layout directory
		/// \param [in] filename
		//----------------------------------------------------------------------------------
		View* GetLayout( std::string filename );
		//----------------------------------------------------------------------------------
		/// \brief For collapsing a View hierarchy to xml
		/// \param [in] filename
		/// \param [in] rootNode
		//----------------------------------------------------------------------------------
		bool OutputLayout( std::string filename, View *rootNode );
		//----------------------------------------------------------------------------------
		/// \brief Factory method for creating a Drawable, mainly for use by xml parsing
		/// \param drawableName
		//----------------------------------------------------------------------------------
		Drawable* CreateDrawable( std::string drawableName );
		//----------------------------------------------------------------------------------
		/// \brief Factory method for creating a View, mainly for use by xml parsing
		/// \param [in] viewName
		//----------------------------------------------------------------------------------
		View* CreateView( std::string viewName );
		//----------------------------------------------------------------------------------
		/// \brief Get audio clip
		/// \param [in] filename
		//----------------------------------------------------------------------------------
		AudioClip* GetAudioClip( std::string filename );
		//----------------------------------------------------------------------------------
		/// \brief Attributes are retrieved during inflation which can refer to constants
		/// This function retrieves constant values, or returns the original attribute if no replacement is required
		/// Constant names are expected to start with the character '@'
		/// Constants in Profiles override those in Themes
		/// Views should use this function whenever they read properties from XML
		/// Drawables should also use this function when reading properties from XML
		/// \param [in] originalAttrib
		//----------------------------------------------------------------------------------
		std::string GetInflationAttribute( std::string originalAttrib );
		//----------------------------------------------------------------------------------
		/// \brief Load profile attribute constants
		/// \param [in] 
		void LoadProfileAttributeConsts( ProfileManager* );
		//----------------------------------------------------------------------------------
		/// \brief Sets the current theme
		/// Note that it does not apply it to existing Layouts / Views - to do this you should restart the Activity or application
		/// \param [in] filename
		//----------------------------------------------------------------------------------
		void SetTheme( std::string filename );
		//----------------------------------------------------------------------------------
		/// \brief Sets the default font
		/// \param [in] filename
		//----------------------------------------------------------------------------------
		void SetDefaultFont( std::string filename ) { _defaultFont = filename; }
		//----------------------------------------------------------------------------------
		/// \brief Retrieves the Listener from the current Activity's connected GUIController
		/// \param [in] name
		//----------------------------------------------------------------------------------
		ViewEventListener* GetListener( std::string name );
		//----------------------------------------------------------------------------------
		/// \brief During the Layout inflation process, Views often want connections between each other for scan and focus links
		/// However, not all Views will have been loaded yet, so their IDs will be unknown
		/// If that is the case, this function can be used to queue up a link for evaluation after the Layout has been fully loaded
		/// \param [in] src
		/// \param [in] dstID
		/// \param [in] scanForward
		//----------------------------------------------------------------------------------
		void RegisterPostInflationLink( View *src, std::string dstID, bool scanForward );
		//----------------------------------------------------------------------------------
		/// \param [in] src
		/// \param [in] dstID
		/// \param [in] focusDir
		//----------------------------------------------------------------------------------
		void RegisterPostInflationLink( View *src, std::string dstID, Definitions::FocusDirection focusDir );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief GUI Manager instance
		//----------------------------------------------------------------------------------
		GUIManager *_GUIManager;
		//----------------------------------------------------------------------------------
		/// \brief Windox index
		//----------------------------------------------------------------------------------
		unsigned int _windowIndex;
		//----------------------------------------------------------------------------------
		/// \brief Controls whether mipmaps are automatically generated for loaded images (Default value is true)
		//----------------------------------------------------------------------------------
		bool _mipmapImages;
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
		std::map< std::string, unsigned int > _glTextures;
		//----------------------------------------------------------------------------------
		std::map< std::string, unsigned int > _glExternalTextures;
		//----------------------------------------------------------------------------------
		/// \brief Constants for use when looking up xml attributes
		//----------------------------------------------------------------------------------
		std::map< std::string, std::string > _attribConstsTheme;
		//----------------------------------------------------------------------------------
		std::map< std::string, std::string > _attribConstsProfile;
		//----------------------------------------------------------------------------------
		/// \brief Current theme node
		//----------------------------------------------------------------------------------
		TiXmlElement *_currentThemeNode;
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
		std::map< std::string, TTF_Font* > _fontcache;
		//----------------------------------------------------------------------------------
		/// \brief For loading a font
		//----------------------------------------------------------------------------------
		TTF_Font* LoadFont( std::string filename, unsigned int size );
		//----------------------------------------------------------------------------------
		/// \brief Default font
		//----------------------------------------------------------------------------------
		std::string _defaultFont;
		//----------------------------------------------------------------------------------
		
		/// \brief During inflation of Layout links need to be made between Views for scanning and selection order purposes
		/// The Views they need to link to may not have been created yet, so we save the requests using instances of this sub-class and deal with them after inflation
		
		class PostInflationLink
		{
		public:

			//----------------------------------------------------------------------------------
			/// \brief Ctor 
			/// \param [in] inSrc
			/// \param [in] inDstID
			/// \param [in] inScanForward
			//----------------------------------------------------------------------------------
			PostInflationLink( View *inSrc, std::string inDstID, bool inScanForward ) : src( inSrc ), dstID( inDstID ), scan( true ), scanForward( inScanForward ), focus( false ), focusDir( Definitions::INVALID ) {}
			//----------------------------------------------------------------------------------
			/// \brief Ctor 
			/// \param [in] inSrc
			/// \param [in] inDstID
			/// \param [in] inFocusDir
			//----------------------------------------------------------------------------------
			PostInflationLink( View *inSrc, std::string inDstID, Definitions::FocusDirection inFocusDir ) : src( inSrc ), dstID( inDstID ), scan( false ), scanForward( false ), focus( true ), focusDir( inFocusDir ) {}
			//----------------------------------------------------------------------------------
			View *src;
			//----------------------------------------------------------------------------------
			std::string dstID;
			//----------------------------------------------------------------------------------
			bool scan;
			//----------------------------------------------------------------------------------
			bool scanForward;
			//----------------------------------------------------------------------------------
			bool focus;
			//----------------------------------------------------------------------------------
			Definitions::FocusDirection focusDir;
			//----------------------------------------------------------------------------------
		};

		//----------------------------------------------------------------------------------
		std::vector< PostInflationLink > _postInflationLinks;
		//----------------------------------------------------------------------------------
		void DoPostEvaluationLinks();
		//----------------------------------------------------------------------------------
	};
}

#endif
