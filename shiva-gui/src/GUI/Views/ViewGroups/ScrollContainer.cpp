
#include "GUI/Views/ViewGroups/ScrollContainer.h"
#include "GUI/GUIController.h"
#include "GUI/Drawables/BitmapDrawable.h"

#include <boost/algorithm/string.hpp>
#include <iostream>
#include <GL/GLee.h>

ShivaGUI::ScrollContainer::ScrollContainer()
{
	_scrollHorizontal = false;
	_scrollVertical = false;
	_child = NULL;
	_scrollX = _scrollY = 0;
	_selected = _dragSelected = false;
	_selectedMouseX = _selectedMouseY = 0;
	_needLayout = false;
	_scrollCurrentX = _scrollCurrentY = 0;
	_scrollTrackVertDrawable = _scrollTrackHorizDrawable = _scrollThumbVertDrawable = _scrollThumbHorizDrawable = NULL;
	_scrollTrackVertFromTheme =  _scrollTrackHorizFromTheme = _scrollThumbVertFromTheme = _scrollThumbHorizFromTheme = false;

	_showHorizBar = false;
	_showVertBar = false;

	_scrollJumpAmount = 30;
	_verticalScrollDrag = _horizontalScrollDrag = false;
	_scrollDragOriginalTop = _scrollDragOriginalLeft = 0;
}

ShivaGUI::ScrollContainer::~ScrollContainer()
{
	delete _child;
	delete _scrollTrackVertDrawable;
	delete _scrollTrackHorizDrawable;
	delete _scrollThumbVertDrawable;
	delete _scrollThumbHorizDrawable;
}

void ShivaGUI::ScrollContainer::NotifyDrawingContextChange(ResourceManager *resources)
{
	if( _child != NULL )
		_child->NotifyDrawingContextChange(resources);
}

void ShivaGUI::ScrollContainer::Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight)
{
	View::Layout(left,top,right,bottom,windowWidth,windowHeight);

	if( _child != NULL )
	{
		// Need to layout child:

		int childLeft = left, childRight = right, childTop = top, childBottom = bottom;

		// Whether we need to show the scrollbars
		_showHorizBar = false;
		_showVertBar = false;

		int containerWidth = right - left;
		int childWidth = _child->GetWrapWidth();

		int containerHeight = bottom - top;
		int childHeight = _child->GetWrapHeight();


		// how are we actually going to do this scrolling stuff?
		// from the way things work at the moment, the only way is to tell the child to be its maximum size and then clip the drawing
		// ok, let's give this a go
		if( _scrollHorizontal && _scrollTrackHorizDrawable != NULL && _scrollThumbHorizDrawable != NULL )
		{
			if( childWidth > containerWidth )
			{
				// We need to do some scrolling
				_showHorizBar = true;
				childLeft = left - _scrollX - _scrollCurrentX;
				if( childLeft > left )
					childLeft = left;
				childRight = childLeft + childWidth;
				if( childRight < right )
				{
					// We have reached the end of the scroll
					childRight = right;
					childLeft = childRight - childWidth;
				}
			}
		}

		if( _scrollVertical && _scrollTrackVertDrawable != NULL && _scrollThumbVertDrawable != NULL )
		{
			if( childHeight > containerHeight )
			{
				// We need to do some scrolling
				_showVertBar = true;
				childTop = top - _scrollY - _scrollCurrentY;
				if( childTop > top )
					childTop = top;
				childBottom = childTop + childHeight;
				if( childBottom < bottom )
				{
					// We have reached the end of the scroll
					childBottom = bottom;
					childTop = childBottom - childHeight;
				}
			}
		}


		// Scrollbar offset sizes
		int scrollHeight = GetScrollbarHeight();
		int scrollWidth = GetScrollbarWidth();


		// Sort out the scrollbars
		if( _showHorizBar && _scrollTrackHorizDrawable != NULL )
		{
			childBottom -= scrollHeight;

			if( _showVertBar )
				_scrollTrackHorizDrawable->SetBounds(left,bottom - scrollHeight, right - scrollWidth, bottom);
			else
				_scrollTrackHorizDrawable->SetBounds(left,bottom - scrollHeight, right, bottom);

			BitmapDrawable *bitDraw = dynamic_cast<BitmapDrawable*>(_scrollTrackHorizDrawable);
			if( bitDraw != NULL )
				bitDraw->SetScaleup(true);

			// Sort out the thumb thing
			if( _scrollThumbHorizDrawable != NULL )
			{
				int thumbLeft, thumbRight;
				thumbLeft = GetScrollThumbLeft();
				thumbRight = GetScrollThumbRight();

				_scrollThumbHorizDrawable->SetBounds(thumbLeft,bottom-scrollHeight, thumbRight, bottom);

				BitmapDrawable *bitDrawThumb = dynamic_cast<BitmapDrawable*>(_scrollThumbHorizDrawable);
				if( bitDrawThumb != NULL )
					bitDrawThumb->SetScaleup(true);
			}
		}

		if( _showVertBar && _scrollTrackVertDrawable != NULL )
		{
			childRight -= scrollWidth;

			if( _showHorizBar )
				_scrollTrackVertDrawable->SetBounds(right-scrollWidth,top, right, bottom - scrollHeight);
			else
				_scrollTrackVertDrawable->SetBounds(right-scrollWidth,top, right, bottom);

			BitmapDrawable *bitDraw = dynamic_cast<BitmapDrawable*>(_scrollTrackVertDrawable);
			if( bitDraw != NULL )
				bitDraw->SetScaleup(true);

			// Sort out the thumb thing
			if( _scrollThumbVertDrawable != NULL )
			{
				int thumbTop, thumbBottom;
				thumbTop = GetScrollThumbTop();
				thumbBottom = GetScrollThumbBottom();

				_scrollThumbVertDrawable->SetBounds(right-scrollWidth,thumbTop, right, thumbBottom);

				BitmapDrawable *bitDrawThumb = dynamic_cast<BitmapDrawable*>(_scrollThumbVertDrawable);
				if( bitDrawThumb != NULL )
					bitDrawThumb->SetScaleup(true);
			}
		}

		//std::cout<<"INFO: ScrollContainer layout child: left: "<<childLeft<<" top: "<< childTop<<" right: "<<childRight<<" bottom: "<<childBottom<<std::endl;
		_child->Layout(childLeft,childTop,childRight,childBottom,windowWidth,windowHeight);
	}

}

