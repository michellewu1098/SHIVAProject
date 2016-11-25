#include "GUI/Drawables/LayeredImageDrawable.h"

//////////////////////////////////////////////////////////////////////////
#include <GL/GLee.h>
#include <iostream>
#include <sstream>

ShivaGUI::LayeredImageDrawable::LayeredImageDrawable()
{
	_maxLayerGroups = 4;
	_scaleUp = true;
	_isNinePatch = false;
	_texWidth = _texHeight = 10;
	_keepAspectRatio = false;
	
	_centreLeftProp = _centreRightProp = _centreTopProp = _centreBottomProp = 0.0f;
	_centreLeftBounds = _centreRightBounds = _centreTopBounds = _centreBottomBounds = 0.0f;

	_contentLeftProp = 0.0f;
	_contentRightProp = 1.0f;
	_contentTopProp = 0.0f;
	_contentBottomProp = 1.0f;
	_contentLeftBounds = _contentRightBounds = _contentTopBounds = _contentBottomBounds = 0.0f;

	// Create vertex buffer objects
	glGenBuffers(1,&_squareVertexVBO);
	glGenBuffers(1,&_squareTexcoordVBO);
	glGenBuffers(1,&_squareIndexVBO);

	BuildVBOs();


	_layerProgram = new Utility::GPUProgram();
	_layerProgram->Create("Resources/Shaders/LayeredImageDrawable",Utility::GPUProgram::VERTEX_AND_FRAGMENT);

	_layerGroups = new LayerGroup[_maxLayerGroups];
	for( unsigned int i = 0; i < _maxLayerGroups; ++i )
	{
		_layerGroups[i].Init(i,_layerProgram->GetProgramID());
	}
}

ShivaGUI::LayeredImageDrawable::~LayeredImageDrawable()
{
	delete _layerProgram;

	glDeleteBuffers(1,&_squareVertexVBO);
	glDeleteBuffers(1,&_squareTexcoordVBO);
	glDeleteBuffers(1,&_squareIndexVBO);
}

void ShivaGUI::LayeredImageDrawable::SetTexID(unsigned int OpenGLTexID, unsigned int layerGroup)
{
	if( layerGroup >= _maxLayerGroups )
	{
		std::cerr<<"WARNING: LayeredImageDrawable given layer group >= _maxLayerGroups"<<std::endl;
		return;
	}
	//std::cout<<"INFO: LayeredImageDrawable::SetTexID OpenGLTexID = "<<OpenGLTexID<<std::endl;
	_layerGroups[layerGroup].SetGLTexID(OpenGLTexID);

	// Not very efficient, but we're not into rendering yet and this is easier for the moment...
	glBindTexture(GL_TEXTURE_2D, OpenGLTexID);
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &_texWidth);
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &_texHeight);
	glBindTexture(GL_TEXTURE_2D, 0);
	//std::cout<<"INFO: BitmapDrawable dimensions: "<<_texWidth<<" "<<_texHeight<<std::endl;
}

void ShivaGUI::LayeredImageDrawable::GetContentBounds(float &left, float &top, float &right, float &bottom)
{
	left = _contentLeftBounds;
	right = _contentRightBounds;
	top = _contentTopBounds;
	bottom = _contentBottomBounds;
}

int ShivaGUI::LayeredImageDrawable::GetNativeWidthFromContent(int contentWidth)
{
	return (int)( ((_contentLeftProp + (1.0f-_contentRightProp) ) * (float)_texWidth) + contentWidth );
}

int ShivaGUI::LayeredImageDrawable::GetNativeHeightFromContent(int contentHeight)
{
	return (int)( ((_contentTopProp + (1.0f-_contentBottomProp) ) * (float)_texHeight) + contentHeight );
}

