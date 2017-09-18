#include "GUI/Views/AdapterViews/ListView.h"
#include "GUI/GUIController.h"

//----------------------------------------------------------------------------------

ShivaGUI::ListView::ListView()
{
	m_initialised = false;
}

//----------------------------------------------------------------------------------

ShivaGUI::ListView::~ListView()
{
	for( std::vector< View* >::iterator it = m_children.begin(); it != m_children.end(); ++it )
	{
		delete ( *it );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::NotifyDrawingContextChange( ResourceManager *_resources )
{
	for( std::vector< View* >::iterator it = m_children.begin(); it != m_children.end(); ++it )
	{
		( *it )->NotifyDrawingContextChange( _resources );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	int lastBottom = _top, n = 0;
	float childSize = ( ( float )_bottom - _top ) / ( ( float )m_children.size() );
	for( std::vector< View* >::iterator it = m_children.begin(); it != m_children.end(); ++it )
	{
		++n;
		( *it )->Layout( _left, lastBottom, _right, ( lastBottom + ( int )childSize ), _windowWidth, _windowHeight );
		lastBottom = _top + ( int )( childSize * n );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::Update( float _deltaTs, GUIController *_guiController )
{
	if( !m_initialised && m_adapter != NULL  )
	{
		int numChildren = m_adapter->GetNumEntries();
		for( int i = 0; i < numChildren; ++i )
		{
			m_children.push_back( m_adapter->GetView( i, _guiController->GetResources() ) );
		}

		RefreshConnectionLinks();

		m_initialised = true;
		_guiController->Layout();
	}

	for( std::vector< View* >::iterator it = m_children.begin(); it != m_children.end(); ++it )
	{
		( *it )->Update( _deltaTs, _guiController );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::Draw()
{
	for( std::vector< View* >::iterator it = m_children.begin(); it != m_children.end(); ++it )
	{
		( *it )->Draw();
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "ListView_";
	View::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::ListView::Deflate( ResourceManager *_resources )
{
	TiXmlElement *xmlNode = View::Deflate( _resources );
	xmlNode->SetValue( "ListView" );

	// Children don't get deflated because they are pulled from the DataAdapter

	return xmlNode;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::SetFocus( bool _value )
{
	// We give focus to our first child
	if( m_children.size() > 0 )
	{
		m_children.front()->SetFocus( _value );
	}
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ListView::HandleEvent( InternalEvent *_event )
{
	bool absorbed = false;
	for( std::vector< View* >::iterator it = m_children.begin(); it != m_children.end(); ++it )
		absorbed = absorbed || ( *it )->HandleEvent( _event );
	return absorbed;
}

//----------------------------------------------------------------------------------

int ShivaGUI::ListView::GetWrapWidth()
{
	int maxWidth = 0;
	for( std::vector< View* >::iterator it = m_children.begin(); it != m_children.end(); ++it )
	{
		int currentWidth = ( *it )->GetWrapWidth();
		if( currentWidth > maxWidth )
			currentWidth = maxWidth;
	}
	return maxWidth;
}

//----------------------------------------------------------------------------------

int ShivaGUI::ListView::GetWrapHeight()
{
	int totalHeight = 0;
	for( std::vector< View* >::iterator it = m_children.begin(); it != m_children.end(); ++it )
	{
		totalHeight += ( *it )->GetWrapHeight();
	}
	return totalHeight;
}

//----------------------------------------------------------------------------------

int ShivaGUI::ListView::GetDataIndex( View *_value )
{
	unsigned int i = 0;
	for( std::vector< View* >::iterator it = m_children.begin(); it != m_children.end(); ++it )
	{
		if( ( *it ) == _value )
			return i;
		++i;
	}
	return -1;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::RefreshFromSource()
{
	for( std::vector< View* >::iterator it = m_children.begin(); it != m_children.end(); ++it )
		delete ( *it );
	m_children.clear();

	if( m_prevScan != NULL && m_nextScan != NULL )
	{
		// Remove ourselves from the scan chain
		m_prevScan->SetNextScan( m_nextScan, true );
		m_nextScan->SetNextScan( m_prevScan, false );
	}

	m_initialised = false;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::RefreshConnectionLinks()
{	
	if( m_children.size() > 0 )
	{
		if( m_prevScan != NULL && m_nextScan != NULL )
		{
#ifdef _DEBUG
			std::cout << "INFO: ListView::RefreshConnectionLinks() ready to change scan links" << std::endl;
#endif			
			// First child links to what we've been told is the previous scan item
			// Children link to each other
			View *previous = m_prevScan, *current = NULL;
			for( std::vector< View* >::iterator it = m_children.begin(); it != m_children.end(); ++it )
			{
				current = ( *it );

				previous->SetNextScan( current, true );
				current->SetNextScan( previous, false );

				if( previous != m_prevScan )
				{
					// Only linking children here: _prevScan is not necessarily the previous focus
					previous->SetNextFocus( current, Definitions::Down );
					current->SetNextFocus( previous, Definitions::Up );
				}

				previous = current;
			}

			current->SetNextScan( m_nextScan, true );
			m_nextScan->SetNextScan( current, false );
			
			if( m_isFirstScan ) // _children.front() is safe because _children.size() > 0
			{
				m_children.front()->SetIsFirstScan( true );
			}
		}

		// Focus
		// Left/right will just link all children the same:
		View *focusLeft = GetNextFocus( Definitions::Left );
		View *focusRight = GetNextFocus( Definitions::Right );
		View *focusUp = GetNextFocus( Definitions::Up );
		View *focusDown = GetNextFocus( Definitions::Down );
		for( std::vector< View* >::iterator it = m_children.begin(); it != m_children.end(); ++it )
		{
			if( focusLeft != this )
			{
				( *it )->SetNextFocus( focusLeft, Definitions::Left );
			}
			if( focusRight != this )
			{
				( *it )->SetNextFocus( focusRight, Definitions::Right );
			}
		}


		if( focusUp != NULL && focusUp != this )
		{
			m_children.front()->SetNextFocus( focusUp, Definitions::Up );
		}
		else
		{
			m_children.front()->SetNextFocus( m_children.back(), Definitions::Up );
		}


		if( focusDown != NULL && focusDown != this )
		{
			m_children.back()->SetNextFocus( focusDown, Definitions::Down );
		}
		else
		{
			m_children.back()->SetNextFocus( m_children.front(), Definitions::Down );
		}

		if( m_isFirstFocus )
		{
			m_children.front()->SetIsFirstFocus( true );
		}
	}	
}

//----------------------------------------------------------------------------------
