#include "GUI/Views/ViewGroups/LinearLayout.h"
#include "GUI/Views/ViewGroups/LinearLayoutParams.h"

//////////////////////////////////////////////////////////////////////////
#include <iostream>

ShivaGUI::LinearLayout::LinearLayout()
{
	_verticalOrientation = true;
}

ShivaGUI::LinearLayout::~LinearLayout()
{
	for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
	{
		delete *it;
	}
}

void ShivaGUI::LinearLayout::NotifyDrawingContextChange(ResourceManager *resources)
{
	for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
	{
		(*it)->NotifyDrawingContextChange(resources);
	}
}

void ShivaGUI::LinearLayout::Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight)
{
	View::Layout(left,top,right,bottom,windowWidth,windowHeight);

	if( _views.empty() )
		return;

	float totalWeight = 0.0f;
	for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
	{
		LayoutParams *params = ((*it)->GetLayoutParams());
		LinearLayoutParams *viewLayoutParams = dynamic_cast<LinearLayoutParams*>(params);
		if( params != NULL )
		{
			totalWeight += viewLayoutParams->GetWeight();
		}
	}

	// This is not going to be terribly efficient...
	if( _verticalOrientation )
	{
		float availableHeight = ( float )( bottom - top );

		if( totalWeight > 0.0f )
		{
			float unitHeight = availableHeight / totalWeight;
			float viewTop = ( float )top;
			float viewBottom = 0.0f;
			for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
			{
				LayoutParams *params = ((*it)->GetLayoutParams());
				LinearLayoutParams *viewLayoutParams = dynamic_cast<LinearLayoutParams*>(params);
				if( viewLayoutParams != NULL )
				{
					int paddingLeft   = params->GetPaddingLeft(),
						paddingRight  = params->GetPaddingRight(),
						paddingTop    = params->GetPaddingTop(),
						paddingBottom = params->GetPaddingBottom();

					viewBottom = viewTop + (unitHeight * viewLayoutParams->GetWeight());
					(*it)->Layout( left + paddingLeft, ( int )( viewTop + paddingTop ), right - paddingRight, ( int )( viewBottom - paddingBottom ), windowWidth, windowHeight );
					viewTop = viewBottom;
				}
			}
		}
		else
		{
			// The weights were undefined, so we make them be equal
			float unitHeight = availableHeight / ((float) _views.size());
			float viewTop = ( float )top;
			float viewBottom = 0.0f;
			for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
			{
				viewBottom = viewTop + unitHeight;
				(*it)->Layout( left, ( int )viewTop, right, ( int )viewBottom, windowWidth, windowHeight );
				viewTop = viewBottom;
			}
		}
	}
	else
	{
		// Horizontal orientation
		float availableWidth = ( float )( right - left );

		if( totalWeight > 0.0f )
		{
			float unitWidth = availableWidth / totalWeight;
			float viewLeft =  ( float )left;
			float viewRight = 0.0f;
			for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
			{
				LayoutParams *params = ((*it)->GetLayoutParams());
				LinearLayoutParams *viewLayoutParams = dynamic_cast<LinearLayoutParams*>(params);
				if( viewLayoutParams != NULL )
				{
					int paddingLeft   = params->GetPaddingLeft(),
						paddingRight  = params->GetPaddingRight(),
						paddingTop    = params->GetPaddingTop(),
						paddingBottom = params->GetPaddingBottom();

					viewRight = viewLeft + (unitWidth * viewLayoutParams->GetWeight());
					(*it)->Layout( ( int )( viewLeft + paddingLeft ), top + paddingTop, ( int )( viewRight - paddingRight ), bottom - paddingBottom, windowWidth, windowHeight );
					viewLeft = viewRight;
				}
			}
		}
		else
		{
			// The weights were undefined, so we make them be equal
			float unitWidth = availableWidth / ((float) _views.size());
			float viewLeft = ( float )left;
			float viewRight = 0.0f;
			for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
			{
				viewRight = viewLeft + unitWidth;
				(*it)->Layout( ( int )viewLeft, top, ( int )viewRight, bottom, windowWidth, windowHeight );
				viewLeft = viewRight;
			}
		}
	}
/*
	for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
	{
		// Only set it up if the LayoutParams are of the right type
		LayoutParams *params = ((*it)->GetLayoutParams());
		AbsoluteLayoutParams *viewLayoutParams = dynamic_cast<AbsoluteLayoutParams*>(params);
		if( viewLayoutParams != NULL )
		{
			float left =   viewLayoutParams->GetCentreXAbs() - (viewLayoutParams->GetWidthAbs()/2.0f);
			float right =  viewLayoutParams->GetCentreXAbs() + (viewLayoutParams->GetWidthAbs()/2.0f);
			float top =    viewLayoutParams->GetCentreYAbs() + (viewLayoutParams->GetHeightAbs()/2.0f);
			float bottom = viewLayoutParams->GetCentreYAbs() - (viewLayoutParams->GetHeightAbs()/2.0f);
			(*it)->Layout( left, top, right, bottom );
		}
	}
	*/
}

