#include "GUI/Drawables/NinePatch.h"

//////////////////////////////////////////////////////////////////////////
#include <GL/GLee.h>
#include <iostream>

ShivaGUI::NinePatch::NinePatch()
{
	_contentLeftProp = 0.0f;
	_contentRightProp = 1.0f;
	_contentTopProp = 0.0f;
	_contentBottomProp = 1.0f;
	_centreLeftBounds = _centreRightBounds = _centreTopBounds = _centreBottomBounds = 0.0f;
	_fixedX = _fixedY = false;
}

ShivaGUI::NinePatch::~NinePatch()
{

}

void ShivaGUI::NinePatch::SetTexID(unsigned int value)
{
	_texID=value;
	// Not very efficient, but we're not into rendering yet and this is easier for the moment...
	glBindTexture(GL_TEXTURE_2D, _texID);
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &_texWidth);
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &_texHeight);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ShivaGUI::NinePatch::Inflate(TiXmlElement *xmlElement, ResourceManager *resources)
{

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( currentAttribute->Name() == std::string("src") )
		{
			std::string filename( currentAttribute->Value() );

			// TODO: specifying the directory should *really* not be done here
			SetTexID( resources->GetBitmap(std::string("Resources/Drawables/")+filename) );
		}

		else if( currentAttribute->Name() == std::string("content_left") )
		{
			float value = (float) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			_contentLeftProp = value;
		}
		else if( currentAttribute->Name() == std::string("content_right") )
		{
			float value = (float) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			_contentRightProp = value;
		}
		else if( currentAttribute->Name() == std::string("content_top") )
		{
			float value = (float) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			_contentTopProp = value;
		}
		else if( currentAttribute->Name() == std::string("content_bottom") )
		{
			float value = (float) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			_contentBottomProp = value;
		}

		else if( currentAttribute->Name() == std::string("centre_left") )
		{
			float value = (float) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			_centreLeftProp = value;
		}
		else if( currentAttribute->Name() == std::string("centre_right") )
		{
			float value = (float) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			_centreRightProp = value;
		}
		else if( currentAttribute->Name() == std::string("centre_top") )
		{
			float value = (float) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			_centreTopProp = value;
		}
		else if( currentAttribute->Name() == std::string("centre_bottom") )
		{
			float value = (float) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			_centreBottomProp = value;
		}

		else if( (std::string)("fixedX") == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				_fixedX = true;
			else if( value == "false" )
				_fixedX = false;
		}
		else if( (std::string)("fixedY") == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				_fixedY = true;
			else if( value == "false" )
				_fixedY = false;
		}
	}
}

void ShivaGUI::NinePatch::GetContentBounds(float &left, float &top, float &right, float &bottom)
{
	left = _contentLeftBounds;
	right = _contentRightBounds;
	top = _contentTopBounds;
	bottom = _contentBottomBounds;
}

int ShivaGUI::NinePatch::GetNativeWidthFromContent(int contentWidth)
{
	// is multiplying by the texWidth wrong??
	int value = ((_contentLeftProp + (1.0f-_contentRightProp) ) * (float)_texWidth) + contentWidth;
	//std::cout<<"INFO: NinePatch::GetNativeWidthFromContent, contentWidth = "<<contentWidth<<" result = "<< value<<std::endl;
	return value;
}

int ShivaGUI::NinePatch::GetNativeHeightFromContent(int contentHeight)
{
	int value = ((_contentTopProp + (1.0f-_contentBottomProp) ) * (float)_texHeight) + contentHeight;
	//std::cout<<"INFO: NinePatch::GetNativeHeightFromContent, _contentTopProp = "<<_contentTopProp<<" _contentBottomProp = "<< _contentBottomProp<<" texHeight = "<<_texHeight<<" contentHeight = "<<contentHeight<<" result = "<< value<<std::endl;
	return value;
}

