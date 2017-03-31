#include "GUI/Views/View.h"
#include "GUI/Drawables/Drawable.h"
#include "GUIManager.h"

//----------------------------------------------------------------------------------

ShivaGUI::View::View()
{
	m_background = NULL;
	m_layoutParams = NULL;
	m_nextFocus = new View*[ 4 ];
	for( unsigned int i = 0; i < 4; i++ )
	{
		m_nextFocus[ i ] = NULL;
	}

	m_visible = true;
	m_active = true;
	m_internalGravity = Definitions::CENTRE;
	m_isFirstFocus = false;
	m_isFirstScan = false;
	m_nextScan = m_prevScan = NULL;
	m_layoutBoundsLeft = m_layoutBoundsTop = m_layoutBoundsRight = m_layoutBoundsBottom = 0;
	m_fromTheme_bgd = false;
}

//----------------------------------------------------------------------------------

ShivaGUI::View::~View()
{
	delete [] m_nextFocus;
	delete m_background;
	delete m_layoutParams;
}

//----------------------------------------------------------------------------------

void ShivaGUI::View::Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	m_layoutBoundsLeft = _left;
	m_layoutBoundsTop = _top;
	m_layoutBoundsRight = _right;
	m_layoutBoundsBottom = _bottom;
	m_layoutWindowWidth = _windowWidth;
	m_layoutWindowHeight = _windowHeight;

	if( m_background != NULL )
		m_background->SetBounds( ( float )_left, ( float )_top, ( float )_right, ( float )_bottom, m_internalGravity );
}

//----------------------------------------------------------------------------------

ShivaGUI::View* ShivaGUI::View::GetNextFocus( Definitions::FocusDirection _dir )
{
	View *next = m_nextFocus[ _dir ];
	if( next == NULL )
		return this;
	if( next->GetVisibility() && next->GetActive() )
		return next;
	else
	{
		View *oneAfter = next->GetNextFocus( _dir );
		if( oneAfter != next )
			return oneAfter;
	}
	return this;
}

//----------------------------------------------------------------------------------

void ShivaGUI::View::SetNextFocus( View *_value, Definitions::FocusDirection _dir )
{
	m_nextFocus[ _dir ] = _value;
	OnSetNextFocus( _value, _dir );
}

//----------------------------------------------------------------------------------

ShivaGUI::View* ShivaGUI::View::GetNextScan( bool _forward )
{
	View *next = NULL;
	if( _forward )
		next = m_nextScan;
	else
		next = m_prevScan;
	if( next != NULL )
	{
		if( next->GetVisibility() && next->GetActive() )
			return next;
		else
		{
			View *oneAfter = next->GetNextScan( _forward );
			if( oneAfter != next )
				return oneAfter;
		}
	}
	return this;
}

//----------------------------------------------------------------------------------

void ShivaGUI::View::SetNextScan( View *_value, bool _forward )
{
	if( _forward )
		m_nextScan = _value;
	else
		m_prevScan = _value;

	OnSetNextScan( _value, _forward );
}

//----------------------------------------------------------------------------------

