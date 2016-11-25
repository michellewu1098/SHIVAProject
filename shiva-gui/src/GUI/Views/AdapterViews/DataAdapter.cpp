
#include "GUI/Views/AdapterViews/DataAdapter.h"
//////////////////////////////////////////////////////////////////////////
#include "GUI/Views/TextButton.h"

#include "ResourceManager.h"

ShivaGUI::DataAdapter::DataAdapter()
{
	_provider = NULL;

	_mappingAttributes = NULL;
	_mappingViews = NULL;
	_mappingLength = 0;
}

ShivaGUI::DataAdapter::~DataAdapter()
{
	delete [] _mappingAttributes;
	delete [] _mappingViews;
}

int ShivaGUI::DataAdapter::GetNumEntries()
{
	if( _provider != NULL )
		return _provider->GetNumEntries();
	return 0;
}

ShivaGUI::View* ShivaGUI::DataAdapter::GetView(int index, ResourceManager *resources)
{
	View *rootView = resources->GetLayout(_layoutFile);

	if( rootView == NULL )
	{
		std::cerr<<"WARNING: DataAdapter::GetView failed to load Layout"<<std::endl;
		return NULL;
	}

	// Need to populate Views with data according to the mapping
	if( _provider != NULL && _mappingLength > 0)
	{
		for( unsigned int i = 0; i < _mappingLength; ++i )
		{
			// Retrieve View:
			View *currentView = resources->GetViewFromID( _mappingViews[i] );
			// Retrieve data:
			std::string currentData = _provider->GetAttributeString(index,_mappingAttributes[i]);

			if( currentView != NULL &&  !currentData.empty() )
			{
				// bind data depending on the type of View we have:
				ImageButton *currentImageButton = dynamic_cast<ImageButton*>(currentView);
				TextView *currentTextView = dynamic_cast<TextView*>(currentView);
				TextButton *currentTextButton = dynamic_cast<TextButton*>(currentView);

				if( currentTextButton != NULL )
				{
					SetViewText(currentTextButton,currentData,resources);
				}
				else if( currentTextView != NULL )
				{
					SetViewText(currentTextView,currentData,resources);
				}
				else if( currentImageButton != NULL )
				{
					SetViewImage(currentImageButton,currentData,resources);
				}
				else
				{
					SetViewImage(currentView,currentData,resources);
				}
			}
		}
	}

	return rootView;
}

void ShivaGUI::DataAdapter::SetMapping( std::string *from_attribute, std::string *to_view, unsigned int arrayLength )
{
	delete [] _mappingAttributes;
	delete [] _mappingViews;
	_mappingAttributes = NULL;
	_mappingViews = NULL;
	_mappingLength = 0;

	if( arrayLength == 0 )
	{
		// Treat this as a reset
		return;
	}

	_mappingAttributes = new std::string[arrayLength];
	_mappingViews = new std::string[arrayLength];
	for( unsigned int i = 0; i < arrayLength; ++i )
	{
		_mappingAttributes[i] = from_attribute[i];
		_mappingViews[i] = to_view[i];
	}
	_mappingLength = arrayLength;
}

void ShivaGUI::DataAdapter::SetViewImage(ImageButton *view, std::string imageResource, ResourceManager *resources)
{
	view->SetContent( resources->GetDrawable(imageResource) );
}

void ShivaGUI::DataAdapter::SetViewImage(View *view, std::string imageResource, ResourceManager *resources)
{
	view->SetBackground( resources->GetDrawable(imageResource) );
}

void ShivaGUI::DataAdapter::SetViewText(TextView *view, std::string text, ResourceManager *resources)
{
	view->SetText(text,resources);
}

void ShivaGUI::DataAdapter::SetViewText(TextButton *view, std::string text, ResourceManager *resources)
{
	view->SetText(text,resources);
}
