///-----------------------------------------------------------------------------------------------
/// \file CustomLayout.h
/// \brief This ViewGroup allows the user to enter an 'edit mode' to customise the arrangement of its child Views
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUISYSTEM_CUSTOMLAYOUT__
#define __SHIVA_GUISYSTEM_CUSTOMLAYOUT__

#include <vector>
#include <iostream>

#include "GUI/Views/View.h"
#include "GUI/Views/ViewGroups/ViewGroup.h"
#include "GUI/Drawables/RectDrawable.h"
#include "GUI/Views/TextButton.h"

namespace ShivaGUI
{
	class CustomLayout : public ViewGroup
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		CustomLayout();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~CustomLayout();
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
		/// \brief Function is called to create unique VAO ids when having multiple OGL contexts
		//----------------------------------------------------------------------------------
		virtual void CreateBBoxVAOs( unsigned int _context );
		//----------------------------------------------------------------------------------
		/// \brief Update function
		/// \param [in] _deltaTs
		/// \param [in] _guiController
		//----------------------------------------------------------------------------------
		virtual void Update( float _deltaTs, GUIController *_guiController );
		//----------------------------------------------------------------------------------
		/// \brief Needs to draw its children
		//----------------------------------------------------------------------------------
		virtual void Draw( unsigned int _context );
		//----------------------------------------------------------------------------------
		virtual void Draw();
		//----------------------------------------------------------------------------------
		/// \brief Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		/// \return "CustomLayout_"
		//----------------------------------------------------------------------------------
		virtual std::string GetThemePrefix() { return "CustomLayout_"; }
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
		virtual TiXmlElement* Deflate( ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief Input event given to View, expected to filter down hierarchy
		/// \return false if event is not absorbed (e.g. did not hit button etc)
		//----------------------------------------------------------------------------------
		virtual bool HandleEvent( InternalEvent* _event );
		//----------------------------------------------------------------------------------
		/// \brief Adds a child view to the group
		/// \param [in] _value
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual void AddView( View* _value, ResourceManager* _resources );
		//----------------------------------------------------------------------------------
		/// \brief For creating a relevant LayoutParams object from xml
		/// LayoutParams are used by children of a ViewGroup to tell the parent ViewGroup how they want to be arranged
		/// You must pass in the child element and this function should find the relevant attributes
		//----------------------------------------------------------------------------------
		virtual LayoutParams* InflateLayoutParams( TiXmlElement* _xmlElement, LayoutParams *_params = NULL, std::string _themePrefix = "" );
		//----------------------------------------------------------------------------------
		/// \brief For transforming coordinates from literal screen pixel-coords into a proportional space relative to the parent's position and size
		/// \param [in] _inX
		/// \param [in] _inY
		/// \param [out] _outX
		/// \param [out] _outY
		//----------------------------------------------------------------------------------
		void GetCoordsAsProportion( int _inX, int _inY, float &_outX, float &_outY );
		//----------------------------------------------------------------------------------
		/// \brief For transforming width into proportional space relative to the parent's position and size
		/// \param [in] widthIn
		//----------------------------------------------------------------------------------
		float GetWidthAsProportion( int _widthIn );
		//----------------------------------------------------------------------------------
		/// \brief For transforming height into proportional space relative to the parent's position and size
		/// \param [in] heigthIn
		//----------------------------------------------------------------------------------
		float GetHeightAsProportion( int _heightIn );
		//----------------------------------------------------------------------------------
		int GridSnapPosition( int _value );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Vector of views
		//----------------------------------------------------------------------------------
		std::vector< View* > m_views;
		//----------------------------------------------------------------------------------
		/// \brief Views that are part of the edit mode
		//----------------------------------------------------------------------------------
		std::vector< View* > m_internalViews;
		//----------------------------------------------------------------------------------
		/// \brief If the layout is in its edit mode, to allow the user to move its children around etc
		/// Edit select is a mode that is activated first, usually by a special key press, then the user must select the layout to edit, putting it in actual edit mode
		bool m_editSelect;
		//----------------------------------------------------------------------------------
		bool m_editMode;
		//----------------------------------------------------------------------------------
		/// \brief Something to give a highlight when in edit mode
		//----------------------------------------------------------------------------------
		RectDrawable *m_editDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Button that appears in edit mode which allows the user to exit that mode
		//----------------------------------------------------------------------------------
		TextButton *m_editExitButton;
		//----------------------------------------------------------------------------------
		/// \brief Button that appears in edit mode which allows the user to save the customised layout
		//----------------------------------------------------------------------------------
		TextButton *m_editSaveButton;
		//----------------------------------------------------------------------------------
		/// \brief Set edit mode
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetEditMode( bool _value );
		//----------------------------------------------------------------------------------
		/// \brief For laying out an individual View
		//----------------------------------------------------------------------------------
		void LayoutView( View *_currentView, int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight );
		//----------------------------------------------------------------------------------
		// Used to get events from our buttons
		//----------------------------------------------------------------------------------
		class CustomLayoutEventHandler : public ViewEventListener
		{
		public:

			//----------------------------------------------------------------------------------
			/// \brief Ctor
			/// \param [in] parent
			//----------------------------------------------------------------------------------
			CustomLayoutEventHandler( CustomLayout *_parent );
			//----------------------------------------------------------------------------------
			/// \brif Function to handle events
			/// \param [in] view
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
			CustomLayout *m_parent;
			//----------------------------------------------------------------------------------
		};

		//----------------------------------------------------------------------------------
		/// \brief Function called when one of our buttons is pressed
		//----------------------------------------------------------------------------------
		void CustomEventReceived( CustomLayoutEventHandler*, View* );
		//----------------------------------------------------------------------------------
		/// \brief Event handlers for exit button press
		//----------------------------------------------------------------------------------
		CustomLayoutEventHandler *m_handlerExitButton;
		//----------------------------------------------------------------------------------
		/// \brief Event handlers for save button press
		//----------------------------------------------------------------------------------
		CustomLayoutEventHandler *m_handlerSaveButton;
		//----------------------------------------------------------------------------------
		/// \brief Set if we've had children added and need Layout to be called again
		//----------------------------------------------------------------------------------
		bool m_needLayout;
		//----------------------------------------------------------------------------------
		/// \brief Will save the layout on next Update() call
		//----------------------------------------------------------------------------------
		bool m_needSave;
		//----------------------------------------------------------------------------------
		/// \brief Whether to use grid snap
		bool m_useGridSnap;
		//----------------------------------------------------------------------------------
		/// \brief Grid snap size
		//----------------------------------------------------------------------------------
		int m_gridSnapSize;
		//----------------------------------------------------------------------------------

	};
}

#endif
