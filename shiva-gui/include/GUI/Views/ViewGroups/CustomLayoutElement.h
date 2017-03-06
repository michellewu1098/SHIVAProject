///-----------------------------------------------------------------------------------------------
/// \file CustomLayoutElement.h
/// \brief This View contains a single element for the editing mode of the CustomLayout
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUISYSTEM_CUSTOMLAYOUTELEMENT__
#define __SHIVA_GUISYSTEM_CUSTOMLAYOUTELEMENT__

#include "GUI/Views/View.h"
#include "GUI/Views/ViewGroups/ViewGroup.h"
#include "GUI/Views/ImageButton.h"
#include "GUI/Views/TextButton.h"
#include "GUI/Drawables/RectDrawable.h"

namespace ShivaGUI
{

	class CustomLayout;

	class CustomLayoutElement : public View
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		/// \param [in] _child
		/// \param [in] _resources
		/// \param [in] _parent
		//----------------------------------------------------------------------------------
		CustomLayoutElement( View *_child, ResourceManager *_resources, CustomLayout *_parent );
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~CustomLayoutElement();
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
		virtual std::string GetThemePrefix() { return "CustomLayoutElement_"; }
		//----------------------------------------------------------------------------------
		/// \brief Get layour parameters
		//----------------------------------------------------------------------------------
		virtual LayoutParams* GetLayoutParams() { return m_child->GetLayoutParams(); }
		//----------------------------------------------------------------------------------
		/// \brief For setting the View's attributes from xml
		/// \param [in] _xmlElement
		/// \param [in] _resources
		/// \param [in] _themePrefix
		/// \param [in] _rootNode
		//----------------------------------------------------------------------------------
		virtual void Inflate( TiXmlElement* _xmlElement, ResourceManager* _resources, std::string _themePrefix = "", bool _rootNode = false);
		//----------------------------------------------------------------------------------
		/// \brief Input event given to View, expected to filter down hierarchy
		/// \param [in] _event
		/// \return false if event is not absorbed (e.g. did not hit button etc)
		//----------------------------------------------------------------------------------
		virtual bool HandleEvent( InternalEvent* _event );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Child view
		//----------------------------------------------------------------------------------
		View *m_child;
		//----------------------------------------------------------------------------------
		/// \brief Parent layout
		//----------------------------------------------------------------------------------
		CustomLayout *m_parent;
		//----------------------------------------------------------------------------------
		/// \brief If we have focus for editing
		//----------------------------------------------------------------------------------
		bool m_selected;
		//----------------------------------------------------------------------------------
		/// \brief If we are selected for dragging
		//----------------------------------------------------------------------------------
		bool m_dragSelected;
		//----------------------------------------------------------------------------------
		/// \brief If we are selected for resizing
		//----------------------------------------------------------------------------------
		bool m_resizeSelected;
		//----------------------------------------------------------------------------------
		/// \brief Mouse x-offset from centre of View when widget is selected, mainly used for dragging
		//----------------------------------------------------------------------------------
		float m_selectedX;
		//----------------------------------------------------------------------------------
		/// \brief Mouse y-offset from centre of View when widget is selected, mainly used for dragging
		//----------------------------------------------------------------------------------
		float m_selectedY;
		//----------------------------------------------------------------------------------
		float m_selectedXProp;
		//----------------------------------------------------------------------------------
		float m_selectedYProp;
		//----------------------------------------------------------------------------------
		float m_selectedWidth;
		//----------------------------------------------------------------------------------
		float m_selectedHeight;
		//----------------------------------------------------------------------------------
		/// \brief Views that are part of the edit mode
		//----------------------------------------------------------------------------------
		std::vector< View* > m_internalViews;
		//----------------------------------------------------------------------------------
		/// \brief Something to give a highlight
		//----------------------------------------------------------------------------------
		RectDrawable *m_editDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Something for resizing
		//----------------------------------------------------------------------------------
		RectDrawable *m_resizeDrawable;
		//----------------------------------------------------------------------------------
		/// \brief For toggling child View's visibility
		//----------------------------------------------------------------------------------
		ImageButton *m_editVisibilityButton;
		//----------------------------------------------------------------------------------
		/// \brief For re-enabling wrapping for the child's size
		//----------------------------------------------------------------------------------
		ImageButton *m_editWrapSizeButton;
		//----------------------------------------------------------------------------------
		/// \brief Set selected
		/// \param [in] _value
		/// \param [in] _drag
		//----------------------------------------------------------------------------------
		bool SetSelected( bool _value, bool _drag = false );
		//----------------------------------------------------------------------------------
		/// \brief For laying out an individual View
		//----------------------------------------------------------------------------------
		void LayoutView( View *_value, int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight );
		//----------------------------------------------------------------------------------
		/// \brief For checking whether a mouse event hit the resize widget
		//----------------------------------------------------------------------------------
		bool EventHitResize( InternalEvent* _event );
		//----------------------------------------------------------------------------------
		/// \brief Retrieves the current size of the view in proportion to the parent's size
		/// \param [out] _width
		/// \param [out] _height
		//----------------------------------------------------------------------------------
		void GetCurrentSizeProp( float &_width, float &_height );
		//----------------------------------------------------------------------------------

		// Used to get events from our buttons

		class CustomElementEventHandler : public ViewEventListener
		{
		public:

			//----------------------------------------------------------------------------------
			/// \brief Ctor
			/// \param [in] _parent
			//----------------------------------------------------------------------------------
			CustomElementEventHandler( CustomLayoutElement *_parent );
			//----------------------------------------------------------------------------------
			/// \brief Handle event
			/// \param [in] _view
			//----------------------------------------------------------------------------------
			virtual void HandleEvent( View *_view )
			{
				m_parent->CustomEventReceived( this, _view );
			}
			//----------------------------------------------------------------------------------

		protected:

			//----------------------------------------------------------------------------------
			/// \brief Parent layout
			//----------------------------------------------------------------------------------
			CustomLayoutElement *m_parent;
			//----------------------------------------------------------------------------------

		};

		//----------------------------------------------------------------------------------
		/// \brief This is called when one of our buttons is pressed
		//----------------------------------------------------------------------------------
		void CustomEventReceived( CustomElementEventHandler*, View* );
		//----------------------------------------------------------------------------------
		/// \brief Event handler for button presses
		//----------------------------------------------------------------------------------
		CustomElementEventHandler *m_eventHandler;
		//----------------------------------------------------------------------------------

	};
}

#endif
