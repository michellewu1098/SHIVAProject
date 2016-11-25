
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUI_GUI_VIEWS_ADAPTERVIEWS_DATAADAPTER__
#define __SHIVA_GUI_GUI_VIEWS_ADAPTERVIEWS_DATAADAPTER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <string>

//////////////////////////////////////////////////////////////////////////
#include "GUI/Views/AdapterViews/DataProvider.h"
#include "GUI/Views/View.h"
#include "GUI/Views/TextView.h"
#include "GUI/Views/ImageButton.h"
#include "GUI/Views/TextButton.h"
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////


namespace ShivaGUI
{

	/// This is the base 'adapter' for a Model-View-Adapter pattern that is designed to work with a DataModel
	/// This uses a pull method: the AdapterView pulls child Views from the DataAdapter, which in turn populates them with data from the DataModel
	///
	/// How does this know which sub-View within the Layout it should fit the data from the DataProvider?
	///  * Have derived classes that each have a specific Layout or specific things it looks for in the Layout?
	///  * A more generic way, where the links are specified in an xml or a map or something
	///  * Android seems to use the second option - simply give it a list of DataProvider entries to map from and a list of View ID's to map to;
	///    it then binds them depending on the type of View that is provided. Sounds good to me
	class DataAdapter
	{
	public:

		DataAdapter();
		virtual ~DataAdapter();

		virtual void SetDataProvider( DataProvider *value ) {_provider=value;}

		virtual int GetNumEntries();

		/// Requests the completed View for display
		/// This could be customised with derived classes for specific AdapterView types
		virtual View* GetView(int index, ResourceManager *resources);

		/// Specifies the Layout file that defines the View hierarchy that is returned by the DataAdapter
		virtual void SetLayoutFile( std::string xmlfile ) {_layoutFile=xmlfile;}

		/// This specifies the mapping from the DataProvider to the Views in the Layout file
		/// The idea here is that you set the attribute that would appear in the DataProvider and map this to a View ID in the Layout
		/// When GetView is called, the system gets the data from the DataProvider,
		///  interprets it according to the type of View it's supposed to go into,
		///  and then puts the data in the View before returning it
		/// \note This function makes an internal copy of the arrays you give it
		virtual void SetMapping( std::string *from_attribute, std::string *to_view, unsigned int arrayLength );

	protected:

		DataProvider *_provider;

		std::string _layoutFile;

		/// Called when GetView needs to bind an image to a View
		virtual void SetViewImage(ImageButton *, std::string imageResource, ResourceManager *resources);
		virtual void SetViewImage(View *, std::string imageResource, ResourceManager *resources);

		/// Called when GetView needs to bind a piece of text to a View
		virtual void SetViewText(TextView *, std::string text, ResourceManager *resources);
		virtual void SetViewText(TextButton *, std::string text, ResourceManager *resources);

		std::string *_mappingAttributes;
		std::string *_mappingViews;
		unsigned int _mappingLength;
	};

}

//////////////////////////////////////////////////////////////////////////
#endif