void ShivaGUI::NinePatch::Draw()
{
	// TODO: clean all this up ;)
	glEnable(GL_TEXTURE_2D);
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE ) ;
	glBindTexture(GL_TEXTURE_2D, _texID);
	glColor3f(1.0f,1.0f,1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);

		glTexCoord2f(0.0f,0.0f);						glVertex2f(_boundsLeft, _boundsTop);
		glTexCoord2f(_centreLeftProp, 0.0f);			glVertex2f(_centreLeftBounds, _boundsTop);
		glTexCoord2f(_centreLeftProp,_centreTopProp);	glVertex2f(_centreLeftBounds, _centreTopBounds);
		glTexCoord2f(0.0f,_centreTopProp);				glVertex2f(_boundsLeft, _centreTopBounds);

		glTexCoord2f(_centreLeftProp,0.0f);				glVertex2f(_centreLeftBounds, _boundsTop);
		glTexCoord2f(_centreRightProp, 0.0f);			glVertex2f(_centreRightBounds, _boundsTop);
		glTexCoord2f(_centreRightProp,_centreTopProp);	glVertex2f(_centreRightBounds, _centreTopBounds);
		glTexCoord2f(_centreLeftProp,_centreTopProp);	glVertex2f(_centreLeftBounds, _centreTopBounds);

		glTexCoord2f(_centreRightProp,0.0f);			glVertex2f(_centreRightBounds, _boundsTop);
		glTexCoord2f(1.0f, 0.0f);						glVertex2f(_boundsRight, _boundsTop);
		glTexCoord2f(1.0f,_centreTopProp);				glVertex2f(_boundsRight, _centreTopBounds);
		glTexCoord2f(_centreRightProp,_centreTopProp);	glVertex2f(_centreRightBounds, _centreTopBounds);


		glTexCoord2f(0.0f,_centreTopProp);					glVertex2f(_boundsLeft, _centreTopBounds);
		glTexCoord2f(_centreLeftProp, _centreTopProp);		glVertex2f(_centreLeftBounds, _centreTopBounds);
		glTexCoord2f(_centreLeftProp,_centreBottomProp);	glVertex2f(_centreLeftBounds, _centreBottomBounds);
		glTexCoord2f(0.0f,_centreBottomProp);				glVertex2f(_boundsLeft, _centreBottomBounds);

		glTexCoord2f(_centreLeftProp,_centreTopProp);		glVertex2f(_centreLeftBounds, _centreTopBounds);
		glTexCoord2f(_centreRightProp, _centreTopProp);		glVertex2f(_centreRightBounds, _centreTopBounds);
		glTexCoord2f(_centreRightProp,_centreBottomProp);	glVertex2f(_centreRightBounds, _centreBottomBounds);
		glTexCoord2f(_centreLeftProp,_centreBottomProp);	glVertex2f(_centreLeftBounds, _centreBottomBounds);

		glTexCoord2f(_centreRightProp,_centreTopProp);		glVertex2f(_centreRightBounds, _centreTopBounds);
		glTexCoord2f(1.0f, _centreTopProp);					glVertex2f(_boundsRight, _centreTopBounds);
		glTexCoord2f(1.0f,_centreBottomProp);				glVertex2f(_boundsRight, _centreBottomBounds);
		glTexCoord2f(_centreRightProp,_centreBottomProp);	glVertex2f(_centreRightBounds, _centreBottomBounds);


		glTexCoord2f(0.0f,_centreBottomProp);				glVertex2f(_boundsLeft, _centreBottomBounds);
		glTexCoord2f(_centreLeftProp, _centreBottomProp);	glVertex2f(_centreLeftBounds, _centreBottomBounds);
		glTexCoord2f(_centreLeftProp,1.0f);					glVertex2f(_centreLeftBounds, _boundsBottom);
		glTexCoord2f(0.0f,1.0f);							glVertex2f(_boundsLeft, _boundsBottom);

		glTexCoord2f(_centreLeftProp,_centreBottomProp);	glVertex2f(_centreLeftBounds, _centreBottomBounds);
		glTexCoord2f(_centreRightProp, _centreBottomProp);	glVertex2f(_centreRightBounds, _centreBottomBounds);
		glTexCoord2f(_centreRightProp,1.0f);				glVertex2f(_centreRightBounds, _boundsBottom);
		glTexCoord2f(_centreLeftProp,1.0f);					glVertex2f(_centreLeftBounds, _boundsBottom);

		glTexCoord2f(_centreRightProp,_centreBottomProp);	glVertex2f(_centreRightBounds, _centreBottomBounds);
		glTexCoord2f(1.0f, _centreBottomProp);				glVertex2f(_boundsRight, _centreBottomBounds);
		glTexCoord2f(1.0f,1.0f);							glVertex2f(_boundsRight, _boundsBottom);
		glTexCoord2f(_centreRightProp,1.0f);				glVertex2f(_centreRightBounds, _boundsBottom);



