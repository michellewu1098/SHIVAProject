///-----------------------------------------------------------------------------------------------
/// \file AdapterView.h
/// \brief These are like ViewGroups but the children are specified by a DataAdapter
/// This uses a pull method: the AdapterView pulls child Views from the DataAdapter, which in turn populates them with data from the DataModel
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_GUI_VIEWS_ADAPTERVIEWS_ADAPTERVIEW__
#define __SHIVA_GUI_GUI_VIEWS_ADAPTERVIEWS_ADAPTERVIEW__

#include "GUI/Views/View.h"
#include "GUI/Views/AdapterViews/DataAdapter.h"


namespace ShivaGUI
{
	// Where does the scrolling take place?
	// it's a useful feature, it shouldn't be restricted to this thing
	// maybe have a separate container for it?
	// Android puts it in the base View class - that seems a bit overkill to me...
	// Ok, we now have a ScrollContainer that can be used to hold one of these (or anything) and make it scroll :)
	
	// At which point does the AdapterView pull the child Views from the DataAdapter?

	class AdapterView : public View
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		AdapterView();
		//----------------------------------------------------------------------------------
		/// \brief Dtor: will not delete any adapters attached to it because they may be shared
		//----------------------------------------------------------------------------------
		virtual ~AdapterView();
		//----------------------------------------------------------------------------------
		/// \brief Set the Adapter
		/// \param [in] value
		//----------------------------------------------------------------------------------
		virtual void SetAdapter( DataAdapter *_value ) { m_adapter = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Get the Adapter
		/// \return _adapter
		//----------------------------------------------------------------------------------
		virtual DataAdapter* GetAdapter( void ) { return m_adapter; }
		//----------------------------------------------------------------------------------
		/// \brief For retrieving the original data entry index that was used to generate the View
		/// Returns the index or -1 if the View that was given is not recognised
		/// Note that this function is *not* required to traverse entire trees,
		/// it should simply check against the View that was returned by the data adapter's GetView() function
		//----------------------------------------------------------------------------------
		virtual int GetDataIndex( View * ) { return -1; }
		//----------------------------------------------------------------------------------
		/// \brief This should be called when the source data is known to have changed
		/// The AdapterView should then remove all child Views and rebuild them
		//----------------------------------------------------------------------------------
		virtual void RefreshFromSource() {}
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief The Adapter
		//----------------------------------------------------------------------------------
		DataAdapter *m_adapter;
		//----------------------------------------------------------------------------------

	};
}

#endif
