///-----------------------------------------------------------------------------------------------
/// \file View.h
/// \brief This is the base widget class. It does not use pure virtual functions so that you can use it as an 'empty' View
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_GUI_VIEWS_VIEW__
#define __SHIVA_GUI_GUI_VIEWS_VIEW__

//#define _CRTDBG_MAP_ALLOC
//#define _CRTDBG_MAP_ALLOC_NEW
//#include <stdlib.h>
//#include <crtdbg.h>
//
//#ifdef _DEBUG
//   #ifndef DBG_NEW
//      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//      #define new DBG_NEW
//   #endif
//#endif  // _DEBUG


#include "Input/InternalEvent.h"
#include "ResourceManager.h"
#include "GUI/Views/ViewGroups/LayoutParams.h"
#include "GUI/Definitions.h"
#include "Utility/tinyxml.h"

namespace ShivaGUI
{
	// Annoying forward declaration
	class GUIController;
	class ResourceManager;

	class View
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		View();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~View();
		//----------------------------------------------------------------------------------
		/// \brief Set view id
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetID( std::string _value ) { m_id = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Returns view's id
		/// \return m_id
		//----------------------------------------------------------------------------------
		std::string GetID() const { return m_id; }
		//----------------------------------------------------------------------------------
		/// \brief This is called when the drawing context is changed
		/// All Drawables loaded from file will be automatically reloaded,
		/// but any locally-generated textures (e.g. text caches) must be remade here
		/// This must be passed down to any children of the View
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual void NotifyDrawingContextChange( ResourceManager *_resources ) {}
		//----------------------------------------------------------------------------------
		/// \brief Gives the size of the View
		/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
		/// The base class function will set up the background drawable, so this must be called by derived View classes that wish to retain the background
		//----------------------------------------------------------------------------------
		virtual void Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight );
		//----------------------------------------------------------------------------------
		/// \brief Function is called to create unique VAO ids when having multiple OGL contexts
		//----------------------------------------------------------------------------------
		virtual void CreateBBoxVAOs( unsigned int _context ) {}
		//----------------------------------------------------------------------------------
		/// \brief Update method
		//----------------------------------------------------------------------------------
		virtual void Update( float _deltaTs, GUIController *_guiController ) {}
		//----------------------------------------------------------------------------------
		/// \brief The base class will draw a background drawable if it exists
		/// Derived View classes must call this at an appropriate time if they want to retain the background
		//----------------------------------------------------------------------------------
		virtual void Draw();
		//----------------------------------------------------------------------------------
		/// \brief The base class will draw a background drawable if it exists
		/// \param [in] _context Current OGL context
		//----------------------------------------------------------------------------------
		virtual void Draw( unsigned int _context );
		//----------------------------------------------------------------------------------
		/// \brief Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		//----------------------------------------------------------------------------------
		virtual std::string GetThemePrefix() { return "View_"; }
		//----------------------------------------------------------------------------------
		/// \brief For setting the View's attributes from xml
		/// \param themePrefix this is used when parsing a theme file. Derived classes should specify the prefix that identifies their attributes in a style. If empty, the string returned from GetThemePrefix should be used
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
		/// \brief Set view background
		/// \param [in] _bgd
		//----------------------------------------------------------------------------------
		void SetBackground( Drawable *_bgd ) { m_background = _bgd; }
		//----------------------------------------------------------------------------------
		/// \brief The LayoutParams specify how we *want* the View to be set out, in terms of size and position inside its parent
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetLayoutParams( LayoutParams *_value );
		//----------------------------------------------------------------------------------
		/// \brief Get layout parameters
		/// \return m_layoutParams;
		//----------------------------------------------------------------------------------
		virtual LayoutParams* GetLayoutParams() { return m_layoutParams; }
		//----------------------------------------------------------------------------------
		/// \brief Retrieves the next View for selection in the given direction
		/// The idea is that all focusable widgets are connected to each other, so the user can cycle through them
		/// \return Itself if there is no next focusable View
		//----------------------------------------------------------------------------------
		View* GetNextFocus( Definitions::FocusDirection _dir );
		//----------------------------------------------------------------------------------
		/// \brief Set next focus
		/// \param [in] _value
		/// \param [in] _dir
		//----------------------------------------------------------------------------------
		void SetNextFocus( View* _value, Definitions::FocusDirection _dir );
		//----------------------------------------------------------------------------------
		/// \brief Set if it's first focus
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetIsFirstFocus( bool _value ) { m_isFirstFocus = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Scan is like focus, but only 2D
		/// \param [in] _forward
		//----------------------------------------------------------------------------------
		View* GetNextScan( bool _forward );
		//----------------------------------------------------------------------------------
		/// \brief Set next scan
		/// \param [in] _value
		/// \param [in] _forward
		//----------------------------------------------------------------------------------
		void SetNextScan( View* _value, bool _forward );
		//----------------------------------------------------------------------------------
		/// \brief Set if it first scan
		/// \param [in] _value Either true or false
		//----------------------------------------------------------------------------------
		void SetIsFirstScan( bool _value ) { m_isFirstScan = _value; }
		//----------------------------------------------------------------------------------
		/// \brief For setting the focus of this View
		/// If focussed, it is expected that the view will show this visually
		/// and then allow other forms of input to be received, e.g. 'enter' button pressed etc
		//----------------------------------------------------------------------------------
		virtual void SetFocus( bool ) {}
		//----------------------------------------------------------------------------------
		/// \brief For selecting the View, e.g. after focussing on it you press the 'enter' key
		//----------------------------------------------------------------------------------
		virtual void SetSelect( bool ) {}
		//----------------------------------------------------------------------------------
		/// \brief Input event given to View, expected to filter down hierarchy
		/// \return false if event is not absorbed (e.g. did not hit button etc)
		//----------------------------------------------------------------------------------
		virtual bool HandleEvent( InternalEvent* _event ) { return false; }
		//----------------------------------------------------------------------------------
		/// \brief Set visibility
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetVisibility( bool _value ) { m_visible = _value; OnChangeVisible(); }
		//----------------------------------------------------------------------------------
		/// \brief Returns if it's visible
		/// \return m_visible
		//----------------------------------------------------------------------------------
		bool GetVisibility() const { return m_visible; }
		//----------------------------------------------------------------------------------
		/// \brief Set active
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetActive( bool _value ) { m_active = _value; OnChangeActive(); }
		//----------------------------------------------------------------------------------
		/// \brief Returns if it's active
		/// \return m_active
		//----------------------------------------------------------------------------------
		bool GetActive() const { return m_active; }
		//----------------------------------------------------------------------------------
		/// \brief Requested wrap pixel dimensions for layout
		/// \note These are used when SetLayoutParams() is called, which will be after the View has been inflated
		//----------------------------------------------------------------------------------
		virtual int GetWrapWidth() {return 0;}
		//----------------------------------------------------------------------------------
		/// \brief Requested wrap pixel dimensions for layout
		/// \note These are used when SetLayoutParams() is called, which will be after the View has been inflated
		//----------------------------------------------------------------------------------
		virtual int GetWrapHeight() { return 0; }
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief ID for the View. This is mainly for use in xml layouts, where Views need to refer to each other
		//----------------------------------------------------------------------------------
		std::string m_id;
		//----------------------------------------------------------------------------------
		/// \brief Whether the view will take focus on issue of the first focus event
		/// \default False
		//----------------------------------------------------------------------------------
		bool m_isFirstFocus;
		//----------------------------------------------------------------------------------
		/// \brief Each View stores the next View to select when focus changes in one of the four directions
		//----------------------------------------------------------------------------------
		View **m_nextFocus;
		//----------------------------------------------------------------------------------
		/// \brief Whether the view will take focus on issue of the first scan event
		/// \default False
		//----------------------------------------------------------------------------------
		bool m_isFirstScan;
		//----------------------------------------------------------------------------------
		/// \brief Next scan
		//----------------------------------------------------------------------------------
		View *m_nextScan;
		//----------------------------------------------------------------------------------
		/// \brief Previous scan
		//----------------------------------------------------------------------------------
		View* m_prevScan;
		//----------------------------------------------------------------------------------
		/// \brief These tell the parent layout how the View wants to be arranged
		//----------------------------------------------------------------------------------
		LayoutParams *m_layoutParams;
		//----------------------------------------------------------------------------------
		/// \brief A background drawable normally to be drawn behind the View-specific graphics
		//----------------------------------------------------------------------------------
		Drawable *m_background;
		//----------------------------------------------------------------------------------
		/// \brief If various attributes were loaded from theme rather than directly from xml
		/// This is mainly used when deflating to xml, so we don't write too much stuff
		//----------------------------------------------------------------------------------
		bool m_fromTheme_bgd;
		//----------------------------------------------------------------------------------
		/// \brief Whether view is visible
		//----------------------------------------------------------------------------------
		bool m_visible;
		//----------------------------------------------------------------------------------
		/// \brief Whether view is active
		//----------------------------------------------------------------------------------
		bool m_active;
		//----------------------------------------------------------------------------------
		/// \brief Called when active
		//----------------------------------------------------------------------------------
		virtual void OnChangeActive() {}
		//----------------------------------------------------------------------------------
		/// \brief Called when visible
		//----------------------------------------------------------------------------------
		virtual void OnChangeVisible() {}
		//----------------------------------------------------------------------------------
		/// \brief Called when setting next scan
		/// \param [in] _value
		/// \param [in] _forward
		//----------------------------------------------------------------------------------
		virtual void OnSetNextScan( View *_value, bool _forward ) {}
		//----------------------------------------------------------------------------------
		/// \brief Called when setting next focus
		/// \param [in] _value
		/// \param [in] _dir Focus direction
		//----------------------------------------------------------------------------------
		virtual void OnSetNextFocus( View* _value, Definitions::FocusDirection _dir ) {}
		//----------------------------------------------------------------------------------
		/// \brief Specifies the gravity of View's contents When View is larger than its content and that content does not stretch to fit View's size
		/// Default value is CENTRE
		//----------------------------------------------------------------------------------
		unsigned int m_internalGravity;
		//----------------------------------------------------------------------------------
		// Values given to the View through the Layout() function
		//----------------------------------------------------------------------------------
		/// \brief Layout bounds on left
		//----------------------------------------------------------------------------------
		int m_layoutBoundsLeft;
		//----------------------------------------------------------------------------------
		/// \brief Layout bounds on top
		//----------------------------------------------------------------------------------
		int m_layoutBoundsTop;
		//----------------------------------------------------------------------------------
		/// \brief Layout bounds on right
		//----------------------------------------------------------------------------------
		int m_layoutBoundsRight;
		//----------------------------------------------------------------------------------
		/// \brief Layout bounds on bottom
		//----------------------------------------------------------------------------------
		int m_layoutBoundsBottom;
		//----------------------------------------------------------------------------------
		/// \brief Layout window width
		//----------------------------------------------------------------------------------
		int m_layoutWindowWidth;
		//----------------------------------------------------------------------------------
		/// \brief Layout window height
		//----------------------------------------------------------------------------------
		int m_layoutWindowHeight;
		//----------------------------------------------------------------------------------
		/// \brief Utility function, returns true if a positional event coord is on the View
		/// Uses the values given by the Layout function
		/// \param [in] _event
		//----------------------------------------------------------------------------------
		bool EventHit( InternalEvent* _event );
		//----------------------------------------------------------------------------------

	};
}

#endif
