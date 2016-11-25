
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUI_GUI_VIEWS_VIEW__
#define __SHIVA_GUI_GUI_VIEWS_VIEW__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#include "Input/InternalEvent.h"
#include "ResourceManager.h"
#include "GUI/Views/ViewGroups/LayoutParams.h"
#include "GUI/Definitions.h"
#include "Utility/tinyxml.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	// Annoying forward declaration
	class GUIController;
	class ResourceManager;

	/// This is the base widget class
	/// It does not use pure virtual functions so that you can use it as an 'empty' View
	class View
	{
	public:

		View();
		virtual ~View();

		void SetID(std::string value) {_ID=value;}
		std::string GetID() {return _ID;}

		/// This is called when the drawing context is changed
		/// All Drawables loaded from file will be automatically reloaded,
		/// but any locally-generated textures (e.g. text caches) must be remade here
		/// This must be passed down to any children of the View
		virtual void NotifyDrawingContextChange(ResourceManager *resources) {}

		/// Gives the size of the View
		/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
		/// The base class function will set up the background drawable, so this must be called by derived View classes that wish to retain the background
		virtual void Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight);

		virtual void Update(float deltaTs, GUIController *guiController) {}

		/// The base class will draw a background drawable if it exists
		/// Derived View classes must call this at an appropriate time if they want to retain the background
		virtual void Draw();

		/// Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		virtual std::string GetThemePrefix() {return "View_";}

		/// For setting the View's attributes from xml
		/// \param themePrefix this is used when parsing a theme file. Derived classes should specify the prefix that identifies their attributes in a style. If empty, the string returned from GetThemePrefix should be used
		virtual void Inflate(TiXmlElement*, ResourceManager*, std::string themePrefix = "", bool rootNode = false);
		/// For saving the View's attributes to xml
		/// \note This must be hierarchical and the element must include all child elements
		virtual TiXmlElement* Deflate(ResourceManager *resources);


		void SetBackground(Drawable *bgd) {_background=bgd;}

		/// The LayoutParams specify how we *want* the View to be set out, in terms of size and position inside its parent
		void SetLayoutParams(LayoutParams *value);
		virtual LayoutParams* GetLayoutParams() {return _layoutParams;}

		/// Retrieves the next View for selection in the given direction
		/// The idea is that all focusable widgets are connected to each other, so the user can cycle through them
		/// \return Itself if there is no next focusable View
		View* GetNextFocus(Definitions::FocusDirection);
		void SetNextFocus(View*, Definitions::FocusDirection);
		void SetIsFirstFocus(bool value) {_isFirstFocus = value;}

		/// Scan is like focus, but only 2D
		View* GetNextScan(bool forward);
		void SetNextScan(View*, bool forward);
		void SetIsFirstScan(bool value) {_isFirstScan=value;}

		/// For setting the focus of this View
		/// If focussed, it is expected that the view will show this visually
		/// and then allow other forms of input to be received, e.g. 'enter' button pressed etc
		virtual void SetFocus(bool) {}

		/// For selecting the View, e.g. after focussing on it you press the 'enter' key
		virtual void SetSelect(bool) {}

		/// Input event given to View, expected to filter down hierarchy
		/// \return false if event is not absorbed (e.g. did not hit button etc)
		virtual bool HandleEvent(InternalEvent*) {return false;}

		void SetVisibility(bool value) {_visible=value;OnChangeVisible();}
		bool GetVisibility() {return _visible;}

		void SetActive(bool value) {_active=value;OnChangeActive();}
		bool GetActive() {return _active;}

		/// Requested wrap pixel dimensions for layout
		/// \note These are used when SetLayoutParams() is called, which will be after the View has been inflated
		virtual int GetWrapWidth() {return 0;}
		virtual int GetWrapHeight() {return 0;}

	protected:

		/// ID for the View
		/// This is mainly for use in xml layouts, where Views need to refer to each other
		std::string _ID;

		/// Whether the view will take focus on issue of the first focus event
		/// \default False
		bool _isFirstFocus;

		/// Each View stores the next View to select when focus changes in one of the four directions
		View **_nextFocus;

		/// Whether the view will take focus on issue of the first scan event
		/// \default False
		bool _isFirstScan;

		View *_nextScan, *_prevScan;

		/// These tell the parent layout how the View wants to be arranged
		LayoutParams *_layoutParams;

		/// A background drawable
		/// normally to be drawn behind the View-specific graphics
		Drawable *_background;

		/// If various attributes were loaded from theme rather than directly from xml
		/// This is mainly used when deflating to xml, so we don't write too much stuff
		bool _fromTheme_bgd;

		bool _visible, _active;

		virtual void OnChangeActive() {}
		virtual void OnChangeVisible() {}
		virtual void OnSetNextScan(View *value, bool forward) {}
		virtual void OnSetNextFocus(View*, Definitions::FocusDirection) {}

		/// Specifies the gravity of View's contents When View is larger than its content and that content does not stretch to fit View's size
		/// Default value is CENTRE
		unsigned int _internalGravity;

		/// Values given to the View through the Layout() function
		int _layoutBoundsLeft, _layoutBoundsTop, _layoutBoundsRight, _layoutBoundsBottom, _layoutWindowWidth, _layoutWindowHeight;

		/// Utility function, returns true if a positional event coord is on the View
		/// Uses the values given by the Layout function
		bool EventHit(InternalEvent*);
	};

}

//////////////////////////////////////////////////////////////////////////
#endif