void ShivaGUI::View::Inflate( TiXmlElement *_xmlElement, ShivaGUI::ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "View_";

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( std::string( "id" ) == currentAttribute->Name() )
		{
			std::string ID( currentAttribute->Value() );

			if( !ID.empty() )
			{
				m_id = ID;
				_resources->RegisterViewID( this, ID );
			}
		}
		else if( ( std::string( "background" ) == currentAttribute->Name() )
				|| ( _themePrefix + "background" == currentAttribute->Name() )
				|| ( _rootNode && ( std::string( "RootNode_background" ) == currentAttribute->Name() ) ) )
		{
			std::string resourceName( currentAttribute->Value() );

			m_fromTheme_bgd = ( _themePrefix + "background" == currentAttribute->Name()) || ( _rootNode && (std::string( "RootNode_background" ) == currentAttribute->Name() ) );
			m_background = _resources->GetDrawable( resourceName );
		}
		else if( std::string( "first_focus" ) == currentAttribute->Name() )
		{
			m_isFirstFocus = true;
		}
		else if( std::string( "next_focus_left" ) == currentAttribute->Name() )
		{
			std::string ID( currentAttribute->Value() );

			if( !ID.empty() )
			{
				View *nextView = _resources->GetViewFromID( ID );
				if( nextView != NULL )
				{
					SetNextFocus( nextView, Definitions::Left );
					nextView->SetNextFocus( this, Definitions::Right );
				}
				else
				{
					//std::cerr<<"WARNING: GUISystem::View::Inflate  next_focus_left, no resource exists named: "<<ID<<std::endl;
					_resources->RegisterPostInflationLink( this, ID, Definitions::Left );
				}
			}
		}
		else if( std::string( "next_focus_right" ) == currentAttribute->Name() )
		{
			std::string ID( currentAttribute->Value() );

			if( !ID.empty() )
			{
				View *nextView = _resources->GetViewFromID( ID );
				if( nextView != NULL )
				{
					SetNextFocus( nextView, Definitions::Right );
					nextView->SetNextFocus( this, Definitions::Left );
				}
				else
				{
					//std::cerr<<"WARNING: GUISystem::View::Inflate  next_focus_right, no resource exists named: "<<ID<<std::endl;
					_resources->RegisterPostInflationLink( this, ID, Definitions::Right );
				}
			}
		}
		else if( std::string( "next_focus_up" ) == currentAttribute->Name() )
		{
			std::string ID( currentAttribute->Value() );

			if( !ID.empty() )
			{
				View *nextView = _resources->GetViewFromID( ID );
				if( nextView != NULL )
				{
					SetNextFocus( nextView, Definitions::Up );
					nextView->SetNextFocus( this, Definitions::Down );
				}
				else
				{
					//std::cerr<<"WARNING: GUISystem::View::Inflate  next_focus_up, no resource exists named: "<<ID<<std::endl;
					_resources->RegisterPostInflationLink( this, ID, Definitions::Up );
				}
			}
		}
		else if( std::string( "next_focus_down" ) == currentAttribute->Name() )
		{
			std::string ID( currentAttribute->Value() );

			if( !ID.empty() )
			{
				View *nextView = _resources->GetViewFromID( ID );
				if( nextView != NULL )
				{
					SetNextFocus( nextView, Definitions::Down );
					nextView->SetNextFocus( this, Definitions::Up );
				}
				else
				{
					//std::cerr<<"WARNING: GUISystem::View::Inflate  next_focus_down, no resource exists named: "<<ID<<std::endl;
					_resources->RegisterPostInflationLink( this, ID, Definitions::Down );
				}
			}
		}
		else if( std::string( "first_scan" ) == currentAttribute->Name() )
		{
			m_isFirstScan = true;
		}
		else if( std::string( "scan_next" ) == currentAttribute->Name() )
		{
			std::string ID( currentAttribute->Value() );

			if( !ID.empty() )
			{
				View *nextView = _resources->GetViewFromID( ID );
				if( nextView != NULL )
				{
					SetNextScan( nextView, true );
					nextView->SetNextScan( this, false );
				}
				else
				{
					//std::cerr<<"WARNING: GUISystem::View::Inflate  scan_next, no resource exists named: "<<ID<<std::endl;
					_resources->RegisterPostInflationLink( this, ID, true );
				}
			}
		}
		else if( std::string( "scan_prev" ) == currentAttribute->Name() )
		{
			std::string ID( currentAttribute->Value() );

			if( !ID.empty() )
			{
				View *nextView = _resources->GetViewFromID( ID );
				if( nextView != NULL )
				{
					SetNextScan( nextView, false );
					nextView->SetNextScan( this, true );
				}
				else
				{
					//std::cerr<<"WARNING: GUISystem::View::Inflate  scan_prev, no resource exists named: "<<ID<<std::endl;
					_resources->RegisterPostInflationLink( this, ID, false );
				}
			}
		}
		else if( std::string( "gravity" ) == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value.find( "BOTTOM" ) != std::string::npos )
			{
				m_internalGravity = ( m_internalGravity & Definitions::HORIZONTAL_MASK ) | Definitions::BOTTOM;
			}
			if( value.find( "TOP" ) != std::string::npos )
			{
				m_internalGravity = ( m_internalGravity & Definitions::HORIZONTAL_MASK ) | Definitions::TOP;
			}
			if( value.find( "CENTRE" ) != std::string::npos )
			{
				m_internalGravity = Definitions::CENTRE;
			}
			if( value.find( "CENTRE_VERTICAL" ) != std::string::npos )
			{
				m_internalGravity = ( m_internalGravity & Definitions::HORIZONTAL_MASK ) | Definitions::CENTRE_VERTICAL;
			}
			if( value.find( "CENTRE_HORIZONTAL" ) != std::string::npos )
			{
				m_internalGravity = ( m_internalGravity & Definitions::VERTICAL_MASK ) | Definitions::CENTRE_HORIZONTAL;
			}
			if( value.find( "LEFT" ) != std::string::npos )
			{
				m_internalGravity = ( m_internalGravity & Definitions::VERTICAL_MASK ) | Definitions::LEFT;
			}
			if( value.find( "RIGHT" ) != std::string::npos )
			{
				m_internalGravity = ( m_internalGravity & Definitions::VERTICAL_MASK ) | Definitions::RIGHT;
			}
		}
		else if( ( std::string( "visibility" ) == currentAttribute->Name() ) || ( _themePrefix + "visibility" == currentAttribute->Name() ) )
		{
			std::string value( currentAttribute->Value() );

			if( value == "false" )
			{
				SetVisibility( false );
			}
			else
			{
				SetVisibility( true );
			}
		}
	}
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::View::Deflate( ResourceManager *_resources )
{
	TiXmlElement *xmlNode = new TiXmlElement( "View" );

	if( !m_id.empty() )
		xmlNode->SetAttribute( "id", m_id );
	if( m_background != NULL && !m_fromTheme_bgd )
		xmlNode->SetAttribute( "background", m_background->GetFilename() );

	if( m_isFirstFocus ) {
		xmlNode->SetAttribute( "first_focus", 1 );
	}
	if( GetNextFocus( Definitions::Left ) != NULL && GetNextFocus( Definitions::Left ) != this ) {
		xmlNode->SetAttribute( "next_focus_left", GetNextFocus( Definitions::Left )->GetID() );
	}
	if( GetNextFocus( Definitions::Right ) != NULL && GetNextFocus( Definitions::Right ) != this ) {
		xmlNode->SetAttribute( "next_focus_right", GetNextFocus( Definitions::Right )->GetID() );
	}
	if( GetNextFocus( Definitions::Up ) != NULL && GetNextFocus( Definitions::Up ) != this ) {
		xmlNode->SetAttribute( "next_focus_up", GetNextFocus( Definitions::Up )->GetID() );
	}
	if( GetNextFocus( Definitions::Down ) != NULL && GetNextFocus( Definitions::Down ) != this ) {
		xmlNode->SetAttribute( "next_focus_down", GetNextFocus( Definitions::Down )->GetID() );
	}

	if( m_isFirstScan ) {
		xmlNode->SetAttribute( "first_scan", 1 );
	}
	if( GetNextScan( true ) != NULL && GetNextScan( true ) != this ) {
		xmlNode->SetAttribute( "scan_next", GetNextScan( true )->GetID() ); 
	}
	if( GetNextScan( false ) != NULL && GetNextScan( false ) != this ) {
		xmlNode->SetAttribute( "scan_prev", GetNextScan( false )->GetID() );
	}

	if( m_internalGravity != Definitions::CENTRE )
	{
		if( m_internalGravity == ( ( m_internalGravity & Definitions::HORIZONTAL_MASK) | Definitions::BOTTOM ) ) {
			xmlNode->SetAttribute( "gravity", "BOTTOM" );
		}

		if( m_internalGravity == ( ( m_internalGravity & Definitions::HORIZONTAL_MASK) | Definitions::TOP ) ) {
			xmlNode->SetAttribute( "gravity", "TOP" );
		}

		if( m_internalGravity == ( ( m_internalGravity & Definitions::HORIZONTAL_MASK ) | Definitions::CENTRE_VERTICAL ) ) {
			xmlNode->SetAttribute( "gravity","CENTRE_VERTICAL" );
		}

		if( m_internalGravity == ( ( m_internalGravity & Definitions::VERTICAL_MASK ) | Definitions::CENTRE_HORIZONTAL ) ) {
			xmlNode->SetAttribute( "gravity", "CENTRE_HORIZONTAL" );
		}

		if( m_internalGravity == ( ( m_internalGravity & Definitions::VERTICAL_MASK ) | Definitions::LEFT) ) {
			xmlNode->SetAttribute( "gravity", "LEFT" );
		}

		if( m_internalGravity == ( ( m_internalGravity & Definitions::VERTICAL_MASK) | Definitions::RIGHT) ) {
			xmlNode->SetAttribute( "gravity", "RIGHT" );
		}
	}

	if( !GetVisibility() )
		xmlNode->SetAttribute( "visibility", "false" );

	if( GetLayoutParams() != NULL )
		GetLayoutParams()->Deflate( xmlNode, _resources );

	return xmlNode;
}

//----------------------------------------------------------------------------------

void ShivaGUI::View::SetLayoutParams( LayoutParams *_value )
{
	if( _value != NULL )
	{
		_value->SetWrapHeight( GetWrapHeight() );
		_value->SetWrapWidth( GetWrapWidth() );
	}
	m_layoutParams = _value;
}

//----------------------------------------------------------------------------------

void ShivaGUI::View::Draw()
{
	if( m_visible && m_background != NULL )
		m_background->Draw();
}

//----------------------------------------------------------------------------------

void ShivaGUI::View::Draw( unsigned int _context )
{
	if( m_visible && m_background != NULL )
		m_background->Draw();
}

//----------------------------------------------------------------------------------

bool ShivaGUI::View::EventHit( InternalEvent *_event )
{
	int x, y;
	_event->GetPosition( x, y );
	return ( x > m_layoutBoundsLeft ) && ( x < m_layoutBoundsRight )
			&& ( y > m_layoutBoundsTop ) && ( y < m_layoutBoundsBottom );
}

//----------------------------------------------------------------------------------