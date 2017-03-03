#include "GUI/Drawables/RectDrawable.h"

//----------------------------------------------------------------------------------

ShivaGUI::RectDrawable::RectDrawable()
{
	m_fillR = m_fillG = m_fillB = m_fillA = 1.0f;
	m_borderR = m_borderG = m_borderB = m_borderA = 0.0f;
}

//----------------------------------------------------------------------------------

ShivaGUI::RectDrawable::~RectDrawable()
{ }

//----------------------------------------------------------------------------------

void ShivaGUI::RectDrawable::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources)
{
	// TODO: fill and border colours

	//for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	//{
		/*
		if( std::string("src") == currentAttribute->Name() )
		{
			std::string resourceName( currentAttribute->Value() );

			// TODO: specifying the directory should *really* not be done here
			SetTexID( _resources->GetBitmap(std::string("Resources/Drawables/")+resourceName) );
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
	//}
}

//----------------------------------------------------------------------------------

void ShivaGUI::RectDrawable::Draw()
{

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// TODO: change from immediate mode

	//glColor4f(_fillR, _fillG, _fillB, _fillA);
	//glBegin(GL_QUADS);
	//	glVertex2f(m_boundsLeft, m_boundsTop);
	//	glVertex2f(m_boundsRight, m_boundsTop);
	//	glVertex2f(m_boundsRight, m_boundsBottom);
	//	glVertex2f(m_boundsLeft, m_boundsBottom);
	//glEnd();

	//glColor4f(_borderR, _borderG, _borderB, _borderA);
	//glLineWidth(5.0f);
	//glBegin(GL_LINE_LOOP);
	//	glVertex2f(m_boundsLeft, m_boundsTop);
	//	glVertex2f(m_boundsRight, m_boundsTop);
	//	glVertex2f(m_boundsRight, m_boundsBottom);
	//	glVertex2f(m_boundsLeft, m_boundsBottom);
	//glEnd();

	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//----------------------------------------------------------------------------------

void ShivaGUI::RectDrawable::OnSetBounds( float _left, float _top, float _right, float _bottom, unsigned int _gravity )
{
	m_boundsLeft = _left;
	m_boundsRight = _right;
	m_boundsTop = _top;
	m_boundsBottom = _bottom;
}

//----------------------------------------------------------------------------------