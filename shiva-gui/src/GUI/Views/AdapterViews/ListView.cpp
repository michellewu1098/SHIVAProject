#include "GUI/Views/AdapterViews/ListView.h"
#include "GUI/GUIController.h"

//----------------------------------------------------------------------------------

ShivaGUI::ListView::ListView()
{
	_initialised = false;
}

//----------------------------------------------------------------------------------

ShivaGUI::ListView::~ListView()
{
	for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
	{
		delete ( *it );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::NotifyDrawingContextChange( ResourceManager *resources )
{
	for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
	{
		( *it )->NotifyDrawingContextChange( resources );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::Layout( int left, int top, int right, int bottom, int windowWidth, int windowHeight )
{
	int lastBottom = top, n = 0;
	float childSize = ( ( float )bottom - top ) / ( ( float )_children.size() );
	for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
	{
		++n;
		( *it )->Layout( left, lastBottom, right, ( lastBottom + ( int )childSize ), windowWidth, windowHeight );
		lastBottom = top + ( int )( childSize * n );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::Update( float deltaTs, GUIController *guiController )
{
	if( !_initialised && _adapter != NULL  )
	{
		int numChildren = _adapter->GetNumEntries();
		for( int i = 0; i < numChildren; ++i )
		{
			_children.push_back( _adapter->GetView( i, guiController->GetResources() ) );
		}

		RefreshConnectionLinks();

		_initialised = true;
		guiController->Layout();
	}

	for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
	{
		( *it )->Update( deltaTs, guiController );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::Draw()
{
	for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
	{
		( *it )->Draw();
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::Inflate( TiXmlElement *xmlElement, ResourceManager *resources, std::string themePrefix, bool rootNode )
{
	if( themePrefix.empty() )
		themePrefix = "ListView_";
	View::Inflate( xmlElement, resources, themePrefix, rootNode );
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::ListView::Deflate( ResourceManager *resources )
{
	TiXmlElement *xmlNode = View::Deflate( resources );
	xmlNode->SetValue( "ListView" );

	// Children don't get deflated because they are pulled from the DataAdapter

	return xmlNode;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::SetFocus( bool value )
{
	// We give focus to our first child
	if( _children.size() > 0 )
	{
		_children.front()->SetFocus( value );
	}
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ListView::HandleEvent( InternalEvent *event )
{
	bool absorbed = false;
	for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
		absorbed = absorbed || ( *it )->HandleEvent( event );
	return absorbed;
}

//----------------------------------------------------------------------------------

int ShivaGUI::ListView::GetWrapWidth()
{
	int maxWidth = 0;
	for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
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
	for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
	{
		totalHeight += ( *it )->GetWrapHeight();
	}
	return totalHeight;
}

//----------------------------------------------------------------------------------

int ShivaGUI::ListView::GetDataIndex( View *value )
{
	unsigned int i = 0;
	for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
	{
		if( ( *it ) == value )
			return i;
		++i;
	}
	return -1;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::RefreshFromSource()
{
	for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
		delete ( *it );
	_children.clear();

	if( m_prevScan != NULL && m_nextScan != NULL )
	{
		// Remove ourselves from the scan chain
		m_prevScan->SetNextScan( m_nextScan, true );
		m_nextScan->SetNextScan( m_prevScan, false );
	}

	_initialised = false;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ListView::RefreshConnectionLinks()
{	
	if( _children.size() > 0 )
	{
		if( m_prevScan != NULL && m_nextScan != NULL )
		{
			std::cout << "INFO: ListView::RefreshConnectionLinks() ready to change scan links" << std::endl;
			
			// First child links to what we've been told is the previous scan item
			// Children link to each other
			View *previous = m_prevScan, *current = NULL;
			for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
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
				_children.front()->SetIsFirstScan( true );
			}
		}

		// Focus
		// Left/right will just link all children the same:
		View *focusLeft = GetNextFocus( Definitions::Left );
		View *focusRight = GetNextFocus( Definitions::Right );
		View *focusUp = GetNextFocus( Definitions::Up );
		View *focusDown = GetNextFocus( Definitions::Down );
		for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
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
			_children.front()->SetNextFocus( focusUp, Definitions::Up );
		}
		else
		{
			_children.front()->SetNextFocus( _children.back(), Definitions::Up );
		}


		if( focusDown != NULL && focusDown != this )
		{
			_children.back()->SetNextFocus( focusDown, Definitions::Down );
		}
		else
		{
			_children.back()->SetNextFocus( _children.front(), Definitions::Down );
		}

		if( m_isFirstFocus )
		{
			_children.front()->SetIsFirstFocus( true );
		}
	}	
}

//----------------------------------------------------------------------------------
