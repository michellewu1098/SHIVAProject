
#include "GUI/Views/ViewGroups/CustomLayout.h"
#include "GUI/Views/ViewGroups/CustomLayoutElement.h"
#include "GUI/Views/ViewGroups/CustomLayoutParams.h"

//////////////////////////////////////////////////////////////////////////
#include <iostream>

ShivaGUI::CustomLayoutElement::CustomLayoutElement(View *child, ResourceManager *resources, CustomLayout *parent)
{
	_child = child;
	_parent = parent;

	_editDrawable = new RectDrawable();
	_editDrawable->SetFillColour(0.9f,0.5f,0.4f,0.3f);
	_editDrawable->SetBorderColour(0.9f,0.5f,0.4f,1.0f);

	_resizeDrawable = new RectDrawable();
	_resizeDrawable->SetFillColour(0.9f,0.5f,0.4f,0.3f);
	_resizeDrawable->SetBorderColour(0.9f,0.5f,0.4f,1.0f);

	_eventHandler = new CustomElementEventHandler(this);


	_editVisibilityButton = new ImageButton();
	CustomLayoutParams *buttonParams = new CustomLayoutParams();
	buttonParams->SetWidthConst(LayoutParams::PERCENTAGE_PARENT);
	buttonParams->SetHeightConst(LayoutParams::PERCENTAGE_PARENT);
	buttonParams->SetWidthPercent(0.1f);
	buttonParams->SetHeightPercent(0.1f);
	buttonParams->SetCentreX(0.5f);
	buttonParams->SetCentreY(0.9f);
	//_editVisibilityButton->SetText("Visible",resources);
		// Set background images
		RectDrawable *bgd = new RectDrawable();
		bgd->SetFillColour(0.4f,0.0f,0.6f,0.1f);
		bgd->SetBorderColour(0.4f,0.0f,0.6f,0.8f);
	_editVisibilityButton->SetVisibility(false);
	_editVisibilityButton->SetBackground(bgd);
	_editVisibilityButton->SetLayoutParams(buttonParams);		// Must be done last, as it gets the size here
	_editVisibilityButton->SetOnClickListener(_eventHandler);
	_internalViews.push_back(_editVisibilityButton);

	_editWrapSizeButton = new ImageButton();
	buttonParams = new CustomLayoutParams();
	buttonParams->SetWidthConst(LayoutParams::PERCENTAGE_PARENT);
	buttonParams->SetHeightConst(LayoutParams::PERCENTAGE_PARENT);
	buttonParams->SetWidthPercent(0.1f);
	buttonParams->SetHeightPercent(0.1f);
	buttonParams->SetCentreX(0.1f);
	buttonParams->SetCentreY(0.9f);
	//_editWrapSizeButton->SetText("Size Wrap",resources);
		// Set background images
		bgd = new RectDrawable();
		bgd->SetFillColour(0.4f,0.0f,0.6f,0.1f);
		bgd->SetBorderColour(0.4f,0.0f,0.6f,0.8f);
	_editWrapSizeButton->SetVisibility(false);
	_editWrapSizeButton->SetBackground(bgd);
	_editWrapSizeButton->SetLayoutParams(buttonParams);		// Must be done last, as it gets the size here
	_editWrapSizeButton->SetOnClickListener(_eventHandler);
	_internalViews.push_back(_editWrapSizeButton);

	_selected = _dragSelected = _resizeSelected = false;
	_selectedX = _selectedY = 0.0f;
	_selectedXProp = _selectedYProp = 0.0f;
	_selectedWidth = _selectedHeight = 0.0f;

}

ShivaGUI::CustomLayoutElement::~CustomLayoutElement()
{
	// Must not delete child
	for( std::vector<View*>::iterator it = _internalViews.begin(); it != _internalViews.end(); ++it )
	{
		delete *it;
	}
	delete _editDrawable;
	//delete _editVisibilityButton; // This is in the _internalViews vector
	delete _eventHandler;
}

void ShivaGUI::CustomLayoutElement::NotifyDrawingContextChange(ResourceManager *resources)
{
	_child->NotifyDrawingContextChange(resources);

	for( std::vector<View*>::iterator it = _internalViews.begin(); it != _internalViews.end(); ++it )
	{
		(*it)->NotifyDrawingContextChange(resources);
	}
}

