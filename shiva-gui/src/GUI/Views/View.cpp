#include "GUI/Views/View.h"
#include "GUI/Drawables/Drawable.h"
#include "GUIManager.h"

//////////////////////////////////////////////////////////////////////////
#include <iostream>

ShivaGUI::View::View()
{
	_background = NULL;
	_layoutParams = NULL;
	_nextFocus = new View*[4];
	for( unsigned int i = 0; i < 4; i++ )
		_nextFocus[i] = NULL;
	_visible = true;
	_active = true;
	_internalGravity = Definitions::CENTRE;
	_isFirstFocus = false;
	_isFirstScan = false;
	_nextScan = _prevScan = NULL;
	_layoutBoundsLeft = _layoutBoundsTop = _layoutBoundsRight = _layoutBoundsBottom = 0;

	_fromTheme_bgd = false;
}

ShivaGUI::View::~View()
{
	delete [] _nextFocus;
	delete _background;
	delete _layoutParams;
}

void ShivaGUI::View::Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight)
{
	_layoutBoundsLeft = left;
	_layoutBoundsTop = top;
	_layoutBoundsRight = right;
	_layoutBoundsBottom = bottom;
	_layoutWindowWidth = windowWidth;
	_layoutWindowHeight = windowHeight;
	if( _background != NULL )
		_background->SetBounds(left,top,right,bottom,_internalGravity);
}

ShivaGUI::View* ShivaGUI::View::GetNextFocus(Definitions::FocusDirection dir)
{
	View *next = _nextFocus[dir];
	if( next == NULL )
		return this;
	if( next->GetVisibility() && next->GetActive() )
		return next;
	else
	{
		View *oneAfter = next->GetNextFocus(dir);
		if( oneAfter != next )
			return oneAfter;
	}
	return this;
}

void ShivaGUI::View::SetNextFocus(View *value, Definitions::FocusDirection dir)
{
	_nextFocus[dir] = value;
	OnSetNextFocus(value,dir);
}

ShivaGUI::View* ShivaGUI::View::GetNextScan(bool forward)
{
	View *next = NULL;
	if( forward )
		next = _nextScan;
	else
		next = _prevScan;
	if( next != NULL )
	{
		if( next->GetVisibility() && next->GetActive() )
			return next;
		else
		{
			View *oneAfter = next->GetNextScan(forward);
			if( oneAfter != next )
				return oneAfter;
		}
	}
	return this;
}

void ShivaGUI::View::SetNextScan(View *value, bool forward)
{
	if( forward )
		_nextScan = value;
	else
		_prevScan = value;

	OnSetNextScan(value,forward);
}

