#include "GUI/Views/ViewGroups/CustomLayout.h"
#include "GUI/Views/ViewGroups/CustomLayoutParams.h"
#include "GUI/Views/ViewGroups/CustomLayoutElement.h"
#include "GUI/GUIController.h"

//////////////////////////////////////////////////////////////////////////
#include <iostream>

ShivaGUI::CustomLayout::CustomLayout()
{
	_editDrawable = new RectDrawable();
	_editDrawable->SetFillColour(1.0f,0.0f,0.0f,0.3f);
	_editDrawable->SetBorderColour(1.0f,0.0f,0.0f,1.0f);
	_editMode = false;
	_editSelect = false;

	_editExitButton = NULL;
	_editSaveButton = NULL;
	_handlerExitButton = new CustomLayoutEventHandler(this);
	_handlerSaveButton = new CustomLayoutEventHandler(this);

	_needLayout = false;
	_needSave = false;

	_useGridSnap = true;
	_gridSnapSize = 20;
}

ShivaGUI::CustomLayout::~CustomLayout()
{
	for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
	{
		delete *it;
	}
	for( std::vector<View*>::iterator it2 = _internalViews.begin(); it2 != _internalViews.end(); ++it2 )
	{
		delete *it2;
	}
	delete _editDrawable;
	delete _handlerExitButton;
	delete _handlerSaveButton;
}

void ShivaGUI::CustomLayout::NotifyDrawingContextChange(ResourceManager *resources)
{
	for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
	{
		(*it)->NotifyDrawingContextChange(resources);
	}

	for( std::vector<View*>::iterator it = _internalViews.begin(); it != _internalViews.end(); ++it )
	{
		(*it)->NotifyDrawingContextChange(resources);
	}
}

void ShivaGUI::CustomLayout::Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight)
{
	View::Layout(left,top,right,bottom,windowWidth,windowHeight);

//	std::cout<<"INFO: CustomLayout::Layout called"<<std::endl;

	if( _editDrawable != NULL )
	{
		_editDrawable->SetBounds(left,top,right,bottom,Definitions::CENTRE);
	}


	for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
	{
		LayoutView(*it, left, top, right, bottom, windowWidth, windowHeight);
	}

	for( std::vector<View*>::iterator it = _internalViews.begin(); it != _internalViews.end(); ++it )
	{
		LayoutView(*it, left, top, right, bottom, windowWidth, windowHeight);
	}

}

void ShivaGUI::CustomLayout::Update(float deltaTs, GUIController *guiController)
{
	if( _needLayout )
	{
		guiController->Layout();
		_needLayout = false;
	}
	if( _needSave )
	{
		guiController->SaveLayoutToProfile();
		_needSave = false;
	}

	if( _editMode )
	{
		for( std::vector<View*>::iterator it = _internalViews.begin(); it != _internalViews.end(); ++it )
		{
			(*it)->Update(deltaTs,guiController);
		}
	}
	else
	{
		for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
		{
			(*it)->Update(deltaTs,guiController);
		}
	}
}

void ShivaGUI::CustomLayout::Draw()
{
	View::Draw();

	if( _visible )
	{
		if( _editMode )
		{
			_editDrawable->Draw();
		}


		if( _editMode )
		{
			for( std::vector<View*>::iterator it = _internalViews.begin(); it != _internalViews.end(); ++it )
			{
				(*it)->Draw();
			}
		}
		else
		{
			for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
			{
				(*it)->Draw();
			}
		}
	}
}

void ShivaGUI::CustomLayout::Inflate(TiXmlElement *xmlElement, ResourceManager *resources, std::string themePrefix, bool rootNode )
{
	if( themePrefix.empty() )
		themePrefix = "CustomLayout_";
	View::Inflate(xmlElement,resources,themePrefix,rootNode);


	if( _editExitButton == NULL )
	{
		_editExitButton = new TextButton();
		CustomLayoutParams *exitButtonParams = new CustomLayoutParams();
		exitButtonParams->SetWidthConst(LayoutParams::WRAP_CONTENT);
		exitButtonParams->SetHeightConst(LayoutParams::WRAP_CONTENT);
		exitButtonParams->SetCentreX(0.3f);
		exitButtonParams->SetCentreY(0.9f);
		_editExitButton->SetText("Leave edit mode",resources);
		_editExitButton->Inflate(xmlElement,resources,"",false);
		_editExitButton->SetLayoutParams(exitButtonParams);		// Must be done last, as it gets the size here
		_editExitButton->SetOnClickListener(_handlerExitButton);
		_internalViews.push_back(_editExitButton);
	}
	else
		_editExitButton->Inflate(xmlElement,resources,"",false);

	if( _editSaveButton == NULL )
	{
		_editSaveButton = new TextButton();
		CustomLayoutParams *saveButtonParams = new CustomLayoutParams();
		saveButtonParams->SetWidthConst(LayoutParams::WRAP_CONTENT);
		saveButtonParams->SetHeightConst(LayoutParams::WRAP_CONTENT);
		saveButtonParams->SetCentreX(0.75f);
		saveButtonParams->SetCentreY(0.9f);
		_editSaveButton->SetText("Save",resources);
		_editSaveButton->Inflate(xmlElement,resources,"",false);
		_editSaveButton->SetLayoutParams(saveButtonParams);		// Must be done last, as it gets the size here
		_editSaveButton->SetOnClickListener(_handlerSaveButton);
		_internalViews.push_back(_editSaveButton);
	}
	else
		_editSaveButton->Inflate(xmlElement,resources,"",false);


	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
/*
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
				std::cerr<<"WARNING: CustomLayout::Inflate unknown orientation: "<<orientation<<std::endl;
			}
		}
*/
	}

}

