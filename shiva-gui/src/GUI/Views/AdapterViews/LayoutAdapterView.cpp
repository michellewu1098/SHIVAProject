#include "GUI/Views/AdapterViews/LayoutAdapterView.h"
#include "GUI/GUIController.h"

//----------------------------------------------------------------------------------

ShivaGUI::LayoutAdapterView::LayoutAdapterView()
{
	_initialised = false;
	_containerGroup = NULL;
}

//----------------------------------------------------------------------------------

ShivaGUI::LayoutAdapterView::~LayoutAdapterView()
{
	if( _containerGroup != NULL )
	{
		// If we have a containergroup, let that delete the children
		delete _containerGroup;
	}
	else
	{
		for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
			delete ( *it );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayoutAdapterView::NotifyDrawingContextChange( ResourceManager *resources )
{
	if( _containerGroup != NULL )
	{
		_containerGroup->NotifyDrawingContextChange( resources );
	}
	else
	{
	for( std::vector<View*>::iterator it = _children.begin(); it != _children.end(); ++it )
		( *it )->NotifyDrawingContextChange( resources );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayoutAdapterView::Layout( int left, int top, int right, int bottom, int windowWidth, int windowHeight )
{
	if( _containerGroup != NULL )
	{
		_containerGroup->Layout( left, top, right, bottom, windowWidth, windowHeight );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayoutAdapterView::Update( float deltaTs, GUIController *guiController )
{
	if( !_initialised && _adapter != NULL  )
	{
		int numChildren = _adapter->GetNumEntries();
		for( int i = 0; i < numChildren; ++i )
		{
			View *currentChild = _adapter->GetView( i, guiController->GetResources() );
			if( currentChild != NULL )
			{
				_children.push_back( currentChild );
				if( _containerGroup != NULL )
				{
					_containerGroup->AddView( currentChild, guiController->GetResources() );
				}
			}
		}

		RefreshConnectionLinks();

		_initialised = true;
		guiController->Layout();
	}

	if( _containerGroup != NULL )
	{
		_containerGroup->Update( deltaTs, guiController );
	}
	else
	{
		for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
			( *it )->Update( deltaTs, guiController );
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayoutAdapterView::Draw()
{
	if( _containerGroup != NULL )
	{
		_containerGroup->Draw();
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayoutAdapterView::Inflate( TiXmlElement *xmlElement, ResourceManager *resources, std::string themePrefix, bool rootNode )
{
	if( themePrefix.empty() )
		themePrefix = "LayoutAdapterView_";
	View::Inflate( xmlElement, resources, themePrefix, rootNode );

	// First time through this function the xmlElement will be the theme node
	// Second time through it will be the layout node
	if( xmlElement->Value() != "theme" )
	{
		// Look for a child - this will be the container group
		TiXmlNode *childNode = xmlElement->FirstChild();
		if( childNode->Type() == TiXmlNode::TINYXML_ELEMENT )
		{
			View *typeView = resources->CreateView( childNode->Value() );

			if( typeView != NULL )
			{
				ViewGroup *groupView = dynamic_cast< ViewGroup* >( typeView );

				if( groupView != NULL )
				{
					// Loads settings for ViewGroup from the xml
					groupView->Inflate( childNode->ToElement(), resources, "" );

					// Children are added procedurally, not in layout xml, so don't deflate to xml
					groupView->SetDeflateChildren( false );

					_containerGroup = groupView;
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

TiXmlElement* ShivaGUI::LayoutAdapterView::Deflate( ResourceManager *resources )
{
	TiXmlElement *xmlNode = View::Deflate( resources );
	xmlNode->SetValue( "LayoutAdapterView" );

	if( _containerGroup != NULL )
	{
		xmlNode->InsertEndChild( *( _containerGroup->Deflate( resources ) ) );
	}

	// Children don't get deflated because they are pulled from the DataAdapter

	return xmlNode;
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayoutAdapterView::SetFocus( bool value )
{
	// We give focus to our first child
	if( _children.size() > 0 )
	{
		_children.front()->SetFocus( value );
	}
}

//----------------------------------------------------------------------------------

bool ShivaGUI::LayoutAdapterView::HandleEvent( InternalEvent *event )
{
	if( _containerGroup != NULL )
	{
		return _containerGroup->HandleEvent( event );
	}
	else
	{
		bool absorbed = false;
		for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
			absorbed = absorbed || ( *it )->HandleEvent( event );
		return absorbed;
	}
}

//----------------------------------------------------------------------------------

int ShivaGUI::LayoutAdapterView::GetWrapWidth()
{
	if( _containerGroup != NULL )
	{
		return _containerGroup->GetWrapWidth();
	}

	return 10;
}

//----------------------------------------------------------------------------------

int ShivaGUI::LayoutAdapterView::GetWrapHeight()
{
	if( _containerGroup != NULL )
	{
		return _containerGroup->GetWrapHeight();
	}

	return 10;
}

//----------------------------------------------------------------------------------

int ShivaGUI::LayoutAdapterView::GetDataIndex( View *value )
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

void ShivaGUI::LayoutAdapterView::RefreshFromSource()
{
	for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
		delete ( *it );
	_children.clear();

	if( _prevScan != NULL && _nextScan != NULL )
	{
		// Remove ourselves from the scan chain
		_prevScan->SetNextScan( _nextScan, true );
		_nextScan->SetNextScan( _prevScan, false );
	}

	_initialised = false;
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayoutAdapterView::RefreshConnectionLinks()
{
	
	if( _children.size() > 0 )
	{
		if( _prevScan != NULL && _nextScan != NULL )
		{
			std::cout << "INFO: ListView::RefreshConnectionLinks() ready to change scan links" << std::endl;
			
			// First child links to what we've been told is the previous scan item
			// Children link to each other
			View *previous = _prevScan, *current = NULL;
			for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
			{
				current = ( *it );

				previous->SetNextScan( current, true );
				current->SetNextScan( previous, false );

				if( previous != _prevScan )
				{
					// Only linking children here: _prevScan is not necessarily the previous focus
					previous->SetNextFocus( current, Definitions::FocusDirection::Down );
					current->SetNextFocus( previous, Definitions::FocusDirection::Up );
				}

				previous = current;
			}

			current->SetNextScan( _nextScan, true );
			_nextScan->SetNextScan( current, false );
			
			if( _isFirstScan ) // _children.front() is safe because _children.size() > 0
			{
				_children.front()->SetIsFirstScan( true );
			}
		}

		// Focus
		// Left/right will just link all children the same:
		View *focusLeft = GetNextFocus( Definitions::FocusDirection::Left );
		View *focusRight = GetNextFocus( Definitions::FocusDirection::Right );
		View *focusUp = GetNextFocus( Definitions::FocusDirection::Up );
		View *focusDown = GetNextFocus( Definitions::FocusDirection::Down );
		for( std::vector< View* >::iterator it = _children.begin(); it != _children.end(); ++it )
		{
			if( focusLeft != this )
			{
				( *it )->SetNextFocus( focusLeft, Definitions::FocusDirection::Left );
			}
			if( focusRight != this )
			{
				( *it )->SetNextFocus( focusRight, Definitions::FocusDirection::Right );
			}
		}

		if( focusUp != NULL && focusUp != this )
		{
			_children.front()->SetNextFocus( focusUp, Definitions::FocusDirection::Up );
		}
		else
		{
			_children.front()->SetNextFocus( _children.back(), Definitions::FocusDirection::Up );
		}

		if( focusDown != NULL && focusDown != this )
		{
			_children.back()->SetNextFocus( focusDown, Definitions::FocusDirection::Down );
		}
		else
		{
			_children.back()->SetNextFocus( _children.front(), Definitions::FocusDirection::Down );
		}

		if( _isFirstFocus )
		{
			_children.front()->SetIsFirstFocus( true );
		}
	}
}

//----------------------------------------------------------------------------------