void ShivaGUI::ScrollContainer::Update(float deltaTs, GUIController *guiController)
{
	if( _needLayout )
	{
		guiController->Layout();
		_needLayout = false;
	}
	if( _child != NULL )
		_child->Update(deltaTs,guiController);
}

void ShivaGUI::ScrollContainer::Draw()
{
	View::Draw();

	if( _visible )
	{
		glEnable(GL_CLIP_PLANE0);
		glEnable(GL_CLIP_PLANE1);
		glEnable(GL_CLIP_PLANE2);
		glEnable(GL_CLIP_PLANE3);
		//glViewport(_layoutBoundsLeft,_layoutBoundsBottom,_layoutBoundsRight-_layoutBoundsLeft,_layoutBoundsBottom-_layoutBoundsTop);

		int vertOffset = 0;
		int horizOffset = 0;
		if( _showHorizBar )
			vertOffset = GetScrollbarHeight();
		if( _showVertBar )
			horizOffset = GetScrollbarWidth();

		GLdouble planeEquationBottom[4] = {0.0, -1.0, 0.0, _layoutBoundsBottom-vertOffset};
		GLdouble planeEquationTop[4] = {0.0, 1.0, 0.0, -_layoutBoundsTop};
		GLdouble planeEquationRight[4] = {-1.0, 0.0, 0.0, _layoutBoundsRight-horizOffset};
		GLdouble planeEquationLeft[4] = {1.0, 0.0, 0.0, -_layoutBoundsLeft};

		glClipPlane(GL_CLIP_PLANE0,planeEquationBottom);
		glClipPlane(GL_CLIP_PLANE1,planeEquationTop);
		glClipPlane(GL_CLIP_PLANE2,planeEquationRight);
		glClipPlane(GL_CLIP_PLANE3,planeEquationLeft);



		// Draw child
		if( _child != NULL )
			_child->Draw();
		// Draw scroll-bars
		glDisable(GL_CLIP_PLANE0);
		glDisable(GL_CLIP_PLANE1);
		glDisable(GL_CLIP_PLANE2);
		glDisable(GL_CLIP_PLANE3);

		if( _showHorizBar )
		{
			_scrollTrackHorizDrawable->Draw();
			_scrollThumbHorizDrawable->Draw();
		}
		if( _showVertBar )
		{
			_scrollTrackVertDrawable->Draw();
			_scrollThumbVertDrawable->Draw();
		}
	}
}


