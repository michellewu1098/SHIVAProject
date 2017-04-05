#include "GUI/Views/ViewGroups/CustomLayout.h"
#include "GUI/Views/ViewGroups/CustomLayoutElement.h"
#include "GUI/Views/ViewGroups/CustomLayoutParams.h"

//----------------------------------------------------------------------------------

ShivaGUI::CustomLayoutElement::CustomLayoutElement( View *_child, ResourceManager *_resources, CustomLayout *_parent )
{
	m_child = _child;
	m_parent = _parent;

	m_editDrawable = new RectDrawable();
	m_editDrawable->SetFillColour( 0.9f, 0.5f, 0.4f, 0.3f );
	m_editDrawable->SetBorderColour( 0.9f, 0.5f, 0.4f, 1.0f );

	m_resizeDrawable = new RectDrawable();
	m_resizeDrawable->SetFillColour( 0.9f, 0.5f, 0.4f, 0.3f );
	m_resizeDrawable->SetBorderColour( 0.9f, 0.5f, 0.4f, 1.0f );

	m_eventHandler = new CustomElementEventHandler( this );

	m_editVisibilityButton = new ImageButton();

	// MEMORY LEAK?
	CustomLayoutParams *buttonParams = new CustomLayoutParams();
	buttonParams->SetWidthConst( LayoutParams::PERCENTAGE_PARENT );
	buttonParams->SetHeightConst( LayoutParams::PERCENTAGE_PARENT );
	buttonParams->SetWidthPercent( 0.1f );
	buttonParams->SetHeightPercent( 0.1f );
	buttonParams->SetCentreX( 0.5f );
	buttonParams->SetCentreY( 0.9f );
	//_editVisibilityButton->SetText("Visible",resources);
	
	// Set background images
	// MEMORY LEAK?
	RectDrawable *bgd = new RectDrawable();
	bgd->SetFillColour( 0.4f, 0.0f, 0.6f, 0.1f );
	bgd->SetBorderColour( 0.4f, 0.0f, 0.6f, 0.8f );
	
	m_editVisibilityButton->SetVisibility( false );
	m_editVisibilityButton->SetBackground( bgd );
	m_editVisibilityButton->SetLayoutParams( buttonParams );		// Must be done last, as it gets the size here
	m_editVisibilityButton->SetOnClickListener( m_eventHandler );
	m_internalViews.push_back( m_editVisibilityButton );

	// MEMORY LEAK?
	buttonParams = new CustomLayoutParams();
	buttonParams->SetWidthConst( LayoutParams::PERCENTAGE_PARENT );
	buttonParams->SetHeightConst( LayoutParams::PERCENTAGE_PARENT );
	buttonParams->SetWidthPercent( 0.1f );
	buttonParams->SetHeightPercent( 0.1f );
	buttonParams->SetCentreX( 0.1f );
	buttonParams->SetCentreY( 0.9f );
	//_editWrapSizeButton->SetText("Size Wrap",resources);

	// Set background images
	// MEMORY LEAK?
	bgd = new RectDrawable();
	bgd->SetFillColour( 0.4f, 0.0f, 0.6f, 0.1f );
	bgd->SetBorderColour( 0.4f, 0.0f, 0.6f, 0.8f );

	m_editWrapSizeButton = new ImageButton();

	m_editWrapSizeButton->SetVisibility( false );
	m_editWrapSizeButton->SetBackground( bgd );
	m_editWrapSizeButton->SetLayoutParams( buttonParams );		// Must be done last, as it gets the size here
	m_editWrapSizeButton->SetOnClickListener( m_eventHandler );
	m_internalViews.push_back( m_editWrapSizeButton );

	m_selected = m_dragSelected = m_resizeSelected = false;
	m_selectedX = m_selectedY = 0.0f;
	m_selectedXProp = m_selectedYProp = 0.0f;
	m_selectedWidth = m_selectedHeight = 0.0f;

}

//----------------------------------------------------------------------------------

