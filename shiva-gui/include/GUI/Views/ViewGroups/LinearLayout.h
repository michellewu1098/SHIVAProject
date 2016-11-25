
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUISYSTEM_LINEARLAYOUT__
#define __SHIVA_GUISYSTEM_LINEARLAYOUT__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <vector>

//////////////////////////////////////////////////////////////////////////
#include "GUI/Views/View.h"
#include "GUI/Views/ViewGroups/ViewGroup.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	/// This ViewGroup arranges its child Views linearly
	class LinearLayout : public ViewGroup
	{
	public:

		LinearLayout();
		virtual ~LinearLayout();

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
		virtual std::string GetThemePrefix() {return "LinearLayout_";}

		/// For setting the View's attributes from xml
		virtual void Inflate(TiXmlElement*,ResourceManager*, std::string themePrefix = "", bool rootNode = false);
		/// For saving the View's attributes to xml
		/// \note This must be hierarchical and the element must include all child elements
		virtual TiXmlElement* Deflate(ResourceManager *resources);

		/// Input event given to View, expected to filter down hierarchy
		/// \return false if event is not absorbed (e.g. did not hit button etc)
		virtual bool HandleEvent(InternalEvent*);

		/// LinearLayouts either arrange their children in a vertical column or a horizontal row
		void SetOrientation( bool vertical );

		/// Adds a child view to the group
		virtual void AddView(View*,ResourceManager*);

		/// For creating a relevant LayoutParams object from xml
		/// LayoutParams are used by children of a ViewGroup to tell the parent ViewGroup how they want to be arranged
		/// You must pass in the child element and this function should find the relevant attributes
		virtual LayoutParams* InflateLayoutParams(TiXmlElement*, LayoutParams *params = NULL, std::string themePrefix = "");

		/// Requested wrap pixel dimensions for layout
		/// \note These are used when SetLayoutParams() is called, which will be after the View has been inflated
		virtual int GetWrapWidth();
		virtual int GetWrapHeight();

	protected:
		std::vector<View*> _views;

		bool _verticalOrientation;
	};

}

//////////////////////////////////////////////////////////////////////////
#endif