void ShivaGUI::CustomLayoutElement::Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight)
{
	View::Layout(left,top,right,bottom,windowWidth,windowHeight);

	//std::cout<<"INFO: CustomLayoutElement::Layout: left="<<left<<" top="<<top<<" right="<<right<<" bottom="<<bottom<<std::endl;

	if( _editDrawable != NULL )
	{
		_editDrawable->SetBounds( ( float )left, ( float )top, ( float )right, ( float )bottom, Definitions::CENTRE );
	}

	if( _resizeDrawable != NULL )
	{
		_resizeDrawable->SetBounds( ( float )right - 20.f, ( float )bottom - 20.f, ( float )right, ( float )bottom, Definitions::CENTRE );
	}

	if( _child != NULL )
	{
		_child->Layout(left,top,right,bottom,windowWidth,windowHeight);
		//LayoutView(_child, left, top, right, bottom, windowWidth, windowHeight);
	}

	for( std::vector<View*>::iterator it = _internalViews.begin(); it != _internalViews.end(); ++it )
	{
		LayoutView(*it, left, top, right, bottom, windowWidth, windowHeight);
	}

}

void ShivaGUI::CustomLayoutElement::Update(float deltaTs, GUIController *guiController)
{
	if( _child != NULL )
	{
		_child->Update(deltaTs,guiController);
	}

	for( std::vector<View*>::iterator it = _internalViews.begin(); it != _internalViews.end(); ++it )
	{
		(*it)->Update(deltaTs,guiController);
	}
}

void ShivaGUI::CustomLayoutElement::Draw()
{
	View::Draw();

	if( m_visible )
	{
		_child->Draw();

		_editDrawable->Draw();
		_resizeDrawable->Draw();

		for( std::vector<View*>::iterator it = _internalViews.begin(); it != _internalViews.end(); ++it )
		{
			(*it)->Draw();
		}
	}
}

void ShivaGUI::CustomLayoutElement::Inflate(TiXmlElement *xmlElement, ResourceManager *resources, std::string themePrefix, bool rootNode )
{
	if( themePrefix.empty() )
		themePrefix = "CustomLayoutElement_";
	View::Inflate(xmlElement,resources,themePrefix,rootNode);

}

bool ShivaGUI::CustomLayoutElement::HandleEvent(InternalEvent *currentEvent)
{
	bool eventAbsorbed = false;
	if( currentEvent->GetType() == InternalEvent::POSITIONAL_SELECT )
	{
		if( SetSelected( EventHit(currentEvent) ) )
		{
			if( EventHitResize(currentEvent) )
			{
				// The select was for the resize widget
				_resizeSelected = true;
				eventAbsorbed = true;
			}
			else
			{
				// This was a selection within the body of the View
				// It might turn into a drag, so set dragSelected
				_dragSelected = true;
			}

			// Whatever happens, we need to store the current mouse position, relative to the centre of the View
			// Retrieve mouse hit coords
			int mouseX, mouseY;
			currentEvent->GetPosition(mouseX,mouseY);

			CustomLayoutParams *params = dynamic_cast<CustomLayoutParams*>( GetLayoutParams() );
			if( params != NULL )
			{
				// The coords space is a proportional one, relative to the parent
				// So we must ask the parent to work out the positions
				float mousePropX, mousePropY;
				_parent->GetCoordsAsProportion(mouseX,mouseY, mousePropX,mousePropY);
				// These coords are offsets from the centre of the child View
				_selectedXProp = mousePropX;
				_selectedYProp = mousePropY;
				_selectedX = mousePropX - params->GetCentreX();
				_selectedY = mousePropY - params->GetCentreY();
				GetCurrentSizeProp(_selectedWidth,_selectedHeight);

				//std::cout<<"INFO: CustomLayoutElement storing _selectedX = "<< _selectedX <<" _selectedY = "<< _selectedY <<std::endl;
			}

		}
	}
	else if( currentEvent->GetType() == InternalEvent::POSITIONAL_DESELECT )
	{
		if( EventHit(currentEvent) )
		{
			SetSelected(_selected,false);
			_resizeSelected = false;
		}
	}
	else if( currentEvent->GetType() == InternalEvent::POSITIONAL_DRAG )
	{
		if( _dragSelected )
		{
			// We are moving the whole View

			SetSelected(true,true);
			// Retrieve mouse hit coords
			int mouseX, mouseY;
			currentEvent->GetPosition(mouseX,mouseY);

			CustomLayoutParams *params = dynamic_cast<CustomLayoutParams*>( GetLayoutParams() );
			if( params != NULL )
			{
				// The coords space is a proportional one, relative to the parent
				// So we must ask the parent to work out the positions
				float mousePropX, mousePropY;
				_parent->GetCoordsAsProportion( mouseX,mouseY, mousePropX,mousePropY );
				params->SetCentreX( mousePropX - _selectedX );
				params->SetCentreY( mousePropY - _selectedY );
			}
			eventAbsorbed = true;
		}
		else if( _resizeSelected )
		{
			// We are resizing the View

			// Retrieve mouse hit coords
			int mouseX, mouseY;
			currentEvent->GetPosition(mouseX,mouseY);

			CustomLayoutParams *params = dynamic_cast<CustomLayoutParams*>( GetLayoutParams() );
			if( params != NULL )
			{

				// Get the current size of the View
//					float currentWidth, currentHeight;
//					GetCurrentSizeProp(currentWidth,currentHeight);

				params->SetWidthConst(LayoutParams::PERCENTAGE_PARENT);
				params->SetHeightConst(LayoutParams::PERCENTAGE_PARENT);

				// The coords space is a proportional one, relative to the parent
				// So we must ask the parent to work out the positions
				float mousePropX, mousePropY;
				_parent->GetCoordsAsProportion(mouseX,mouseY, mousePropX,mousePropY);

				float newWidth = _selectedWidth + (mousePropX - _selectedXProp)*2.0f;
				float newHeight = _selectedHeight + (mousePropY - _selectedYProp)*2.0f;

				params->SetWidthPercent(newWidth);
				params->SetHeightPercent(newHeight);


				//std::cout<<"INFO: CustomLayoutElement resizing, _selectedWidth = "<<_selectedWidth<<" mousePropX = "<<mousePropX<<" _selectedXProp = "<<_selectedXProp<<std::endl;


			}

			eventAbsorbed = true;
		}
	}

	if( eventAbsorbed )
		return true;
	// If the event was absorbed, the child should not be given it

	for( std::vector<View*>::iterator it = _internalViews.begin(); it != _internalViews.end(); ++it )
	{
		if( (*it)->HandleEvent(currentEvent) )
			return true;
	}
	return eventAbsorbed;
}

