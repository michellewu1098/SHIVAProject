
#include "GUI/Views/AdapterViews/ListView.h"
//////////////////////////////////////////////////////////////////////////

#include "GUI/GUIController.h"

ShivaGUI::ListView::ListView()
{
	_initialised = false;
}

ShivaGUI::ListView::~ListView()
{
	for( std::vector<View*>::iterator it = _children.begin(); it != _children.end(); ++it )
		delete (*it);
}

void ShivaGUI::ListView::NotifyDrawingContextChange(ResourceManager *resources)
{
	for( std::vector<View*>::iterator it = _children.begin(); it != _children.end(); ++it )
		(*it)->NotifyDrawingContextChange(resources);
}

void ShivaGUI::ListView::Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight)
{
	int lastBottom = top, n = 0;
	float childSize = ((float)bottom-top)/((float)_children.size());
	for( std::vector<View*>::iterator it = _children.begin(); it != _children.end(); ++it )
	{
		++n;
		(*it)->Layout(left,lastBottom,right,lastBottom+childSize,windowWidth,windowHeight);
		lastBottom = top + (childSize*n);
	}
}

void ShivaGUI::ListView::Update(float deltaTs, GUIController *guiController)
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

	for( std::vector<View*>::iterator it = _children.begin(); it != _children.end(); ++it )
		(*it)->Update(deltaTs,guiController);
}

void ShivaGUI::ListView::Draw()
{
	for( std::vector<View*>::iterator it = _children.begin(); it != _children.end(); ++it )
		(*it)->Draw();
}

void ShivaGUI::ListView::Inflate(TiXmlElement *xmlElement, ResourceManager *resources, std::string themePrefix, bool rootNode)
{
	if( themePrefix.empty() )
		themePrefix = "ListView_";
	View::Inflate(xmlElement,resources,themePrefix,rootNode);
}

TiXmlElement* ShivaGUI::ListView::Deflate(ResourceManager *resources)
{
	TiXmlElement *xmlNode = View::Deflate(resources);
	xmlNode->SetValue("ListView");

	// Children don't get deflated because they are pulled from the DataAdapter

	return xmlNode;
}

void ShivaGUI::ListView::SetFocus(bool value)
{
	// We give focus to our first child
	if( _children.size() > 0 )
	{
		_children.front()->SetFocus(value);
	}
}

bool ShivaGUI::ListView::HandleEvent(InternalEvent *event)
{
	bool absorbed = false;
	for( std::vector<View*>::iterator it = _children.begin(); it != _children.end(); ++it )
		absorbed = absorbed || (*it)->HandleEvent(event);
	return absorbed;
}

int ShivaGUI::ListView::GetWrapWidth()
{
	int maxWidth = 0;
	for( std::vector<View*>::iterator it = _children.begin(); it != _children.end(); ++it )
	{
		int currentWidth = (*it)->GetWrapWidth();
		if( currentWidth > maxWidth )
			currentWidth = maxWidth;
	}
	return maxWidth;
}

int ShivaGUI::ListView::GetWrapHeight()
{
	int totalHeight = 0;
	for( std::vector<View*>::iterator it = _children.begin(); it != _children.end(); ++it )
	{
		totalHeight += (*it)->GetWrapHeight();
	}
	return totalHeight;
}

int ShivaGUI::ListView::GetDataIndex( View *value )
{
	unsigned int i = 0;
	for( std::vector<View*>::iterator it = _children.begin(); it != _children.end(); ++it )
	{
		if( (*it) == value )
			return i;
		++i;
	}
	return -1;
}

void ShivaGUI::ListView::RefreshFromSource()
{
	for( std::vector<View*>::iterator it = _children.begin(); it != _children.end(); ++it )
		delete (*it);
	_children.clear();

	if( _prevScan != NULL && _nextScan != NULL )
	{
		// Remove ourselves from the scan chain
		_prevScan->SetNextScan(_nextScan,true);
		_nextScan->SetNextScan(_prevScan,false);
	}

	_initialised = false;
}

void ShivaGUI::ListView::RefreshConnectionLinks()
{
	
	if( _children.size() > 0 )
	{
		if( _prevScan != NULL && _nextScan != NULL )
		{
			std::cout<<"INFO: ListView::RefreshConnectionLinks() ready to change scan links"<<std::endl;
			
			// First child links to what we've been told is the previous scan item
			// Children link to each other
			View *previous = _prevScan, *current = NULL;
			for( std::vector<View*>::iterator it = _children.begin(); it != _children.end(); ++it )
			{
				current = (*it);

				previous->SetNextScan(current,true);
				current->SetNextScan(previous,false);

				if( previous != _prevScan )
				{
					// Only linking children here: _prevScan is not necessarily the previous focus
					previous->SetNextFocus(current,Definitions::FocusDirection::Down);
					current->SetNextFocus(previous,Definitions::FocusDirection::Up);
				}

				previous = current;
			}

			current->SetNextScan(_nextScan,true);
			_nextScan->SetNextScan(current,false);
			
			if( _isFirstScan ) // _children.front() is safe because _children.size() > 0
			{
				_children.front()->SetIsFirstScan(true);
			}
		}


		// Focus
		// Left/right will just link all children the same:
		View *focusLeft = GetNextFocus(Definitions::FocusDirection::Left);
		View *focusRight = GetNextFocus(Definitions::FocusDirection::Right);
		View *focusUp = GetNextFocus(Definitions::FocusDirection::Up);
		View *focusDown = GetNextFocus(Definitions::FocusDirection::Down);
		for( std::vector<View*>::iterator it = _children.begin(); it != _children.end(); ++it )
		{
			if( focusLeft != this )
			{
				(*it)->SetNextFocus(focusLeft,Definitions::FocusDirection::Left);
			}
			if( focusRight != this )
			{
				(*it)->SetNextFocus(focusRight,Definitions::FocusDirection::Right);
			}
		}


		if( focusUp != NULL && focusUp != this )
		{
			_children.front()->SetNextFocus(focusUp,Definitions::FocusDirection::Up);
		}
		else
		{
			_children.front()->SetNextFocus(_children.back(),Definitions::FocusDirection::Up);
		}


		if( focusDown != NULL && focusDown != this )
		{
			_children.back()->SetNextFocus(focusDown,Definitions::FocusDirection::Down);
		}
		else
		{
			_children.back()->SetNextFocus(_children.front(),Definitions::FocusDirection::Down);
		}

		if( _isFirstFocus )
		{
			_children.front()->SetIsFirstFocus(true);
		}
	}
	
}
