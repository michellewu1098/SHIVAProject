#include "GUI/Drawables/RectDrawable.h"

//----------------------------------------------------------------------------------

ShivaGUI::RectDrawable::RectDrawable()
{
	m_fillR = m_fillG = m_fillB = m_fillA = 1.0f;
	m_borderR = m_borderG = m_borderB = m_borderA = 0.0f;

	m_fillShader = new Utility::GPUProgram();
	m_fillShader->Create( "Resources/Shaders/Colour", Utility::GPUProgram::VERTEX_AND_FRAGMENT );

	m_borderShader = new Utility::GPUProgram();
	m_borderShader->Create( "Resources/Shaders/Colour", Utility::GPUProgram::VERTEX_AND_FRAGMENT );
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

void ShivaGUI::RectDrawable::BuildVBOs()
{
	float rectVertices[ 4 * 3 ] = { m_boundsLeft, m_boundsBottom, 0.f,
									m_boundsRight, m_boundsBottom, 0.f,
									m_boundsRight, m_boundsTop, 0.f,
									m_boundsLeft, m_boundsTop, 0.f };
	
	GLuint rectIndices[ 6 ] = {	0, 1, 2,
								0, 2, 3 };

	GLuint vertsVBO;

	glGenBuffers( 1, &vertsVBO );
	glBindBuffer( GL_ARRAY_BUFFER, vertsVBO );
	glBufferData( GL_ARRAY_BUFFER, 4 * 3 * sizeof( float ), rectVertices, GL_STATIC_DRAW );
	
	glGenVertexArrays( 1, &m_rectFillVAO );
	glBindVertexArray( m_rectFillVAO );

	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, vertsVBO );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ( GLfloat* ) NULL );

	GLuint indicesVBO;
	glGenBuffers( 1, &indicesVBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indicesVBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof( GLuint ), rectIndices, GL_STATIC_DRAW );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	glGenVertexArrays( 1, &m_rectBorderVAO );
	glBindVertexArray( m_rectBorderVAO );

	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, vertsVBO );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ( GLfloat* ) NULL );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

//----------------------------------------------------------------------------------

void ShivaGUI::RectDrawable::Draw()
{

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// TODO: change from immediate mode
	BuildVBOs();

	m_fillShader->Bind();
	glUniform4f( glGetUniformLocation( m_fillShader->GetProgramID(), "u_Colour" ), m_fillR, m_fillG, m_fillB, m_fillA );
	
	glBindVertexArray( m_rectFillVAO );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
	glBindVertexArray( 0 );

	m_fillShader->Unbind();


	m_borderShader->Bind();

	glUniform4f( glGetUniformLocation( m_borderShader->GetProgramID(), "u_Colour" ), m_borderR, m_borderG, m_borderB, m_borderA );

	glLineWidth( 5.0f );
	glBindVertexArray( m_rectBorderVAO );
		glDrawArrays( GL_LINE_LOOP, 0, 4 );
	glBindVertexArray( 0 );

	m_borderShader->Unbind();

	glDisable( GL_BLEND );

	glDisable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );
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