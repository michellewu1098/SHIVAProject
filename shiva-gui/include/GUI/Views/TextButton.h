///-----------------------------------------------------------------------------------------------
/// \file TextButton.h
/// \brief This is a very simple test widget: it takes as input text from xml file of profile and 
/// generate a bitmap texture corresponding to the text
/// \author Leigh McLoughlin, Michelle Wu
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUISYSTEM_TEXT_BUTTON__
#define __SHIVA_GUISYSTEM_TEXT_BUTTON__

#include "GUI/Views/ImageButton.h"

namespace ShivaGUI
{
	class TextButton : public ImageButton
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
		TextButton();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~TextButton();
		//----------------------------------------------------------------------------------
		/// \brief This is called when the drawing context is changed
		/// All Drawables loaded from file will be automatically reloaded,
		/// but any locally-generated textures (e.g. text caches) must be remade here
		/// \param [in] resources
		//----------------------------------------------------------------------------------
		virtual void NotifyDrawingContextChange( ResourceManager *resources ) { BuildTextCache( resources ); }
		//----------------------------------------------------------------------------------
		//virtual void Draw();
		//----------------------------------------------------------------------------------
		/// \brief Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		/// \return "TextButton_"
		//----------------------------------------------------------------------------------
		virtual std::string GetThemePrefix() { return "TextButton_"; }
		//----------------------------------------------------------------------------------
		/// \brief For setting the View's attributes from xml
		/// \param [in] xmlElement
		/// \param [in] resources
		/// \param [in] themePrefix
		/// \param [in] rootNode
		//----------------------------------------------------------------------------------
		virtual void Inflate( TiXmlElement *xmlElement, ResourceManager *resources, std::string themePrefix = "", bool rootNode = false );
		//----------------------------------------------------------------------------------
		/// \brief For saving the View's attributes to xml
		/// \note This must be hierarchical and the element must include all child elements
		/// \param [in] resources
		//----------------------------------------------------------------------------------
		virtual TiXmlElement* Deflate( ResourceManager *resources );
		//----------------------------------------------------------------------------------
		/// \brief For changing the body text
		/// \param [in] text
		/// \param [in] resources
		//----------------------------------------------------------------------------------
		void SetText( std::string text, ShivaGUI::ResourceManager *resources ) { _textBody = text; BuildTextCache( resources ); }
		//----------------------------------------------------------------------------------
		/// \brief Return body of text
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