void ShivaGUI::View::Inflate( TiXmlElement *xmlElement, ShivaGUI::ResourceManager *resources, std::string themePrefix, bool rootNode )
{
	if( themePrefix.empty() )
		themePrefix = "View_";

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( std::string("id") == currentAttribute->Name() )
		{
			std::string ID( currentAttribute->Value() );

			if( !ID.empty() )
			{
				_ID = ID;
				resources->RegisterViewID(this,ID);
			}
		}
		else if( (std::string("background") == currentAttribute->Name())
				|| (themePrefix+"background" == currentAttribute->Name())
				|| (rootNode && (std::string("RootNode_background") == currentAttribute->Name())) )
		{
			std::string resourceName( currentAttribute->Value() );

			_fromTheme_bgd = (themePrefix+"background" == currentAttribute->Name()) || (rootNode && (std::string("RootNode_background") == currentAttribute->Name()));
			_background = resources->GetDrawable( resourceName );
		}
		else if( std::string("first_focus") == currentAttribute->Name() )
		{
			_isFirstFocus = true;
		}
		else if( std::string("next_focus_left") == currentAttribute->Name() )
		{
			std::string ID( currentAttribute->Value() );

			if( !ID.empty() )
			{
				View *nextView = resources->GetViewFromID(ID);
				if( nextView != NULL )
				{
					SetNextFocus(nextView,Definitions::Left);
					nextView->SetNextFocus(this,Definitions::Right);
				}
				else
				{
					//std::cerr<<"WARNING: GUISystem::View::Inflate  next_focus_left, no resource exists named: "<<ID<<std::endl;
					resources->RegisterPostInflationLink(this,ID,Definitions::Left);
				}
			}
		}
		else if( std::string("next_focus_right") == currentAttribute->Name() )
		{
			std::string ID( currentAttribute->Value() );

			if( !ID.empty() )
			{
				View *nextView = resources->GetViewFromID(ID);
				if( nextView != NULL )
				{
					SetNextFocus(nextView,Definitions::Right);
					nextView->SetNextFocus(this,Definitions::Left);
				}
				else
				{
					//std::cerr<<"WARNING: GUISystem::View::Inflate  next_focus_right, no resource exists named: "<<ID<<std::endl;
					resources->RegisterPostInflationLink(this,ID,Definitions::Right);
				}
			}
		}
		else if( std::string("next_focus_up") == currentAttribute->Name() )
		{
			std::string ID( currentAttribute->Value() );

			if( !ID.empty() )
			{
				View *nextView = resources->GetViewFromID(ID);
				if( nextView != NULL )
				{
					SetNextFocus(nextView,Definitions::Up);
					nextView->SetNextFocus(this,Definitions::Down);
				}
				else
				{
					//std::cerr<<"WARNING: GUISystem::View::Inflate  next_focus_up, no resource exists named: "<<ID<<std::endl;
					resources->RegisterPostInflationLink(this,ID,Definitions::Up);
				}
			}
		}
		else if( std::string("next_focus_down") == currentAttribute->Name() )
		{
			std::string ID( currentAttribute->Value() );

			if( !ID.empty() )
			{
				View *nextView = resources->GetViewFromID(ID);
				if( nextView != NULL )
				{
					SetNextFocus(nextView,Definitions::Down);
					nextView->SetNextFocus(this,Definitions::Up);
				}
				else
				{
					//std::cerr<<"WARNING: GUISystem::View::Inflate  next_focus_down, no resource exists named: "<<ID<<std::endl;
					resources->RegisterPostInflationLink(this,ID,Definitions::Down);
				}
			}
		}
		else if( std::string("first_scan") == currentAttribute->Name() )
		{
			_isFirstScan = true;
		}
		else if( std::string("scan_next") == currentAttribute->Name() )
		{
			std::string ID( currentAttribute->Value() );

			if( !ID.empty() )
			{
				View *nextView = resources->GetViewFromID(ID);
				if( nextView != NULL )
				{
					SetNextScan(nextView,true);
					nextView->SetNextScan(this,false);
				}
				else
				{
					//std::cerr<<"WARNING: GUISystem::View::Inflate  scan_next, no resource exists named: "<<ID<<std::endl;
					resources->RegisterPostInflationLink(this,ID,true);
				}
			}
		}
		else if( std::string("scan_prev") == currentAttribute->Name() )
		{
			std::string ID( currentAttribute->Value() );

			if( !ID.empty() )
			{
				View *nextView = resources->GetViewFromID(ID);
				if( nextView != NULL )
				{
					SetNextScan(nextView,false);
					nextView->SetNextScan(this,true);
				}
				else
				{
					//std::cerr<<"WARNING: GUISystem::View::Inflate  scan_prev, no resource exists named: "<<ID<<std::endl;
					resources->RegisterPostInflationLink(this,ID,false);
				}
			}
		}
		else if( std::string("gravity") == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value.find("BOTTOM") != std::string::npos )
			{
				_internalGravity = (_internalGravity & Definitions::HORIZONTAL_MASK) | Definitions::BOTTOM;
			}
			if( value.find("TOP") != std::string::npos )
			{
				_internalGravity = (_internalGravity & Definitions::HORIZONTAL_MASK) | Definitions::TOP;
			}
			if( value.find("CENTRE") != std::string::npos )
			{
				_internalGravity = Definitions::CENTRE;
			}
			if( value.find("CENTRE_VERTICAL") != std::string::npos )
			{
				_internalGravity = (_internalGravity & Definitions::HORIZONTAL_MASK) | Definitions::CENTRE_VERTICAL;
			}
			if( value.find("CENTRE_HORIZONTAL") != std::string::npos )
			{
				_internalGravity = (_internalGravity & Definitions::VERTICAL_MASK) | Definitions::CENTRE_HORIZONTAL;
			}
			if( value.find("LEFT") != std::string::npos )
			{
				_internalGravity = (_internalGravity & Definitions::VERTICAL_MASK) | Definitions::LEFT;
			}
			if( value.find("RIGHT") != std::string::npos )
			{
				_internalGravity = (_internalGravity & Definitions::VERTICAL_MASK) | Definitions::RIGHT;
			}
		}
		else if( (std::string("visibility") == currentAttribute->Name()) || (themePrefix+"visibility" == currentAttribute->Name()) )
		{
			std::string value( currentAttribute->Value() );

			if( value == "false" )
			{
				SetVisibility(false);
			}
			else
			{
				SetVisibility(true);
			}
		}
	}
}

