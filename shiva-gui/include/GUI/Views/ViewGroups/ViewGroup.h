
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUISYSTEM_VIEWGROUP__
#define __SHIVA_GUISYSTEM_VIEWGROUP__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#include "GUI/Views/View.h"
#include "GUI/Views/ViewGroups/LayoutParams.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	/// This is the base ViewGroup class, which is supposed to contain a number of views arranged in a specific way
	class ViewGroup : public View
	{
	public:

		ViewGroup();
		virtual ~ViewGroup();

		/// Needs to draw its children
		virtual void Draw() = 0;

		/// For setting the focus of this View
		/// If focussed, it is expected that the view will show this visually
		/// and then allow other forms of input to be received, e.g. 'enter' button pressed etc
		virtual void SetFocus(bool) {}

		/// For selecting the View, e.g. after focussing on it you press the 'enter' key
		virtual void SetSelect(bool) {}

		/// Input event given to View, expected to filter down hierarchy
		/// \return false if event is not absorbed (e.g. did not hit button etc)
		virtual bool HandleEvent(InternalEvent*) = 0;

		/// Adds a child view to the group
		virtual void AddView(View*, ResourceManager*) = 0;

		/// For creating a relevant LayoutParams object from xml
		/// LayoutParams are used by children of a ViewGroup to tell the parent ViewGroup how they want to be arranged
		/// You must pass in the child element and this function should find the relevant attributes
		virtual LayoutParams* InflateLayoutParams(TiXmlElement*, LayoutParams *params = NULL, std::string themePrefix = "" );

		/// For overriding deflation behaviour
		/// This can be useful if your group is populated procedurally
		void SetDeflateChildren( bool value ) {_deflateChildren = value;}

	protected:

		bool _deflateChildren;
	};

}

//////////////////////////////////////////////////////////////////////////
#endif