/*
		glTexCoord2f(0.0f,0.0f);	glVertex2f(_boundsLeft, _boundsTop);
		glTexCoord2f(1.0f,0.0f);	glVertex2f(_boundsRight, _boundsTop);
		glTexCoord2f(1.0f,1.0f);	glVertex2f(_boundsRight, _boundsBottom);
		glTexCoord2f(0.0f,1.0f);	glVertex2f(_boundsLeft, _boundsBottom);
		*/
	glEnd();

	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ShivaGUI::NinePatch::OnSetBounds(float left, float top, float right, float bottom, unsigned int gravity)
{
//	float width = right - left;
	float height = bottom - top;

/*
	_contentLeftBounds   = left + (_contentLeftProp * width);
	_contentRightBounds  = left + (_contentRightProp * width);
	_centreLeftBounds   = left + (_centreLeftProp * _texWidth);
	_centreRightBounds  = right - ( (1.0f-_centreRightProp) * _texWidth);

	_contentTopBounds    = top  + (_contentTopProp * height);
	_contentBottomBounds = top  + (_contentBottomProp * height);
	_centreTopBounds    = top  + (_centreTopProp * _texHeight);
	_centreBottomBounds = bottom  - ( (1.0f-_centreRightProp) * _texWidth);
*/


	if( _fixedX )
	{
		_contentLeftBounds   = left + (_contentLeftProp * _texWidth);
		_contentRightBounds  = right - ((1.0f-_contentRightProp) * _texWidth);
		_centreLeftBounds   = left + (_centreLeftProp * _texWidth);
		_centreRightBounds  = right - ( (1.0f-_centreRightProp) * _texWidth);
	}
	else
	{
		_contentLeftBounds   = left + (_contentLeftProp * _texWidth);
		_contentRightBounds  = right - ((1.0f-_contentRightProp) * _texWidth);
		_centreLeftBounds   = left + (_centreLeftProp * _texWidth);
		_centreRightBounds  = right - ( (1.0f-_centreRightProp) * _texWidth);
	}

	//std::cout<<"INFO: NinePatch::OnSetBounds width = "<<width<<" _contentLeftProp = "<<_contentLeftProp<<" _contentRightProp = "<<_contentRightProp<<std::endl;
	//std::cout<<"INFO: NinePatch::OnSetBounds texWidth = "<<_texWidth<<" _contentLeftProp*texWidth = "<<_contentLeftProp*_texWidth<<" 1-_contentRightProp*texWidth = "<<(1.0f-_contentRightProp)*_texWidth<<std::endl;
	//std::cout<<"INFO: NinePatch::OnSetBounds resulting content width = "<< _contentRightBounds - _contentLeftBounds<<std::endl;


	if( _fixedY )
	{
		_contentTopBounds    = top  + (height/2.0f) - ((float)_texHeight/2.0f);
		_contentBottomBounds = bottom- (height/2.0f) + ((float)_texHeight/2.0f);
		_contentBottomBounds = bottom - ((1.0f-_contentBottomProp) * _texHeight);
		_centreTopBounds    = _contentTopBounds  + (_centreTopProp * _texHeight/2.0f);
		_centreBottomBounds = _contentBottomBounds  - ( (1.0f-_centreBottomProp) * _texHeight/2.0f);
	}
	else
	{
		//_centreLeftBounds   = left + (_centreLeftProp * _texWidth);
		//_centreRightBounds  = right - ( (1.0f-_centreRightProp) * _texWidth);
		//_centreTopBounds    = top  + (_centreTopProp * _texHeight);
		//_centreBottomBounds = bottom  - ( (1.0f-_centreBottomProp) * _texHeight);

		_contentTopBounds    = top  + (_contentTopProp * _texHeight);
		_contentBottomBounds = bottom - ((1.0f-_contentBottomProp) * _texHeight);
		_centreTopBounds    = top  + (_centreTopProp * _texHeight);
		_centreBottomBounds = bottom  - ( (1.0f-_centreBottomProp) * _texHeight);
	}

	//std::cout<<"INFO: NinePatch::OnSetBounds height = "<<height<<" _contentTopProp = "<<_contentTopProp<<" _contentBottomProp = "<<_contentBottomProp<<std::endl;
	//std::cout<<"INFO: NinePatch::OnSetBounds texHeight = "<<_texHeight<<" _contentTopProp*texHeight = "<<_contentTopProp*_texHeight<<" 1-_contentBottomProp*texHeight = "<<(1.0f-_contentBottomProp)*_texHeight<<std::endl;
	//std::cout<<"INFO: NinePatch::OnSetBounds resulting content height = "<< _contentBottomBounds - _contentTopBounds<<std::endl;



	if( _centreBottomBounds < _centreTopBounds )
	{
		float average = (_centreBottomBounds + _centreTopBounds) / 2.0f;
		_centreBottomBounds = _centreTopBounds = average;
	}

	/*
	_contentLeftBounds   = left + (_contentLeftProp * width);
	_contentRightBounds  = left + (_contentRightProp * width);
	_centreLeftBounds   = left + (_centreLeftProp * _texWidth);
	_centreRightBounds  = right - ( (1.0f-_centreRightProp) * _texWidth);

	_contentTopBounds    = top  + (_contentTopProp * height);
	_contentBottomBounds = top  + (_contentBottomProp * height);
	_centreTopBounds    = top  + (_centreTopProp * _texHeight);
	_centreBottomBounds = bottom  - ( (1.0f-_centreRightProp) * _texWidth);
	 */


	/*
	_contentLeftBounds   = left + (_contentLeftProp * width);
	_contentRightBounds  = left + (_contentRightProp * width);
	_contentTopBounds    = top  + (_contentTopProp * height);
	_contentBottomBounds = top  + (_contentBottomProp * height);

	_centreLeftBounds   = left + (_centreLeftProp * _texWidth);
	_centreRightBounds  = right - ( (1.0f-_centreRightProp) * _texWidth);
	_centreTopBounds    = top  + (_centreTopProp * _texHeight);
	_centreBottomBounds = bottom  - ( (1.0f-_centreRightProp) * _texWidth);
	*/
}


