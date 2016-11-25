#include "GUI/Drawables/RectDrawable.h"

//////////////////////////////////////////////////////////////////////////
#include <GL/GLee.h>
#include <iostream>

ShivaGUI::RectDrawable::RectDrawable()
{
	_fillR = _fillG = _fillB = _fillA = 1.0f;
	_borderR = _borderG = _borderB = _borderA = 0.0f;
}


ShivaGUI::RectDrawable::~RectDrawable()
{
}


void ShivaGUI::RectDrawable::Inflate(TiXmlElement *xmlElement, ResourceManager *resources)
{
	// TODO: fill and border colours

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		/*
		if( std::string("src") == currentAttribute->Name() )
		{
			std::string resourceName( currentAttribute->Value() );

			// TODO: specifying the directory should *really* not be done here
			SetTexID( resources->GetBitmap(std::string("Resources/Drawables/")+resourceName) );
		}
		else if( (std::string)("scaleUp") == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				_scaleUp = true;
			else if( value == "false" )
				_scaleUp = false;
		}
		*/
	}
}

void ShivaGUI::RectDrawable::Draw()
{

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// TODO: change from immediate mode

	glColor4f(_fillR, _fillG, _fillB, _fillA);
	glBegin(GL_QUADS);
		glVertex2f(_boundsLeft, _boundsTop);
		glVertex2f(_boundsRight, _boundsTop);
		glVertex2f(_boundsRight, _boundsBottom);
		glVertex2f(_boundsLeft, _boundsBottom);
	glEnd();

	glColor4f(_borderR, _borderG, _borderB, _borderA);
	glLineWidth(5.0f);
	glBegin(GL_LINE_LOOP);
		glVertex2f(_boundsLeft, _boundsTop);
		glVertex2f(_boundsRight, _boundsTop);
		glVertex2f(_boundsRight, _boundsBottom);
		glVertex2f(_boundsLeft, _boundsBottom);
	glEnd();

	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ShivaGUI::RectDrawable::OnSetBounds(float left, float top, float right, float bottom, unsigned int gravity)
{
	_boundsLeft = left;
	_boundsRight = right;
	_boundsTop = top;
	_boundsBottom = bottom;
}
