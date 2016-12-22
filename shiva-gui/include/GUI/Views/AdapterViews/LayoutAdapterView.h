///-----------------------------------------------------------------------------------------------
/// \file LayoutAdapterView.h
/// \brief This presents Views from an Adapter in a customisable format, by using a generic ViewGroup to arrange the Views
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_GUI_VIEWS_ADAPTERVIEWS_LAYOUTADAPTERVIEW__
#define __SHIVA_GUI_GUI_VIEWS_ADAPTERVIEWS_LAYOUTADAPTERVIEW__

#include <vector>

#include "GUI/Views/AdapterViews/AdapterView.h"
#include "GUI/Views/ViewGroups/ViewGroup.h"


namespace ShivaGUI
{
	class LayoutAdapterView : public AdapterView
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		LayoutAdapterView();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~LayoutAdapterView();
		//----------------------------------------------------------------------------------
		/// This is called when the drawing context is changed
		/// All Drawables loaded from file will be automatically reloaded,
		/// but any locally-generated textures (e.g. text caches) must be remade here
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
		virtual std::string GetThemePrefix() {return "LayoutAdapterView_";}

		/// For setting the View's attributes from xml
		/// \param themePrefix this is used when parsing a theme file. Derived classes should specify the prefix that identifies their attributes in a style. If empty, the string returned from GetThemePrefix should be used
		virtual void Inflate(TiXmlElement*, ResourceManager*, std::string themePrefix = "", bool rootNode = false);
		/// For saving the View's attributes to xml
		/// \note This must be hierarchical and the element must include all child elements
		virtual TiXmlElement* Deflate(ResourceManager *resources);
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
		/// \brief Requested wrap pixel dimensions for layout
		/// \note These are used when SetLayoutParams() is called, which will be after the View has been inflated
		//----------------------------------------------------------------------------------
		virtual int GetWrapWidth();
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
		/// \brief Container for childern (views)
		//----------------------------------------------------------------------------------
		std::vector< View* > _children;
		//----------------------------------------------------------------------------------
		/// \brief Flat use to check whether it has attempted to retrieve the Views from the DataAdapter
		//----------------------------------------------------------------------------------
		bool _initialised;
		//----------------------------------------------------------------------------------
		/// \param [in] value
		/// \param [in] forward
		//----------------------------------------------------------------------------------
		void OnSetNextScan( View *value, bool forward ) { RefreshConnectionLinks(); }
		//----------------------------------------------------------------------------------
		/// \param [in] value
		/// \param [in] 
		//----------------------------------------------------------------------------------
		virtual void OnSetNextFocus(View*, Definitions::FocusDirection) { RefreshConnectionLinks(); }
		//----------------------------------------------------------------------------------
		/// \brief Rebuilds next scan and next focus links
		//----------------------------------------------------------------------------------
		void RefreshConnectionLinks();
		//----------------------------------------------------------------------------------
		/// \brief ListView now has a ViewGroup to manage the display of the children. This allows for more flexibility in layout
		//----------------------------------------------------------------------------------
		ViewGroup *_containerGroup;
		//----------------------------------------------------------------------------------
	};
}

#endif
