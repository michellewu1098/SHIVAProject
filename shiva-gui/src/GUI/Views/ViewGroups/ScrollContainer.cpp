#include "GUI/Views/ViewGroups/ScrollContainer.h"
#include "GUI/GUIController.h"
#include "GUI/Drawables/BitmapDrawable.h"

//----------------------------------------------------------------------------------

ShivaGUI::ScrollContainer::ScrollContainer()
{
	m_scrollHorizontal = false;
	m_scrollVertical = false;
	m_child = NULL;
	m_scrollX = m_scrollY = 0;
	m_selected = m_dragSelected = false;
	m_selectedMouseX = m_selectedMouseY = 0;
	m_needLayout = false;
	m_scrollCurrentX = m_scrollCurrentY = 0;
	m_scrollTrackVertDrawable = m_scrollTrackHorizDrawable = m_scrollThumbVertDrawable = m_scrollThumbHorizDrawable = NULL;
	m_scrollTrackVertFromTheme =  m_scrollTrackHorizFromTheme = m_scrollThumbVertFromTheme = m_scrollThumbHorizFromTheme = false;

	m_showHorizBar = false;
	m_showVertBar = false;

	m_scrollJumpAmount = 30;
	m_verticalScrollDrag = m_horizontalScrollDrag = false;
	m_scrollDragOriginalTop = m_scrollDragOriginalLeft = 0;
}

//----------------------------------------------------------------------------------

