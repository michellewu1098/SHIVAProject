#include "GUI/Drawables/BitmapDrawable.h"

#include <GL/GLee.h>
//#include <iostream>

//----------------------------------------------------------------------------------

ShivaGUI::BitmapDrawable::BitmapDrawable()
{
	_texID = 0;
	_scaleUp = false;
	_keepAspectRatio = false;
	_texWidth = _texHeight = 10;

	//_testProgram = new Utility::GPUProgram();
	//_testProgram->Create("Resources/Shaders/TestProgram",Utility::GPUProgram::FRAGMENT);
}

//----------------------------------------------------------------------------------

ShivaGUI::BitmapDrawable::BitmapDrawable( unsigned int OpenGLTexID )
{
	_scaleUp = false;
	_texWidth = _texHeight = 10;
	SetTexID( OpenGLTexID );

	//_testProgram = new Utility::GPUProgram();
	//_testProgram->Create("Resources/Shaders/TestProgram",Utility::GPUProgram::FRAGMENT);
}

//----------------------------------------------------------------------------------

ShivaGUI::BitmapDrawable::~BitmapDrawable()
{
	//delete _testProgram;
}

//----------------------------------------------------------------------------------

void ShivaGUI::BitmapDrawable::SetTexID( unsigned int value )
{
	_texID = value;
	// Not very efficient, but we're not into rendering yet and this is easier for the moment...
	glBindTexture( GL_TEXTURE_2D, _texID );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &_texWidth );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &_texHeight );
	glBindTexture( GL_TEXTURE_2D, 0 );
	//std::cout<<"INFO: BitmapDrawable dimensions: "<<_texWidth<<" "<<_texHeight<<std::endl;
}

//----------------------------------------------------------------------------------

void ShivaGUI::BitmapDrawable::Inflate( TiXmlElement *xmlElement, ResourceManager *resources )
{
	// TODO: load image, set wrapping repeat / etc

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( std::string( "src" ) == currentAttribute->Name() )
		{
			std::string resourceName( currentAttribute->Value() );

			// TODO: specifying the directory should *really* not be done here
			SetTexID( resources->GetBitmap( std::string( "Resources/Drawables/" ) + resourceName ) );
		}
		else if( ( std::string )( "scaleUp" ) == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				_scaleUp = true;
			else if( value == "false" )
				_scaleUp = false;
		}
		else if( ( std::string )( "keepAspectRatio" ) == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				_keepAspectRatio = true;
			else if( value == "false" )
				_keepAspectRatio = false;
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::BitmapDrawable::Draw()
{
	// TODO: clean all this up ;)
	glEnable( GL_TEXTURE_2D );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glBindTexture( GL_TEXTURE_2D, _texID );
	glColor3f( 1.0f, 1.0f, 1.0f );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	//_testProgram->On();

	glBegin( GL_QUADS );
		glTexCoord2f( 0.0f, 0.0f );	glVertex2f( _boundsLeft, _boundsTop );
		glTexCoord2f( 1.0f, 0.0f );	glVertex2f( _boundsRight, _boundsTop );
		glTexCoord2f( 1.0f, 1.0f );	glVertex2f( _boundsRight, _boundsBottom );
		glTexCoord2f( 0.0f, 1.0f );	glVertex2f( _boundsLeft, _boundsBottom );
	glEnd();

	//_testProgram->Off();

	glDisable( GL_BLEND );

	glDisable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

//----------------------------------------------------------------------------------

void ShivaGUI::BitmapDrawable::OnSetBounds( float left, float top, float right, float bottom, unsigned int gravity )
{
	float width = right - left;
	float height = bottom - top;

	if( _scaleUp )
	{
		if( _keepAspectRatio )
		{
			float horizRatio = width / ( float ) _texWidth;
			float vertRatio = height / ( float ) _texHeight;

			if( horizRatio > vertRatio )
			{
				_boundsTop = top;
				_boundsBottom = bottom;

				_boundsLeft = left + ( width / 2.0f ) - ( ( ( float )_texWidth ) * 0.5f * vertRatio );
				_boundsRight = left + ( width / 2.0f ) + ( ( ( float )_texWidth ) * 0.5f * vertRatio );
			}
			else
			{
				_boundsLeft = left;
				_boundsRight = right;

				_boundsTop = top + ( height / 2.0f ) - ( ( ( float )_texHeight ) * 0.5f * horizRatio );
				_boundsBottom = top + ( height / 2.0f ) + ( ( ( float )_texHeight ) * 0.5f * horizRatio );
			}

		}
		else
		{
			_boundsLeft = left;
			_boundsRight = right;
			_boundsTop = top;
			_boundsBottom = bottom;
		}
	}
	else
	{

		if( width > _texWidth )
		{
			if( ( gravity == Definitions::CENTRE ) ||
				( ( gravity & Definitions::HORIZONTAL_MASK ) == Definitions::CENTRE_HORIZONTAL ) )
			{
				_boundsLeft  = left + ( width / 2.0f ) - ( ( float )_texWidth / 2.0f );
				_boundsRight = right- ( width / 2.0f ) + ( ( float )_texWidth / 2.0f );
			}
			else if( ( gravity & Definitions::HORIZONTAL_MASK ) == Definitions::LEFT )
			{
				_boundsLeft  = left;
				_boundsRight = left + ( ( float )_texWidth );
			}
			else if( ( gravity & Definitions::HORIZONTAL_MASK ) == Definitions::RIGHT )
			{
				_boundsLeft  = right -( ( float )_texWidth );
				_boundsRight = right;
			}
		}
		if( height > _texHeight )
		{
			if( ( gravity == Definitions::CENTRE ) ||
				( ( gravity & Definitions::VERTICAL_MASK ) == Definitions::CENTRE_VERTICAL) )
			{
				_boundsTop   = top  + ( height / 2.0f ) - ( ( float )_texHeight / 2.0f );
				_boundsBottom = bottom - ( height / 2.0f ) + ( ( float )_texHeight / 2.0f );
			}
			else if( ( gravity & Definitions::VERTICAL_MASK ) == Definitions::TOP )
			{
				_boundsTop = top;
				_boundsBottom = top + ( ( float )_texHeight );
			}
			else if( ( gravity & Definitions::VERTICAL_MASK ) == Definitions::BOTTOM )
			{
				_boundsTop = bottom - ( ( float )_texHeight );
				_boundsBottom = bottom;
			}
		}
	}
}

//----------------------------------------------------------------------------------