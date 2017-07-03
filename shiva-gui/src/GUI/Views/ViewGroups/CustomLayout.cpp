#include "GUI/Views/ViewGroups/CustomLayout.h"
#include "GUI/Views/ViewGroups/CustomLayoutParams.h"
#include "GUI/Views/ViewGroups/CustomLayoutElement.h"
#include "GUI/GUIController.h"

//----------------------------------------------------------------------------------

ShivaGUI::CustomLayout::CustomLayout()
{
	m_editDrawable = new RectDrawable();
	m_editDrawable->SetFillColour( 1.0f, 0.0f, 0.0f, 0.3f );
	m_editDrawable->SetBorderColour( 1.0f, 0.0f, 0.0f, 1.0f );
	m_editMode = false;
	m_editSelect = false;

	m_editExitButton = NULL;
	m_editSaveButton = NULL;
	m_handlerExitButton = new CustomLayoutEventHandler( this );
	m_handlerSaveButton = new CustomLayoutEventHandler( this );

	m_needLayout = false;
	m_needSave = false;

	m_useGridSnap = true;
	m_gridSnapSize = 20;
}

//----------------------------------------------------------------------------------

ShivaGUI::CustomLayout::~CustomLayout()
{
	for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
	{
		delete *it;
	}
	for( std::vector< View* >::iterator it2 = m_internalViews.begin(); it2 != m_internalViews.end(); ++it2 )
	{
		delete *it2;
	}
	delete m_editDrawable;
	delete m_editExitButton;
	delete m_editSaveButton;
	delete m_handlerExitButton;
	delete m_handlerSaveButton;
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayout::NotifyDrawingContextChange( ResourceManager *_resources )
{
	for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
	{
		( *it )->NotifyDrawingContextChange( _resources );
	}

	for( std::vector< View* >::iterator it = m_internalViews.begin(); it != m_internalViews.end(); ++it )
	{
		( *it )->NotifyDrawingContextChange( _resources );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayout::Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	View::Layout( _left, _top, _right, _bottom, _windowWidth, _windowHeight );
	
	if( m_editDrawable != NULL )
	{
		m_editDrawable->SetBounds( ( float )_left, ( float )_top, ( float )_right, ( float )_bottom, Definitions::CENTRE );
	}
	
	for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
	{
		LayoutView( *it, _left, _top, _right, _bottom, _windowWidth, _windowHeight );
	}

	for( std::vector< View* >::iterator it = m_internalViews.begin(); it != m_internalViews.end(); ++it )
	{
		LayoutView( *it, _left, _top, _right, _bottom, _windowWidth, _windowHeight );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayout::CreateBBoxVAOs( unsigned int _context )
{
	for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
	{
		( *it )->CreateBBoxVAOs( _context );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayout::Update( float _deltaTs, GUIController *_guiController )
{
	if( m_needLayout )
	{
		_guiController->Layout();
		m_needLayout = false;
	}
	if( m_needSave )
	{
		_guiController->SaveLayoutToProfile();
		m_needSave = false;
	}

	if( m_editMode )
	{
		for( std::vector< View* >::iterator it = m_internalViews.begin(); it != m_internalViews.end(); ++it )
		{
			( *it )->Update( _deltaTs, _guiController );
		}
	}
	else
	{
		for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
		{
			( *it )->Update( _deltaTs, _guiController );
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayout::Draw( unsigned int _context )
{
	View::Draw();

	if( m_visible )
	{
		if( m_editMode )
		{
			m_editDrawable->Draw();
		}


		if( m_editMode )
		{
			for( std::vector< View* >::iterator it = m_internalViews.begin(); it != m_internalViews.end(); ++it )
			{
				( *it )->Draw( _context );
			}
		}
		else
		{
			for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
			{
				( *it )->Draw( _context );
			}
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayout::Draw()
{
	View::Draw();

	if( m_visible )
	{
		if( m_editMode )
		{
			m_editDrawable->Draw();
		}


		if( m_editMode )
		{
			for( std::vector< View* >::iterator it = m_internalViews.begin(); it != m_internalViews.end(); ++it )
			{
				( *it )->Draw();
			}
		}
		else
		{
			for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
			{
				( *it )->Draw();
			}
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayout::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "CustomLayout_";
	View::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );


	if( m_editExitButton == NULL )
	{
		m_editExitButton = new TextButton();
		// MEMORY LEAK?
		CustomLayoutParams *exitButtonParams = new CustomLayoutParams();
		exitButtonParams->SetWidthConst( LayoutParams::WRAP_CONTENT );
		exitButtonParams->SetHeightConst( LayoutParams::WRAP_CONTENT );
		exitButtonParams->SetCentreX( 0.3f );
		exitButtonParams->SetCentreY( 0.88f );
		m_editExitButton->SetText( "Leave edit mode", _resources );
		m_editExitButton->Inflate( _xmlElement, _resources, "", false );
		m_editExitButton->SetLayoutParams( exitButtonParams );		// Must be done last, as it gets the size here
		m_editExitButton->SetOnClickListener( m_handlerExitButton );
		m_internalViews.push_back( m_editExitButton );
	}
	else
		m_editExitButton->Inflate( _xmlElement, _resources, "", false );

	if( m_editSaveButton == NULL )
	{
		m_editSaveButton = new TextButton();
		// MEMORY LEAK?
		CustomLayoutParams *saveButtonParams = new CustomLayoutParams();
		saveButtonParams->SetWidthConst( LayoutParams::WRAP_CONTENT );
		saveButtonParams->SetHeightConst( LayoutParams::WRAP_CONTENT );
		saveButtonParams->SetCentreX( 0.3f );
		saveButtonParams->SetCentreY( 0.95f );
		m_editSaveButton->SetText( "Save new layout", _resources );
		m_editSaveButton->Inflate( _xmlElement, _resources, "", false );
		m_editSaveButton->SetLayoutParams( saveButtonParams );		// Must be done last, as it gets the size here
		m_editSaveButton->SetOnClickListener( m_handlerSaveButton );
		m_internalViews.push_back( m_editSaveButton );
	}
	else
		m_editSaveButton->Inflate( _xmlElement, _resources, "", false );


//	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
//	{
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
//	}
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::CustomLayout::Deflate( ResourceManager *_resources )
{
	TiXmlElement *xmlNode = ViewGroup::Deflate( _resources );
	xmlNode->SetValue( "CustomLayout" );

	// Currently nothing to add

	// Deflate all children
	for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
	{
		xmlNode->InsertEndChild( *( *it )->Deflate( _resources ) );
	}

	return xmlNode;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::CustomLayout::HandleEvent( InternalEvent *_currentEvent )
{
	if( _currentEvent != NULL && ( _currentEvent->GetType() == InternalEvent::EDIT_CUSTOMLAYOUT ) )
	{
		// User has requested to edit custom layouts
		m_editSelect = true;
		SetEditMode( false );
	}
	else if( _currentEvent != NULL && m_editSelect && ( _currentEvent->GetType() == InternalEvent::POSITIONAL_SELECT ) )
	{
		m_editSelect = false;
		SetEditMode( EventHit( _currentEvent ) );
		if( m_editMode )
			return true;
	}

	bool eventAbsorbed = false;
	if( m_editMode )
	{
		for( std::vector< View* >::iterator it = m_internalViews.begin(); it != m_internalViews.end(); ++it )
		{
			if( ( *it )->HandleEvent( _currentEvent ) )
			{
				// If one of our internal child Views has absorbed an event, as they are all about rearranging the layout,
				// it's very likely that we will need to Layout() again
				m_needLayout = true;
				eventAbsorbed = true;
			}
		}
	}
	else
	{
		for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
		{
			if( ( *it )->HandleEvent( _currentEvent ) )
				return true; //eventAbsorbed = true;
		}
	}
	return eventAbsorbed;
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayout::AddView( View *_value, ResourceManager *_resources )
{
	m_views.push_back( _value );

	// MEMORY LEAK? Nope, it should get deleted once we delete m_internalViews
	CustomLayoutElement *container = new CustomLayoutElement( _value, _resources, this );
	m_internalViews.push_back( container );
	m_needLayout = true;
}

//----------------------------------------------------------------------------------

ShivaGUI::LayoutParams* ShivaGUI::CustomLayout::InflateLayoutParams( TiXmlElement *_xmlElement, LayoutParams *_params, std::string _themePrefix )
{
	CustomLayoutParams *typedParams;
	if( _params == NULL )
	{
		// MEMORY LEAK?
		typedParams = new CustomLayoutParams();
		_params = typedParams;
	}
	else
	{
		typedParams = dynamic_cast< CustomLayoutParams* >( _params );
		if( typedParams == NULL )
		{
			std::cerr << "WARNING: CustomLayout::InflateLayoutParams given params that are not CustomLayoutParams" << std::endl;
			return NULL;
		}
	}

	// Let the base class fill in the base params
	ViewGroup::InflateLayoutParams( _xmlElement, _params, _themePrefix );

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{

		if( ( std::string( "layout_centreX" ) == currentAttribute->Name()) || ( _themePrefix + "layout_centreX" == currentAttribute->Name() ) )
		{
			double value = 0.0;
			if( currentAttribute->QueryDoubleValue( &value ) == TIXML_SUCCESS )
			{
				typedParams->SetCentreX( ( float )value );
			}
			else
			{
				std::cerr << "WARNING: CustomLayout::InflateLayoutParams attribute layout_centreX does not have a value of correct type (double)" << std::endl;
			}
		}
		else if( ( std::string( "layout_centreY" ) == currentAttribute->Name() ) || ( _themePrefix + "layout_centreY" == currentAttribute->Name() ) )
		{
			double value = 0.0;
			if( currentAttribute->QueryDoubleValue( &value ) == TIXML_SUCCESS )
			{
				typedParams->SetCentreY( ( float )value );
			}
			else
			{
				std::cerr << "WARNING: CustomLayout::InflateLayoutParams attribute layout_centreY does not have a value of correct type (double)" << std::endl;
			}
		}
	}

	return _params;
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayout::GetCoordsAsProportion( int _inX, int _inY, float &_outX, float &_outY )
{
	float viewWidth = ( float )( m_layoutBoundsRight - m_layoutBoundsLeft );
	float viewHeight = ( float )( m_layoutBoundsBottom - m_layoutBoundsTop );

	_outX = ( ( float )_inX - m_layoutBoundsLeft ) / viewWidth;
	_outY = ( ( float )_inY - m_layoutBoundsTop ) / viewHeight;
}

//----------------------------------------------------------------------------------

float ShivaGUI::CustomLayout::GetWidthAsProportion( int _widthIn )
{
	float viewWidth = ( float )( m_layoutBoundsRight - m_layoutBoundsLeft );
	return ( float )_widthIn / viewWidth;
}

//----------------------------------------------------------------------------------

float ShivaGUI::CustomLayout::GetHeightAsProportion( int _heightIn )
{
	float viewHeight = ( float )( m_layoutBoundsBottom - m_layoutBoundsTop );
	return ( float )_heightIn / viewHeight;
}

//----------------------------------------------------------------------------------

int ShivaGUI::CustomLayout::GridSnapPosition( int _value )
{
	if( m_useGridSnap )
	{
		if( ( float )( _value % m_gridSnapSize ) > ( float )( m_gridSnapSize / 2.0f ) )
			_value = _value - ( _value % m_gridSnapSize ) + m_gridSnapSize;
		else
			_value = _value - ( _value % m_gridSnapSize );
	}
	return _value;
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayout::SetEditMode( bool _value )
{
	m_editMode = _value;
/*
	if( _editExitButton != NULL )
		_editExitButton->SetVisibility(_editMode);

	if( _editSaveButton != NULL )
		_editSaveButton->SetVisibility(_editMode);
*/
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayout::LayoutView( View *_currentView, int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
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

			int naturalHeight = params->GetWrapHeight();
			
			// Not ideal as well, should find a better way to do this
			if( std::abs( childTop - childBottom ) < naturalHeight && !viewLayoutParams->IsHeightConstrained() )
			{
				childTop = _top + ( int )( ( centreYProportion * layoutHeight ) - ( ( float )naturalHeight / 2.0f ) );
				childBottom = _top + ( int )( ( centreYProportion * layoutHeight ) + ( ( float )naturalHeight / 2.0f ) );
			}
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
				//std::cout << "INFO: CustomLayout: naturalWidth=" << naturalWidth << std::endl;
				childLeft = _left + ( int )( ( centreXProportion * layoutWidth ) - ( ( float )naturalWidth / 2.0f ) );
				childRight = _left + ( int)( ( centreXProportion * layoutWidth ) + ( ( float )naturalWidth / 2.0f ) );
			}
			// If we can't fit this, just give it the maximum size of the layout - not ideal, but it's a bit mad anyway
		}
		else if( viewLayoutParams->GetWidthConst() == LayoutParams::PERCENTAGE_PARENT )
		{
			float percentageSize = viewLayoutParams->GetWidthPercent();
			if( percentageSize > 1.0f )
				percentageSize = 1.0f;
			childLeft = _left + ( int )( ( centreXProportion * layoutWidth ) - ( ( layoutWidth * percentageSize ) / 2.0f ) );
			childRight = _left + ( int )( ( centreXProportion * layoutWidth ) + ( ( layoutWidth * percentageSize ) / 2.0f ) );

			//int naturalWidth = params->GetWrapWidth();

			// Not ideal as well, should find a better way to do this
			//if( std::abs( childTop - childBottom ) < naturalWidth && !viewLayoutParams->IsWidthConstrained() )
			//{
			//	childLeft = _left + ( int )( ( centreXProportion * layoutWidth ) - ( ( float )naturalWidth / 2.0f ) );
			//	childRight = _left + ( int )( ( centreXProportion * layoutWidth ) - ( ( float )naturalWidth / 2.0f ) );
			//}
		}

		_currentView->Layout( childLeft, childTop, childRight, childBottom, _windowWidth, _windowHeight );
	}
	else
		std::cout << "WARNING: CustomLayout has child with no layout params" << std::endl;
}

//----------------------------------------------------------------------------------

ShivaGUI::CustomLayout::CustomLayoutEventHandler::CustomLayoutEventHandler( CustomLayout *_parent ) 
{ 
	m_parent = _parent;
}

//----------------------------------------------------------------------------------

void ShivaGUI::CustomLayout::CustomEventReceived( CustomLayoutEventHandler *_handler, View *_inputView )
{
	if( _handler == m_handlerExitButton )
	{
		SetEditMode( false );
	}
	else if( _handler == m_handlerSaveButton )
	{
		m_needSave = true;
	}
}

//----------------------------------------------------------------------------------