void ShivaGUI::ScrollContainer::Inflate(TiXmlElement *xmlElement, ResourceManager *resources, std::string themePrefix, bool rootNode)
{
	if( themePrefix.empty() )
		themePrefix = "ScrollContainer_";
	ViewGroup::Inflate(xmlElement,resources,themePrefix,rootNode);

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{

		if( currentAttribute->Name() == std::string("scroll_horizontal") )
		{
			std::string result( currentAttribute->Value() );
			if( boost::algorithm::iequals("true",result) )
				_scrollHorizontal = true;
			else
				_scrollHorizontal = false;
		}
		else if( currentAttribute->Name() == std::string("scroll_vertical") )
		{
			std::string result( currentAttribute->Value() );
			if( boost::algorithm::iequals("true",result) )
				_scrollVertical= true;
			else
				_scrollVertical = false;
		}
		else if( (currentAttribute->Name() == std::string("scrollbarTrackVertical"))
				|| (currentAttribute->Name() == (themePrefix + std::string("scrollbarTrackVertical"))) )
		{
			_scrollTrackVertName = currentAttribute->Value();

			_scrollTrackVertFromTheme = ((themePrefix+"scrollbarTrackVertical") == currentAttribute->Name());
			_scrollTrackVertDrawable = resources->GetDrawable( _scrollTrackVertName );
		}
		else if( (currentAttribute->Name() == std::string("scrollbarTrackHorizontal"))
				|| (currentAttribute->Name() == (themePrefix + std::string("scrollbarTrackHorizontal"))) )
		{
			_scrollTrackHorizName = currentAttribute->Value();

			_scrollTrackHorizFromTheme = ((themePrefix+"scrollbarTrackHorizontal") == currentAttribute->Name());
			_scrollTrackHorizDrawable = resources->GetDrawable( _scrollTrackHorizName );
		}
		else if( (currentAttribute->Name() == std::string("scrollbarThumbVertical"))
				|| (currentAttribute->Name() == (themePrefix + std::string("scrollbarThumbVertical"))) )
		{
			_scrollThumbVertName = currentAttribute->Value();

			_scrollThumbVertFromTheme = ((themePrefix+"scrollbarThumbVertical") == currentAttribute->Name());
			_scrollThumbVertDrawable = resources->GetDrawable( _scrollThumbVertName );
		}
		else if( (currentAttribute->Name() == std::string("scrollbarThumbHorizontal"))
				|| (currentAttribute->Name() == (themePrefix + std::string("scrollbarThumbHorizontal"))) )
		{
			_scrollThumbHorizName = currentAttribute->Value();

			_scrollThumbHorizFromTheme = ((themePrefix+"scrollbarThumbHorizontal") == currentAttribute->Name());
			_scrollThumbHorizDrawable = resources->GetDrawable( _scrollThumbHorizName );
		}


	}
}

TiXmlElement* ShivaGUI::ScrollContainer::Deflate(ResourceManager *resources)
{
	TiXmlElement *xmlNode = View::Deflate(resources);
	xmlNode->SetValue("ScrollContainer");

	if( _scrollHorizontal )
		xmlNode->SetAttribute("scroll_horizontal","true" );
	else
		xmlNode->SetAttribute("scroll_horizontal","false" );
	if( _scrollVertical )
		xmlNode->SetAttribute("scroll_vertical","true" );
	else
		xmlNode->SetAttribute("scroll_vertical","false" );

	if( (_scrollTrackVertDrawable != NULL) && !_scrollTrackVertFromTheme )
	{
		xmlNode->SetAttribute("scrollbarTrackVertical", _scrollTrackVertName );
	}
	if( (_scrollTrackHorizDrawable != NULL) && !_scrollTrackHorizFromTheme )
	{
		xmlNode->SetAttribute("scrollbarTrackHorizontal", _scrollTrackHorizName );
	}
	if( (_scrollThumbVertDrawable != NULL) && !_scrollThumbVertFromTheme )
	{
		xmlNode->SetAttribute("scrollbarThumbVertical", _scrollThumbVertName );
	}
	if( (_scrollThumbHorizDrawable != NULL) && !_scrollThumbHorizFromTheme )
	{
		xmlNode->SetAttribute("scrollbarTrackHorizontal", _scrollThumbHorizName );
	}



	// Child
	if( _child != NULL )
	{
		xmlNode->InsertEndChild( *_child->Deflate(resources) );
	}

	return xmlNode;
}

void ShivaGUI::ScrollContainer::AddView(View *value, ResourceManager*)
{
	_child = value;
}

