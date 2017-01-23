#ifndef __SHIVA_GUISYSTEM_IMAGETEXT_BUTTON__
#define __SHIVA_GUISYSTEM_IMAGETEXT_BUTTON__

#include "GUI/Views/ImageButton.h"

namespace ShivaGUI
{
	class ImageTextButton : public ImageButton
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
		ImageTextButton();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~ImageTextButton();
		//----------------------------------------------------------------------------------
		/// \brief This is called when the drawing context is changed
		/// All Drawables loaded from file will be automatically reloaded,
		/// but any locally-generated textures (e.g. text caches) must be remade here
		/// \param [in] resources
		//----------------------------------------------------------------------------------
		//virtual void NotifyDrawingContextChange( ResourceManager *resources ) { BuildTextCache( resources ); }
		//----------------------------------------------------------------------------------
		/// \brief Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		//----------------------------------------------------------------------------------
		virtual std::string GetThemePrefix() { return "ImageTextButton_"; }
		//----------------------------------------------------------------------------------
		/// \brief For setting the View's attributes from xml
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
//		void SetText( std::string text, ShivaGUI::ResourceManager *resources ) { _textBody = text; BuildTextCache( resources ); }
		//----------------------------------------------------------------------------------
		/// \brief Get body text
		/// \return _textBody
		//----------------------------------------------------------------------------------
		std::string GetText() const { return _textBody; }
		//----------------------------------------------------------------------------------

	protected:

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
		// The following flags are mainly for use when deflating to xml
		//----------------------------------------------------------------------------------
		/// \brief Flag to check Whether font name was acquired from a theme file
		//----------------------------------------------------------------------------------
		bool _fontNameFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Flag to check Whether font size was acquired from a theme file
		//----------------------------------------------------------------------------------
		bool _fontSizeFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Flag to check Whether font colour was acquired from a theme file
		//----------------------------------------------------------------------------------
		bool _fontColourFromTheme;
		//----------------------------------------------------------------------------------
		bool _textAlignFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief The OpenGL texture ID of the image that holds the text
		/// TODO: fix me. This *really* should not be here and is only here so it can be deleted properly
		//----------------------------------------------------------------------------------
		unsigned int _texID;
		//----------------------------------------------------------------------------------
		/// \brief Makes the texture from the text
		/// \param [in] resources
		//----------------------------------------------------------------------------------
		//void BuildTextCache( ResourceManager *resources );
		//----------------------------------------------------------------------------------
	};
}

#endif
