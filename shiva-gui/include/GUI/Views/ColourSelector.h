///-----------------------------------------------------------------------------------------------
/// \file ColourSelector.h
/// \brief This widget is for selecting a colour, it consists of a colour selector, with a movable handle, and a sample of the chosen colour
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------


#ifndef __SHIVA_GUISYSTEM_COLOUR_SELECTOR__
#define __SHIVA_GUISYSTEM_COLOUR_SELECTOR__


#include "GUI/Views/ViewEventListener.h"
#include "GUI/Drawables/StateListDrawable.h"
#include "Utility/GPUProgram.h"


namespace ShivaGUI
{
	class ColourSelector : public View
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		ColourSelector();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~ColourSelector();
		//----------------------------------------------------------------------------------
		/// \brief Gives the size of the View
		/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
		//----------------------------------------------------------------------------------
		virtual void Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight );
		//----------------------------------------------------------------------------------
		/// \brief Draw method
		//----------------------------------------------------------------------------------
		virtual void Draw();
		//----------------------------------------------------------------------------------
		/// \brief Draw method
		/// \param [in] _context Current OGL context
		//----------------------------------------------------------------------------------
		virtual void Draw( unsigned int _context );
		//----------------------------------------------------------------------------------
		/// \brief Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		//----------------------------------------------------------------------------------
		virtual std::string GetThemePrefix() { return "ColourSelector_"; }
		//----------------------------------------------------------------------------------
		/// \brief For setting the View's attributes from xml
		//----------------------------------------------------------------------------------
		virtual void Inflate( TiXmlElement* _xmlElement, ResourceManager* _resources, std::string _themePrefix = "", bool _rootNode = false );
		//----------------------------------------------------------------------------------
		/// \brief For saving the View's attributes to xml
		/// \note This must be hierarchical and the element must include all child elements
		//----------------------------------------------------------------------------------
		virtual TiXmlElement* Deflate( ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief For setting the focus of this View
		/// If focussed, it is expected that the view will show this visually
		/// and then allow other forms of input to be received, e.g. 'enter' button pressed etc
		//----------------------------------------------------------------------------------
		virtual void SetFocus( bool );
		//----------------------------------------------------------------------------------
		/// \brief For setting selection
		//----------------------------------------------------------------------------------
		virtual void SetSelect( bool );
		//----------------------------------------------------------------------------------
		/// \brief Handle event
		/// \param [in] _event
		//----------------------------------------------------------------------------------
		virtual bool HandleEvent( InternalEvent* _event );
		//----------------------------------------------------------------------------------
		/// \brief These listeners will be triggered whenever the colour is changed
		//----------------------------------------------------------------------------------
		void SetOnColourChangeListener( ViewEventListener *_value ) { m_colourListener = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Get colour
		/// \param [out] _R
		/// \param [out] _G
		/// \param [out] _B
		//----------------------------------------------------------------------------------
		void GetColour( float &_R, float &_G, float &_B ) { _R = m_sampleR; _G = m_sampleG; _B= m_sampleB; }
		//----------------------------------------------------------------------------------
		/// \brief Returns handle position
		/// \param [out] _x
		/// \param [out] _y
		//----------------------------------------------------------------------------------
		void GetHandlePosition( float &_x, float &_y ) { _x = m_handlePropPosX; _y = m_handlePropPosY; }
		//----------------------------------------------------------------------------------
		/// \brief Set handle position
		/// \param [in] _x
		/// \param [in] _y
		//----------------------------------------------------------------------------------
		void SetHandlePosition( float _x, float _y );
		//----------------------------------------------------------------------------------
		/// \brief Create vbos 
		//----------------------------------------------------------------------------------
		void BuildVBOs();
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Selector left bounds
		//----------------------------------------------------------------------------------
		float m_selectorBoundsLeft;
		//----------------------------------------------------------------------------------
		/// \brief Selector right bounds
		//----------------------------------------------------------------------------------
		float m_selectorBoundsRight;
		//----------------------------------------------------------------------------------
		/// \brief Selector top bounds
		float m_selectorBoundsTop;
		//----------------------------------------------------------------------------------
		/// \brief Selector bottom bounds
		//----------------------------------------------------------------------------------
		float m_selectorBoundsBottom;
		//----------------------------------------------------------------------------------
		/// \brief Sample left bounds
		//----------------------------------------------------------------------------------
		float m_sampleBoundsLeft;
		//----------------------------------------------------------------------------------
		/// \brief Sample right bounds
		//----------------------------------------------------------------------------------
		float m_sampleBoundsRight;
		//----------------------------------------------------------------------------------
		/// \brief Sample top bounds
		//----------------------------------------------------------------------------------
		float m_sampleBoundsTop;
		//----------------------------------------------------------------------------------
		/// \brief Sample bottom bounds
		//----------------------------------------------------------------------------------
		float m_sampleBoundsBottom;
		//----------------------------------------------------------------------------------
		/// \brief Handle half size
		//----------------------------------------------------------------------------------
		float m_handleHalfSize;
		//----------------------------------------------------------------------------------
		// Proportional handle positions, in coord space within the selector
		//----------------------------------------------------------------------------------
		/// \brief Proportional handle x-coord
		//----------------------------------------------------------------------------------
		float m_handlePropPosX;
		//----------------------------------------------------------------------------------
		/// \brief Proportional handle y-coord
		//----------------------------------------------------------------------------------
		float m_handlePropPosY;
		//----------------------------------------------------------------------------------
		/// \brief Handle left bounds
		//----------------------------------------------------------------------------------
		float m_handleBoundsLeft;
		//----------------------------------------------------------------------------------
		/// \brief Handle right bounds
		//----------------------------------------------------------------------------------
		float m_handleBoundsRight;
		//----------------------------------------------------------------------------------
		/// \brief Handle top bounds
		//----------------------------------------------------------------------------------
		float m_handleBoundsTop;
		//----------------------------------------------------------------------------------
		/// \brief Handle bottom bounds
		//----------------------------------------------------------------------------------
		float m_handleBoundsBottom;
		//----------------------------------------------------------------------------------
		/// \brief Returns true if a mouse coord is on the selector widget
		/// It will also return true if the handle is being held, so that the position can still be set in that 'sliding' way
		/// Sets the proportional positions posX and posY on a hit
		//----------------------------------------------------------------------------------
		bool MouseHitSelector(int _mouseX, int _mouseY, float &_posX, float &_posY);
		//----------------------------------------------------------------------------------
		/// \brief Converts a colour from HSL to RGB
		//----------------------------------------------------------------------------------
		void HSLtoRGB( float _H, float _S, float _L, float &_R, float &_G, float &_B );
		//----------------------------------------------------------------------------------
		/// \brief Updates the handle's actual position based on the proportional positions
		//----------------------------------------------------------------------------------
		void SetHandleActualPos();
		//----------------------------------------------------------------------------------
		/// \brief Program that draws an HSL colour swatch
		//----------------------------------------------------------------------------------
		Utility::GPUProgram *m_colourSelectorShader;
		//----------------------------------------------------------------------------------
		// The RGB of the currently selected sample colour
		//----------------------------------------------------------------------------------
		/// \brief Red component of currently selected sample colour
		//----------------------------------------------------------------------------------
		float m_sampleR;
		//----------------------------------------------------------------------------------
		/// \brief Green component of currently selected sample colour
		//----------------------------------------------------------------------------------
		float m_sampleG;
		//----------------------------------------------------------------------------------
		/// \brief Blue component of currently selected sample colour
		//----------------------------------------------------------------------------------
		float m_sampleB;
		//----------------------------------------------------------------------------------
		/// \brief If the handle is currently being manipulated
		//----------------------------------------------------------------------------------
		bool m_holdingHandle;
		//----------------------------------------------------------------------------------
		/// \brief Selector drawable
		//----------------------------------------------------------------------------------
		Drawable *m_selectorDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Selector state list drawable
		//----------------------------------------------------------------------------------
		StateListDrawable *m_selectorStateListDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Listener for colour change events
		//----------------------------------------------------------------------------------
		ViewEventListener *m_colourListener;
		//----------------------------------------------------------------------------------
		/// \brief Listener for colour change events name
		//----------------------------------------------------------------------------------
		std::string m_colourListenerName;
		//----------------------------------------------------------------------------------
		/// \brief Colour selector VAO
		//----------------------------------------------------------------------------------
		GLuint m_selectorVAO;
		//----------------------------------------------------------------------------------
		/// \brief Colour selected VAO
		//----------------------------------------------------------------------------------
		GLuint m_sampleVAO;
		//----------------------------------------------------------------------------------
		/// \brief Colour sampled shader		
		//----------------------------------------------------------------------------------
		Utility::GPUProgram* m_colourSampleShader;
		//----------------------------------------------------------------------------------
		/// \brief Projection matrix 
		//----------------------------------------------------------------------------------
		cml::matrix44f_c m_projMat;
		//----------------------------------------------------------------------------------
		/// \brief ModelView matrix
		//----------------------------------------------------------------------------------
		cml::matrix44f_c m_mvMat;
		//----------------------------------------------------------------------------------

	};
}

#endif
