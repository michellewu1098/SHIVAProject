///-----------------------------------------------------------------------------------------------
/// \file DataAdapter.h
/// \brief This is the base 'adapter' for a Model-View-Adapter pattern that is designed to work with a DataModel
/// This uses a pull method: the AdapterView pulls child Views from the DataAdapter, which in turn populates them with data from the DataModel
/// \author Leigh McLoughlin
/// \date Jun 7, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_GUI_VIEWS_ADAPTERVIEWS_DATAADAPTER__
#define __SHIVA_GUI_GUI_VIEWS_ADAPTERVIEWS_DATAADAPTER__

#include <string>

#include "GUI/Views/AdapterViews/DataProvider.h"
#include "GUI/Views/View.h"
#include "GUI/Views/TextView.h"
#include "GUI/Views/ImageTextView.h"
#include "GUI/Views/ImageButton.h"
#include "GUI/Views/TextButton.h"
#include "GUI/Views/ImageTextButton.h"


namespace ShivaGUI
{

	// How does this know which sub-View within the Layout it should fit the data from the DataProvider?
	//  * Have derived classes that each have a specific Layout or specific things it looks for in the Layout?
	//  * A more generic way, where the links are specified in an xml or a map or something
	//  * Android seems to use the second option - simply give it a list of DataProvider entries to map from and a list of View ID's to map to;
	//    it then binds them depending on the type of View that is provided. Sounds good to me

	class DataAdapter
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		DataAdapter();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~DataAdapter();
		//----------------------------------------------------------------------------------
		/// \brief Set the data provider
		/// \param [in] value
		//----------------------------------------------------------------------------------
		virtual void SetDataProvider( DataProvider *_value ) { m_provider = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Retrieve the number of entries of DataProvider
		//----------------------------------------------------------------------------------
		virtual int GetNumEntries();
		//----------------------------------------------------------------------------------
		/// \brief Requests the completed View for display
		/// This could be customised with derived classes for specific AdapterView types
		/// \param [in] index
		/// \param [in] resources
		//----------------------------------------------------------------------------------
		virtual View* GetView( int _index, ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief Specifies the Layout file that defines the View hierarchy that is returned by the DataAdapter
		//----------------------------------------------------------------------------------
		virtual void SetLayoutFile( std::string _xmlfile ) { m_layoutFile = _xmlfile; }
		//----------------------------------------------------------------------------------
		/// \brief This specifies the mapping from the DataProvider to the Views in the Layout file
		/// The idea here is that you set the attribute that would appear in the DataProvider and map this to a View ID in the Layout
		/// When GetView is called, the system gets the data from the DataProvider,
		///  interprets it according to the type of View it's supposed to go into,
		///  and then puts the data in the View before returning it
		/// \note This function makes an internal copy of the arrays you give it
		//----------------------------------------------------------------------------------
		virtual void SetMapping( std::string *_from_attribute, std::string *_to_view, unsigned int _arrayLength );
		//----------------------------------------------------------------------------------
		/// \brief Set if it is model adapter
		/// \param [in] _value True or false
		//----------------------------------------------------------------------------------
		void SetIsModelAdapter( const bool &_value ) { m_isModelAdapter = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Returns if it is model adapter
		/// \retunr m_isModelAdapter
		//----------------------------------------------------------------------------------
		bool GetIsModelAdapter() const { return m_isModelAdapter; }
		//----------------------------------------------------------------------------------


	protected:

		//----------------------------------------------------------------------------------
		/// \brief Data provider
		//----------------------------------------------------------------------------------
		DataProvider *m_provider;
		//----------------------------------------------------------------------------------
		/// \brief Name of file containing layout 
		//----------------------------------------------------------------------------------
		std::string m_layoutFile;
		//----------------------------------------------------------------------------------
		/// \brief Called when GetView needs to bind an image to a View
		/// \param [in] _view
		/// \param [in] _imageResource
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual void SetViewImage( ImageButton *_view, std::string _imageResource, ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief Called when GetView needs to bind an image to a View
		/// \param [in] _view
		/// \param [in] _imageResource
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual void SetViewImage( ImageTextButton *_view, std::string _imageResource, ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief Called when GetView needs to bind an image to a View
		/// \param [in] _view
		/// \param [in] _imageResource
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual void SetViewImage( ImageTextView *_view, std::string _imageResource, ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief Called when GetView needs to bind an image to a View
		/// \param [in] _view
		/// \param [in] _imageResource
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual void SetViewImage( View *_view, std::string _imageResource, ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief Called when GetView needs to bind a piece of text to a View
		/// \param [in] _view
		/// \param [in] _text
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual void SetViewText( TextView *_view, std::string _text, ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief Called when GetView needs to bind a piece of text to a View
		/// \param [in] _view
		/// \param [in] _text
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual void SetViewText( TextButton *_view, std::string _text, ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief Attributes
		//----------------------------------------------------------------------------------
		std::string *m_mappingAttributes;
		//----------------------------------------------------------------------------------
		/// \brief Views
		//----------------------------------------------------------------------------------
		std::string *m_mappingViews;
		//----------------------------------------------------------------------------------
		/// \brief Length
		//----------------------------------------------------------------------------------
		unsigned int m_mappingLength;
		//----------------------------------------------------------------------------------
		/// \brief Flag used to check/set if it is model adapter
		//----------------------------------------------------------------------------------
		bool m_isModelAdapter;
		//----------------------------------------------------------------------------------

	};
}

#endif