bool ShivaGUI::CustomLayoutElement::SetSelected(bool value, bool drag)
{
	_selected = value;
	_dragSelected = false;

	if( drag )
	{
		_dragSelected = true;
		_editDrawable->SetFillColour(0.0f,1.0f,0.0f,0.3f);
		_editDrawable->SetBorderColour(0.0f,1.0f,1.0f,1.0f);
		_resizeDrawable->SetFillColour(0.0f,1.0f,0.0f,0.3f);
		_resizeDrawable->SetBorderColour(0.0f,1.0f,1.0f,1.0f);
		_editVisibilityButton->SetVisibility(false);
		_editWrapSizeButton->SetVisibility(false);
	}
	else if( _selected )
	{
		_editDrawable->SetFillColour(0.0f,1.0f,0.0f,0.3f);
		_editDrawable->SetBorderColour(0.0f,1.0f,0.0f,1.0f);
		_resizeDrawable->SetFillColour(0.0f,1.0f,0.0f,0.3f);
		_resizeDrawable->SetBorderColour(0.0f,1.0f,0.0f,1.0f);
		_editVisibilityButton->SetVisibility(true);
		_editWrapSizeButton->SetVisibility(true);
	}
	else
	{
		_editDrawable->SetFillColour(0.9f,0.5f,0.4f,0.3f);
		_editDrawable->SetBorderColour(0.9f,0.5f,0.4f,1.0f);
		_resizeDrawable->SetFillColour(0.9f,0.5f,0.4f,0.3f);
		_resizeDrawable->SetBorderColour(0.9f,0.5f,0.4f,1.0f);
		_editVisibilityButton->SetVisibility(false);
		_editWrapSizeButton->SetVisibility(false);
	}
	return _selected;
}