bool ShivaGUI::ScrollContainer::HandleEvent(InternalEvent *currentEvent)
{
	bool eventAbsorbed = false;
	bool passToChild = true;

	// Handle scrolling
	if( currentEvent->GetType() == InternalEvent::POSITIONAL_SELECT )
	{
		if( EventHit(currentEvent) )
		{
			currentEvent->GetPosition(_selectedMouseX,_selectedMouseY);
			passToChild = false;
			int dragTestVertScroll = EventHitVertScroll(currentEvent);
			int dragTestHorizScroll = EventHitHorizScroll(currentEvent);
			if( dragTestVertScroll == 1 ) // hit vertical scroll track at the top, so scroll a bit
			{
				_scrollY -= _scrollJumpAmount;
				_needLayout = true;
			}
			else if( dragTestVertScroll == 2 ) // hit vertical scroll thumb
			{
				_verticalScrollDrag = true;
				_scrollDragOriginalTop = GetScrollThumbTop();
			}
			else if( dragTestVertScroll == 3 ) // hit vertical scroll track at the bottom, so scroll a bit
			{
				_scrollY += _scrollJumpAmount;
				_needLayout = true;
			}
			else if( dragTestHorizScroll == 1 ) // hit horizontal scroll track at the left, so scroll a bit
			{
				_scrollX -= _scrollJumpAmount;
				_needLayout = true;
			}
			else if( dragTestHorizScroll == 2 ) // hit horizontal scroll thumb
			{
				_horizontalScrollDrag = true;
				_scrollDragOriginalLeft = GetScrollThumbLeft();
			}
			else if( dragTestHorizScroll == 3 ) // hit horizontal scroll track at the left, so scroll a bit
			{
				_scrollX += _scrollJumpAmount;
				_needLayout = true;
			}
			else // Didn't hit a scrollbar
			{
				_selected = true;
				eventAbsorbed = true;
				passToChild = true;
			}

		}
		else
		{
			passToChild = false;
			_selected = false;
		}
	}
	else if( currentEvent->GetType() == InternalEvent::POSITIONAL_DESELECT )
	{
		_dragSelected = _selected = false;
		_scrollX += _scrollCurrentX;
		if( _scrollX < 0 )
			_scrollX = 0;
		_scrollY += _scrollCurrentY;
		if( _scrollY < 0 )
			_scrollY = 0;
		_scrollCurrentX = 0;
		_scrollCurrentY = 0;
		_verticalScrollDrag = _horizontalScrollDrag = false;
	}
	else if( currentEvent->GetType() == InternalEvent::POSITIONAL_DRAG )
	{
		if( (!_selected) && (!_dragSelected) && (!_verticalScrollDrag) && (!_horizontalScrollDrag) )
		{
			if( EventHit(currentEvent) )
			{
				// Nothing to do with the ScrollContainer, but the event may be needed by our children, so pass it on
				passToChild = true;
			}
		}
		else
		{

			int currentMouseX, currentMouseY;
			currentEvent->GetPosition(currentMouseX, currentMouseY);
			int offsetX = currentMouseX - _selectedMouseX;
			int offsetY = currentMouseY - _selectedMouseY;

			// Which part are we dragging?
			if( _selected || _dragSelected )
			{
				passToChild = false;
				_dragSelected = true;

				if( _scrollHorizontal )
				{
					_needLayout = true;
					_scrollCurrentX = -offsetX;
				}
				if( _scrollVertical )
				{
					_needLayout = true;
					_scrollCurrentY = -offsetY;
				}

				eventAbsorbed = true;
			}
			else if( _verticalScrollDrag )
			{
				// we need to mostly do the opposite to what we did for finding the thumb positions :S
				// Raw sizes
				int containerHeight = _layoutBoundsBottom - _layoutBoundsTop;
				int childHeight = _child->GetWrapHeight();

				// Scrollbar offset sizes
				int scrollHeight = GetScrollbarHeight();

				// Container size taking into account scrollbars
				int effectiveContainerHeight = 0;
				if( _showHorizBar )
					effectiveContainerHeight = containerHeight - scrollHeight;
				else
					effectiveContainerHeight = containerHeight;

				float thumbHeightProportion = ((float) effectiveContainerHeight / (float) childHeight);
				int thumbHeight = thumbHeightProportion * (float) (effectiveContainerHeight);


				// new position of thumb tab thing
				int newThumbTop = _scrollDragOriginalTop + offsetY;

				// now work backwards through equations in GetScrollThumbTop()
				float thumbTopProportion = (newThumbTop - _layoutBoundsTop) / ((float)effectiveContainerHeight - thumbHeight);

				float scrollOffset = thumbTopProportion * ((float) childHeight - containerHeight);

				_scrollY = scrollOffset;
				_needLayout = true;
			}
			else if( _horizontalScrollDrag )
			{
				// we need to mostly do the opposite to what we did for finding the thumb positions :S
				// Raw sizes
				int containerWidth = _layoutBoundsRight - _layoutBoundsLeft;
				int childWidth = _child->GetWrapWidth();

				// Scrollbar offset sizes
				int scrollWidth = GetScrollbarWidth();

				// Container size taking into account scrollbars
				int effectiveContainerWidth = 0;
				if( _showVertBar )
					effectiveContainerWidth = containerWidth - scrollWidth;
				else
					effectiveContainerWidth = containerWidth;

				float thumbWidthProportion = ((float) effectiveContainerWidth / (float) childWidth);
				int thumbWidth = thumbWidthProportion * (float) (effectiveContainerWidth);


				// new position of thumb tab thing
				int newThumbLeft = _scrollDragOriginalLeft + offsetX;

				// now work backwards through equations in GetScrollThumbLeft()
				float thumbLeftProportion = (newThumbLeft - _layoutBoundsLeft) / ((float)effectiveContainerWidth - thumbWidth);

				float scrollOffset = thumbLeftProportion * ((float) childWidth - containerWidth);

				_scrollX = scrollOffset;
				_needLayout = true;
			}
		}
	}



	// Pass event to child
	if( _child != NULL && passToChild )
	{
		eventAbsorbed = _child->HandleEvent(currentEvent);
	}

	return eventAbsorbed;
}


