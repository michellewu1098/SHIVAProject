
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUISYSTEM_SLIDER__
#define __SHIVA_GUISYSTEM_SLIDER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#include "GUI/Views/View.h"
#include "GUI/Views/ViewEventListener.h"
#include "GUI/Drawables/BitmapDrawable.h"
#include "GUI/Drawables/StateListDrawable.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	class Slider : public View
	{
	public:

		Slider();
		virtual ~Slider();

		/// Gives the size of the View
		/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
		/// The base class function will set up the background drawable, so this must be called by derived View classes that wish to retain the background
		virtual void Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight);

		virtual void Update(float deltaTs) {}

		/// The base class will draw a background drawable if it exists
		/// Derived View classes must call this at an appropriate time if they want to retain the background
		virtual void Draw();

		/// Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		virtual std::string GetThemePrefix() {return "Slider_";}

		/// For setting the View's attributes from xml
		virtual void Inflate(TiXmlElement*,ResourceManager*, std::string themePrefix = "", bool rootNode = false);
		/// For saving the View's attributes to xml
		/// \note This must be hierarchical and the element must include all child elements
		virtual TiXmlElement* Deflate(ResourceManager *resources);

		/// For setting the focus of this View
		/// If focussed, it is expected that the view will show this visually
		/// and then allow other forms of input to be received, e.g. 'enter' button pressed etc
		virtual void SetFocus(bool);

		/// For selecting the View, e.g. after focussing on it you press the 'enter' key
		virtual void SetSelect(bool) {}

		/// Input event given to View, expected to filter down hierarchy
		/// \return false if event is not absorbed (e.g. did not hit button etc)
		virtual bool HandleEvent(InternalEvent*);

		void SetHandlePosition( float value );
		float GetHandlePosition() {return _handlePosition;}

		void SetOnSlideListener(ViewEventListener *value) {_slideListener=value;}

	protected:

		/// If the slider has focus
		bool _focussed;

		/// Size of the handle
		float _handleHeight, _handleWidth;

		int _boundsLeft, _boundsRight, _boundsTop, _boundsBottom;
		int _handleLeft, _handleRight, _handleTop, _handleBottom;

		/// Drawable for the bar
		Drawable *_barDrawable;

		/// Drawable for the handle
		Drawable *_handleGeneralDrawable;
		StateListDrawable *_handleStateListDrawable;

		/// If the drawables were loaded from theme files
		/// Mainly for use with deflating to xml
		bool _barDrawableFromTheme, _handleDrawableFromTheme, _handleHeightFromTheme, _handleWidthFromTheme;

		/// Internal function for determining a mouse hit
		/// \return 0 on no hit, 1 on hit handle, 2 on hit bar left of handle, 3 on hit right bar
		int MouseHit(int mouseX, int mouseY);

		/// Sets the position of the handle
		void LayoutHandle();

		/// ranges from 0 to 1
		float _handlePosition;

		/// Whether the user is holding the handle
		bool _holdingHandle;

		//bool _active;

		/// Listener for slide events
		ViewEventListener *_slideListener;

		std::string _slideListenerName;

		/// Sets the state of the StateListDrawable to match that of the button
		void SetStateDrawable();

		/// Whether to only generate events when the handle is released
		/// Default is false
		bool _eventOnRelease;

		virtual void OnChangeActive() {SetStateDrawable();}
	};

}

//////////////////////////////////////////////////////////////////////////
#endif
