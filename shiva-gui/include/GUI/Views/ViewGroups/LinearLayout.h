///-----------------------------------------------------------------------------------------------
/// \file LinearLayout.h
/// \brief This ViewGroup arranges its child Views linearly
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUISYSTEM_LINEARLAYOUT__
#define __SHIVA_GUISYSTEM_LINEARLAYOUT__

#include "GUI/Views/ViewGroups/ViewGroup.h"

namespace ShivaGUI
{
	class LinearLayout : public ViewGroup
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		LinearLayout();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~LinearLayout();
		//----------------------------------------------------------------------------------
		/// \brief This is called when the drawing context is changed
		/// All Drawables loaded from file will be automatically reloaded,
		/// but any locally-generated textures (e.g. text caches) must be remade here
		/// This must be passed down to any children of the View
		//----------------------------------------------------------------------------------
		virtual void NotifyDrawingContextChange( ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief Gives the size of the View
		/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
		//----------------------------------------------------------------------------------
		virtual void Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight );
		//----------------------------------------------------------------------------------
		/// \brief Update method
		/// \param [in] _deltaTs
		/// \param [in] _guiController
		//----------------------------------------------------------------------------------
		virtual void Update( float _deltaTs, GUIController *_guiController );
		//----------------------------------------------------------------------------------
		/// \brief Needs to draw its children
		//----------------------------------------------------------------------------------
		virtual void Draw();
		//----------------------------------------------------------------------------------
		/// \brief Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		//----------------------------------------------------------------------------------
		virtual std::string GetThemePrefix() { return "LinearLayout_"; }
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
		/// \param [in] _resources
		/// \note This must be hierarchical and the element must include all child elements
		//----------------------------------------------------------------------------------
		virtual TiXmlElement* Deflate( ResourceManager *resources );
		//----------------------------------------------------------------------------------
		/// \brief Input event given to View, expected to filter down hierarchy
		/// \param [in] _event
		/// \return false if event is not absorbed (e.g. did not hit button etc)
		//----------------------------------------------------------------------------------
		virtual bool HandleEvent( InternalEvent* _event );
		//----------------------------------------------------------------------------------
		/// \brief LinearLayouts either arrange their children in a vertical column or a horizontal row
		//----------------------------------------------------------------------------------
		void SetOrientation( bool _vertical );
		//----------------------------------------------------------------------------------
		/// \brief Adds a child view to the group
		/// \param [in] _view
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual void AddView( View*, ResourceManager* );
		//----------------------------------------------------------------------------------
		/// \brief For creating a relevant LayoutParams object from xml
		/// LayoutParams are used by children of a ViewGroup to tell the parent ViewGroup how they want to be arranged
		/// You must pass in the child element and this function should find the relevant attributes
		//----------------------------------------------------------------------------------
		virtual LayoutParams* InflateLayoutParams( TiXmlElement* _xmlElement, LayoutParams *_params = NULL, std::string _themePrefix = "" );
		//----------------------------------------------------------------------------------
		/// \brief Requested wrap pixel width for layout
		/// \note These are used when SetLayoutParams() is called, which will be after the View has been inflated
		//----------------------------------------------------------------------------------
		virtual int GetWrapWidth();
		//----------------------------------------------------------------------------------
		/// \brief Requested wrap pixel height for layout
		/// \note These are used when SetLayoutParams() is called, which will be after the View has been inflated
		//----------------------------------------------------------------------------------
		virtual int GetWrapHeight();
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Vector of views
		//----------------------------------------------------------------------------------
		std::vector< View* > m_views;
		//----------------------------------------------------------------------------------
		/// \brief Whether children are in a vertical column or horizontal row
		//----------------------------------------------------------------------------------
		bool m_verticalOrientation;
		//----------------------------------------------------------------------------------

	};
}

#endif
