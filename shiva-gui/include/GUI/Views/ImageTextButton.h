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
			Right = 3,
			Top = 4,
			Bottom = 5
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
		/// \brief Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		//----------------------------------------------------------------------------------
		virtual std::string GetThemePrefix() { return "ImageTextButton_"; }
		//----------------------------------------------------------------------------------
		/// \brief For setting the View's attributes from xml
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
		void SetText( std::string _text, ResourceManager* _resources );

		void SetStateDrawable();

		void SetFocussed( bool _focussed );

		bool GetFocussed();

		//----------------------------------------------------------------------------------
		/// \brief Get body text
		/// \return m_textBody
		//----------------------------------------------------------------------------------
		std::string GetText() const { return m_textBody; }
		//----------------------------------------------------------------------------------
		/// \brief Set content drawable
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		virtual void SetContent( Drawable *_value );
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
		/// \brief Holds position for icon (left/top)
		//----------------------------------------------------------------------------------
		unsigned int m_iconPosition;
		//----------------------------------------------------------------------------------
		// The following flags are mainly for use when deflating to xml
		//----------------------------------------------------------------------------------
		/// \brief Flag to check whether font name was acquired from a theme file
		//----------------------------------------------------------------------------------
		bool m_fontNameFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Flag to check whether font size was acquired from a theme file
		//----------------------------------------------------------------------------------
		bool m_fontSizeFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Flag to check whether font colour was acquired from a theme file
		//----------------------------------------------------------------------------------
		bool m_fontColourFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Flag to check whether text alignment was acquired from a theme file
		//----------------------------------------------------------------------------------
		bool m_textAlignFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Flag to check whether icon position was acquired from a a theme file
		//----------------------------------------------------------------------------------
		bool m_iconPositionFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Makes the texture from the text
		/// \param [in] resources
		//----------------------------------------------------------------------------------
		//void BuildTextCache( ResourceManager *resources );
		//----------------------------------------------------------------------------------
		/// \brief % of space occupied by icon on button
		//----------------------------------------------------------------------------------
		float m_iconPercentSize;
		//----------------------------------------------------------------------------------
		/// \brief Flag to check whether icon percent size was acquired from a theme file
		//----------------------------------------------------------------------------------
		bool m_iconPercentSizeFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief % of space occupied by text on button
		//----------------------------------------------------------------------------------
		float m_textPercentSize;
		//----------------------------------------------------------------------------------
		/// \brief Flat to check whether text percent size was acquired froma theme file
		//----------------------------------------------------------------------------------
		bool m_textPercentSizeFromTheme;
		//----------------------------------------------------------------------------------
	};
}

#endif
