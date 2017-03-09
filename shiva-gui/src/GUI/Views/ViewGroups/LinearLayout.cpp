#include "GUI/Views/ViewGroups/LinearLayout.h"
#include "GUI/Views/ViewGroups/LinearLayoutParams.h"

//----------------------------------------------------------------------------------

ShivaGUI::LinearLayout::LinearLayout()
{
	m_verticalOrientation = true;
}

//----------------------------------------------------------------------------------

ShivaGUI::LinearLayout::~LinearLayout()
{
	for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
	{
		delete *it;
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LinearLayout::NotifyDrawingContextChange( ResourceManager *_resources )
{
	for( std::vector<View*>::iterator it = m_views.begin(); it != m_views.end(); ++it )
	{
		( *it )->NotifyDrawingContextChange( _resources );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LinearLayout::Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	View::Layout( _left, _top, _right, _bottom, _windowWidth, _windowHeight );

	if( m_views.empty() )
		return;

	float totalWeight = 0.0f;
	for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
	{
		LayoutParams *params = ( ( *it )->GetLayoutParams() );
		LinearLayoutParams *viewLayoutParams = dynamic_cast< LinearLayoutParams* >( params );
		if( params != NULL )
		{
			totalWeight += viewLayoutParams->GetWeight();
		}
	}

	// This is not going to be terribly efficient...
	if( m_verticalOrientation )
	{
		float availableHeight = ( float )( _bottom - _top );

		if( totalWeight > 0.0f )
		{
			float unitHeight = availableHeight / totalWeight;
			float viewTop = ( float )_top;
			float viewBottom = 0.0f;
			for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
			{
				LayoutParams *params = ( ( *it )->GetLayoutParams() );
				LinearLayoutParams *viewLayoutParams = dynamic_cast< LinearLayoutParams* >( params );
				if( viewLayoutParams != NULL )
				{
					int paddingLeft   = params->GetPaddingLeft(),
						paddingRight  = params->GetPaddingRight(),
						paddingTop    = params->GetPaddingTop(),
						paddingBottom = params->GetPaddingBottom();

					viewBottom = viewTop + ( unitHeight * viewLayoutParams->GetWeight() );
					( *it )->Layout( _left + paddingLeft, ( int )( viewTop + paddingTop ), _right - paddingRight, ( int )( viewBottom - paddingBottom ), _windowWidth, _windowHeight );
					viewTop = viewBottom;
				}
			}
		}
		else
		{
			// The weights were undefined, so we make them be equal
			float unitHeight = availableHeight / ( ( float )m_views.size() );
			float viewTop = ( float )_top;
			float viewBottom = 0.0f;
			for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
			{
				viewBottom = viewTop + unitHeight;
				(*it)->Layout( _left, ( int )viewTop, _right, ( int )viewBottom, _windowWidth, _windowHeight );
				viewTop = viewBottom;
			}
		}
	}
	else
	{
		// Horizontal orientation
		float availableWidth = ( float )( _right - _left );

		if( totalWeight > 0.0f )
		{
			float unitWidth = availableWidth / totalWeight;
			float viewLeft =  ( float )_left;
			float viewRight = 0.0f;
			for( std::vector<View*>::iterator it = m_views.begin(); it != m_views.end(); ++it )
			{
				LayoutParams *params = ( ( *it )->GetLayoutParams() );
				LinearLayoutParams *viewLayoutParams = dynamic_cast< LinearLayoutParams* >( params );
				if( viewLayoutParams != NULL )
				{
					int paddingLeft   = params->GetPaddingLeft(),
						paddingRight  = params->GetPaddingRight(),
						paddingTop    = params->GetPaddingTop(),
						paddingBottom = params->GetPaddingBottom();

					viewRight = viewLeft + ( unitWidth * viewLayoutParams->GetWeight() );
					( *it )->Layout( ( int )( viewLeft + paddingLeft ), _top + paddingTop, ( int )( viewRight - paddingRight ), _bottom - paddingBottom, _windowWidth, _windowHeight );
					viewLeft = viewRight;
				}
			}
		}
		else
		{
			// The weights were undefined, so we make them be equal
			float unitWidth = availableWidth / ( ( float )m_views.size() );
			float viewLeft = ( float )_left;
			float viewRight = 0.0f;
			for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
			{
				viewRight = viewLeft + unitWidth;
				( *it )->Layout( ( int )viewLeft, _top, ( int )viewRight, _bottom, _windowWidth, _windowHeight );
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

//----------------------------------------------------------------------------------

void ShivaGUI::LinearLayout::Update( float _deltaTs, GUIController *_guiController )
{
	for( std::vector<View*>::iterator it = m_views.begin(); it != m_views.end(); ++it )
	{
		( *it )->Update( _deltaTs, _guiController );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LinearLayout::Draw()
{
	View::Draw();

	if( m_visible )
	{

		for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
		{
			( *it )->Draw();
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LinearLayout::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "LinearLayout_";
	View::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( currentAttribute->Name() == std::string( "orientation" ) )
		{
			std::string orientation( currentAttribute->Value() );
			if( orientation == "horizontal" )
			{
				m_verticalOrientation = false;
			}
			else if( orientation == "vertical" )
			{
				m_verticalOrientation = true;
			}
			else
			{
				std::cerr << "WARNING: LinearLayout::Inflate unknown orientation: " << orientation << std::endl;
			}
		}
	}
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::LinearLayout::Deflate( ResourceManager *_resources )
{
	TiXmlElement *xmlNode = ViewGroup::Deflate( _resources );
	xmlNode->SetValue( "LinearLayout" );


	if( m_verticalOrientation )
		xmlNode->SetAttribute( "orientation", "vertical" );
	else
		xmlNode->SetAttribute( "orientation", "horizontal" );


	// Deflate all children
	if( m_deflateChildren )
	{
		for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
		{
			xmlNode->InsertEndChild( *(*it)->Deflate( _resources ) );
		}
	}

	return xmlNode;
}

//----------------------------------------------------------------------------------

bool ShivaGUI::LinearLayout::HandleEvent( InternalEvent *_currentEvent )
{
	for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
	{
		if( ( *it )->HandleEvent( _currentEvent ) )
			return true;
	}
	return false;
}

//----------------------------------------------------------------------------------

void ShivaGUI::LinearLayout::SetOrientation( bool _vertical )
{
	m_verticalOrientation = _vertical;
	// TODO: re-measure?
}

//----------------------------------------------------------------------------------

void ShivaGUI::LinearLayout::AddView( View *_value, ResourceManager* _resources)
{
	m_views.push_back( _value );
}

//----------------------------------------------------------------------------------

ShivaGUI::LayoutParams* ShivaGUI::LinearLayout::InflateLayoutParams( TiXmlElement *_xmlElement, LayoutParams *_params, std::string _themePrefix )
{
	LinearLayoutParams *typedParams;
	if( _params == NULL )
	{
		typedParams = new LinearLayoutParams();
		_params = typedParams;
	}
	else
	{
		typedParams = dynamic_cast< LinearLayoutParams * >( _params );
		if( typedParams == NULL )
		{
			std::cerr << "WARNING: LinearLayout::InflateLayoutParams given params that are not LinearLayoutParams" << std::endl;
			return NULL;
		}
	}

	// Let the base class fill in the base params
	ViewGroup::InflateLayoutParams( _xmlElement, _params, _themePrefix );

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( ( std::string( "layout_weight" ) == currentAttribute->Name() ) || ( _themePrefix + "layout_weight" == currentAttribute->Name() ) )
		{
			double value = 0.0;
			if( currentAttribute->QueryDoubleValue( &value ) == TIXML_SUCCESS )
			{
				typedParams->SetWeight( ( float )value );
			}
			else
			{
				std::cerr << "WARNING: LinearLayout::InflateLayoutParams attribute layout_weight does not have a value of correct type (double)" << std::endl;
			}
		}
	}

	return _params;
}

//----------------------------------------------------------------------------------

int ShivaGUI::LinearLayout::GetWrapWidth()
{
	if( m_verticalOrientation )
	{
		// return wrap of biggest child
		int biggest = 0;
		for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
		{
			int currentChildValue = ( *it )->GetWrapWidth();
			if( currentChildValue > biggest )
				biggest = currentChildValue;
		}
		return biggest;
	}
	else
	{
		// return total of all children
		int total = 0;
		for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
		{
			total += ( *it )->GetWrapWidth();
		}
		return total;
	}
}

//----------------------------------------------------------------------------------

int ShivaGUI::LinearLayout::GetWrapHeight()
{
	if( !m_verticalOrientation )
	{
		// return wrap of biggest child
		int biggest = 0;
		for( std::vector<View*>::iterator it = m_views.begin(); it != m_views.end(); ++it )
		{
			int currentChildValue = ( *it )->GetWrapHeight();
			if( currentChildValue > biggest )
				biggest = currentChildValue;
		}
		return biggest;
	}
	else
	{
		// return total of all children
		int total = 0;
		for( std::vector< View* >::iterator it = m_views.begin(); it != m_views.end(); ++it )
		{
			total += ( *it )->GetWrapHeight();
		}
		return total;
	}
}

//----------------------------------------------------------------------------------