TiXmlElement* ShivaGUI::CustomLayout::Deflate(ResourceManager *resources)
{
	TiXmlElement *xmlNode = ViewGroup::Deflate(resources);
	xmlNode->SetValue("CustomLayout");

	// Currently nothing to add

	// Deflate all children
	for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
	{
		xmlNode->InsertEndChild( *(*it)->Deflate(resources) );
	}

	return xmlNode;
}


bool ShivaGUI::CustomLayout::HandleEvent(InternalEvent *currentEvent)
{
	if( currentEvent != NULL && (currentEvent->GetType() == InternalEvent::EDIT_CUSTOMLAYOUT) )
	{
		// User has requested to edit custom layouts
		_editSelect = true;
		SetEditMode(false);
	}
	else if( currentEvent != NULL && _editSelect && (currentEvent->GetType() == InternalEvent::POSITIONAL_SELECT) )
	{
		_editSelect = false;
		SetEditMode( EventHit(currentEvent) );
		if( _editMode )
			return true;
	}


	bool eventAbsorbed = false;
	if( _editMode )
	{
		for( std::vector<View*>::iterator it = _internalViews.begin(); it != _internalViews.end(); ++it )
		{
			if( (*it)->HandleEvent(currentEvent) )
			{
				// If one of our internal child Views has absorbed an event, as they are all about rearranging the layout,
				// it's very likely that we will need to Layout() again
				_needLayout = true;
				eventAbsorbed = true;
			}
		}
	}
	else
	{
		for( std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it )
		{
			if( (*it)->HandleEvent(currentEvent) )
				return true;//eventAbsorbed = true;
		}
	}
	return eventAbsorbed;
}

void ShivaGUI::CustomLayout::AddView(View *value, ResourceManager *resources)
{
	_views.push_back(value);

	//std::cout<<"INFO: CustomLayout::AddView"<<std::endl;

	CustomLayoutElement *container = new CustomLayoutElement(value,resources,this);
	_internalViews.push_back(container);
	_needLayout = true;
}

ShivaGUI::LayoutParams* ShivaGUI::CustomLayout::InflateLayoutParams(TiXmlElement *xmlElement, LayoutParams *params, std::string themePrefix)
{
	CustomLayoutParams *typedParams;
	if( params == NULL )
	{
		typedParams = new CustomLayoutParams();
		params = typedParams;
	}
	else
	{
		typedParams = dynamic_cast<CustomLayoutParams *>(params);
		if( typedParams == NULL )
		{
			std::cerr<<"WARNING: CustomLayout::InflateLayoutParams given params that are not CustomLayoutParams"<<std::endl;
			return NULL;
		}
	}

	// Let the base class fill in the base params
	ViewGroup::InflateLayoutParams(xmlElement,params,themePrefix);

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{

		if( (std::string("layout_centreX") == currentAttribute->Name()) || (themePrefix+"layout_centreX" == currentAttribute->Name()) )
		{
			double value = 0.0;
			if( currentAttribute->QueryDoubleValue(&value) == TIXML_SUCCESS )
			{
				typedParams->SetCentreX((float) value);
			}
			else
			{
				std::cerr<<"WARNING: CustomLayout::InflateLayoutParams attribute layout_centreX does not have a value of correct type (double)"<<std::endl;
			}
		}
		else if( (std::string("layout_centreY") == currentAttribute->Name()) || (themePrefix+"layout_centreY" == currentAttribute->Name()) )
		{
			double value = 0.0;
			if( currentAttribute->QueryDoubleValue(&value) == TIXML_SUCCESS )
			{
				typedParams->SetCentreY((float) value);
			}
			else
			{
				std::cerr<<"WARNING: CustomLayout::InflateLayoutParams attribute layout_centreY does not have a value of correct type (double)"<<std::endl;
			}
		}

	}

	return params;
}

void ShivaGUI::CustomLayout::GetCoordsAsProportion(int inX, int inY, float &outX, float &outY)
{
	float viewWidth = _layoutBoundsRight - _layoutBoundsLeft;
	float viewHeight = _layoutBoundsBottom - _layoutBoundsTop;

	outX = ((float) inX - _layoutBoundsLeft) / viewWidth;
	outY = ((float) inY - _layoutBoundsTop) / viewHeight;

}

