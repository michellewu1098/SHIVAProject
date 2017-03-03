#ifndef __SHIVA_GUISYSTEM_COLOUR_SELECTOR__
#define __SHIVA_GUISYSTEM_COLOUR_SELECTOR__

#include <iostream>

#include "GUI/Views/View.h"
#include "GUI/Views/ViewEventListener.h"
#include "GUI/Drawables/StateListDrawable.h"
#include "Utility/GPUProgram.h"
#include "Utility/GPUVariable.h"

namespace ShivaGUI
{

	/// This widget is for selecting a colour
	/// It consists of a colour selector, with a movable handle, and a sample of the chosen colour
	class ColourSelector : public View
	{
	public:

		ColourSelector();
		//ImageButton(ResourceSystem::BitmapDrawable *drawable);
		//ImageButton(ResourceSystem::StateListDrawable *drawable);
		virtual ~ColourSelector();

		/// Gives the size of the View
		/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
		virtual void Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight);

		virtual void Draw();

		/// Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		virtual std::string GetThemePrefix() {return "ColourSelector_";}

		/// For setting the View's attributes from xml
		virtual void Inflate(TiXmlElement*,ResourceManager*, std::string themePrefix = "", bool rootNode = false);
		/// For saving the View's attributes to xml
		/// \note This must be hierarchical and the element must include all child elements
		virtual TiXmlElement* Deflate(ResourceManager *resources);

		/// For setting the focus of this View
		/// If focussed, it is expected that the view will show this visually
		/// and then allow other forms of input to be received, e.g. 'enter' button pressed etc
		virtual void SetFocus(bool);

		virtual void SetSelect(bool);

		virtual bool HandleEvent(InternalEvent*);

		/// These listeners will be triggered whenever the colour is changed
		void SetOnColourChangeListener(ViewEventListener *value) {_colourListener=value;}

		void GetColour(float &R, float &G, float &B) {R=_sampleR;G=_sampleG;B=_sampleB;}

		void GetHandlePosition( float &x, float &y ) { x = _handlePropPosX; y = _handlePropPosY; }

		void SetHandlePosition(float x, float y);

	protected:
		float _selectorBoundsLeft, _selectorBoundsRight, _selectorBoundsTop, _selectorBoundsBottom;
		float _sampleBoundsLeft, _sampleBoundsRight, _sampleBoundsTop, _sampleBoundsBottom;

		float _handleHalfSize;
		/// Proportional handle positions, in coord space within the selector
		float _handlePropPosX, _handlePropPosY;
		float _handleBoundsLeft, _handleBoundsRight, _handleBoundsTop, _handleBoundsBottom;

		/// Returns true if a mouse coord is on the selector widget
		/// It will also return true if the handle is being held, so that the position can still be set in that 'sliding' way
		/// Sets the proportional positions posX and posY on a hit
		bool MouseHitSelector(int mouseX, int mouseY, float &posX, float &posY);

		/// Converts a colour from HSL to RGB
		void HSLtoRGB(float H, float S, float L, float &R, float &G, float &B);

		/// Updates the handle's actual position based on the proportional positions
		void SetHandleActualPos();

		//virtual void OnChangeActive() {SetStateDrawable();}

		/// Program that draws an HSL colour swatch
		Utility::GPUProgram *_colourSelectorProgram;

		/// The RGB of the currently selected sample colour
		float _sampleR, _sampleG, _sampleB;

		/// If the handle is currently being manipulated
		bool _holdingHandle;

		Drawable *_selectorDrawable;
		StateListDrawable *_selectorStateListDrawable;

		/// Listener for colour change events
		ViewEventListener *_colourListener;
		std::string _colourListenerName;
	};

}

//////////////////////////////////////////////////////////////////////////
#endif