int ShivaGUI::ScrollContainer::GetWrapWidth()
{
	if( (_child != NULL) && (!_scrollHorizontal) )
		return _child->GetWrapWidth();
	return 10;
}

int ShivaGUI::ScrollContainer::GetWrapHeight()
{
	if( (_child != NULL) && (!_scrollVertical) )
		return _child->GetWrapHeight();
	return 10;
}

int ShivaGUI::ScrollContainer::GetScrollbarWidth()
{
	if( _scrollTrackVertDrawable != NULL )
		return _scrollTrackVertDrawable->GetNativeWidth();
	return 0;
}

int ShivaGUI::ScrollContainer::GetScrollbarHeight()
{
	if( _scrollTrackHorizDrawable != NULL )
		return _scrollTrackHorizDrawable->GetNativeHeight();
	return 0;
}

int ShivaGUI::ScrollContainer::GetScrollThumbTop()
{
	// Raw sizes
	int containerHeight = _layoutBoundsBottom - _layoutBoundsTop;
	int childHeight = _child->GetWrapHeight();

	// Scrollbar offset sizes
	int scrollHeight = GetScrollbarHeight();

	// Container size taking into account scrollbars
	int effectiveContainerHeight = 0;
	if( _showHorizBar )
		effectiveContainerHeight = containerHeight - scrollHeight;
	else
		effectiveContainerHeight = containerHeight;

	float thumbHeightProportion = ((float) effectiveContainerHeight / (float) childHeight);
	int thumbHeight = thumbHeightProportion * (float) (effectiveContainerHeight);

	float thumbTopProportion = ((float)_scrollY + _scrollCurrentY) / ((float) childHeight - containerHeight );
	if( thumbTopProportion < 0.0f )
		thumbTopProportion = 0.0f;
	else if( thumbTopProportion > 1.0f )
		thumbTopProportion = 1.0f;

	return _layoutBoundsTop + (thumbTopProportion * ((float)effectiveContainerHeight - thumbHeight) );
}

int ShivaGUI::ScrollContainer::GetScrollThumbBottom()
{
	// Raw sizes
	int containerHeight = _layoutBoundsBottom - _layoutBoundsTop;
	int childHeight = _child->GetWrapHeight();

	// Scrollbar offset sizes
	int scrollHeight = GetScrollbarHeight();

	// Container size taking into account scrollbars
	int effectiveContainerHeight = 0;
	if( _showHorizBar )
		effectiveContainerHeight = containerHeight - scrollHeight;
	else
		effectiveContainerHeight = containerHeight;

	float thumbHeightProportion = ((float) effectiveContainerHeight / (float) childHeight);
	int thumbHeight = thumbHeightProportion * (float) (effectiveContainerHeight);

	float thumbTopProportion = ((float)_scrollY + _scrollCurrentY) / ((float) childHeight - containerHeight );
	if( thumbTopProportion < 0.0f )
		thumbTopProportion = 0.0f;
	else if( thumbTopProportion > 1.0f )
		thumbTopProportion = 1.0f;

	int thumbTop = _layoutBoundsTop + (thumbTopProportion * ((float)effectiveContainerHeight - thumbHeight) );
	return thumbTop + thumbHeight;
}