float ShivaGUI::CustomLayout::GetWidthAsProportion(int widthIn)
{
	float viewWidth = _layoutBoundsRight - _layoutBoundsLeft;
	return (float)widthIn / viewWidth;
}

float ShivaGUI::CustomLayout::GetHeightAsProportion(int heightIn)
{
	float viewHeight = _layoutBoundsBottom - _layoutBoundsTop;
	return (float)heightIn / viewHeight;
}


int ShivaGUI::CustomLayout::GridSnapPosition(int value)
{
	if( _useGridSnap )
	{
		if( (float)(value % _gridSnapSize) > (float)(_gridSnapSize/2.0f) )
			value = value - (value % _gridSnapSize) + _gridSnapSize;
		else
			value = value - (value % _gridSnapSize);
	}
	return value;
}

void ShivaGUI::CustomLayout::SetEditMode(bool value)
{
	_editMode = value;
/*
	if( _editExitButton != NULL )
		_editExitButton->SetVisibility(_editMode);

	if( _editSaveButton != NULL )
		_editSaveButton->SetVisibility(_editMode);
*/
}

void ShivaGUI::CustomLayout::LayoutView(View *currentView, int left, int top, int right, int bottom, int windowWidth, int windowHeight)
{
	if( currentView == NULL )
		return;

	int layoutWidth = right - left;
	int layoutHeight = bottom - top;

	LayoutParams *params = currentView->GetLayoutParams();
	CustomLayoutParams *viewLayoutParams = dynamic_cast<CustomLayoutParams*>(params);
	if( viewLayoutParams != NULL )
	{
		int childLeft = left, childRight = right, childTop = top, childBottom = bottom;
		float centreXProportion   = viewLayoutParams->GetCentreX(),
			centreYProportion   = viewLayoutParams->GetCentreY();

		// Determine vertical bounds
		if( viewLayoutParams->GetHeightConst() == LayoutParams::WRAP_CONTENT )
		{
			// Need to figure out the 'natural' size of the View
			int naturalHeight = params->GetWrapHeight();
			if( naturalHeight == 0 )
				naturalHeight = 10; // Just to stop it disappearing

			if( naturalHeight < layoutHeight )
			{
				childTop = top + (centreYProportion * layoutHeight) - ((float)naturalHeight/2.0f);
				childBottom = top + (centreYProportion * layoutHeight) + ((float)naturalHeight/2.0f);
			}
			// If we can't fit this, just give it the maximum size of the layout - not ideal, but it's a bit mad anyway
		}
		else if( viewLayoutParams->GetHeightConst() == LayoutParams::PERCENTAGE_PARENT )
		{
			float percentageSize = viewLayoutParams->GetHeightPercent();
			if( percentageSize > 1.0f )
				percentageSize = 1.0f;
			childTop = top + (centreYProportion * layoutHeight) - ( ( layoutHeight * percentageSize ) / 2.0f);
			childBottom = top + (centreYProportion * layoutHeight) + ( ( layoutHeight * percentageSize ) / 2.0f);
		}

		// Determine horizontal bounds
		if( viewLayoutParams->GetWidthConst() == LayoutParams::WRAP_CONTENT )
		{
			// Need to figure out the 'natural' size of the View
			int naturalWidth = params->GetWrapWidth();
			if( naturalWidth == 0 )
				naturalWidth = 10; // Just to stop it disappearing

			if( naturalWidth < layoutWidth )
			{
				//std::cout<<"INFO: CustomLayout: naturalWidth="<<naturalWidth<<std::endl;
				childLeft = left + (centreXProportion * layoutWidth) - ((float)naturalWidth/2.0f);
				childRight = left + (centreXProportion * layoutWidth) + ((float)naturalWidth/2.0f);
			}
			// If we can't fit this, just give it the maximum size of the layout - not ideal, but it's a bit mad anyway
		}
		else if( viewLayoutParams->GetWidthConst() == LayoutParams::PERCENTAGE_PARENT )
		{
			float percentageSize = viewLayoutParams->GetWidthPercent();
			if( percentageSize > 1.0f )
				percentageSize = 1.0f;
			childLeft = left + (centreXProportion * layoutWidth) - ( ( layoutWidth * percentageSize ) / 2.0f);
			childRight = left + (centreXProportion * layoutWidth) + ( ( layoutWidth * percentageSize ) / 2.0f);
		}

		currentView->Layout( childLeft, childTop, childRight, childBottom, windowWidth, windowHeight );
	}
	else
		std::cout<<"WARNING: CustomLayout has child with no layout params"<<std::endl;
}

ShivaGUI::CustomLayout::CustomLayoutEventHandler::CustomLayoutEventHandler(CustomLayout *parent) {_parent = parent;}

void ShivaGUI::CustomLayout::CustomEventReceived(CustomLayoutEventHandler *handler, View *inputView)
{
	if( handler == _handlerExitButton )
	{
		SetEditMode(false);
	}
	else if( handler == _handlerSaveButton )
	{
		_needSave = true;
	}
}