void ShivaGUI::LinearLayout::Update(float deltaTs, GUIController *guiController)
{
	for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
	{
		(*it)->Update(deltaTs,guiController);
	}
}

void ShivaGUI::LinearLayout::Draw()
{
	View::Draw();

	if( m_visible )
	{

		for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
		{
			(*it)->Draw();
		}
	}
}

void ShivaGUI::LinearLayout::Inflate(TiXmlElement *xmlElement, ResourceManager *resources, std::string themePrefix, bool rootNode )
{
	if( themePrefix.empty() )
		themePrefix = "LinearLayout_";
	View::Inflate(xmlElement,resources,themePrefix,rootNode);

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( currentAttribute->Name() == std::string("orientation") )
		{
			std::string orientation( currentAttribute->Value() );
			if( orientation == "horizontal" )
			{
				_verticalOrientation = false;
			}
			else if( orientation == "vertical" )
			{
				_verticalOrientation = true;
			}
			else
			{
				std::cerr<<"WARNING: LinearLayout::Inflate unknown orientation: "<<orientation<<std::endl;
			}
		}
	}

}


TiXmlElement* ShivaGUI::LinearLayout::Deflate(ResourceManager *resources)
{
	TiXmlElement *xmlNode = ViewGroup::Deflate(resources);
	xmlNode->SetValue("LinearLayout");


	if( _verticalOrientation )
		xmlNode->SetAttribute("orientation","vertical" );
	else
		xmlNode->SetAttribute("orientation","horizontal" );


	// Deflate all children
	if( _deflateChildren )
	{
		for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
		{
			xmlNode->InsertEndChild( *(*it)->Deflate(resources) );
		}
	}

	return xmlNode;
}


bool ShivaGUI::LinearLayout::HandleEvent(InternalEvent *currentEvent)
{
	for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
	{
		if( (*it)->HandleEvent(currentEvent) )
			return true;
	}
	return false;
}

void ShivaGUI::LinearLayout::SetOrientation( bool vertical )
{
	_verticalOrientation = vertical;
	// TODO: re-measure?
}

void ShivaGUI::LinearLayout::AddView(View *value,ResourceManager*)
{
	_views.push_back(value);
}

ShivaGUI::LayoutParams* ShivaGUI::LinearLayout::InflateLayoutParams(TiXmlElement *xmlElement, LayoutParams *params,std::string themePrefix)
{
	LinearLayoutParams *typedParams;
	if( params == NULL )
	{
		typedParams = new LinearLayoutParams();
		params = typedParams;
	}
	else
	{
		typedParams = dynamic_cast<LinearLayoutParams *>(params);
		if( typedParams == NULL )
		{
			std::cerr<<"WARNING: LinearLayout::InflateLayoutParams given params that are not LinearLayoutParams"<<std::endl;
			return NULL;
		}
	}

	// Let the base class fill in the base params
	ViewGroup::InflateLayoutParams(xmlElement,params,themePrefix);

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( (std::string("layout_weight") == currentAttribute->Name()) || (themePrefix+"layout_weight" == currentAttribute->Name()) )
		{
			double value = 0.0;
			if( currentAttribute->QueryDoubleValue(&value) == TIXML_SUCCESS )
			{
				typedParams->SetWeight((float) value);
			}
			else
			{
				std::cerr<<"WARNING: LinearLayout::InflateLayoutParams attribute layout_weight does not have a value of correct type (double)"<<std::endl;
			}
		}
	}

	return params;
}

int ShivaGUI::LinearLayout::GetWrapWidth()
{
	if( _verticalOrientation )
	{
		// return wrap of biggest child
		int biggest = 0;
		for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
		{
			int currentChildValue = (*it)->GetWrapWidth();
			if( currentChildValue > biggest )
				biggest = currentChildValue;
		}
		return biggest;
	}
	else
	{
		// return total of all children
		int total = 0;
		for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
		{
			total += (*it)->GetWrapWidth();
		}
		return total;
	}
}

int ShivaGUI::LinearLayout::GetWrapHeight()
{
	if( !_verticalOrientation )
	{
		// return wrap of biggest child
		int biggest = 0;
		for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
		{
			int currentChildValue = (*it)->GetWrapHeight();
			if( currentChildValue > biggest )
				biggest = currentChildValue;
		}
		return biggest;
	}
	else
	{
		// return total of all children
		int total = 0;
		for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
		{
			total += (*it)->GetWrapHeight();
		}
		return total;
	}
}


