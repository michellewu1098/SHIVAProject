//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUISYSTEM_TEXT_BUTTON__
#define __SHIVA_GUISYSTEM_TEXT_BUTTON__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#include "GUI/Views/ImageButton.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	/// This is a very simple test widget
	/// It's just a set of images for a set of states
	/// It can be selected and pressed
	/// If this is given a StateListDrawable, it will use its states
	class TextButton : public ImageButton
	{
	public:
		TextButton();
		virtual ~TextButton();

		/// This is called when the drawing context is changed
		/// All Drawables loaded from file will be automatically reloaded,
		/// but any locally-generated textures (e.g. text caches) must be remade here
		virtual void NotifyDrawingContextChange(ResourceManager *resources) {BuildTextCache(resources);}

		//virtual void Draw();

		/// Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		virtual std::string GetThemePrefix() {return "TextButton_";}

		/// For setting the View's attributes from xml
		virtual void Inflate(TiXmlElement*,ResourceManager*, std::string themePrefix = "", bool rootNode = false);
		/// For saving the View's attributes to xml
		/// \note This must be hierarchical and the element must include all child elements
		virtual TiXmlElement* Deflate(ResourceManager *resources);

		/// For changing the body text
		void SetText(std::string text, ShivaGUI::ResourceManager *resources) {_textBody = text; BuildTextCache(resources);}
		std::string GetText() {return _textBody;}

	protected:

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