void ShivaGUI::LayeredImageDrawable::SetLayerColour(std::string colourString, unsigned int group, unsigned int layer)
{
	unsigned int fontColour = 0;
	std::stringstream colourStream;
	colourStream << std::hex << colourString;
	colourStream >> fontColour;
	SetLayerColour(fontColour,group,layer);
}

void ShivaGUI::LayeredImageDrawable::SetLayerColour(unsigned int colour, unsigned int group, unsigned int layer)
{
	if( group >= (_maxLayerGroups) )
	{
		std::cerr<<"WARNING: LayeredImageDrawable given layer group >= _maxLayerGroups"<<std::endl;
		return;
	}
	if( layer >= 4 )
	{
		std::cerr<<"WARNING: LayeredImageDrawable given layer >= 4"<<std::endl;
		return;
	}
	_layerGroups[group].SetLayerColour(colour,layer);
}

void ShivaGUI::LayeredImageDrawable::Inflate(TiXmlElement *xmlElement, ResourceManager *resources)
{
	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( (std::string)("scaleUp") == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				_scaleUp = true;
			else if( value == "false" )
				_scaleUp = false;
		}
		else if( (std::string)("isNinePatch") == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				_isNinePatch = true;
			else if( value == "false" )
				_isNinePatch = false;
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
		else if( (std::string)("keepAspectRatio") == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				_keepAspectRatio = true;
			else if( value == "false" )
				_keepAspectRatio = false;
		}


	}

	// Each layer is a child of the LayeredImageDrawable element
	// Each layer child contains its properties as attributes
	unsigned int imageLayerGroup = 0;
	for( TiXmlElement *currentChild = xmlElement->FirstChildElement("Layer"); currentChild != NULL; currentChild = currentChild->NextSiblingElement("Layer") )
	{
		for( TiXmlAttribute *currentAttribute = currentChild->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
		{
			if( std::string("src") == currentAttribute->Name() )
			{
				std::string resourceName( currentAttribute->Value() );

				//std::cout<<"INFO: LayeredImageDrawable loading layer source: "<<resourceName<<std::endl;
				// TODO: specifying the directory should *really* not be done here
				SetTexID( resources->GetBitmap(std::string("Resources/Drawables/")+resourceName),imageLayerGroup );

			}
			else if( std::string("colour0") == currentAttribute->Name() )
			{
				SetLayerColour( resources->GetInflationAttribute( currentAttribute->Value() ), imageLayerGroup, 0 );
			}
			else if( std::string("colour1") == currentAttribute->Name() )
			{
				SetLayerColour( resources->GetInflationAttribute( currentAttribute->Value() ), imageLayerGroup, 1 );
			}
			else if( std::string("colour2") == currentAttribute->Name() )
			{
				SetLayerColour( resources->GetInflationAttribute( currentAttribute->Value() ), imageLayerGroup, 2 );
			}
			else if( std::string("colour3") == currentAttribute->Name() )
			{
				SetLayerColour( resources->GetInflationAttribute( currentAttribute->Value() ), imageLayerGroup, 3 );
			}
		}
		++imageLayerGroup;
	}
}

void ShivaGUI::LayeredImageDrawable::Draw()
{
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(0,1,0,1);
	_layerProgram->On();

	for( unsigned int i = 0; i < _maxLayerGroups; i++ )
	{
		_layerGroups[i].Bind();
	}

/*
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);	glVertex2f(_boundsLeft, _boundsTop);
		glTexCoord2f(1.0f,0.0f);	glVertex2f(_boundsRight, _boundsTop);
		glTexCoord2f(1.0f,1.0f);	glVertex2f(_boundsRight, _boundsBottom);
		glTexCoord2f(0.0f,1.0f);	glVertex2f(_boundsLeft, _boundsBottom);
	glEnd();
*/

	// Bind VBOs
	glBindBufferARB(GL_ARRAY_BUFFER, _squareVertexVBO);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, 0);

	glBindBufferARB(GL_ARRAY_BUFFER, _squareTexcoordVBO);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer( 2, GL_FLOAT, 0, 0 );

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, _squareIndexVBO);

		if( _isNinePatch )
			glDrawElements(GL_QUADS, 9*4, GL_UNSIGNED_INT, 0);
		else
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);


	_layerProgram->Off();

	glDisable(GL_BLEND);

	for( unsigned int i = 0; i < _maxLayerGroups; i++ )
	{
		_layerGroups[i].Unbind();
	}

	glActiveTexture(GL_TEXTURE0);
}

