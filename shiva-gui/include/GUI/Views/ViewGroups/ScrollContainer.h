///-----------------------------------------------------------------------------------------------
/// \file ScrollContainer.h
/// \brief Contains a single child View and makes it scrollable if it's larger than the space it's been given to display in
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_GUI_VIEWS_SCROLLCONTAINER__
#define __SHIVA_GUI_GUI_VIEWS_SCROLLCONTAINER__

#include "GUI/Views/ViewGroups/ViewGroup.h"
#include "GUI/Drawables/Drawable.h"

namespace ShivaGUI
{
	class ScrollContainer : public ViewGroup
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		ScrollContainer();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~ScrollContainer();
		//----------------------------------------------------------------------------------
		/// \brief This is called when the drawing context is changed
		/// All Drawables loaded from file will be automatically reloaded,
		/// but any locally-generated textures (e.g. text caches) must be remade here
		/// This must be passed down to any children of the View
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual void NotifyDrawingContextChange( ResourceManager *_resources );
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
		/// \param [in] _guiController
		//----------------------------------------------------------------------------------
		virtual void Update( float _deltaTs, GUIController *_guiController );
		//----------------------------------------------------------------------------------
		/// \brief The base class will draw a background drawable if it exists
		/// Derived View classes must call this at an appropriate time if they want to retain the background
		//----------------------------------------------------------------------------------
		virtual void Draw();
		//----------------------------------------------------------------------------------
		virtual void Draw( unsigned int _context );
		//----------------------------------------------------------------------------------
		/// \brief Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		/// \return "ScrollContainer_"
		//----------------------------------------------------------------------------------
		virtual std::string GetThemePrefix() { return "ScrollContainer_"; }
		//----------------------------------------------------------------------------------
		/// \brief For setting the View's attributes from xml
		/// \param [in] _themePrefix this is used when parsing a theme file. Derived classes should specify the prefix that identifies their attributes in a style. 
		/// If empty, the string returned from GetThemePrefix should be used
		virtual void Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources, std::string _themePrefix = "", bool _rootNode = false );
		//----------------------------------------------------------------------------------
		/// \brief For saving the View's attributes to xml
		/// \note This must be hierarchical and the element must include all child elements
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual TiXmlElement* Deflate( ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief Which directions to allow scrolling
		/// \param [in] _horizontal
		/// \param [in] _vertical
		//----------------------------------------------------------------------------------
		void EnableScroll( bool _horizontal, bool _vertical ) { m_scrollHorizontal = _horizontal; m_scrollVertical = _vertical; }
		//----------------------------------------------------------------------------------
		/// \brief Add view
		/// \param [in] _view
		//----------------------------------------------------------------------------------
		virtual void AddView( View *_view, ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief Input event given to View, expected to filter down hierarchy
		/// \return false if event is not absorbed (e.g. did not hit button etc)
		/// \param [in] _currentEvent
		//----------------------------------------------------------------------------------
		virtual bool HandleEvent( InternalEvent *_currentEvent );
		//----------------------------------------------------------------------------------
		/// \brief Requested wrap pixel dimensions for layout
		/// \note These are used when SetLayoutParams() is called, which will be after the View has been inflated
		/// For this View, these are difficult to define for the scrolling direction
		//----------------------------------------------------------------------------------
		virtual int GetWrapWidth();
		//----------------------------------------------------------------------------------
		virtual int GetWrapHeight();
		//----------------------------------------------------------------------------------
		/// \brief Returns the vertical scrollbar's width
		//----------------------------------------------------------------------------------
		int GetScrollbarWidth();
		//----------------------------------------------------------------------------------
		/// \brief Returns the horizontal scrollbar's height
		//----------------------------------------------------------------------------------
		int GetScrollbarHeight();
		//----------------------------------------------------------------------------------
		/// \brief Returns the vertical scrollbar's top bound
		//----------------------------------------------------------------------------------
		int GetScrollThumbTop();
		//----------------------------------------------------------------------------------
		/// \brief Returns the vertical scrollbar's bottom bound
		//----------------------------------------------------------------------------------
		int GetScrollThumbBottom();
		//----------------------------------------------------------------------------------
		/// \brief Returns the horizontal scrollbar's left bound
		//----------------------------------------------------------------------------------
		int GetScrollThumbLeft();
		//----------------------------------------------------------------------------------
		/// \brief Returns the horizontal scrollbar's right bound
		//----------------------------------------------------------------------------------
		int GetScrollThumbRight();
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Flag to check if scrollbar is horizontal
		//----------------------------------------------------------------------------------
		bool m_scrollHorizontal;
		//----------------------------------------------------------------------------------
		/// \brief Flag to check if scrollbar is vertical
		//----------------------------------------------------------------------------------
		bool m_scrollVertical;
		//----------------------------------------------------------------------------------
		/// \brief When you press a scrollbar on the track, it 'jumps' to scroll, this amount determines how much
		//----------------------------------------------------------------------------------
		int m_scrollJumpAmount;
		//----------------------------------------------------------------------------------
		/// \brief Current x scroll value, in pixels
		//----------------------------------------------------------------------------------
		int m_scrollX;
		//----------------------------------------------------------------------------------
		/// \brief Current y scroll value in pixels
		//----------------------------------------------------------------------------------
		int m_scrollY;
		//----------------------------------------------------------------------------------
		/// \brief While we are scrolling, we need to store the current offset differently: current x-offset
		//----------------------------------------------------------------------------------
		int m_scrollCurrentX;
		//----------------------------------------------------------------------------------
		/// \brief Current y-offset
		//----------------------------------------------------------------------------------
		int m_scrollCurrentY;
		//----------------------------------------------------------------------------------
		/// \brief View child
		//----------------------------------------------------------------------------------
		View *m_child;
		//----------------------------------------------------------------------------------
		/// \brief If the container has been pressed
		//----------------------------------------------------------------------------------
		bool m_selected;
		//----------------------------------------------------------------------------------
		/// \brief If the child is being directly dragged around
		//----------------------------------------------------------------------------------
		bool m_dragSelected;
		//----------------------------------------------------------------------------------
		/// \brief If the vertical scroll thumb is being dragged
		//----------------------------------------------------------------------------------
		bool m_verticalScrollDrag;
		//----------------------------------------------------------------------------------
		/// \brief If the horizontal scroll thumb is being dragged
		//----------------------------------------------------------------------------------
		bool m_horizontalScrollDrag;
		//----------------------------------------------------------------------------------
		int m_scrollDragOriginalTop;
		//----------------------------------------------------------------------------------
		int m_scrollDragOriginalLeft;
		//----------------------------------------------------------------------------------	
		// Coords of mouse when selected
		//----------------------------------------------------------------------------------
		/// \brief Mouse x-coord
		//----------------------------------------------------------------------------------
		int m_selectedMouseX;
		//----------------------------------------------------------------------------------
		/// \brief Mouse y-coord
		//----------------------------------------------------------------------------------
		int m_selectedMouseY;
		//----------------------------------------------------------------------------------
		/// \brief Indicates that the Update function needs to call Layout()
		//----------------------------------------------------------------------------------
		bool m_needLayout;
		//----------------------------------------------------------------------------------
		/// \brief Drawable for scroll track vertical
		//----------------------------------------------------------------------------------
		Drawable *m_scrollTrackVertDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Drawable for scroll track horizontal
		//----------------------------------------------------------------------------------
		Drawable *m_scrollTrackHorizDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Drawable for scroll thumb vertical
		//----------------------------------------------------------------------------------
		Drawable *m_scrollThumbVertDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Drawable for scroll thumb horizontal
		//----------------------------------------------------------------------------------
		Drawable *m_scrollThumbHorizDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Indicates whether scroll track vertical info are from theme
		//----------------------------------------------------------------------------------
		bool m_scrollTrackVertFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Indicates whether scroll track horizontal info are from theme
		//----------------------------------------------------------------------------------
		bool m_scrollTrackHorizFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Indicates whether scroll thumb vertical info are from theme
		//----------------------------------------------------------------------------------
		bool m_scrollThumbVertFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Indicates whether scroll thumb horizontal info are from theme
		//----------------------------------------------------------------------------------
		bool m_scrollThumbHorizFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Scroll track vertical name
		//----------------------------------------------------------------------------------
		std::string m_scrollTrackVertName;
		//----------------------------------------------------------------------------------
		/// \brief Scroll track horizontal name
		//----------------------------------------------------------------------------------
		std::string m_scrollTrackHorizName;
		//----------------------------------------------------------------------------------
		/// \brief Scroll thumb vertical name
		//----------------------------------------------------------------------------------
		std::string m_scrollThumbVertName;
		//----------------------------------------------------------------------------------
		/// \brief Scroll thumb horizontal name
		//----------------------------------------------------------------------------------
		std::string m_scrollThumbHorizName;
		//----------------------------------------------------------------------------------
		/// \brief Indicates whether to show horizontal bar
		//----------------------------------------------------------------------------------
		bool m_showHorizBar;
		//----------------------------------------------------------------------------------
		/// \brief Indicates whether to show vertical bar
		//----------------------------------------------------------------------------------
		bool m_showVertBar;
		//----------------------------------------------------------------------------------
		/// \brief Tests whether an event hits the vertical scrollbar
		/// result:
		///  0 means no hit
		///  1 means hit top / left scroll track
		///  2 means hit scroll thumb
		///  3 means hit bottom / right scroll track
		/// \param [in] _event
		//----------------------------------------------------------------------------------
		int EventHitVertScroll( InternalEvent *_event );
		//----------------------------------------------------------------------------------
		/// \brief Tests whether an event hits the horizontal scrollbar
		/// result:
		///  0 means no hit
		///  1 means hit top / left scroll track
		///  2 means hit scroll thumb
		///  3 means hit bottom / right scroll track
		/// \param [in] _event
		//----------------------------------------------------------------------------------
		int EventHitHorizScroll( InternalEvent *_event );
		//----------------------------------------------------------------------------------
		/// \brief Store the window width
		//----------------------------------------------------------------------------------
		int m_windowWidth;
		//----------------------------------------------------------------------------------
		/// \brief Store the window height
		//----------------------------------------------------------------------------------
		int m_windowHeight;
		//----------------------------------------------------------------------------------

	};
}

#endif
