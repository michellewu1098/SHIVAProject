
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUI_GUI_VIEWS_SCROLLCONTAINER__
#define __SHIVA_GUI_GUI_VIEWS_SCROLLCONTAINER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#include "GUI/Views/ViewGroups/ViewGroup.h"
#include "GUI/Drawables/Drawable.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	/// Contains a single child View and makes it scrollable if it's larger than the space it's been given to display in
	class ScrollContainer : public ViewGroup
	{
	public:

		ScrollContainer();
		virtual ~ScrollContainer();

		/// This is called when the drawing context is changed
		/// All Drawables loaded from file will be automatically reloaded,
		/// but any locally-generated textures (e.g. text caches) must be remade here
		/// This must be passed down to any children of the View
		virtual void NotifyDrawingContextChange(ResourceManager *resources);

		/// Gives the size of the View
		/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
		/// The base class function will set up the background drawable, so this must be called by derived View classes that wish to retain the background
		virtual void Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight);

		virtual void Update(float deltaTs, GUIController *guiController);

		/// The base class will draw a background drawable if it exists
		/// Derived View classes must call this at an appropriate time if they want to retain the background
		virtual void Draw();

		/// Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		virtual std::string GetThemePrefix() {return "ScrollContainer_";}

		/// For setting the View's attributes from xml
		/// \param themePrefix this is used when parsing a theme file. Derived classes should specify the prefix that identifies their attributes in a style. If empty, the string returned from GetThemePrefix should be used
		virtual void Inflate(TiXmlElement*, ResourceManager*, std::string themePrefix = "", bool rootNode = false);
		/// For saving the View's attributes to xml
		/// \note This must be hierarchical and the element must include all child elements
		virtual TiXmlElement* Deflate(ResourceManager *resources);

		/// Which directions to allow scrolling
		void EnableScroll(bool horizontal, bool vertical) {_scrollHorizontal = horizontal; _scrollVertical = vertical;}

		virtual void AddView(View*, ResourceManager*);

		/// Input event given to View, expected to filter down hierarchy
		/// \return false if event is not absorbed (e.g. did not hit button etc)
		virtual bool HandleEvent(InternalEvent*);

		/// Requested wrap pixel dimensions for layout
		/// \note These are used when SetLayoutParams() is called, which will be after the View has been inflated
		/// For this View, these are difficult to define for the scrolling direction
		virtual int GetWrapWidth();
		virtual int GetWrapHeight();

		/// Returns the vertical scrollbar's width
		int GetScrollbarWidth();
		/// Returns the horizontal scrollbar's height
		int GetScrollbarHeight();

		/// Returns the vertical scrollbar's top bound
		int GetScrollThumbTop();
		/// Returns the vertical scrollbar's bottom bound
		int GetScrollThumbBottom();
		/// Returns the horizontal scrollbar's left bound
		int GetScrollThumbLeft();
		/// Returns the horizontal scrollbar's right bound
		int GetScrollThumbRight();

	protected:

		bool _scrollHorizontal, _scrollVertical;

		/// When you press a scrollbar on the track, it 'jumps' to scroll, this amount determines how much
		int _scrollJumpAmount;

		/// Current scroll values, in pixels
		int _scrollX, _scrollY;
		/// While we are scrolling, we need to store the current offset differently
		int _scrollCurrentX, _scrollCurrentY;

		View *_child;

		/// If the container has been pressed
		bool _selected;
		/// If the child is being directly dragged around
		bool _dragSelected;
		/// If the vertical scroll thumb is being dragged
		bool _verticalScrollDrag;
		/// If the horizontal scroll thumb is being dragged
		bool _horizontalScrollDrag;

		int _scrollDragOriginalTop, _scrollDragOriginalLeft;


		/// Coords of mouse when selected
		int _selectedMouseX, _selectedMouseY;

		/// Indicates that the Update function needs to call Layout()
		bool _needLayout;

		Drawable 	*_scrollTrackVertDrawable, 	*_scrollTrackHorizDrawable, 	*_scrollThumbVertDrawable, 	*_scrollThumbHorizDrawable;
		bool 		_scrollTrackVertFromTheme, 	_scrollTrackHorizFromTheme, 	_scrollThumbVertFromTheme, 	_scrollThumbHorizFromTheme;
		std::string _scrollTrackVertName, 		_scrollTrackHorizName, 			_scrollThumbVertName, 		_scrollThumbHorizName;

		bool _showHorizBar, _showVertBar;

		/// Tests whether an event hits the scrollbars
		/// result:
		///  0 means no hit
		///  1 means hit top / left scroll track
		///  2 means hit scroll thumb
		///  3 means hit bottom / right scroll track
		int EventHitVertScroll(InternalEvent *event);
		int EventHitHorizScroll(InternalEvent *event);



	};

}

//////////////////////////////////////////////////////////////////////////
#endif
