
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUISYSTEM_TEXTVIEW__
#define __SHIVA_GUISYSTEM_TEXTVIEW__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#include "GUI/Views/View.h"
#include "GUI/Drawables/BitmapDrawable.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{
	/// This is the base widget class
	/// It does not use pure virtual functions so that you can use it as an 'empty' View
	class TextView : public View
	{
	public:

		TextView();
		virtual ~TextView();

		/// This is called when the drawing context is changed
		/// All Drawables loaded from file will be automatically reloaded,
		/// but any locally-generated textures (e.g. text caches) must be remade here
		virtual void NotifyDrawingContextChange(ResourceManager *resources) {BuildTextCache(resources);}

		/// Gives the size of the View
		/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
		/// The base class function will set up the background drawable, so this must be called by derived View classes that wish to retain the background
		virtual void Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight);

		/// The base class will draw a background drawable if it exists
		/// Derived View classes must call this at an appropriate time if they want to retain the background
		virtual void Draw();

		/// Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		virtual std::string GetThemePrefix() {return "TextView_";}

		/// For setting the View's attributes from xml
		/// \param themePrefix this is used when parsing a theme file. Derived classes should specify the prefix that identifies their attributes in a style. If empty, the string returned from GetThemePrefix should be used
		virtual void Inflate(TiXmlElement*, ResourceManager*, std::string themePrefix = "", bool rootNode = false);
		/// For saving the View's attributes to xml
		/// \note This must be hierarchical and the element must include all child elements
		virtual TiXmlElement* Deflate(ResourceManager *resources);

		/// For changing the body text
		void SetText(std::string text, ShivaGUI::ResourceManager *);

		/// Requested wrap pixel dimensions for layout
		/// \note These are used when SetLayoutParams() is called, which will be after the View has been inflated
		virtual int GetWrapWidth();
		virtual int GetWrapHeight();

	protected:

		/// The text is converted to an image and stored in this drawable
		BitmapDrawable *_textDrawable;

		/// The main body of text to display, as a string
		std::string _textBody;

		/// Font to use
		std::string _fontName;

		unsigned int _fontSize;

		unsigned int _fontColour;

		/// Whether values were acquired from a theme file
		/// Mainly for use when deflating to xml
		bool _fontNameFromTheme, _fontSizeFromTheme, _fontColourFromTheme;

		/// The OpenGL texture ID of the image that holds the text
		/// TODO: fix me. This *really* should not be here and is only here so it can be deleted properly
		unsigned int _texID;

		/// Makes the texture from the text
		void BuildTextCache(ResourceManager *resources);
	};

}

//////////////////////////////////////////////////////////////////////////
#endif
