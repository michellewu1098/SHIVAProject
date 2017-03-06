///-----------------------------------------------------------------------------------------------
/// \file ListView.h
/// \brief This presents Views from an Adapter in a list format, similar to a LinearLayout 
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_GUI_VIEWS_ADAPTERVIEWS_LISTVIEW__
#define __SHIVA_GUI_GUI_VIEWS_ADAPTERVIEWS_LISTVIEW__

#include "GUI/Views/AdapterViews/AdapterView.h"
#include "GUI/Views/ViewGroups/ViewGroup.h"

#include <vector>

namespace ShivaGUI
{
	class ListView : public AdapterView
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		ListView();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~ListView();
		//----------------------------------------------------------------------------------
		/// \brief This is called when the drawing context is changed
		/// All Drawables loaded from file will be automatically reloaded,
		/// but any locally-generated textures (e.g. text caches) must be remade here
		/// \param [in] _resources Reference to resource manager
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
		/// \param [in] _deltaTs
		/// \param [in] _guiController Reference to GUI controller
		//----------------------------------------------------------------------------------
		virtual void Update( float _deltaTs, GUIController* _guiController );
		//----------------------------------------------------------------------------------
		/// \brief The base class will draw a background drawable if it exists
		/// Derived View classes must call this at an appropriate time if they want to retain the background
		//----------------------------------------------------------------------------------
		virtual void Draw();
		//----------------------------------------------------------------------------------
		/// \brief Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		/// \return "ListView_"
		//----------------------------------------------------------------------------------
		virtual std::string GetThemePrefix() { return "ListView_"; }
		//----------------------------------------------------------------------------------
		/// \brief For setting the View's attributes from xml
		/// \param [in] _xmlElement
		/// \param [in] _resources Reference to resource manager
		/// \param [in] _themePrefix this is used when parsing a theme file. Derived classes should specify the prefix that identifies their attributes in a style. If empty, the string returned from GetThemePrefix should be used
		/// \param [in] _rootNode
		//----------------------------------------------------------------------------------
		virtual void Inflate( TiXmlElement* _xmlElement, ResourceManager* _resources, std::string _themePrefix = "", bool _rootNode = false );
		//----------------------------------------------------------------------------------
		/// \brief For saving the View's attributes to xml
		/// \param [in] _resources Reference to resource manager
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
		/// \brief Input event given to View, expected to filter down hierarchy
		/// \return false if event is not absorbed (e.g. did not hit button etc)
		//----------------------------------------------------------------------------------
		virtual bool HandleEvent( InternalEvent* );
		//----------------------------------------------------------------------------------
		/// \brief Requested wrap pixel width dimension for layout
		/// \note These are used when SetLayoutParams() is called, which will be after the View has been inflated
		//----------------------------------------------------------------------------------
		virtual int GetWrapWidth();
		//----------------------------------------------------------------------------------
		/// \brief Requested wrap pixel height dimension for layout
		//----------------------------------------------------------------------------------
		virtual int GetWrapHeight();
		//----------------------------------------------------------------------------------
		/// \brief For retrieving the original data entry index that was used to generate the View
		/// Returns the index or -1 if the View that was given is not recognised
		/// Note that this function is *not* required to traverse entire trees,
		/// it should simply check against the View that was returned by the data adapter's GetView() function
		//----------------------------------------------------------------------------------
		virtual int GetDataIndex( View * );
		//----------------------------------------------------------------------------------
		/// \brief This should be called when the source data is known to have changed
		/// The AdapterView should then remove all child Views and rebuild them
		//----------------------------------------------------------------------------------
		virtual void RefreshFromSource();
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Children views
		//----------------------------------------------------------------------------------
		std::vector< View* > m_children;
		//----------------------------------------------------------------------------------
		/// \brief Whether it has attempted to retrieve the Views from the DataAdapter
		//----------------------------------------------------------------------------------
		bool m_initialised;
		//----------------------------------------------------------------------------------
		/// \param [in] value
		/// \param [in] forward
		//----------------------------------------------------------------------------------
		void OnSetNextScan( View *_value, bool _forward ) { RefreshConnectionLinks(); }
		//----------------------------------------------------------------------------------
		virtual void OnSetNextFocus( View*, Definitions::FocusDirection ) { RefreshConnectionLinks(); }
		//----------------------------------------------------------------------------------
		/// \brief Rebuilds next scan and next focus links
		//----------------------------------------------------------------------------------
		void RefreshConnectionLinks();
		//----------------------------------------------------------------------------------

	};
}

#endif
