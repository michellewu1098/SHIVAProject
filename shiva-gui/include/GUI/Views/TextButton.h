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
		virtual void NotifyDrawingContextChange( ResourceManager *_resources ) { BuildTextCache( _resources ); }
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
		/// \param [in] _xmlElement
		/// \param [in] _resources
		/// \param [in] _themePrefix
		/// \param [in] _rootNode
		//----------------------------------------------------------------------------------
		virtual void Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources, std::string _themePrefix = "", bool _rootNode = false );
		//----------------------------------------------------------------------------------
		/// \brief For saving the View's attributes to xml
		/// \note This must be hierarchical and the element must include all child elements
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual TiXmlElement* Deflate( ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief For changing the body text
		/// \param [in] _text
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		void SetText( std::string _text, ShivaGUI::ResourceManager *_resources ) { m_textBody = _text; BuildTextCache( _resources ); }
		//----------------------------------------------------------------------------------
		/// \brief Return body of text
		/// \return m_textBody
		//----------------------------------------------------------------------------------
		std::string GetText() const { return m_textBody; }
		//----------------------------------------------------------------------------------

	protected:

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
		/// Whether values were acquired from a theme file
		/// Mainly for use when deflating to xml
		//----------------------------------------------------------------------------------
		bool m_fontNameFromTheme;
		//----------------------------------------------------------------------------------
		bool m_fontSizeFromTheme;
		//----------------------------------------------------------------------------------
		bool m_fontColourFromTheme;
		//----------------------------------------------------------------------------------
		bool m_textAlignFromTheme;
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
