
#ifndef __SHIVA_GUISYSTEM_SLIDER__
#define __SHIVA_GUISYSTEM_SLIDER__

#include "GUI/Views/View.h"
#include "GUI/Views/ViewEventListener.h"
#include "GUI/Drawables/BitmapDrawable.h"
#include "GUI/Drawables/StateListDrawable.h"

namespace ShivaGUI
{

	class Slider : public View
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		Slider();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~Slider();
		//----------------------------------------------------------------------------------
		/// \brief Gives the size of the View
		/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
		/// The base class function will set up the background drawable, so this must be called by derived View classes that wish to retain the background
		/// \param [in] _left
		/// \param [in] _top
		/// \param [in] _right
		/// \param [in] _bottom
		/// \param [in] _windowWidth
		/// \param [in] _windowHeight
		//----------------------------------------------------------------------------------
		virtual void Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight );
		//----------------------------------------------------------------------------------
		/// \brief Update method
		/// \param [in] _deltaTs Timestep
		//----------------------------------------------------------------------------------
		virtual void Update( float _deltaTs ) {}
		//----------------------------------------------------------------------------------
		/// \brief The base class will draw a background drawable if it exists
		/// Derived View classes must call this at an appropriate time if they want to retain the background
		//----------------------------------------------------------------------------------
		virtual void Draw();
		//----------------------------------------------------------------------------------
		/// \brief Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		/// \return "Slider_"
		//----------------------------------------------------------------------------------
		virtual std::string GetThemePrefix() { return "Slider_"; }
		//----------------------------------------------------------------------------------
		/// \brief For setting the View's attributes from xml
		/// \param [in] _xmlElement
		/// \param [in] _resources
		/// \param [in] _themePrefix
		/// \param [in] _rootNode
		//----------------------------------------------------------------------------------
		virtual void Inflate( TiXmlElement* _xmlElement, ResourceManager* _resources, std::string _themePrefix = "", bool _rootNode = false );
		//----------------------------------------------------------------------------------
		/// \brief For saving the View's attributes to xml
		/// \note This must be hierarchical and the element must include all child elements
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual TiXmlElement* Deflate( ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief For setting the focus of this View
		/// If focussed, it is expected that the view will show this visually
		/// and then allow other forms of input to be received, e.g. 'enter' button pressed etc
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		virtual void SetFocus( bool _value );
		//----------------------------------------------------------------------------------
		/// \brief For selecting the View, e.g. after focussing on it you press the 'enter' key
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		virtual void SetSelect( bool _value ) {}
		//----------------------------------------------------------------------------------
		/// Input event given to View, expected to filter down hierarchy
		/// \return false if event is not absorbed (e.g. did not hit button etc)
		virtual bool HandleEvent(InternalEvent*);
		//----------------------------------------------------------------------------------
		/// \brief Set handle position
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetHandlePosition( float _value );
		//----------------------------------------------------------------------------------
		/// \brief Returns handle position
		/// \return m_handlePosition
		//----------------------------------------------------------------------------------
		float GetHandlePosition() const { return m_handlePosition; }
		//----------------------------------------------------------------------------------
		/// \brief Set on slide listener
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetOnSlideListener( ViewEventListener *_value ) { m_slideListener = _value; }
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief If the slider has focus
		//----------------------------------------------------------------------------------
		bool m_focussed;
		//----------------------------------------------------------------------------------
		// Size of the handle
		//----------------------------------------------------------------------------------
		/// \brief Handle height
		//----------------------------------------------------------------------------------
		float m_handleHeight;
		//----------------------------------------------------------------------------------
		/// \brief Handle width
		//----------------------------------------------------------------------------------
		float m_handleWidth;
		//----------------------------------------------------------------------------------
		/// \brief Left bounds
		//----------------------------------------------------------------------------------
		int m_boundsLeft;
		//----------------------------------------------------------------------------------
		/// \brief Right bounds
		//----------------------------------------------------------------------------------
		int m_boundsRight;
		//----------------------------------------------------------------------------------
		/// \brief Top bounds
		//----------------------------------------------------------------------------------
		int m_boundsTop;
		//----------------------------------------------------------------------------------
		/// \brief Bottom bounds
		//----------------------------------------------------------------------------------
		int m_boundsBottom;
		//----------------------------------------------------------------------------------
		/// \brief Left handle
		//----------------------------------------------------------------------------------
		float m_handleLeft;
		//----------------------------------------------------------------------------------
		/// \brief Right handle
		//----------------------------------------------------------------------------------
		float m_handleRight;
		//----------------------------------------------------------------------------------
		/// \brief Top handle
		//----------------------------------------------------------------------------------
		float m_handleTop;
		//----------------------------------------------------------------------------------
		/// \brief Bottom handle
		//----------------------------------------------------------------------------------
		float m_handleBottom;
		//----------------------------------------------------------------------------------
		/// \brief Drawable for the bar
		//----------------------------------------------------------------------------------
		Drawable *m_barDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Drawable for the handle
		//----------------------------------------------------------------------------------
		Drawable *m_handleGeneralDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Handle state list drawable
		//----------------------------------------------------------------------------------
		StateListDrawable *m_handleStateListDrawable;
		//----------------------------------------------------------------------------------
		// If the drawables were loaded from theme files
		// Mainly for use with deflating to xml
		//----------------------------------------------------------------------------------
		bool m_barDrawableFromTheme;
		//----------------------------------------------------------------------------------
		bool m_handleDrawableFromTheme;
		//----------------------------------------------------------------------------------
		bool m_handleHeightFromTheme;
		//----------------------------------------------------------------------------------
		bool m_handleWidthFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Internal function for determining a mouse hit
		/// \param [in] _mouseX
		/// \param [in] _mouseY
		/// \return 0 on no hit, 1 on hit handle, 2 on hit bar left of handle, 3 on hit right bar
		//----------------------------------------------------------------------------------
		int MouseHit( int _mouseX, int _mouseY );
		//----------------------------------------------------------------------------------
		/// \brief Sets the position of the handle
		//----------------------------------------------------------------------------------
		void LayoutHandle();
		//----------------------------------------------------------------------------------
		/// \brief Handle position, ranges from 0 to 1
		//----------------------------------------------------------------------------------
		float m_handlePosition;
		//----------------------------------------------------------------------------------
		/// \brief Whether the user is holding the handle
		//----------------------------------------------------------------------------------
		bool m_holdingHandle;
		//----------------------------------------------------------------------------------
		/// \brief Listener for slide events
		//----------------------------------------------------------------------------------
		ViewEventListener *m_slideListener;
		//----------------------------------------------------------------------------------
		/// \brief Slide listener name
		//----------------------------------------------------------------------------------
		std::string m_slideListenerName;
		//----------------------------------------------------------------------------------
		/// \brief Sets the state of the StateListDrawable to match that of the button
		//----------------------------------------------------------------------------------
		void SetStateDrawable();
		//----------------------------------------------------------------------------------
		/// \brief Whether to only generate events when the handle is released
		/// Default is false
		//----------------------------------------------------------------------------------
		bool m_eventOnRelease;
		//----------------------------------------------------------------------------------
		virtual void OnChangeActive() { SetStateDrawable(); }
		//----------------------------------------------------------------------------------

	};
}

#endif
