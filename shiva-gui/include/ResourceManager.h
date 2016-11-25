//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_RESOURCESYSTEM_RESOURCEMANAGER__
#define __SHIVA_RESOURCESYSTEM_RESOURCEMANAGER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <string>
#include <vector>
#include <map>

//////////////////////////////////////////////////////////////////////////
#include "Utility/tinyxml.h"
#include "System/SharedPreferences.h"
#include "GUI/Definitions.h"
#include "System/ProfileManager.h"

//////////////////////////////////////////////////////////////////////////

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
		ResourceManager(GUIManager*, unsigned int windowIndex);
		~ResourceManager();


		/// Sets the ID for a View
		/// This is now actually stored in the GUIController, this function will pass on the request to this
		/// This function is also in the ResourceManager because e.g. Views upon expansion are only given a ResourceManager, so it's easier to resolve this once
		void RegisterViewID(View*, std::string ID);
		/// Retrieves a View from ID
		/// \return NULL if View is unknown
		View* GetViewFromID(std::string ID);


		void SetMipmap(bool value) {_mipmapImages=value;}

		/// Used when the OpenGL context has been invalidated and we need to reload all textures
		/// Note that this cannot reload 
		void ReloadTextures();

		/// Convenience function for if you want to use an OpenGL texture across multiple Views or Activities
		/// Returns true if this is a new texture
		bool GetManagedTexture(std::string name, unsigned int *texID);

		/// Will also remove texture from OpenGL
		void ClearManagedTexture(std::string name);

		/// Loads an image (.png) from file
		/// \return An OpenGL texture ID for the bitmap
		unsigned int GetBitmap(std::string filename);

		/// Renders a simple text string to OpenGL texture, returning the OpenGL texture ID
		/// If the fontfilename is empty, it will attempt to use a default font
		unsigned int GetSimpleText(std::string text, std::string fontfilename, unsigned int fontsize, unsigned int fontColour = 0);

		/// Loads a Drawable from resource file
		/// Most drawables are xml files
		/// If the filename given is a bitmap, this function will wrap it in a BitmapDrawable
		Drawable* GetDrawable(std::string filename);

		/// Loads a layout hierarchy from an xml file
		/// It will first look for it in the active Profile directory, then the normal Layout directory
		View* GetLayout(std::string filename);

		/// For collapsing a View hierarchy to xml
		bool OutputLayout(std::string filename, View *rootNode);


		/// Factory method for creating a Drawable, mainly for use by xml parsing
		Drawable* CreateDrawable(std::string drawableName);

		/// Factory method for creating a View, mainly for use by xml parsing
		View* CreateView(std::string viewName);

		AudioClip* GetAudioClip(std::string filename);

		/// Attributes are retrieved during inflation which can refer to constants
		/// This function retrieves constant values, or returns the original attribute if no replacement is required
		/// Constant names are expected to start with the character '@'
		/// Constants in Profiles override those in Themes
		/// Views should use this function whenever they read properties from XML
		/// Drawables should also use this function when reading properties from XML
		std::string GetInflationAttribute(std::string originalAttrib);

		void LoadProfileAttributeConsts(ProfileManager*);

		/// Sets the current theme
		/// Note that it does not apply it to existing Layouts / Views - to do this you should restart the Activity or application
		void SetTheme(std::string filename);

		/// Sets the default font
		void SetDefaultFont(std::string filename) {_defaultFont=filename;}

		/// Retrieves the Listener from the current Activity's connected GUIController
		ViewEventListener* GetListener(std::string name);

		/// During the Layout inflation process, Views often want connections between each other for scan and focus links
		/// However, not all Views will have been loaded yet, so their IDs will be unknown
		/// If that is the case, this function can be used to queue up a link for evaluation after the Layout has been fully loaded
		void RegisterPostInflationLink( View *src, std::string dstID, bool scanForward);
		void RegisterPostInflationLink( View *src, std::string dstID, Definitions::FocusDirection focusDir);

	protected:

		GUIManager *_GUIManager;
		unsigned int _windowIndex;

		/// Controls whether mipmaps are automatically generated for loaded images
		/// Default value is true
		bool _mipmapImages;


		/// Recursively parses an xml layout tree
		View* ParseLayoutElement( TiXmlNode*, bool rootNode = false );

		/// Parses an xml drawable tree
		Drawable* ParseDrawableElement( TiXmlNode* );

		/// TODO: properly share openGL texIDs, using smart pointers, so can delete ones no longer in use
		std::map<std::string,unsigned int> _glTextures;
		std::map<std::string,unsigned int> _glExternalTextures;


		/// Constants for use when looking up xml attributes
		std::map<std::string,std::string> _attribConstsTheme;
		std::map<std::string,std::string> _attribConstsProfile;

		TiXmlElement *_currentThemeNode;

		/// Utility function for converting an SDL surface to an OpenGL texture
		/// \note This function does not free the SDL surface
		unsigned int SDLSurfaceToOpenGL(SDL_Surface *image, bool mipmap = false, bool addAlpha = false, bool repeat = false, bool reverseOrder = false );

		/// Utility function for putting an SDL Surface in a specific OpenGL texture ID
		void SDLSurfaceToOpenGLID(SDL_Surface *image, unsigned int OpenGLTexID, bool mipmap = false, bool addAlpha = false, bool repeat = false, bool reverseOrder = false );

		/// For storing loaded fonts
		/// \note The size is also included in the string key
		std::map< std::string, TTF_Font*> _fontcache;

		/// For loading a font
		TTF_Font* LoadFont(std::string filename, unsigned int size);

		std::string _defaultFont;

		/// During inflation of Layout links need to be made between Views for scanning and selection order purposes
		/// The Views they need to link to may not have been created yet, so we save the requests using instances of this sub-class and deal with them after inflation
		class PostInflationLink
		{
		public:
			PostInflationLink(View *inSrc, std::string inDstID, bool inScanForward) : src(inSrc), dstID(inDstID), scan(true), scanForward(inScanForward), focus(false), focusDir(Definitions::INVALID) {}
			PostInflationLink(View *inSrc, std::string inDstID, Definitions::FocusDirection inFocusDir) : src(inSrc), dstID(inDstID), scan(false), scanForward(false), focus(true), focusDir(inFocusDir) {}
			View *src;
			std::string dstID;

			bool scan;
			bool scanForward;

			bool focus;
			Definitions::FocusDirection focusDir;
		};

		std::vector< PostInflationLink > _postInflationLinks;

		void DoPostEvaluationLinks();
	};
}

//////////////////////////////////////////////////////////////////////////
#endif
