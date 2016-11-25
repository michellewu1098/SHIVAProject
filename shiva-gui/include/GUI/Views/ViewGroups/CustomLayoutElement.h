
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUISYSTEM_CUSTOMLAYOUTELEMENT__
#define __SHIVA_GUISYSTEM_CUSTOMLAYOUTELEMENT__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <vector>

//////////////////////////////////////////////////////////////////////////
#include "GUI/Views/View.h"
#include "GUI/Views/ViewGroups/ViewGroup.h"
#include "GUI/Views/ImageButton.h"
#include "GUI/Views/TextButton.h"
#include "GUI/Drawables/RectDrawable.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	class CustomLayout;

	/// This View contains a single element for the editing mode of the CustomLayout
	class CustomLayoutElement : public View
	{
	public:

		CustomLayoutElement(View *child, ResourceManager *resources, CustomLayout *parent);
		virtual ~CustomLayoutElement();

		/// This is called when the drawing context is changed
		/// All Drawables loaded from file will be automatically reloaded,
		/// but any locally-generated textures (e.g. text caches) must be remade here
		/// This must be passed down to any children of the View
		virtual void NotifyDrawingContextChange(ResourceManager *resources);

		/// Gives the size of the View
		/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
		virtual void Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight);

		virtual void Update(float deltaTs, GUIController *guiController);

		/// Needs to draw its children
		virtual void Draw();

		/// Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		virtual std::string GetThemePrefix() {return "CustomLayoutElement_";}


		virtual LayoutParams* GetLayoutParams() {return _child->GetLayoutParams();}

		/// For setting the View's attributes from xml
		virtual void Inflate(TiXmlElement*,ResourceManager*, std::string themePrefix = "", bool rootNode = false);

		/// Input event given to View, expected to filter down hierarchy
		/// \return false if event is not absorbed (e.g. did not hit button etc)
		virtual bool HandleEvent(InternalEvent*);


	protected:
		View *_child;

		CustomLayout *_parent;

		/// If we have focus for editing
		bool _selected;

		/// If we are selected for dragging
		bool _dragSelected;

		/// If we are selected for resizing
		bool _resizeSelected;

		/// Mouse offsets from centre of View when widget is selected
		/// Mainly used for dragging
		float _selectedX, _selectedY;
		float _selectedXProp, _selectedYProp;
		float _selectedWidth, _selectedHeight;

		/// Views that are part of the edit mode
		std::vector<View*> _internalViews;

		/// Something to give a highlight
		RectDrawable *_editDrawable;

		/// Something for resizing
		RectDrawable *_resizeDrawable;

		/// For toggling child View's visibility
		ImageButton *_editVisibilityButton;

		/// For re-enabling wrapping for the child's size
		ImageButton *_editWrapSizeButton;

		bool SetSelected(bool value, bool drag = false);

		/// For laying out an individual View
		void LayoutView(View *, int left, int top, int right, int bottom, int windowWidth, int windowHeight);

		/// For checking whether a mouse event hit the resize widget
		bool EventHitResize(InternalEvent*);

		/// Retrieves the current size of the view in proportion to the parent's size
		void GetCurrentSizeProp(float &width, float &height);

		/// Used to get events from our buttons
		class CustomElementEventHandler : public ViewEventListener
		{
		public:
			CustomElementEventHandler(CustomLayoutElement *parent);
			virtual void HandleEvent(View *view)
			{
				_parent->CustomEventReceived(this,view);
			}
		protected:
			CustomLayoutElement *_parent;
		};

		/// This is called when one of our buttons is pressed
		void CustomEventReceived(CustomElementEventHandler*,View*);

		/// Event handler for button presses
		CustomElementEventHandler *_eventHandler;

	};

}

//////////////////////////////////////////////////////////////////////////
#endif
