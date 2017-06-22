///-----------------------------------------------------------------------------------------------
/// \file TextView.h
/// \brief This is the base widget class. It does not use pure virtual functions so that you can use it as an 'empty' View
/// \author Leigh McLoughlin, Michelle Wu
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUISYSTEM_IMAGETEXTVIEW__
#define __SHIVA_GUISYSTEM_IMAGETEXTVIEW__

#include "GUI/Views/View.h"
#include "GUI/Drawables/BitmapDrawable.h"
#include "GUI/Drawables/LayeredImageDrawable.h"

namespace ShivaGUI
{
	class ImageTextView : public View
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Text alignment
		//----------------------------------------------------------------------------------
		enum Alignment
		{
			Left = 1,
			Centre = 2,
			Right = 3,
			Top = 4,
			Bottom = 5
		}; 
		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		ImageTextView();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~ImageTextView();
		//----------------------------------------------------------------------------------
		/// \brief This is called when the drawing context is changed
		/// All Drawables loaded from file will be automatically reloaded,
		/// but any locally-generated textures (e.g. text caches) must be remade here
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual void NotifyDrawingContextChange( ResourceManager *_resources ) { BuildTextCache( _resources ); }
		//----------------------------------------------------------------------------------
		/// \brief Gives the size of the View
		/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
		/// The base class function will set up the background drawable, so this must be called by derived View classes that wish to retain the background
		//----------------------------------------------------------------------------------
		virtual void Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight );
		//----------------------------------------------------------------------------------
		/// \brief The base class will draw a background drawable if it exists
		/// Derived View classes must call this at an appropriate time if they want to retain the background
		//----------------------------------------------------------------------------------
		virtual void Draw();
		//----------------------------------------------------------------------------------
		/// \brief The base class will draw a background drawable if it exists
		/// Derived View classes must call this at an appropriate time if they want to retain the background
		/// \param [in] _context Current OGL context
		//----------------------------------------------------------------------------------
		virtual void Draw( unsigned int _context );
		//----------------------------------------------------------------------------------
		/// \brief Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		/// \return "TextView_"
		//----------------------------------------------------------------------------------
		virtual std::string GetThemePrefix() { return "ImageTextView_"; }
		//----------------------------------------------------------------------------------
		/// \brief For setting the View's attributes from xml
		/// \param [in] _xmlElement
		/// \param [in] _resources
		/// \param [in] _themePrefix this is used when parsing a theme file. Derived classes should specify the prefix that identifies their attributes in a style. If empty, the string returned from GetThemePrefix should be used
		/// \param [in] _rootNode
		//----------------------------------------------------------------------------------
		virtual void Inflate( TiXmlElement* _xmlElement, ResourceManager* _resources, std::string _themePrefix = "", bool _rootNode = false );
		//----------------------------------------------------------------------------------
		/// \brief For saving the View's attributes to xml
		/// \param [in] _resources
		/// \note This must be hierarchical and the element must include all child elements
		//----------------------------------------------------------------------------------
		virtual TiXmlElement* Deflate( ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief For changing the body text
		/// \param [in] _text
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		void SetText( std::string _text, ShivaGUI::ResourceManager *_resources );
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
		/// \brief Set content drawable
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		virtual void SetContent( Drawable *_value );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief The text is converted to an image and stored in this drawable
		//----------------------------------------------------------------------------------
		BitmapDrawable *m_textDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Content general drawable
		//----------------------------------------------------------------------------------
		Drawable *m_contentGenDrawable;	
		//----------------------------------------------------------------------------------
		/// \brief The main body of text to display, as a string
		//----------------------------------------------------------------------------------
		std::string m_textBody;
		//----------------------------------------------------------------------------------
		/// \brief Font to use
		//----------------------------------------------------------------------------------
		std::string m_fontName;
		//----------------------------------------------------------------------------------
		/// \brief Font size
		//----------------------------------------------------------------------------------
		unsigned int m_fontSize;
		//----------------------------------------------------------------------------------
		/// \brief Font colour
		//----------------------------------------------------------------------------------
		unsigned int m_fontColour;
		//----------------------------------------------------------------------------------
		/// \brief Holds type of alignment for text (left/centre/right)
		//----------------------------------------------------------------------------------
		unsigned int m_textAlignment;
		//----------------------------------------------------------------------------------
		/// \brief Holds position for icon (left/top)
		//----------------------------------------------------------------------------------
		unsigned int m_iconPosition;
		//----------------------------------------------------------------------------------
		// Whether values were acquired from a theme file. Mainly for use when deflating to xml
		//----------------------------------------------------------------------------------
		/// \brief Whether font name was acquired from theme file 
		//----------------------------------------------------------------------------------
		bool m_fontNameFromTheme;
		//----------------------------------------------------------------------------------
		/// /brief Whether font size was acquired from theme file
		//----------------------------------------------------------------------------------
		bool m_fontSizeFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Whether font colour was acquired from theme file
		//----------------------------------------------------------------------------------
		bool m_fontColourFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Whether text alignment was acquired from theme file
		//----------------------------------------------------------------------------------
		bool m_textAlignFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Flag to check whether icon position was acquired from a a theme file
		//----------------------------------------------------------------------------------
		bool m_iconPositionFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief To check if drawable is loaded from theme
		//----------------------------------------------------------------------------------
		bool m_contentDrawableFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Used to set content scale up
		//----------------------------------------------------------------------------------
		int m_setContentScaleUp;
		//----------------------------------------------------------------------------------
		/// \brief Used to set content aspect ratio
		//----------------------------------------------------------------------------------
		int m_setContentAspectRatio;
		//----------------------------------------------------------------------------------
		/// \brief The OpenGL texture ID of the image that holds the text
		/// TODO: fix me. This *really* should not be here and is only here so it can be deleted properly
		//----------------------------------------------------------------------------------
		unsigned int m_texID;
		//----------------------------------------------------------------------------------
		/// \brief Makes the texture from the text
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		void BuildTextCache( ResourceManager *_resources );
		//----------------------------------------------------------------------------------

	};
}

#endif