void ShivaGUI::LayeredImageDrawable::OnSetBounds(float left, float top, float right, float bottom, unsigned int gravity)
{
	float width = right - left;
	float height = bottom - top;

	if( _scaleUp )
	{
		if( _keepAspectRatio )
		{
			float horizRatio = width / (float) _texWidth;
			float vertRatio = height / (float) _texHeight;

			if( horizRatio > vertRatio )
			{
				_boundsTop = top;
				_boundsBottom = bottom;

				_boundsLeft = left + (width/2.0f) - (((float)_texWidth)*0.5f*vertRatio);
				_boundsRight = left + (width/2.0f) + (((float)_texWidth)*0.5f*vertRatio);
			}
			else
			{
				_boundsLeft = left;
				_boundsRight = right;

				_boundsTop = top + (height/2.0f) - (((float)_texHeight)*0.5f*horizRatio);
				_boundsBottom = top + (height/2.0f) + (((float)_texHeight)*0.5f*horizRatio);
			}

		}
		else
		{
			_boundsLeft = left;
			_boundsRight = right;
			_boundsTop = top;
			_boundsBottom = bottom;
		}

		if( _isNinePatch )
		{
			_contentLeftBounds   = left + (_contentLeftProp * _texWidth);
			_contentRightBounds  = right - ((1.0f-_contentRightProp) * _texWidth);
			_centreLeftBounds   = left + (_centreLeftProp * _texWidth);
			_centreRightBounds  = right - ( (1.0f-_centreRightProp) * _texWidth);

			_contentTopBounds    = top  + (_contentTopProp * _texHeight);
			_contentBottomBounds = bottom - ((1.0f-_contentBottomProp) * _texHeight);
			_centreTopBounds    = top  + (_centreTopProp * _texHeight);
			_centreBottomBounds = bottom  - ( (1.0f-_centreBottomProp) * _texHeight);

			if( _centreBottomBounds < _centreTopBounds )
			{
				float average = (_centreBottomBounds + _centreTopBounds) / 2.0f;
				_centreBottomBounds = _centreTopBounds = average;
			}

			if( _centreRightBounds < _centreLeftBounds )
			{
				float average = (_centreRightBounds + _centreLeftBounds) / 2.0f;
				_centreRightBounds = _centreLeftBounds = average;
			}

		}
	}
	else
	{
		float width = right - left;
		float height = bottom - top;

		if( width > _texWidth )
		{
			if( (gravity == Definitions::CENTRE) ||
				( (gravity & Definitions::HORIZONTAL_MASK) == Definitions::CENTRE_HORIZONTAL) )
			{
				_boundsLeft  = left + (width/2.0f) - ((float)_texWidth/2.0f);
				_boundsRight = right- (width/2.0f) + ((float)_texWidth/2.0f);
			}
			else if( (gravity & Definitions::HORIZONTAL_MASK) == Definitions::LEFT )
			{
				_boundsLeft  = left;
				_boundsRight = left + ((float)_texWidth);
			}
			else if( (gravity & Definitions::HORIZONTAL_MASK) == Definitions::RIGHT )
			{
				_boundsLeft  = right -((float)_texWidth);
				_boundsRight = right;
			}
		}
		if( height > _texHeight )
		{
			if( (gravity == Definitions::CENTRE) ||
				( (gravity & Definitions::VERTICAL_MASK) == Definitions::CENTRE_VERTICAL) )
			{
				_boundsTop   = top  + (height/2.0f) - ((float)_texHeight/2.0f);
				_boundsBottom = bottom- (height/2.0f) + ((float)_texHeight/2.0f);
			}
			else if( (gravity & Definitions::VERTICAL_MASK) == Definitions::TOP )
			{
				_boundsTop = top;
				_boundsBottom = top + ((float)_texHeight);
			}
			else if( (gravity & Definitions::VERTICAL_MASK) == Definitions::BOTTOM )
			{
				_boundsTop = bottom - ((float)_texHeight);
				_boundsBottom = bottom;
			}
		}
	}
	
	BuildVBOs();


	//std::cout<<"LayeredImageDrawable bounds: "<<_boundsLeft<<" "<<_boundsRight<<" "<<_boundsTop<<" "<<_boundsBottom<<std::endl;
}


