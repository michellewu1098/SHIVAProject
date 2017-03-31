///-----------------------------------------------------------------------------------------------
/// \file ViewGroup.h
/// \brief This is the base ViewGroup class, which is supposed to contain a number of views arranged in a specific way
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUISYSTEM_VIEWGROUP__
#define __SHIVA_GUISYSTEM_VIEWGROUP__

#include <boost/algorithm/string/predicate.hpp>

#include "GUI/Views/View.h"
#include "GUI/Views/ViewGroups/LayoutParams.h"

namespace ShivaGUI
{
	class ViewGroup : public View
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		ViewGroup();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~ViewGroup();
		//----------------------------------------------------------------------------------
		/// \brief Needs to draw its children
		//----------------------------------------------------------------------------------
		virtual void Draw() = 0;
		//----------------------------------------------------------------------------------
		/// \brief Needs to draw its children 
		//----------------------------------------------------------------------------------
		virtual void Draw( unsigned int _context ) = 0;
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
		virtual bool HandleEvent( InternalEvent* _event ) = 0;
		//----------------------------------------------------------------------------------
		/// \brief Adds a child view to the group
		//----------------------------------------------------------------------------------
		virtual void AddView( View* _view, ResourceManager* _resources ) = 0;
		//----------------------------------------------------------------------------------
		/// \brief For creating a relevant LayoutParams object from xml
		/// LayoutParams are used by children of a ViewGroup to tell the parent ViewGroup how they want to be arranged
		/// You must pass in the child element and this function should find the relevant attributes
		//----------------------------------------------------------------------------------
		virtual LayoutParams* InflateLayoutParams( TiXmlElement* _xmlElement, LayoutParams *_params = NULL, std::string _themePrefix = "" );
		//----------------------------------------------------------------------------------
		/// \brief For overriding deflation behaviour
		/// This can be useful if your group is populated procedurally
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetDeflateChildren( bool _value ) { m_deflateChildren = _value; }
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Whether to deflate child nodes
		//----------------------------------------------------------------------------------
		bool m_deflateChildren;
		//----------------------------------------------------------------------------------

	};
}

#endif
