///-----------------------------------------------------------------------------------------------
/// \file CustomLayout.h
/// \brief This ViewGroup allows the user to enter an 'edit mode' to customise the arrangement of its child Views
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUISYSTEM_CUSTOMLAYOUT__
#define __SHIVA_GUISYSTEM_CUSTOMLAYOUT__

#include <vector>

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
		/// \param [in] resources
		//----------------------------------------------------------------------------------
		virtual void NotifyDrawingContextChange( ResourceManager *resources );
		//----------------------------------------------------------------------------------
		/// \brief Gives the size of the View
		/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
		//----------------------------------------------------------------------------------
		virtual void Layout( int left, int top, int right, int bottom, int windowWidth, int windowHeight );
		//----------------------------------------------------------------------------------
		/// \brief Update function
		/// \param [in] deltaTs
		/// \param [in] guiController
		//----------------------------------------------------------------------------------
		virtual void Update( float deltaTs, GUIController *guiController );
		//----------------------------------------------------------------------------------
		/// \brief Needs to draw its children
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
		//----------------------------------------------------------------------------------
		virtual void Inflate( TiXmlElement*, ResourceManager*, std::string themePrefix = "", bool rootNode = false );
		//----------------------------------------------------------------------------------
		/// \brief For saving the View's attributes to xml
		/// \param [in] resources
		/// \note This must be hierarchical and the element must include all child elements
		//----------------------------------------------------------------------------------
		virtual TiXmlElement* Deflate( ResourceManager *resources );
		//----------------------------------------------------------------------------------
		/// \brief Input event given to View, expected to filter down hierarchy
		/// \return false if event is not absorbed (e.g. did not hit button etc)
		//----------------------------------------------------------------------------------
		virtual bool HandleEvent( InternalEvent* );
		//----------------------------------------------------------------------------------
		/// \brief Adds a child view to the group
		/// \param [in] value
		/// \param [in] resources
		//----------------------------------------------------------------------------------
		virtual void AddView( View*, ResourceManager* );
		//----------------------------------------------------------------------------------
		/// \brief For creating a relevant LayoutParams object from xml
		/// LayoutParams are used by children of a ViewGroup to tell the parent ViewGroup how they want to be arranged
		/// You must pass in the child element and this function should find the relevant attributes
		//----------------------------------------------------------------------------------
		virtual LayoutParams* InflateLayoutParams( TiXmlElement*, LayoutParams *params = NULL, std::string themePrefix = "" );
		//----------------------------------------------------------------------------------
		/// \brief For transforming coordinates from literal screen pixel-coords into a proportional space relative to the parent's position and size
		/// \param [in] inX
		/// \param [in] inY
		/// \param [out] outX
		/// \param [out] outY
		//----------------------------------------------------------------------------------
		void GetCoordsAsProportion( int inX, int inY, float &outX, float &outY );
		//----------------------------------------------------------------------------------
		/// \brief For transforming width into proportional space relative to the parent's position and size
		/// \param [in] widthIn
		//----------------------------------------------------------------------------------
		float GetWidthAsProportion( int widthIn );
		//----------------------------------------------------------------------------------
		/// \brief For transforming height into proportional space relative to the parent's position and size
		/// \param [in] heigthIn
		//----------------------------------------------------------------------------------
		float GetHeightAsProportion( int heightIn );
		//----------------------------------------------------------------------------------
		int GridSnapPosition( int value );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Vector of views
		//----------------------------------------------------------------------------------
		std::vector< View* > _views;
		//----------------------------------------------------------------------------------
		/// \brief Views that are part of the edit mode
		//----------------------------------------------------------------------------------
		std::vector< View* > _internalViews;
		//----------------------------------------------------------------------------------
		/// \brief If the layout is in its edit mode, to allow the user to move its children around etc
		/// Edit select is a mode that is activated first, usually by a special key press, then the user must select the layout to edit, putting it in actual edit mode
		bool _editSelect, _editMode;
		//----------------------------------------------------------------------------------
		/// \brief Something to give a highlight when in edit mode
		//----------------------------------------------------------------------------------
		RectDrawable *_editDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Button that appears in edit mode which allows the user to exit that mode
		//----------------------------------------------------------------------------------
		TextButton *_editExitButton;
		//----------------------------------------------------------------------------------
		/// \brief Button that appears in edit mode which allows the user to save the current layout
		//----------------------------------------------------------------------------------
		TextButton *_editSaveButton;
		//----------------------------------------------------------------------------------
		/// \brief Set edit mode
		/// \param [in] value
		//----------------------------------------------------------------------------------
		void SetEditMode( bool value );
		//----------------------------------------------------------------------------------
		/// \brief For laying out an individual View
		//----------------------------------------------------------------------------------
		void LayoutView( View *, int left, int top, int right, int bottom, int windowWidth, int windowHeight );
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
			CustomLayoutEventHandler( CustomLayout *parent );
			//----------------------------------------------------------------------------------
			/// \brif Function to handle events
			/// \param [in] view
			//----------------------------------------------------------------------------------
			virtual void HandleEvent( View *view )
			{
				_parent->CustomEventReceived( this, view );
			}
			//----------------------------------------------------------------------------------

		protected:

			//----------------------------------------------------------------------------------
			/// \brief Parent layout
			//----------------------------------------------------------------------------------
			CustomLayout *_parent;
			//----------------------------------------------------------------------------------
		};

		//----------------------------------------------------------------------------------
		/// \brief Function called when one of our buttons is pressed
		//----------------------------------------------------------------------------------
		void CustomEventReceived( CustomLayoutEventHandler*, View* );
		//----------------------------------------------------------------------------------
		/// \brief Event handlers for exit button press
		//----------------------------------------------------------------------------------
		CustomLayoutEventHandler *_handlerExitButton;
		//----------------------------------------------------------------------------------
		/// \brief Event handlers for save button press
		//----------------------------------------------------------------------------------
		CustomLayoutEventHandler *_handlerSaveButton;
		//----------------------------------------------------------------------------------
		/// \brief Set if we've had children added and need Layout to be called again
		//----------------------------------------------------------------------------------
		bool _needLayout;
		//----------------------------------------------------------------------------------
		/// \brief Will save the layout on next Update() call
		//----------------------------------------------------------------------------------
		bool _needSave;
		//----------------------------------------------------------------------------------
		bool _useGridSnap;
		//----------------------------------------------------------------------------------
		int _gridSnapSize;
		//----------------------------------------------------------------------------------

	};
}

#endif