void ShivaGUI::LayeredImageDrawable::BuildVBOs()
{
	if( _isNinePatch )
	{
		float squareVertices[16*2] = {_boundsLeft, _boundsTop,
									 _centreLeftBounds, _boundsTop,
									 _centreRightBounds, _boundsTop,
									 _boundsRight, _boundsTop,

									 _boundsLeft, _centreTopBounds,
									 _centreLeftBounds, _centreTopBounds,
									 _centreRightBounds, _centreTopBounds,
									 _boundsRight, _centreTopBounds,
									 
									 _boundsLeft, _centreBottomBounds,
									 _centreLeftBounds, _centreBottomBounds,
									 _centreRightBounds, _centreBottomBounds,
									 _boundsRight, _centreBottomBounds,
									 
									 _boundsLeft, _boundsBottom,
									 _centreLeftBounds, _boundsBottom,
									 _centreRightBounds, _boundsBottom,
									 _boundsRight, _boundsBottom };

		float squareTexcoords[16*2] = {0.0f, 0.0f,
									 _centreLeftProp, 0.0f,
									 _centreRightProp, 0.0f,
									 1.0f, 0.0f,

									 0.0f, _centreTopProp,
									 _centreLeftProp, _centreTopProp,
									 _centreRightProp, _centreTopProp,
									 1.0f, _centreTopProp,
									 
									 0.0f, _centreBottomProp,
									 _centreLeftProp, _centreBottomProp,
									 _centreRightProp, _centreBottomProp,
									 1.0f, _centreBottomProp,
									 
									 0.0f, 1.0f,
									 _centreLeftProp, 1.0f,
									 _centreRightProp, 1.0f,
									 1.0f, 1.0f };


		
		int squareIndices[9*4] = {0,1,5,4,
								1,2,6,5,
								2,3,7,6,
								4,5,9,8,
								5,6,10,9,
								6,7,11,10,
								8,9,13,12,
								9,10,14,13,
								10,11,15,14};

		glBindBuffer(GL_ARRAY_BUFFER,_squareVertexVBO);
		glBufferData(GL_ARRAY_BUFFER, 16*2*sizeof(float), squareVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		
		glBindBuffer(GL_ARRAY_BUFFER,_squareTexcoordVBO);
		glBufferData(GL_ARRAY_BUFFER, 16*2*sizeof(float), squareTexcoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_squareIndexVBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 9*4*sizeof(int), squareIndices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);


	}
	else
	{
		float squareVertices[4*2] = {_boundsLeft, _boundsTop,
							 _boundsRight, _boundsTop,
							 _boundsRight, _boundsBottom,
							 _boundsLeft, _boundsBottom };

		float squareTexcoords[4*2] = {0.0f, 0.0f,
									 1.0f, 0.0f,
									 1.0f, 1.0f,
									 0.0f, 1.0f };

		int squareIndices[4] = {0,1,2,3};

		glBindBuffer(GL_ARRAY_BUFFER,_squareVertexVBO);
		glBufferData(GL_ARRAY_BUFFER, 4*2*sizeof(float), squareVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		
		glBindBuffer(GL_ARRAY_BUFFER,_squareTexcoordVBO);
		glBufferData(GL_ARRAY_BUFFER, 4*2*sizeof(float), squareTexcoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_squareIndexVBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4*sizeof(int), squareIndices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}

	

}






ShivaGUI::LayeredImageDrawable::LayerGroup::LayerGroup()
{
	_texID = 0;
	_colourUniforms = new int[4];
	_layerColours = new float[4*4];
	_groupNumber = 0;
	_textureUniform = -1;
	for( unsigned int i = 0; i < 4; ++i )
	{
		_colourUniforms[i] = -1;
		for( unsigned int j = 0; j < 4; ++j )
			_layerColours[i*4+j] = 0;
	}

}

ShivaGUI::LayeredImageDrawable::LayerGroup::~LayerGroup()
{
	delete [] _colourUniforms;
	delete [] _layerColours;
}

void ShivaGUI::LayeredImageDrawable::LayerGroup::SetGLTexID( unsigned int texID ){ _texID=texID;}

void ShivaGUI::LayeredImageDrawable::LayerGroup::Init(unsigned int groupNumber, unsigned int GLSLProgID)
{
	_groupNumber = groupNumber;
	std::stringstream texName;
	texName<<"ImageLayer"<<groupNumber;
	_textureUniform = glGetUniformLocation(GLSLProgID, texName.str().c_str() );

	if( _textureUniform < 0 )
		std::cout<<"WARNING: LayerGroup::Init _textureUniform "<<texName.str().c_str()<<" = "<<_textureUniform<<std::endl;

	for( unsigned int j = 0; j < 4; j++ )
	{
		std::stringstream temp;
		temp<<"LayerColour"<<groupNumber<<"_"<<j;
		_colourUniforms[j] = glGetUniformLocation(GLSLProgID, temp.str().c_str() );
		if( _colourUniforms[j] < 0 )
		{
			std::cerr<<"WARNING: LayerGroup Init could not find GLSL uniform: "<<temp.str().c_str()<<std::endl;
		}
	}
}

void ShivaGUI::LayeredImageDrawable::LayerGroup::SetLayerColour(unsigned int colour, unsigned int layer)
{
	_layerColours[layer*4+0] = ((float)((colour & 0xFF000000)>>24))/255.0f;
	_layerColours[layer*4+1] = ((float)((colour & 0x00FF0000)>>16))/255.0f;
	_layerColours[layer*4+2] = ((float)((colour & 0x0000FF00)>>8))/255.0f;
	_layerColours[layer*4+3] = ((float)(colour & 0x000000FF))/255.0f;

	//std::cout<<"INFO: LayeredImageDrawable setting layer colours to: "<<_layerColours[layer*4+0]<<" "<<_layerColours[layer*4+1]<<" "<<_layerColours[layer*4+2]<<" "<<_layerColours[layer*4+3]<<std::endl;
}

void ShivaGUI::LayeredImageDrawable::LayerGroup::Bind()
{
	if( _texID != 0 )
	{
		glActiveTexture(GL_TEXTURE0+_groupNumber);
		//glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _texID);

		if( _textureUniform >= 0 )
		{
			glUniform1i(_textureUniform,_groupNumber);
		}
	}
	else
	{
		glActiveTexture(GL_TEXTURE0+_groupNumber);
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	for( unsigned int currentLayer = 0; currentLayer < 4; currentLayer++ )
	{
		if( _colourUniforms[currentLayer] >= 0 )
		{
			glUniform4fv( _colourUniforms[currentLayer], 1, &(_layerColours[currentLayer*4]) );
			//glUniform4f(_colourUniforms[currentLayer],1.0f,0,0,1.0f);
		}
	}
}

void ShivaGUI::LayeredImageDrawable::LayerGroup::Unbind()
{
	glActiveTexture(GL_TEXTURE0+_groupNumber);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}
