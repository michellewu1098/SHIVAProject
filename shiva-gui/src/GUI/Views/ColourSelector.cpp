#include "GUI/Views/ColourSelector.h"
#include "GUIManager.h"

//////////////////////////////////////////////////////////////////////////
#include <GL/GLee.h>
#include <iostream>


ShivaGUI::ColourSelector::ColourSelector()
{
	_selectorBoundsLeft = _selectorBoundsRight = _selectorBoundsTop = _selectorBoundsBottom = 0;
	_sampleBoundsLeft = _sampleBoundsRight = _sampleBoundsTop = _sampleBoundsBottom = 0;

	_colourSelectorProgram = new Utility::GPUProgram();
	_colourSelectorProgram->Create("Resources/Shaders/ColourSelector",Utility::GPUProgram::FRAGMENT);

	_sampleR = 1.0f;
	_sampleG = 1.0f;
	_sampleB = 1.0f;

	_holdingHandle = false;

	_handleHalfSize = 10.0f;
	_handlePropPosX = 0.5f;
	_handlePropPosY = 1.0f;

	_handleBoundsLeft = _handleBoundsRight = _handleBoundsTop = _handleBoundsBottom = 0.0f;

	_selectorDrawable = NULL;
	_selectorStateListDrawable = NULL;

	_colourListener = NULL;

	HSLtoRGB( _handlePropPosX,1.0f,_handlePropPosY, _sampleR,_sampleG,_sampleB );
	SetHandleActualPos();
}

ShivaGUI::ColourSelector::~ColourSelector()
{
	delete _colourSelectorProgram;
}

void ShivaGUI::ColourSelector::Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight)
{
	_selectorBoundsLeft = left + _handleHalfSize;
	_selectorBoundsRight = right - _handleHalfSize;
	_selectorBoundsTop = top + 30;
	_selectorBoundsBottom = bottom - _handleHalfSize;

	_sampleBoundsLeft = left + _handleHalfSize;
	_sampleBoundsRight = right - _handleHalfSize;
	_sampleBoundsTop = top;
	_sampleBoundsBottom = top + 25;

	SetHandleActualPos();
}

void ShivaGUI::ColourSelector::Draw()
{
	// Bind Shaders
	_colourSelectorProgram->On();

		glColor3f(1.0f,1.0f,1.0f);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f,0.0f);	glVertex2f(_selectorBoundsLeft, _selectorBoundsTop);
			glTexCoord2f(1.0f,0.0f);	glVertex2f(_selectorBoundsRight, _selectorBoundsTop);
			glTexCoord2f(1.0f,1.0f);	glVertex2f(_selectorBoundsRight, _selectorBoundsBottom);
			glTexCoord2f(0.0f,1.0f);	glVertex2f(_selectorBoundsLeft, _selectorBoundsBottom);
		glEnd();

	_colourSelectorProgram->Off();

	glColor3f(_sampleR,_sampleG,_sampleB);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);	glVertex2f(_sampleBoundsLeft, _sampleBoundsTop);
		glTexCoord2f(1.0f,0.0f);	glVertex2f(_sampleBoundsRight, _sampleBoundsTop);
		glTexCoord2f(1.0f,1.0f);	glVertex2f(_sampleBoundsRight, _sampleBoundsBottom);
		glTexCoord2f(0.0f,1.0f);	glVertex2f(_sampleBoundsLeft, _sampleBoundsBottom);
	glEnd();

	/*
	glColor3f(1.0f,1.0f,1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);	glVertex2f(_handleBoundsLeft, _handleBoundsTop);
		glTexCoord2f(1.0f,0.0f);	glVertex2f(_handleBoundsRight, _handleBoundsTop);
		glTexCoord2f(1.0f,1.0f);	glVertex2f(_handleBoundsRight, _handleBoundsBottom);
		glTexCoord2f(0.0f,1.0f);	glVertex2f(_handleBoundsLeft, _handleBoundsBottom);
	glEnd();
	*/

	if( _selectorStateListDrawable != NULL )
		_selectorStateListDrawable->Draw();
	else if( _selectorDrawable != NULL )
		_selectorDrawable->Draw();

}

void ShivaGUI::ColourSelector::Inflate(TiXmlElement *xmlElement,ResourceManager *resources, std::string themePrefix, bool rootNode )
{
	if( themePrefix.empty() )
		themePrefix = "ColourSelector_";
	View::Inflate(xmlElement,resources,themePrefix,rootNode);

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( (std::string("selectorDrawable") == currentAttribute->Name()) || (themePrefix+"selectorDrawable" == currentAttribute->Name()) )
		{
			std::string resourceName( currentAttribute->Value() );

			// Retrieve resource and store it as correct type
			Drawable *drawable = resources->GetDrawable( resourceName );
			_selectorStateListDrawable = dynamic_cast<StateListDrawable*>(drawable);
			if( _selectorStateListDrawable == NULL )
				_selectorDrawable = drawable;
		}
		else if( std::string("onColourChange") == currentAttribute->Name() )
		{
			_colourListenerName = std::string( currentAttribute->Value() );

			// Retrieve listener
			ViewEventListener *listener = resources->GetListener(_colourListenerName);
			if( listener != NULL )
				_colourListener = listener;
		}
	}
}

TiXmlElement* ShivaGUI::ColourSelector::Deflate(ResourceManager *resources)
{
	TiXmlElement *xmlNode = View::Deflate(resources);
	xmlNode->SetValue("ColourSelector");

	if( _selectorStateListDrawable != NULL )
		xmlNode->SetAttribute("selectorDrawable",_selectorStateListDrawable->GetFilename() );
	else if( _selectorDrawable != NULL )
		xmlNode->SetAttribute("selectorDrawable",_selectorDrawable->GetFilename() );

	if( !_colourListenerName.empty() )
		xmlNode->SetAttribute("onColourChange", _colourListenerName );

	return xmlNode;
}