TiXmlElement* ShivaGUI::View::Deflate(ResourceManager *resources)
{
	TiXmlElement *xmlNode = new TiXmlElement("View");

	if( !_ID.empty() )
		xmlNode->SetAttribute("id",_ID);
	if( _background != NULL && !_fromTheme_bgd )
		xmlNode->SetAttribute("background",_background->GetFilename() );

	if( _isFirstFocus )
		xmlNode->SetAttribute("first_focus",1);
	if( GetNextFocus( Definitions::Left ) != NULL && GetNextFocus( Definitions::Left ) != this )
		xmlNode->SetAttribute("next_focus_left", GetNextFocus( Definitions::Left )->GetID() );
	if( GetNextFocus( Definitions::Right ) != NULL && GetNextFocus( Definitions::Right ) != this )
		xmlNode->SetAttribute("next_focus_right",GetNextFocus( Definitions::Right )->GetID() );
	if( GetNextFocus( Definitions::Up ) != NULL && GetNextFocus( Definitions::Up ) != this )
		xmlNode->SetAttribute("next_focus_up",GetNextFocus( Definitions::Up )->GetID() );
	if( GetNextFocus( Definitions::Down ) != NULL && GetNextFocus( Definitions::Down ) != this )
		xmlNode->SetAttribute("next_focus_down",GetNextFocus( Definitions::Down )->GetID() );

	if( _isFirstScan )
		xmlNode->SetAttribute("first_scan",1);
	if( GetNextScan( true ) != NULL && GetNextScan( true ) != this )
		xmlNode->SetAttribute("scan_next", GetNextScan( true )->GetID() );
	if( GetNextScan( false ) != NULL && GetNextScan( false ) != this )
		xmlNode->SetAttribute("scan_prev",GetNextScan( false )->GetID() );

	if( _internalGravity != Definitions::CENTRE )
	{
		if( _internalGravity == ((_internalGravity & Definitions::HORIZONTAL_MASK) | Definitions::BOTTOM) )
			xmlNode->SetAttribute("gravity","BOTTOM");
		if( _internalGravity == ((_internalGravity & Definitions::HORIZONTAL_MASK) | Definitions::TOP) )
			xmlNode->SetAttribute("gravity","TOP");

		if( _internalGravity == ((_internalGravity & Definitions::HORIZONTAL_MASK) | Definitions::CENTRE_VERTICAL) )
			xmlNode->SetAttribute("gravity","CENTRE_VERTICAL");
		if( _internalGravity == ((_internalGravity & Definitions::VERTICAL_MASK) | Definitions::CENTRE_HORIZONTAL) )
			xmlNode->SetAttribute("gravity","CENTRE_HORIZONTAL");

		if( _internalGravity == ((_internalGravity & Definitions::VERTICAL_MASK) | Definitions::LEFT) )
			xmlNode->SetAttribute("gravity","LEFT");
		if( _internalGravity == ((_internalGravity & Definitions::VERTICAL_MASK) | Definitions::RIGHT) )
			xmlNode->SetAttribute("gravity","RIGHT");
	}

	if( !GetVisibility() )
		xmlNode->SetAttribute("visibility","false");

	if( GetLayoutParams() != NULL )
		GetLayoutParams()->Deflate(xmlNode,resources);

	return xmlNode;
}

void ShivaGUI::View::SetLayoutParams(LayoutParams *value)
{
	if( value != NULL )
	{
		value->SetWrapHeight( GetWrapHeight() );
		value->SetWrapWidth( GetWrapWidth() );
	}
	_layoutParams = value;
}

void ShivaGUI::View::Draw()
{
	if( _visible && _background != NULL )
		_background->Draw();
}

bool ShivaGUI::View::EventHit(InternalEvent *event)
{
	int x, y;
	event->GetPosition(x,y);
	return (x > _layoutBoundsLeft) && (x < _layoutBoundsRight)
			&& (y > _layoutBoundsTop) && (y < _layoutBoundsBottom);
}