ShivaGUI::CustomLayoutElement::~CustomLayoutElement()
{
	// Must not delete child
	for( std::vector< View* >::iterator it = m_internalViews.begin(); it != m_internalViews.end(); ++it )
	{
		delete *it;
	}
	delete m_editDrawable;
	delete m_resizeDrawable;
	//delete _editVisibilityButton; // This is in the _internalViews vector, also m_editWrapSizeButton
	delete m_eventHandler;
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayoutElement::NotifyDrawingContextChange( ResourceManager *_resources )
{
	m_child->NotifyDrawingContextChange( _resources );

	for( std::vector< View* >::iterator it = m_internalViews.begin(); it != m_internalViews.end(); ++it )
	{
		( *it )->NotifyDrawingContextChange( _resources );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayoutElement::Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	View::Layout( _left, _top, _right, _bottom, _windowWidth, _windowHeight );

	//std::cout<<"INFO: CustomLayoutElement::Layout: left="<<left<<" top="<<top<<" right="<<right<<" bottom="<<bottom<<std::endl;

	if( m_editDrawable != NULL )
	{
		m_editDrawable->SetBounds( ( float )_left, ( float )_top, ( float )_right, ( float )_bottom, Definitions::CENTRE );
	}

	if( m_resizeDrawable != NULL )
	{
		m_resizeDrawable->SetBounds( ( float )_right - 20.f, ( float )_bottom - 20.f, ( float )_right, ( float )_bottom, Definitions::CENTRE );
	}

	if( m_child != NULL )
	{
		m_child->Layout( _left, _top, _right, _bottom, _windowWidth, _windowHeight );
	}

	for( std::vector<View*>::iterator it = m_internalViews.begin(); it != m_internalViews.end(); ++it )
	{
		LayoutView(*it, _left, _top, _right, _bottom, _windowWidth, _windowHeight );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayoutElement::Update( float _deltaTs, GUIController *_guiController )
{
	if( m_child != NULL )
	{
		m_child->Update( _deltaTs, _guiController );
	}

	for( std::vector<View*>::iterator it = m_internalViews.begin(); it != m_internalViews.end(); ++it )
	{
		(*it)->Update( _deltaTs, _guiController );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayoutElement::Draw()
{
	View::Draw();

	if( m_visible )
	{
		m_child->Draw();

		m_editDrawable->Draw();
		m_resizeDrawable->Draw();

		for( std::vector< View* >::iterator it = m_internalViews.begin(); it != m_internalViews.end(); ++it )
		{
			( *it )->Draw();
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayoutElement::Inflate(TiXmlElement *_xmlElement, ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "CustomLayoutElement_";
	View::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );
}

//----------------------------------------------------------------------------------

bool ShivaGUI::CustomLayoutElement::HandleEvent( InternalEvent *_currentEvent )
{
	bool eventAbsorbed = false;
	if( _currentEvent->GetType() == InternalEvent::POSITIONAL_SELECT )
	{
		if( SetSelected( EventHit( _currentEvent ) ) )
		{
			if( EventHitResize( _currentEvent ) )
			{
				// The select was for the resize widget
				m_resizeSelected = true;
				eventAbsorbed = true;
			}
			else
			{
				// This was a selection within the body of the View
				// It might turn into a drag, so set dragSelected
				m_dragSelected = true;
			}

			// Whatever happens, we need to store the current mouse position, relative to the centre of the View
			// Retrieve mouse hit coords
			int mouseX, mouseY;
			_currentEvent->GetPosition( mouseX, mouseY );

			CustomLayoutParams *params = dynamic_cast< CustomLayoutParams* >( GetLayoutParams() );
			if( params != NULL )
			{
				// The coords space is a proportional one, relative to the parent
				// So we must ask the parent to work out the positions
				float mousePropX, mousePropY;
				m_parent->GetCoordsAsProportion( mouseX, mouseY, mousePropX, mousePropY );
				// These coords are offsets from the centre of the child View
				m_selectedXProp = mousePropX;
				m_selectedYProp = mousePropY;
				m_selectedX = mousePropX - params->GetCentreX();
				m_selectedY = mousePropY - params->GetCentreY();
				GetCurrentSizeProp( m_selectedWidth, m_selectedHeight );
			}
		}
	}
	else if( _currentEvent->GetType() == InternalEvent::POSITIONAL_DESELECT )
	{
		if( EventHit( _currentEvent ) )
		{
			SetSelected( m_selected, false );
			m_resizeSelected = false;
		}
	}
	else if( _currentEvent->GetType() == InternalEvent::POSITIONAL_DRAG )
	{
		if( m_dragSelected )
		{
			// We are moving the whole View

			SetSelected( true, true );
			// Retrieve mouse hit coords
			int mouseX, mouseY;
			_currentEvent->GetPosition( mouseX, mouseY );

			CustomLayoutParams *params = dynamic_cast< CustomLayoutParams* >( GetLayoutParams() );
			if( params != NULL )
			{
				// The coords space is a proportional one, relative to the parent
				// So we must ask the parent to work out the positions
				float mousePropX, mousePropY;
				m_parent->GetCoordsAsProportion( mouseX, mouseY, mousePropX, mousePropY );
				params->SetCentreX( mousePropX - m_selectedX );
				params->SetCentreY( mousePropY - m_selectedY );
			}
			eventAbsorbed = true;
		}
		else if( m_resizeSelected )
		{
			// We are resizing the View

			// Retrieve mouse hit coords
			int mouseX, mouseY;
			_currentEvent->GetPosition( mouseX, mouseY );

			CustomLayoutParams *params = dynamic_cast< CustomLayoutParams* >( GetLayoutParams() );
			if( params != NULL )
			{

				// Get the current size of the View
//					float currentWidth, currentHeight;
//					GetCurrentSizeProp(currentWidth,currentHeight);

				params->SetWidthConst( LayoutParams::PERCENTAGE_PARENT );
				params->SetHeightConst( LayoutParams::PERCENTAGE_PARENT );

				// The coords space is a proportional one, relative to the parent
				// So we must ask the parent to work out the positions
				float mousePropX, mousePropY;
				m_parent->GetCoordsAsProportion( mouseX, mouseY, mousePropX, mousePropY );

				float newWidth = m_selectedWidth + ( mousePropX - m_selectedXProp ) * 2.0f;
				float newHeight = m_selectedHeight + ( mousePropY - m_selectedYProp ) * 2.0f;

				params->SetWidthPercent( newWidth );
				params->SetHeightPercent( newHeight );

			}

			eventAbsorbed = true;
		}
	}

	if( eventAbsorbed )
		return true;
	// If the event was absorbed, the child should not be given it

	for( std::vector<View*>::iterator it = m_internalViews.begin(); it != m_internalViews.end(); ++it )
	{
		if( ( *it )->HandleEvent( _currentEvent ) )
			return true;
	}
	return eventAbsorbed;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::CustomLayoutElement::SetSelected( bool _value, bool _drag )
{
	m_selected = _value;
	m_dragSelected = false;

	if( _drag )
	{
		m_dragSelected = true;
		m_editDrawable->SetFillColour( 0.0f, 1.0f, 0.0f, 0.3f );
		m_editDrawable->SetBorderColour( 0.0f, 1.0f, 1.0f, 1.0f );
		m_resizeDrawable->SetFillColour( 0.0f, 1.0f, 0.0f, 0.3f );
		m_resizeDrawable->SetBorderColour( 0.0f, 1.0f, 1.0f, 1.0f );
		m_editVisibilityButton->SetVisibility( false );
		m_editWrapSizeButton->SetVisibility( false );
	}
	else if( m_selected )
	{
		m_editDrawable->SetFillColour( 0.0f, 1.0f, 0.0f, 0.3f );
		m_editDrawable->SetBorderColour( 0.0f, 1.0f, 0.0f, 1.0f );
		m_resizeDrawable->SetFillColour( 0.0f, 1.0f, 0.0f, 0.3f );
		m_resizeDrawable->SetBorderColour( 0.0f, 1.0f, 0.0f, 1.0f );
		m_editVisibilityButton->SetVisibility( true );
		m_editWrapSizeButton->SetVisibility( true );
	}
	else
	{
		m_editDrawable->SetFillColour( 0.9f, 0.5f, 0.4f, 0.3f );
		m_editDrawable->SetBorderColour( 0.9f, 0.5f, 0.4f, 1.0f );
		m_resizeDrawable->SetFillColour( 0.9f, 0.5f, 0.4f, 0.3f );
		m_resizeDrawable->SetBorderColour( 0.9f, 0.5f, 0.4f, 1.0f );
		m_editVisibilityButton->SetVisibility( false );
		m_editWrapSizeButton->SetVisibility( false );
	}
	return m_selected;
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayoutElement::LayoutView( View *_currentView, int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	if( _currentView == NULL )
		return;

	int layoutWidth = _right - _left;
	int layoutHeight = _bottom - _top;

	LayoutParams *params = _currentView->GetLayoutParams();
	CustomLayoutParams *viewLayoutParams = dynamic_cast< CustomLayoutParams* >( params );
	if( viewLayoutParams != NULL )
	{
		int childLeft = _left, childRight = _right, childTop = _top, childBottom = _bottom;
		float centreXProportion   = viewLayoutParams->GetCentreX();
		float centreYProportion   = viewLayoutParams->GetCentreY();

		// Determine vertical bounds
		if( viewLayoutParams->GetHeightConst() == LayoutParams::WRAP_CONTENT )
		{
			// Need to figure out the 'natural' size of the View
			int naturalHeight = params->GetWrapHeight();
			if( naturalHeight < layoutHeight )
			{
				childTop = _top + ( int )( ( centreYProportion * layoutHeight ) - ( ( float )naturalHeight / 2.0f ) );
				childBottom = _top + ( int )( ( centreYProportion * layoutHeight ) + ( ( float )naturalHeight / 2.0f ) );
			}
			// If we can't fit this, just give it the maximum size of the layout - not ideal, but it's a bit mad anyway
		}
		else if( viewLayoutParams->GetHeightConst() == LayoutParams::PERCENTAGE_PARENT )
		{
			float percentageSize = viewLayoutParams->GetHeightPercent();
			if( percentageSize > 1.0f )
				percentageSize = 1.0f;
			childTop = _top + ( int )( ( centreYProportion * layoutHeight ) - ( ( layoutHeight * percentageSize ) / 2.0f ) );
			childBottom = _top + ( int )( ( centreYProportion * layoutHeight ) + ( ( layoutHeight * percentageSize ) / 2.0f ) );
		}

		// Determine horizontal bounds
		if( viewLayoutParams->GetWidthConst() == LayoutParams::WRAP_CONTENT )
		{
			// Need to figure out the 'natural' size of the View
			int naturalWidth = params->GetWrapWidth();
			if( naturalWidth < layoutWidth )
			{
				//std::cout << "INFO: CustomLayout: naturalWidth= " << naturalWidth << std::endl;
				childLeft = _left + ( int )( ( centreXProportion * layoutWidth ) - ( ( float )naturalWidth / 2.0f ) );
				childRight = _left + ( int )( ( centreXProportion * layoutWidth ) + ( ( float )naturalWidth / 2.0f ) );
			}
			// If we can't fit this, just give it the maximum size of the layout - not ideal, but it's a bit mad anyway
		}
		else if( viewLayoutParams->GetWidthConst() == LayoutParams::PERCENTAGE_PARENT )
		{
			float percentageSize = viewLayoutParams->GetWidthPercent();
			if( percentageSize > 1.0f )
				percentageSize = 1.0f;
			childLeft = _left + ( int )( (centreXProportion * layoutWidth) - ( ( layoutWidth * percentageSize ) / 2.0f ) );
			childRight = _left + ( int )( (centreXProportion * layoutWidth) + ( ( layoutWidth * percentageSize ) / 2.0f ) );
		}

		_currentView->Layout( childLeft, childTop, childRight, childBottom, _windowWidth, _windowHeight );
	}
}

//----------------------------------------------------------------------------------

bool ShivaGUI::CustomLayoutElement::EventHitResize( InternalEvent *_event )
{
	float left, right, bottom, top;
	m_resizeDrawable->GetBounds( left, top, right, bottom );
	int x, y;
	_event->GetPosition( x, y );
	return ( x > left ) && ( x < right )
			&& ( y > top ) && ( y < bottom );
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayoutElement::GetCurrentSizeProp( float &_currentWidth, float &_currentHeight )
{
	CustomLayoutParams *params = dynamic_cast< CustomLayoutParams* >( GetLayoutParams() );
	if( params == NULL )
	{
		std::cerr << "ERROR: CustomLayoutElement::GetCurrentSizeProp() cannot retrieve layout params" << std::endl;
		_currentWidth = _currentHeight = 0.0f;
		return;
	}

	if( params->GetWidthConst() == LayoutParams::FILL_PARENT )
	{
		// View will be 100% of parent's width
		_currentWidth = 1.0f;
	}
	else if( params->GetWidthConst() == LayoutParams::WRAP_CONTENT )
	{
		int absoluteWidth = m_child->GetWrapWidth();

		_currentWidth = m_parent->GetWidthAsProportion( absoluteWidth );
	}
	else if( params->GetWidthConst() == LayoutParams::PERCENTAGE_PARENT )
	{
		_currentWidth = params->GetWidthPercent();
	}

	if( params->GetHeightConst() == LayoutParams::FILL_PARENT )
	{
		// View will be 100% of parent's height
		_currentHeight = 1.0f;
	}
	else if( params->GetHeightConst() == LayoutParams::WRAP_CONTENT )
	{
		int absoluteHeight = m_child->GetWrapHeight();

		_currentHeight = m_parent->GetHeightAsProportion( absoluteHeight );
	}
	else if( params->GetHeightConst() == LayoutParams::PERCENTAGE_PARENT )
	{
		_currentHeight = params->GetHeightPercent();
	}
}

//----------------------------------------------------------------------------------

ShivaGUI::CustomLayoutElement::CustomElementEventHandler::CustomElementEventHandler( CustomLayoutElement *_parent ) 
{
	m_parent = _parent;
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayoutElement::CustomEventReceived( CustomElementEventHandler *_handler, View *_inputView )
{
//	std::cout<<"CustomLayoutElement::CustomEventReceived"<<std::endl;
	if( _inputView == m_editVisibilityButton )
	{
		m_child->SetVisibility( !m_child->GetVisibility() );
	}
	else if( _inputView == m_editWrapSizeButton )
	{
		GetLayoutParams()->SetHeightConst( LayoutParams::WRAP_CONTENT );
		GetLayoutParams()->SetWidthConst( LayoutParams::WRAP_CONTENT );
	}
}

//----------------------------------------------------------------------------------