int ShivaGUI::ScrollContainer::GetScrollThumbLeft()
{
	// Raw sizes
	int containerWidth = _layoutBoundsRight - _layoutBoundsLeft;
	int childWidth = _child->GetWrapWidth();

	// Scrollbar offset sizes
	int scrollWidth = GetScrollbarWidth();

	// Container size taking into account scrollbars
	int effectiveContainerWidth = 0;
	if( _showVertBar )
		effectiveContainerWidth = containerWidth - scrollWidth;
	else
		effectiveContainerWidth = containerWidth;


	float thumbWidthProportion = ((float) effectiveContainerWidth / (float) childWidth);
	int thumbWidth = thumbWidthProportion * (float) (effectiveContainerWidth);

	float thumbLeftProportion = ((float)_scrollX + _scrollCurrentX) / ((float) childWidth - containerWidth );
	if( thumbLeftProportion < 0.0f )
		thumbLeftProportion = 0.0f;
	else if( thumbLeftProportion > 1.0f )
		thumbLeftProportion = 1.0f;

	return _layoutBoundsLeft + (thumbLeftProportion * ((float)effectiveContainerWidth - thumbWidth) );
}

int ShivaGUI::ScrollContainer::GetScrollThumbRight()
{
	// Raw sizes
	int containerWidth = _layoutBoundsRight - _layoutBoundsLeft;
	int childWidth = _child->GetWrapWidth();

	// Scrollbar offset sizes
	int scrollWidth = GetScrollbarWidth();

	// Container size taking into account scrollbars
	int effectiveContainerWidth = 0;
	if( _showVertBar )
		effectiveContainerWidth = containerWidth - scrollWidth;
	else
		effectiveContainerWidth = containerWidth;


	float thumbWidthProportion = ((float) effectiveContainerWidth / (float) childWidth);
	int thumbWidth = thumbWidthProportion * (float) (effectiveContainerWidth);

	float thumbLeftProportion = ((float)_scrollX + _scrollCurrentX) / ((float) childWidth - containerWidth );
	if( thumbLeftProportion < 0.0f )
		thumbLeftProportion = 0.0f;
	else if( thumbLeftProportion > 1.0f )
		thumbLeftProportion = 1.0f;

	int thumbLeft = _layoutBoundsLeft + (thumbLeftProportion * ((float)effectiveContainerWidth - thumbWidth) );
	return thumbLeft + thumbWidth;
}


int ShivaGUI::ScrollContainer::EventHitVertScroll(InternalEvent *event)
{
	if( _showVertBar )
	{
		int x, y;
		event->GetPosition(x,y);
		int scrollbarWidth = GetScrollbarWidth();
		if( (x > _layoutBoundsRight - scrollbarWidth) && (x < _layoutBoundsRight)
			&& (y > _layoutBoundsTop) && (y < _layoutBoundsBottom) )
		{
			// hit within bounds of scrollbar, now just need to find which part:
			if( y < GetScrollThumbTop() )
				return 1; // hit top scroll track
			if( y < GetScrollThumbBottom() )
				return 2; // hit scroll thumb
			return 3; // hit bottom scroll track
		}
	}
	return 0;
}

int ShivaGUI::ScrollContainer::EventHitHorizScroll(InternalEvent *event)
{
	if( _showHorizBar )
	{
		int x, y;
		event->GetPosition(x,y);
		int scrollbarHeight = GetScrollbarHeight();
		if( (x > _layoutBoundsLeft) && (x < _layoutBoundsRight)
			&& (y > _layoutBoundsBottom - scrollbarHeight) && (y < _layoutBoundsBottom) )
		{
			// hit within bounds of scrollbar, now just need to find which part:
			if( x < GetScrollThumbLeft() )
				return 1; // hit left scroll track
			if( x < GetScrollThumbRight() )
				return 2; // hit scroll thumb
			return 3; // hit right scroll track
		}
	}
	return 0;
}