ShivaGUI::ScrollContainer::~ScrollContainer()
{
	delete m_child;
	delete m_scrollTrackVertDrawable;
	delete m_scrollTrackHorizDrawable;
	delete m_scrollThumbVertDrawable;
	delete m_scrollThumbHorizDrawable;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ScrollContainer::NotifyDrawingContextChange( ResourceManager *_resources )
{
	if( m_child != NULL )
		m_child->NotifyDrawingContextChange( _resources );
}

//----------------------------------------------------------------------------------

void ShivaGUI::ScrollContainer::Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	View::Layout( _left, _top, _right, _bottom, _windowWidth, _windowHeight );

	m_windowWidth = _windowWidth;
	m_windowHeight = _windowHeight;

	if( m_child != NULL )
	{
		// Need to layout child:

		int childLeft = _left, childRight = _right, childTop = _top, childBottom = _bottom;

		// Whether we need to show the scrollbars
		m_showHorizBar = false;
		m_showVertBar = false;

		int containerWidth = _right - _left;
		int childWidth = m_child->GetWrapWidth();

		int containerHeight = _bottom - _top;
		int childHeight = m_child->GetWrapHeight();


		// how are we actually going to do this scrolling stuff?
		// from the way things work at the moment, the only way is to tell the child to be its maximum size and then clip the drawing
		// ok, let's give this a go
		if( m_scrollHorizontal && m_scrollTrackHorizDrawable != NULL && m_scrollThumbHorizDrawable != NULL )
		{
			if( childWidth > containerWidth )
			{
				// We need to do some scrolling
				m_showHorizBar = true;
				childLeft = _left - m_scrollX - m_scrollCurrentX;
				if( childLeft > _left )
					childLeft = _left;
				childRight = childLeft + childWidth;
				if( childRight < _right )
				{
					// We have reached the end of the scroll
					childRight = _right;
					childLeft = childRight - childWidth;
				}
			}
		}

		if( m_scrollVertical && m_scrollTrackVertDrawable != NULL && m_scrollThumbVertDrawable != NULL )
		{
			if( childHeight > containerHeight )
			{
				// We need to do some scrolling
				m_showVertBar = true;
				childTop = _top - m_scrollY - m_scrollCurrentY;
				if( childTop > _top )
					childTop = _top;
				childBottom = childTop + childHeight;
				if( childBottom < _bottom )
				{
					// We have reached the end of the scroll
					childBottom = _bottom;
					childTop = childBottom - childHeight;
				}
			}
		}


		// Scrollbar offset sizes
		int scrollHeight = GetScrollbarHeight();
		int scrollWidth = GetScrollbarWidth();


		// Sort out the scrollbars
		if( m_showHorizBar && m_scrollTrackHorizDrawable != NULL )
		{
			childBottom -= scrollHeight;

			if( m_showVertBar )
				m_scrollTrackHorizDrawable->SetBounds( ( float )_left, ( float )( _bottom - scrollHeight ), ( float )( _right - scrollWidth ), ( float )_bottom );
			else
				m_scrollTrackHorizDrawable->SetBounds( ( float )_left, ( float )( _bottom - scrollHeight ), ( float )_right, ( float )_bottom );

			BitmapDrawable *bitDraw = dynamic_cast< BitmapDrawable* >( m_scrollTrackHorizDrawable );
			if( bitDraw != NULL )
				bitDraw->SetScaleup( true );

			// Sort out the thumb thing
			if( m_scrollThumbHorizDrawable != NULL )
			{
				int thumbLeft, thumbRight;
				thumbLeft = GetScrollThumbLeft();
				thumbRight = GetScrollThumbRight();

				m_scrollThumbHorizDrawable->SetBounds( ( float )thumbLeft, ( float )( _bottom - scrollHeight ), ( float )thumbRight, ( float )_bottom );

				BitmapDrawable *bitDrawThumb = dynamic_cast< BitmapDrawable* >( m_scrollThumbHorizDrawable );
				if( bitDrawThumb != NULL )
					bitDrawThumb->SetScaleup( true );
			}
		}

		if( m_showVertBar && m_scrollTrackVertDrawable != NULL )
		{
			childRight -= scrollWidth;

			if( m_showHorizBar )
				m_scrollTrackVertDrawable->SetBounds( ( float )( _right - scrollWidth ), ( float )_top, ( float )_right, ( float )( _bottom - scrollHeight ) );
			else
				m_scrollTrackVertDrawable->SetBounds( ( float )( _right - scrollWidth ), ( float )_top, ( float )_right, ( float )_bottom );

			BitmapDrawable *bitDraw = dynamic_cast< BitmapDrawable* >( m_scrollTrackVertDrawable );
			if( bitDraw != NULL )
				bitDraw->SetScaleup( true );

			// Sort out the thumb thing
			if( m_scrollThumbVertDrawable != NULL )
			{
				int thumbTop, thumbBottom;
				thumbTop = GetScrollThumbTop();
				thumbBottom = GetScrollThumbBottom();

				m_scrollThumbVertDrawable->SetBounds( ( float )( _right - scrollWidth ), ( float )thumbTop, ( float )_right, ( float )thumbBottom );

				BitmapDrawable *bitDrawThumb = dynamic_cast< BitmapDrawable* >( m_scrollThumbVertDrawable );
				if( bitDrawThumb != NULL )
					bitDrawThumb->SetScaleup( true );
			}
		}

		//std::cout << "INFO: ScrollContainer layout child: left: " << childLeft << " top: " << childTop << " right: " << childRight << " bottom: " << childBottom << std::endl;
		m_child->Layout( childLeft, childTop, childRight, childBottom, _windowWidth, _windowHeight );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ScrollContainer::Update( float _deltaTs, GUIController *_guiController )
{
	if( m_needLayout )
	{
		_guiController->Layout();
		m_needLayout = false;
	}
	if( m_child != NULL )
		m_child->Update( _deltaTs, _guiController );
}

//----------------------------------------------------------------------------------

void ShivaGUI::ScrollContainer::Draw()
{
	View::Draw();

	if( m_visible )
	{
		glEnable( GL_SCISSOR_TEST );

		glScissor( m_layoutBoundsLeft, m_windowHeight - m_layoutBoundsBottom, m_layoutBoundsRight - m_layoutBoundsLeft, m_layoutBoundsBottom - m_layoutBoundsTop );
						
		// Draw child
		if( m_child != NULL )
		{
			m_child->Draw();
		}

		glDisable( GL_SCISSOR_TEST );

		if( m_showHorizBar )
		{
			m_scrollTrackHorizDrawable->Draw();
			m_scrollThumbHorizDrawable->Draw();
		}
		if( m_showVertBar )
		{
			m_scrollTrackVertDrawable->Draw();
			m_scrollThumbVertDrawable->Draw();
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ScrollContainer::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "ScrollContainer_";
	ViewGroup::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{

		if( currentAttribute->Name() == std::string( "scroll_horizontal" ) )
		{
			std::string result( currentAttribute->Value() );
			if( boost::algorithm::iequals( "true", result ) )
				m_scrollHorizontal = true;
			else
				m_scrollHorizontal = false;
		}
		else if( currentAttribute->Name() == std::string( "scroll_vertical" ) )
		{
			std::string result( currentAttribute->Value() );
			if( boost::algorithm::iequals( "true", result ) )
				m_scrollVertical= true;
			else
				m_scrollVertical = false;
		}
		else if( ( currentAttribute->Name() == std::string( "scrollbarTrackVertical" ) )
				|| ( currentAttribute->Name() == ( _themePrefix + std::string( "scrollbarTrackVertical" ) ) ) )
		{
			m_scrollTrackVertName = currentAttribute->Value();

			m_scrollTrackVertFromTheme = ( ( _themePrefix + "scrollbarTrackVertical" ) == currentAttribute->Name() );
			m_scrollTrackVertDrawable = _resources->GetDrawable( m_scrollTrackVertName );
		}
		else if( ( currentAttribute->Name() == std::string( "scrollbarTrackHorizontal" ) )
				|| ( currentAttribute->Name() == ( _themePrefix + std::string( "scrollbarTrackHorizontal" ) ) ) )
		{
			m_scrollTrackHorizName = currentAttribute->Value();

			m_scrollTrackHorizFromTheme = ( ( _themePrefix + "scrollbarTrackHorizontal" ) == currentAttribute->Name() );
			m_scrollTrackHorizDrawable = _resources->GetDrawable( m_scrollTrackHorizName );
		}
		else if( ( currentAttribute->Name() == std::string( "scrollbarThumbVertical" ) )
				|| ( currentAttribute->Name() == ( _themePrefix + std::string( "scrollbarThumbVertical" ) ) ) )
		{
			m_scrollThumbVertName = currentAttribute->Value();

			m_scrollThumbVertFromTheme = ( ( _themePrefix + "scrollbarThumbVertical" ) == currentAttribute->Name() );
			m_scrollThumbVertDrawable = _resources->GetDrawable( m_scrollThumbVertName );
		}
		else if( ( currentAttribute->Name() == std::string( "scrollbarThumbHorizontal" ) )
				|| ( currentAttribute->Name() == ( _themePrefix + std::string( "scrollbarThumbHorizontal" ) ) ) )
		{
			m_scrollThumbHorizName = currentAttribute->Value();

			m_scrollThumbHorizFromTheme = ( ( _themePrefix + "scrollbarThumbHorizontal" ) == currentAttribute->Name() );
			m_scrollThumbHorizDrawable = _resources->GetDrawable( m_scrollThumbHorizName );
		}
	}
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::ScrollContainer::Deflate( ResourceManager *_resources )
{
	TiXmlElement *xmlNode = View::Deflate( _resources );
	xmlNode->SetValue( "ScrollContainer" );

	if( m_scrollHorizontal )
		xmlNode->SetAttribute( "scroll_horizontal", "true" );
	else
		xmlNode->SetAttribute( "scroll_horizontal", "false" );
	if( m_scrollVertical )
		xmlNode->SetAttribute( "scroll_vertical", "true" );
	else
		xmlNode->SetAttribute( "scroll_vertical", "false" );

	if( ( m_scrollTrackVertDrawable != NULL ) && !m_scrollTrackVertFromTheme )
	{
		xmlNode->SetAttribute( "scrollbarTrackVertical", m_scrollTrackVertName );
	}
	if( ( m_scrollTrackHorizDrawable != NULL ) && !m_scrollTrackHorizFromTheme )
	{
		xmlNode->SetAttribute( "scrollbarTrackHorizontal", m_scrollTrackHorizName );
	}
	if( ( m_scrollThumbVertDrawable != NULL ) && !m_scrollThumbVertFromTheme )
	{
		xmlNode->SetAttribute( "scrollbarThumbVertical", m_scrollThumbVertName );
	}
	if( ( m_scrollThumbHorizDrawable != NULL ) && !m_scrollThumbHorizFromTheme )
	{
		xmlNode->SetAttribute( "scrollbarTrackHorizontal", m_scrollThumbHorizName );
	}

	// Child
	if( m_child != NULL )
	{
		xmlNode->InsertEndChild( *m_child->Deflate( _resources ) );
	}

	return xmlNode;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ScrollContainer::AddView( View *_value, ResourceManager *_resources )
{
	m_child = _value;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ScrollContainer::HandleEvent( InternalEvent *_currentEvent )
{
	bool eventAbsorbed = false;
	bool passToChild = true;

	// Handle scrolling
	if( _currentEvent->GetType() == InternalEvent::POSITIONAL_SELECT )
	{
		if( EventHit( _currentEvent ) )
		{
			_currentEvent->GetPosition( m_selectedMouseX, m_selectedMouseY );
			passToChild = false;
			int dragTestVertScroll = EventHitVertScroll( _currentEvent );
			int dragTestHorizScroll = EventHitHorizScroll( _currentEvent );
			if( dragTestVertScroll == 1 ) // hit vertical scroll track at the top, so scroll a bit
			{
				m_scrollY -= m_scrollJumpAmount;
				m_needLayout = true;
			}
			else if( dragTestVertScroll == 2 ) // hit vertical scroll thumb
			{
				m_verticalScrollDrag = true;
				m_scrollDragOriginalTop = GetScrollThumbTop();
			}
			else if( dragTestVertScroll == 3 ) // hit vertical scroll track at the bottom, so scroll a bit
			{
				m_scrollY += m_scrollJumpAmount;
				m_needLayout = true;
			}
			else if( dragTestHorizScroll == 1 ) // hit horizontal scroll track at the left, so scroll a bit
			{
				m_scrollX -= m_scrollJumpAmount;
				m_needLayout = true;
			}
			else if( dragTestHorizScroll == 2 ) // hit horizontal scroll thumb
			{
				m_horizontalScrollDrag = true;
				m_scrollDragOriginalLeft = GetScrollThumbLeft();
			}
			else if( dragTestHorizScroll == 3 ) // hit horizontal scroll track at the left, so scroll a bit
			{
				m_scrollX += m_scrollJumpAmount;
				m_needLayout = true;
			}
			else // Didn't hit a scrollbar
			{
				m_selected = true;
				eventAbsorbed = true;
				passToChild = true;
			}
		}
		else
		{
			passToChild = false;
			m_selected = false;
		}
	}
	else if( _currentEvent->GetType() == InternalEvent::POSITIONAL_DESELECT )
	{
		m_dragSelected = m_selected = false;
		m_scrollX += m_scrollCurrentX;
		if( m_scrollX < 0 )
			m_scrollX = 0;
		m_scrollY += m_scrollCurrentY;
		if( m_scrollY < 0 )
			m_scrollY = 0;
		m_scrollCurrentX = 0;
		m_scrollCurrentY = 0;
		m_verticalScrollDrag = m_horizontalScrollDrag = false;
	}
	else if( _currentEvent->GetType() == InternalEvent::POSITIONAL_DRAG )
	{
		if( ( !m_selected ) && ( !m_dragSelected ) && ( !m_verticalScrollDrag ) &&  ( !m_horizontalScrollDrag ) )
		{
			if( EventHit( _currentEvent ) )
			{
				// Nothing to do with the ScrollContainer, but the event may be needed by our children, so pass it on
				passToChild = true;
			}
		}
		else
		{

			int currentMouseX, currentMouseY;
			_currentEvent->GetPosition( currentMouseX, currentMouseY );
			int offsetX = currentMouseX - m_selectedMouseX;
			int offsetY = currentMouseY - m_selectedMouseY;

			// Which part are we dragging?
			if( m_selected || m_dragSelected )
			{
				passToChild = false;
				m_dragSelected = true;

				if( m_scrollHorizontal )
				{
					m_needLayout = true;
					m_scrollCurrentX = -offsetX;
				}
				if( m_scrollVertical )
				{
					m_needLayout = true;
					m_scrollCurrentY = -offsetY;
				}

				eventAbsorbed = true;
			}
			else if( m_verticalScrollDrag )
			{
				// we need to mostly do the opposite to what we did for finding the thumb positions :S
				// Raw sizes
				int containerHeight = m_layoutBoundsBottom - m_layoutBoundsTop;
				int childHeight = m_child->GetWrapHeight();

				// Scrollbar offset sizes
				int scrollHeight = GetScrollbarHeight();

				// Container size taking into account scrollbars
				int effectiveContainerHeight = 0;
				if( m_showHorizBar )
					effectiveContainerHeight = containerHeight - scrollHeight;
				else
					effectiveContainerHeight = containerHeight;

				float thumbHeightProportion = ( ( float )effectiveContainerHeight / ( float )childHeight );
				int thumbHeight = ( int )( thumbHeightProportion * ( float )( effectiveContainerHeight ) );


				// new position of thumb tab thing
				int newThumbTop = m_scrollDragOriginalTop + offsetY;

				// now work backwards through equations in GetScrollThumbTop()
				float thumbTopProportion = ( newThumbTop - m_layoutBoundsTop ) / ( ( float )effectiveContainerHeight - thumbHeight );

				float scrollOffset = thumbTopProportion * ( ( float ) childHeight - containerHeight );

				m_scrollY = ( int )scrollOffset;
				m_needLayout = true;
			}
			else if( m_horizontalScrollDrag )
			{
				// we need to mostly do the opposite to what we did for finding the thumb positions :S
				// Raw sizes
				int containerWidth = m_layoutBoundsRight - m_layoutBoundsLeft;
				int childWidth = m_child->GetWrapWidth();

				// Scrollbar offset sizes
				int scrollWidth = GetScrollbarWidth();

				// Container size taking into account scrollbars
				int effectiveContainerWidth = 0;
				if( m_showVertBar )
					effectiveContainerWidth = containerWidth - scrollWidth;
				else
					effectiveContainerWidth = containerWidth;

				float thumbWidthProportion = ( ( float )effectiveContainerWidth / ( float )childWidth );
				int thumbWidth = ( int )( thumbWidthProportion * ( float )( effectiveContainerWidth ) );


				// new position of thumb tab thing
				int newThumbLeft = m_scrollDragOriginalLeft + offsetX;

				// now work backwards through equations in GetScrollThumbLeft()
				float thumbLeftProportion = ( newThumbLeft - m_layoutBoundsLeft ) / ( ( float )effectiveContainerWidth - thumbWidth );

				float scrollOffset = thumbLeftProportion * ( ( float )childWidth - containerWidth );

				m_scrollX = ( int )scrollOffset;
				m_needLayout = true;
			}
		}
	}
	

	// Pass event to child
	if( m_child != NULL && passToChild )
	{
		eventAbsorbed = m_child->HandleEvent( _currentEvent );
	}

	return eventAbsorbed;
}

//----------------------------------------------------------------------------------

int ShivaGUI::ScrollContainer::GetWrapWidth()
{
	if( ( m_child != NULL ) && ( !m_scrollHorizontal ) )
		return m_child->GetWrapWidth();
	return 10;
}

//----------------------------------------------------------------------------------

int ShivaGUI::ScrollContainer::GetWrapHeight()
{
	if( ( m_child != NULL ) && ( !m_scrollVertical ) )
		return m_child->GetWrapHeight();
	return 10;
}

//----------------------------------------------------------------------------------

int ShivaGUI::ScrollContainer::GetScrollbarWidth()
{
	if( m_scrollTrackVertDrawable != NULL )
		return m_scrollTrackVertDrawable->GetNativeWidth();
	return 0;
}

//----------------------------------------------------------------------------------

int ShivaGUI::ScrollContainer::GetScrollbarHeight()
{
	if( m_scrollTrackHorizDrawable != NULL )
		return m_scrollTrackHorizDrawable->GetNativeHeight();
	return 0;
}

//----------------------------------------------------------------------------------

int ShivaGUI::ScrollContainer::GetScrollThumbTop()
{
	// Raw sizes
	int containerHeight = m_layoutBoundsBottom - m_layoutBoundsTop;
	int childHeight = m_child->GetWrapHeight();

	// Scrollbar offset sizes
	int scrollHeight = GetScrollbarHeight();

	// Container size taking into account scrollbars
	int effectiveContainerHeight = 0;
	if( m_showHorizBar )
		effectiveContainerHeight = containerHeight - scrollHeight;
	else
		effectiveContainerHeight = containerHeight;

	float thumbHeightProportion = ( ( float )effectiveContainerHeight / ( float )childHeight );
	int thumbHeight = ( int)( thumbHeightProportion * ( float )( effectiveContainerHeight ) );

	float thumbTopProportion = ( ( float )m_scrollY + m_scrollCurrentY) / ( ( float ) childHeight - containerHeight );
	if( thumbTopProportion < 0.0f )
		thumbTopProportion = 0.0f;
	else if( thumbTopProportion > 1.0f )
		thumbTopProportion = 1.0f;

	return m_layoutBoundsTop + ( int )( ( thumbTopProportion * ( ( float )effectiveContainerHeight - thumbHeight ) ) );
}

//----------------------------------------------------------------------------------

int ShivaGUI::ScrollContainer::GetScrollThumbBottom()
{
	// Raw sizes
	int containerHeight = m_layoutBoundsBottom - m_layoutBoundsTop;
	int childHeight = m_child->GetWrapHeight();

	// Scrollbar offset sizes
	int scrollHeight = GetScrollbarHeight();

	// Container size taking into account scrollbars
	int effectiveContainerHeight = 0;
	if( m_showHorizBar )
		effectiveContainerHeight = containerHeight - scrollHeight;
	else
		effectiveContainerHeight = containerHeight;

	float thumbHeightProportion = ( ( float )effectiveContainerHeight / ( float )childHeight );
	int thumbHeight = ( int )( thumbHeightProportion * ( float )( effectiveContainerHeight ) );

	float thumbTopProportion = ( ( float )m_scrollY + m_scrollCurrentY ) / ( ( float )childHeight - containerHeight );
	if( thumbTopProportion < 0.0f )
		thumbTopProportion = 0.0f;
	else if( thumbTopProportion > 1.0f )
		thumbTopProportion = 1.0f;

	int thumbTop = m_layoutBoundsTop + ( int )( ( thumbTopProportion * ( ( float )effectiveContainerHeight - thumbHeight ) ) );
	return thumbTop + thumbHeight;
}

//----------------------------------------------------------------------------------

int ShivaGUI::ScrollContainer::GetScrollThumbLeft()
{
	// Raw sizes
	int containerWidth = m_layoutBoundsRight - m_layoutBoundsLeft;
	int childWidth = m_child->GetWrapWidth();

	// Scrollbar offset sizes
	int scrollWidth = GetScrollbarWidth();

	// Container size taking into account scrollbars
	int effectiveContainerWidth = 0;
	if( m_showVertBar )
		effectiveContainerWidth = containerWidth - scrollWidth;
	else
		effectiveContainerWidth = containerWidth;


	float thumbWidthProportion = ( ( float ) effectiveContainerWidth / ( float ) childWidth );
	int thumbWidth = ( int )( thumbWidthProportion * ( float )( effectiveContainerWidth ) );

	float thumbLeftProportion = ( ( float )m_scrollX + m_scrollCurrentX ) / ( ( float )childWidth - containerWidth );
	if( thumbLeftProportion < 0.0f )
		thumbLeftProportion = 0.0f;
	else if( thumbLeftProportion > 1.0f )
		thumbLeftProportion = 1.0f;

	return m_layoutBoundsLeft + ( int )( ( thumbLeftProportion * ( ( float )effectiveContainerWidth - thumbWidth ) ) );
}

//----------------------------------------------------------------------------------

int ShivaGUI::ScrollContainer::GetScrollThumbRight()
{
	// Raw sizes
	int containerWidth = m_layoutBoundsRight - m_layoutBoundsLeft;
	int childWidth = m_child->GetWrapWidth();

	// Scrollbar offset sizes
	int scrollWidth = GetScrollbarWidth();

	// Container size taking into account scrollbars
	int effectiveContainerWidth = 0;
	if( m_showVertBar )
		effectiveContainerWidth = containerWidth - scrollWidth;
	else
		effectiveContainerWidth = containerWidth;


	float thumbWidthProportion = ( ( float )effectiveContainerWidth / ( float ) childWidth );
	int thumbWidth = ( int )( thumbWidthProportion * ( float )( effectiveContainerWidth ) );

	float thumbLeftProportion = ( ( float )m_scrollX + m_scrollCurrentX ) / ( ( float ) childWidth - containerWidth );
	if( thumbLeftProportion < 0.0f )
		thumbLeftProportion = 0.0f;
	else if( thumbLeftProportion > 1.0f )
		thumbLeftProportion = 1.0f;

	int thumbLeft = m_layoutBoundsLeft + ( int )( ( thumbLeftProportion * ( ( float )effectiveContainerWidth - thumbWidth ) ) );
	return thumbLeft + thumbWidth;
}

//----------------------------------------------------------------------------------

int ShivaGUI::ScrollContainer::EventHitVertScroll( InternalEvent *_event )
{
	if( m_showVertBar )
	{
		int x, y;
		_event->GetPosition( x, y );
		int scrollbarWidth = GetScrollbarWidth();
		if( ( x > m_layoutBoundsRight - scrollbarWidth ) && ( x < m_layoutBoundsRight )
			&& ( y > m_layoutBoundsTop ) && ( y < m_layoutBoundsBottom ) )
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

//----------------------------------------------------------------------------------

int ShivaGUI::ScrollContainer::EventHitHorizScroll( InternalEvent *_event )
{
	if( m_showHorizBar )
	{
		int x, y;
		_event->GetPosition( x, y );
		int scrollbarHeight = GetScrollbarHeight();
		if( ( x > m_layoutBoundsLeft ) && ( x < m_layoutBoundsRight )
			&& ( y > m_layoutBoundsBottom - scrollbarHeight ) && ( y < m_layoutBoundsBottom ) )
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

//----------------------------------------------------------------------------------