void ShivaGUI::ColourSelector::SetFocus(bool)
{

}

void ShivaGUI::ColourSelector::SetSelect(bool)
{

}

bool ShivaGUI::ColourSelector::HandleEvent(InternalEvent *currentEvent)
{
	if( _active )
	{

		if( currentEvent->GetType() == InternalEvent::POSITIONAL_SELECT )
		{
			int mouseX,mouseY;
			currentEvent->GetPosition(mouseX,mouseY);
			float posX, posY;
			if( MouseHitSelector(mouseX,mouseY, posX,posY) )
			{
				_handlePropPosX = posX;
				_handlePropPosY = posY;
				HSLtoRGB( posX,1.0f,posY, _sampleR,_sampleG,_sampleB );
				SetHandleActualPos();
				_holdingHandle = true;
				return true;
			}
		}
		if( _holdingHandle && currentEvent->GetType() == InternalEvent::POSITIONAL_DRAG )
		{
			int mouseX,mouseY;
			currentEvent->GetPosition(mouseX,mouseY);
			float posX, posY;
			if( MouseHitSelector(mouseX,mouseY, posX,posY) )
			{
				_handlePropPosX = posX;
				_handlePropPosY = posY;
				HSLtoRGB( posX,1.0f,posY, _sampleR,_sampleG,_sampleB );
				SetHandleActualPos();
				if( _colourListener != NULL )
					_colourListener->HandleEvent(this);
				return true;
			}
		}
		if( _holdingHandle && currentEvent->GetType() == InternalEvent::POSITIONAL_DESELECT )
		{
			_holdingHandle = false;
			if( _colourListener != NULL )
				_colourListener->HandleEvent(this);
			return true;
		}
	}
	return false;
}

void ShivaGUI::ColourSelector::SetHandlePosition(float x, float y)
{
	_handlePropPosX = x;
	_handlePropPosY = y;
	HSLtoRGB( x,1.0f,y, _sampleR,_sampleG,_sampleB );
	SetHandleActualPos();
}


bool ShivaGUI::ColourSelector::MouseHitSelector(int mouseX, int mouseY, float &posX, float &posY)
{
	if( _holdingHandle || (mouseX > _selectorBoundsLeft && mouseX < _selectorBoundsRight
		&& mouseY > _selectorBoundsTop && mouseY < _selectorBoundsBottom) )
	{
		float widgetWidth = _selectorBoundsRight - _selectorBoundsLeft;
		float widgetHeight = _selectorBoundsBottom - _selectorBoundsTop;
		posX = (float)(mouseX-_selectorBoundsLeft) / widgetWidth;
		posY = (float)(mouseY-_selectorBoundsTop) / widgetHeight;

		if( posX < 0.0f )
			posX = 0.0f;
		else if( posX > 1.0f )
			posX = 1.0f;

		if( posY < 0.0f )
			posY = 0.0f;
		else if( posY > 1.0f )
			posY = 1.0f;

		return true;
	}
	return false;
}

void ShivaGUI::ColourSelector::HSLtoRGB(float H, float S, float L, float &R, float &G, float &B)
{
	/// This code is kinda from Bob Powell (http://www.bobpowell.net/RGBHSB.htm)
	/// But he got it from Foley and van Dam
	/// See also http://130.113.54.154/~monger/hsl-rgb.html
	/// (it's the exact same thing, variables same names etc)

	float temp2 = ((L<=0.5f) ? L*(1.0f+S) : L+S-(L*S));
	float temp1 = 2.0f*L - temp2;

	float temp3[3];
	temp3[0] = H + 1.0f/3.0f;
	temp3[1] = H;
	temp3[2] = H - 1.0f/3.0f;

	float colour[3];
	colour[0] = 0.0f;
	colour[1] = 0.0f;
	colour[2] = 0.0f;

	for(int i=0; i<3; i++)
	{
		if( temp3[i] < 0 )
			temp3[i] += 1.0f;
		else if( temp3[i] > 1 )
			temp3[i] -= 1.0f;

		if( 6.0f*temp3[i] < 1.0f )
			colour[i] = temp1+(temp2-temp1)*temp3[i]*6.0f;
		else if( 2.0f*temp3[i] < 1.0f )
			colour[i] = temp2;
		else if( 3.0f*temp3[i] < 2.0f )
			colour[i] = (temp1+(temp2-temp1)*((2.0f/3.0f)-temp3[i])*6.0f);
		else
			colour[i] = temp1;
	}

	R = colour[0];
	G = colour[1];
	B = colour[2];
}

void ShivaGUI::ColourSelector::SetHandleActualPos()
{
	float widgetWidth = _selectorBoundsRight - _selectorBoundsLeft;
	float widgetHeight = _selectorBoundsBottom - _selectorBoundsTop;
	float posX = (_handlePropPosX * widgetWidth) + _selectorBoundsLeft;
	float posY = (_handlePropPosY * widgetHeight) + _selectorBoundsTop;
	_handleBoundsLeft = posX -_handleHalfSize;
	_handleBoundsRight = posX + _handleHalfSize;
	_handleBoundsTop = posY - _handleHalfSize;
	_handleBoundsBottom = posY + _handleHalfSize;


	if( _selectorStateListDrawable != NULL )
		_selectorStateListDrawable->SetBounds(_handleBoundsLeft,_handleBoundsTop,_handleBoundsRight,_handleBoundsBottom);
	else if( _selectorDrawable != NULL )
		_selectorDrawable->SetBounds(_handleBoundsLeft,_handleBoundsTop,_handleBoundsRight,_handleBoundsBottom);
}


