#include "GUI/Views/AdapterViews/DataAdapter.h"
#include "GUI/Views/TextButton.h"
#include "ResourceManager.h"

//----------------------------------------------------------------------------------

ShivaGUI::DataAdapter::DataAdapter()
{
	m_provider = NULL;

	m_mappingAttributes = NULL;
	m_mappingViews = NULL;
	m_mappingLength = 0;
	m_isModelAdapter = false;
}

//----------------------------------------------------------------------------------

ShivaGUI::DataAdapter::~DataAdapter()
{
	delete [] m_mappingAttributes;
	delete [] m_mappingViews;
}

//----------------------------------------------------------------------------------

int ShivaGUI::DataAdapter::GetNumEntries()
{
	if( m_provider != NULL )
		return m_provider->GetNumEntries();
	return 0;
}

//----------------------------------------------------------------------------------

ShivaGUI::View* ShivaGUI::DataAdapter::GetView( int _index, ResourceManager *_resources )
{
	View *rootView = _resources->GetLayout( m_layoutFile );

	if( rootView == NULL )
	{
		std::cerr << "WARNING: DataAdapter::GetView failed to load Layout" << std::endl;
		return NULL;
	}

	// Need to populate Views with data according to the mapping
	if( m_provider != NULL && m_mappingLength > 0 )
	{
		for( unsigned int i = 0; i < m_mappingLength; ++i )
		{
			// Retrieve View:
			View *currentView = _resources->GetViewFromID( m_mappingViews[ i ] );
			// Retrieve data:
			std::string currentData = m_provider->GetAttributeString( _index, m_mappingAttributes[ i ] );

			if( currentView != NULL &&  !currentData.empty() )
			{
				// bind data depending on the type of View we have:
				ImageButton *currentImageButton = NULL; /*= dynamic_cast< ImageButton* >( currentView );*/
				ImageTextButton *currentImageTextButton = dynamic_cast< ImageTextButton* >( currentView );
				if( currentImageTextButton == NULL )
					currentImageButton = dynamic_cast< ImageButton* >( currentView );
				TextView *currentTextView = dynamic_cast< TextView* >( currentView );
				TextButton *currentTextButton = dynamic_cast< TextButton* >( currentView );

				if( currentTextButton != NULL )
				{
					SetViewText( currentTextButton, currentData, _resources );
				}
				else if( currentTextView != NULL )
				{
					SetViewText( currentTextView, currentData, _resources );
				}
				else if( currentImageButton != NULL )
				{
					SetViewImage( currentImageButton, currentData, _resources );
				}
				else if( currentImageTextButton != NULL )
				{
					if( m_isModelAdapter )
					{
						std::string text = m_provider->GetAttributeString( _index, "name" );
						if( !text.empty() )
							currentImageTextButton->SetText( text, _resources );
					}
					SetViewImage( currentImageTextButton, currentData, _resources );
				}
				else
				{
					SetViewImage( currentView, currentData, _resources );
				}
			}
		}
	}

	return rootView;
}

//----------------------------------------------------------------------------------

void ShivaGUI::DataAdapter::SetMapping( std::string *_from_attribute, std::string *_to_view, unsigned int _arrayLength )
{
	delete [] m_mappingAttributes;
	delete [] m_mappingViews;
	m_mappingAttributes = NULL;
	m_mappingViews = NULL;
	m_mappingLength = 0;

	if( _arrayLength == 0 )
	{
		// Treat this as a reset
		return;
	}

	m_mappingAttributes = new std::string[ _arrayLength ];
	m_mappingViews = new std::string[ _arrayLength ];
	for( unsigned int i = 0; i < _arrayLength; ++i )
	{
		m_mappingAttributes[ i ] = _from_attribute[ i ];
		m_mappingViews[ i ] = _to_view[ i ];
	}
	m_mappingLength = _arrayLength;
}

//----------------------------------------------------------------------------------

void ShivaGUI::DataAdapter::SetViewImage( ImageButton *_view, std::string _imageResource, ResourceManager *_resources )
{
	_view->SetContent( _resources->GetDrawable( _imageResource ) );
}

//----------------------------------------------------------------------------------

void ShivaGUI::DataAdapter::SetViewImage( ImageTextButton *_view, std::string _imageResource, ResourceManager *_resources )
{
	_view->SetContent( _resources->GetDrawable( _imageResource ) );
}

//----------------------------------------------------------------------------------

void ShivaGUI::DataAdapter::SetViewImage( View *_view, std::string _imageResource, ResourceManager *_resources )
{
	_view->SetBackground( _resources->GetDrawable( _imageResource ) );
}

//----------------------------------------------------------------------------------

void ShivaGUI::DataAdapter::SetViewText( TextView *_view, std::string _text, ResourceManager *_resources )
{
	_view->SetText( _text, _resources );
}

//----------------------------------------------------------------------------------

void ShivaGUI::DataAdapter::SetViewText( TextButton *_view, std::string _text, ResourceManager *_resources )
{
	_view->SetText( _text, _resources );
}

//----------------------------------------------------------------------------------