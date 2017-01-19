///-----------------------------------------------------------------------------------------------
/// \file TextView.h
/// \brief This is the base widget class. It does not use pure virtual functions so that you can use it as an 'empty' View
/// \author Leigh McLoughlin, Michelle Wu
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUISYSTEM_TEXTVIEW__
#define __SHIVA_GUISYSTEM_TEXTVIEW__

#include "GUI/Views/View.h"
#include "GUI/Drawables/BitmapDrawable.h"

namespace ShivaGUI
{
	class TextView : public View
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Text alignment
		//----------------------------------------------------------------------------------
		enum Alignment
		{
			Left = 1,
			Centre = 2,
			Right = 3
		}; 
		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		TextView();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~TextView();
		//----------------------------------------------------------------------------------
		/// \brief This is called when the drawing context is changed
		/// All Drawables loaded from file will be automatically reloaded,
		/// but any locally-generated textures (e.g. text caches) must be remade here
		/// \param [in] resources
		//----------------------------------------------------------------------------------
		virtual void NotifyDrawingContextChange( ResourceManager *resources ) { BuildTextCache( resources ); }
		//----------------------------------------------------------------------------------
		/// \brief Gives the size of the View
		/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
		/// The base class function will set up the background drawable, so this must be called by derived View classes that wish to retain the background
		//----------------------------------------------------------------------------------
		virtual void Layout( int left, int top, int right, int bottom, int windowWidth, int windowHeight );
		//----------------------------------------------------------------------------------
		/// \brief The base class will draw a background drawable if it exists
		/// Derived View classes must call this at an appropriate time if they want to retain the background
		//----------------------------------------------------------------------------------
		virtual void Draw();
		//----------------------------------------------------------------------------------
		/// \brief Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		/// \return "TextView_"
		//----------------------------------------------------------------------------------
		virtual std::string GetThemePrefix() { return "TextView_"; }
		//----------------------------------------------------------------------------------
		/// \brief For setting the View's attributes from xml
		/// \param [in] xmlElement
		/// \param [in] resources
		/// \param [in] themePrefix this is used when parsing a theme file. Derived classes should specify the prefix that identifies their attributes in a style. If empty, the string returned from GetThemePrefix should be used
		/// \param [in] rootNode
		//----------------------------------------------------------------------------------
		virtual void Inflate( TiXmlElement*, ResourceManager*, std::string themePrefix = "", bool rootNode = false );
		//----------------------------------------------------------------------------------
		/// \brief For saving the View's attributes to xml
		/// \param [in] resources
		/// \note This must be hierarchical and the element must include all child elements
		//----------------------------------------------------------------------------------
		virtual TiXmlElement* Deflate( ResourceManager *resources );
		//----------------------------------------------------------------------------------
		/// \brief For changing the body text
		/// \param [in] text
		/// \param [in] resources
		//----------------------------------------------------------------------------------
		void SetText( std::string text, ShivaGUI::ResourceManager * );
		//----------------------------------------------------------------------------------
		/// \brief Requested wrap pixel dimensions for layout
		/// \note These are used when SetLayoutParams() is called, which will be after the View has been inflated
		//----------------------------------------------------------------------------------
		virtual int GetWrapWidth();
		//----------------------------------------------------------------------------------
		/// \brief Get wrap height
		//----------------------------------------------------------------------------------
		virtual int GetWrapHeight();
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief The text is converted to an image and stored in this drawable
		//----------------------------------------------------------------------------------
		BitmapDrawable *_textDrawable;
		//----------------------------------------------------------------------------------
		/// \brief The main body of text to display, as a string
		//----------------------------------------------------------------------------------
		std::string _textBody;
		//----------------------------------------------------------------------------------
		/// \brief Font to use
		//----------------------------------------------------------------------------------
		std::string _fontName;
		//----------------------------------------------------------------------------------
		/// \brief Font size
		//----------------------------------------------------------------------------------
		unsigned int _fontSize;
		//----------------------------------------------------------------------------------
		/// \brief Font colour
		//----------------------------------------------------------------------------------
		unsigned int _fontColour;
		//----------------------------------------------------------------------------------
		/// \brief Holds type of alignment for text (left/centre/right)
		//----------------------------------------------------------------------------------
		unsigned int _textAlignment;
		//----------------------------------------------------------------------------------
		/// Whether values were acquired from a theme file
		/// Mainly for use when deflating to xml
		//----------------------------------------------------------------------------------
		bool _fontNameFromTheme, _fontSizeFromTheme, _fontColourFromTheme, _textAlignFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief The OpenGL texture ID of the image that holds the text
		/// TODO: fix me. This *really* should not be here and is only here so it can be deleted properly
		//----------------------------------------------------------------------------------
		unsigned int _texID;
		//----------------------------------------------------------------------------------
		/// \brief Makes the texture from the text
		/// \param [in] resources
		//----------------------------------------------------------------------------------
		void BuildTextCache( ResourceManager *resources );
		//----------------------------------------------------------------------------------
	};
}

#endif