void ShivaGUI::CustomLayoutElement::LayoutView(View *currentView, int left, int top, int right, int bottom, int windowWidth, int windowHeight)
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
			if( naturalHeight < layoutHeight )
			{
				childTop = top + ( int )( ( centreYProportion * layoutHeight ) - ( ( float )naturalHeight / 2.0f ) );
				childBottom = top + ( int )( ( centreYProportion * layoutHeight ) + ( ( float )naturalHeight / 2.0f ) );
			}
			// If we can't fit this, just give it the maximum size of the layout - not ideal, but it's a bit mad anyway
		}
		else if( viewLayoutParams->GetHeightConst() == LayoutParams::PERCENTAGE_PARENT )
		{
			float percentageSize = viewLayoutParams->GetHeightPercent();
			if( percentageSize > 1.0f )
				percentageSize = 1.0f;
			childTop = top + ( int )((centreYProportion * layoutHeight) - ( ( layoutHeight * percentageSize ) / 2.0f));
			childBottom = top + ( int )((centreYProportion * layoutHeight) + ( ( layoutHeight * percentageSize ) / 2.0f));
		}

		// Determine horizontal bounds
		if( viewLayoutParams->GetWidthConst() == LayoutParams::WRAP_CONTENT )
		{
			// Need to figure out the 'natural' size of the View
			int naturalWidth = params->GetWrapWidth();
			if( naturalWidth < layoutWidth )
			{
				//std::cout<<"INFO: CustomLayout: naturalWidth="<<naturalWidth<<std::endl;
				childLeft = left + ( int )( ( centreXProportion * layoutWidth ) - ( ( float )naturalWidth / 2.0f ) );
				childRight = left + ( int )( ( centreXProportion * layoutWidth ) + ( ( float )naturalWidth / 2.0f ) );
			}
			// If we can't fit this, just give it the maximum size of the layout - not ideal, but it's a bit mad anyway
		}
		else if( viewLayoutParams->GetWidthConst() == LayoutParams::PERCENTAGE_PARENT )
		{
			float percentageSize = viewLayoutParams->GetWidthPercent();
			if( percentageSize > 1.0f )
				percentageSize = 1.0f;
			childLeft = left + ( int )( (centreXProportion * layoutWidth) - ( ( layoutWidth * percentageSize ) / 2.0f) );
			childRight = left + ( int )( (centreXProportion * layoutWidth) + ( ( layoutWidth * percentageSize ) / 2.0f) );
		}

		currentView->Layout( childLeft, childTop, childRight, childBottom, windowWidth, windowHeight );
	}
}

bool ShivaGUI::CustomLayoutElement::EventHitResize(InternalEvent *event)
{
	float left,right,bottom,top;
	_resizeDrawable->GetBounds(left,top,right,bottom);
	int x, y;
	event->GetPosition(x,y);
	return (x > left) && (x < right)
			&& (y > top) && (y < bottom);
}

void ShivaGUI::CustomLayoutElement::GetCurrentSizeProp(float &currentWidth, float &currentHeight)
{
	CustomLayoutParams *params = dynamic_cast<CustomLayoutParams*>( GetLayoutParams() );
	if( params == NULL )
	{
		std::cerr<<"ERROR: CustomLayoutElement::GetCurrentSizeProp() cannot retrieve layout params"<<std::endl;
		currentWidth = currentHeight = 0.0f;
		return;
	}

	if( params->GetWidthConst() == LayoutParams::FILL_PARENT )
	{
		// View will be 100% of parent's width
		currentWidth = 1.0f;
	}
	else if( params->GetWidthConst() == LayoutParams::WRAP_CONTENT )
	{
		int absoluteWidth = _child->GetWrapWidth();

		currentWidth = _parent->GetWidthAsProportion(absoluteWidth);
	}
	else if( params->GetWidthConst() == LayoutParams::PERCENTAGE_PARENT )
	{
		currentWidth = params->GetWidthPercent();
	}

	if( params->GetHeightConst() == LayoutParams::FILL_PARENT )
	{
		// View will be 100% of parent's height
		currentHeight = 1.0f;
	}
	else if( params->GetHeightConst() == LayoutParams::WRAP_CONTENT )
	{
		int absoluteHeight = _child->GetWrapHeight();

		currentHeight = _parent->GetHeightAsProportion(absoluteHeight);
	}
	else if( params->GetHeightConst() == LayoutParams::PERCENTAGE_PARENT )
	{
		currentHeight = params->GetHeightPercent();
	}

}

ShivaGUI::CustomLayoutElement::CustomElementEventHandler::CustomElementEventHandler(CustomLayoutElement *parent) {_parent = parent;}

void ShivaGUI::CustomLayoutElement::CustomEventReceived(CustomElementEventHandler *handler, View *inputView)
{
//	std::cout<<"CustomLayoutElement::CustomEventReceived"<<std::endl;
	if( inputView == _editVisibilityButton )
	{
		_child->SetVisibility( !_child->GetVisibility() );
	}
	else if( inputView == _editWrapSizeButton )
	{
		GetLayoutParams()->SetHeightConst(LayoutParams::WRAP_CONTENT);
		GetLayoutParams()->SetWidthConst(LayoutParams::WRAP_CONTENT);
	}
}


