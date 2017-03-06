#include "GUI/Views/AdapterViews/LayoutAdapterView.h"
#include "GUI/GUIController.h"

//----------------------------------------------------------------------------------

ShivaGUI::LayoutAdapterView::LayoutAdapterView()
{
	m_initialised = false;
	m_containerGroup = NULL;
}

//----------------------------------------------------------------------------------

ShivaGUI::LayoutAdapterView::~LayoutAdapterView()
{
	if( m_containerGroup != NULL )
	{
		// If we have a containergroup, let that delete the children
		delete m_containerGroup;
	}
	else
	{
		for( std::vector< View* >::iterator it = m_children.begin(); it != m_children.end(); ++it )
			delete ( *it );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayoutAdapterView::NotifyDrawingContextChange( ResourceManager *_resources )
{
	if( m_containerGroup != NULL )
	{
		m_containerGroup->NotifyDrawingContextChange( _resources );
	}
	else
	{
	for( std::vector<View*>::iterator it = m_children.begin(); it != m_children.end(); ++it )
		( *it )->NotifyDrawingContextChange( _resources );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayoutAdapterView::Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	if( m_containerGroup != NULL )
	{
		m_containerGroup->Layout( _left, _top, _right, _bottom, _windowWidth, _windowHeight );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayoutAdapterView::Update( float _deltaTs, GUIController *_guiController )
{
	if( !m_initialised && m_adapter != NULL  )
	{
		int numChildren = m_adapter->GetNumEntries();
		for( int i = 0; i < numChildren; ++i )
		{
			View *currentChild = m_adapter->GetView( i, _guiController->GetResources() );
			if( currentChild != NULL )
			{
				m_children.push_back( currentChild );
				if( m_containerGroup != NULL )
				{
					m_containerGroup->AddView( currentChild, _guiController->GetResources() );
				}
			}
		}

		RefreshConnectionLinks();

		m_initialised = true;
		_guiController->Layout();
	}

	if( m_containerGroup != NULL )
	{
		m_containerGroup->Update( _deltaTs, _guiController );
	}
	else
	{
		for( std::vector< View* >::iterator it = m_children.begin(); it != m_children.end(); ++it )
			( *it )->Update( _deltaTs, _guiController );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayoutAdapterView::Draw()
{
	if( m_containerGroup != NULL )
	{
		m_containerGroup->Draw();
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayoutAdapterView::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "LayoutAdapterView_";
	View::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );

	// First time through this function the xmlElement will be the theme node
	// Second time through it will be the layout node
	if( _xmlElement->Value() != "theme" )
	{
		// Look for a child - this will be the container group
		TiXmlNode *childNode = _xmlElement->FirstChild();
		if( childNode->Type() == TiXmlNode::TINYXML_ELEMENT )
		{
			View *typeView = _resources->CreateView( childNode->Value() );

			if( typeView != NULL )
			{
				ViewGroup *groupView = dynamic_cast< ViewGroup* >( typeView );

				if( groupView != NULL )
				{
					// Loads settings for ViewGroup from the xml
					groupView->Inflate( childNode->ToElement(), _resources, "" );

					// Children are added procedurally, not in layout xml, so don't deflate to xml
					groupView->SetDeflateChildren( false );

					m_containerGroup = groupView;
				}
				else
				{
					std::cerr << "WARNING: LayoutAdapterView::Inflate given child node with type " << childNode->Value() << " which is not a recognised ViewGroup" << std::endl;
					delete typeView;
				}
			}
		}
	}

	/*
	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( (std::string("groupType") == currentAttribute->Name()) )
		{
			std::string typeName( currentAttribute->Value() );

			View *typeView = resources->CreateView( typeName );

			ViewGroup *groupView = dynamic_cast<ViewGroup*>(groupView);

			if( groupView != NULL )
			{
				_groupTypeString = typeName;
				_containerGroup = groupView;
			}
			else
			{
				std::cerr<<"WARNING: LayoutAdapterView::Inflate given groupType of "<< typeName<<" which is not a recognised ViewGroup"<<std::endl;
				delete typeView;
			}
		}
	}
	*/
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::LayoutAdapterView::Deflate( ResourceManager *_resources )
{
	TiXmlElement *xmlNode = View::Deflate( _resources );
	xmlNode->SetValue( "LayoutAdapterView" );

	if( m_containerGroup != NULL )
	{
		xmlNode->InsertEndChild( *( m_containerGroup->Deflate( _resources ) ) );
	}

	// Children don't get deflated because they are pulled from the DataAdapter

	return xmlNode;
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayoutAdapterView::SetFocus( bool _value )
{
	// We give focus to our first child
	if( m_children.size() > 0 )
	{
		m_children.front()->SetFocus( _value );
	}
}

//----------------------------------------------------------------------------------

bool ShivaGUI::LayoutAdapterView::HandleEvent( InternalEvent *_event )
{
	if( m_containerGroup != NULL )
	{
		return m_containerGroup->HandleEvent( _event );
	}
	else
	{
		bool absorbed = false;
		for( std::vector< View* >::iterator it = m_children.begin(); it != m_children.end(); ++it )
			absorbed = absorbed || ( *it )->HandleEvent( _event );
		return absorbed;
	}
}

//----------------------------------------------------------------------------------

int ShivaGUI::LayoutAdapterView::GetWrapWidth()
{
	if( m_containerGroup != NULL )
	{
		return m_containerGroup->GetWrapWidth();
	}

	return 10;
}

//----------------------------------------------------------------------------------

int ShivaGUI::LayoutAdapterView::GetWrapHeight()
{
	if( m_containerGroup != NULL )
	{
		return m_containerGroup->GetWrapHeight();
	}

	return 10;
}

//----------------------------------------------------------------------------------

int ShivaGUI::LayoutAdapterView::GetDataIndex( View *_value )
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

void ShivaGUI::LayoutAdapterView::RefreshFromSource()
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

void ShivaGUI::LayoutAdapterView::RefreshConnectionLinks()
{
	
	if( m_children.size() > 0 )
	{
		if( m_prevScan != NULL && m_nextScan != NULL )
		{
			std::cout << "INFO: ListView::RefreshConnectionLinks() ready to change scan links" << std::endl;
			
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