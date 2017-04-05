#include "GUI/Drawables/BitmapDrawable.h"

//----------------------------------------------------------------------------------

ShivaGUI::BitmapDrawable::BitmapDrawable()
{
	m_texID = 0;
	m_scaleUp = false;
	m_keepAspectRatio = false;
	m_texWidth = m_texHeight = 10;

	// Create VAO for this object
	glGenVertexArrays( 1, &m_vao );

	BuildVBOs();

	m_shader = new Utility::GPUProgram();
	m_shader->Create( "Resources/Shaders/Drawable", Utility::GPUProgram::VERTEX_AND_FRAGMENT );

	m_projMat.identity();
	m_mvMat.identity();
}

//----------------------------------------------------------------------------------

ShivaGUI::BitmapDrawable::BitmapDrawable( unsigned int _OpenGLTexID )
{
	m_scaleUp = false;
	m_keepAspectRatio = false;
	m_texWidth = m_texHeight = 10;
	SetTexID( _OpenGLTexID );

	// Create VAO for this object
	glGenVertexArrays( 1, &m_vao );

	BuildVBOs();

	m_shader = new Utility::GPUProgram();
	m_shader->Create( "Resources/Shaders/Drawable", Utility::GPUProgram::VERTEX_AND_FRAGMENT );

	m_projMat.identity();
	m_mvMat.identity();
}

//----------------------------------------------------------------------------------

ShivaGUI::BitmapDrawable::~BitmapDrawable()
{
	glDeleteVertexArrays( 1, &m_vao );
	delete m_shader;
}

//----------------------------------------------------------------------------------

void ShivaGUI::BitmapDrawable::SetTexID( unsigned int _value )
{
	m_texID = _value;
	// Not very efficient, but we're not into rendering yet and this is easier for the moment...
	glBindTexture( GL_TEXTURE_2D, m_texID );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_texWidth );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_texHeight );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

//----------------------------------------------------------------------------------

void ShivaGUI::BitmapDrawable::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources )
{
	// TODO: load image, set wrapping repeat / etc

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( std::string( "src" ) == currentAttribute->Name() )
		{
			std::string resourceName( currentAttribute->Value() );

			// TODO: specifying the directory should *really* not be done here
			SetTexID( _resources->GetBitmap( std::string( "Resources/Drawables/" ) + resourceName ) );
		}
		else if( ( std::string )( "scaleUp" ) == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				m_scaleUp = true;
			else if( value == "false" )
				m_scaleUp = false;
		}
		else if( ( std::string )( "keepAspectRatio" ) == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				m_keepAspectRatio = true;
			else if( value == "false" )
				m_keepAspectRatio = false;
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::BitmapDrawable::BuildVBOs()
{
	float verts[ 4 * 2 ] = {	m_boundsLeft, m_boundsTop,
								m_boundsRight, m_boundsTop,
								m_boundsRight, m_boundsBottom,
								m_boundsLeft, m_boundsBottom };

	float uvs[ 4 * 2 ] = {	0.0f, 0.0f,
							1.0f, 0.0f,
							1.0f, 1.0f,
							0.0f, 1.0f };

	int indices[ 4 ] = { 0, 1, 2, 3 };

	GLuint vertVBO, uvsVBO, indicesVBO;

	glGenBuffers( 1, &vertVBO );
	glGenBuffers( 1, &uvsVBO );
	glGenBuffers( 1, &indicesVBO );

	glBindBuffer( GL_ARRAY_BUFFER, vertVBO );
	glBufferData( GL_ARRAY_BUFFER, 4 * 2 * sizeof( float ), verts, GL_STATIC_DRAW );
		
	glBindBuffer( GL_ARRAY_BUFFER, uvsVBO );
	glBufferData( GL_ARRAY_BUFFER, 4 * 2 * sizeof( float ), uvs, GL_STATIC_DRAW );
	
	glBindVertexArray( m_vao );

	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, vertVBO );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, ( GLfloat* )NULL );

	glEnableVertexAttribArray( 1 );
	glBindBuffer( GL_ARRAY_BUFFER, uvsVBO );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, ( GLfloat* )NULL );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indicesVBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof( int ), indices, GL_STATIC_DRAW );
		
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

}

//----------------------------------------------------------------------------------

void ShivaGUI::BitmapDrawable::Draw()
{
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//glColor3f( 1.0f, 1.0f, 1.0f );

	m_shader->Bind();

	// Bind texture
	glActiveTexture( GL_TEXTURE0);
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, m_texID );
		
	glUniform1i( glGetUniformLocation( m_shader->GetProgramID(), "tex" ), 0 );

	int viewport[4]; // Shouldn't really do this, but temporarily it's fine
	glGetIntegerv( GL_VIEWPORT, viewport );

	cml::matrix_orthographic_RH( m_projMat, 0.f, ( float )viewport[ 2 ], ( float )viewport[ 3 ], 0.f, -1.f, 1.f, cml::z_clip_neg_one );
	
	LoadMatricesToShader( m_shader->GetProgramID(), m_projMat, m_mvMat );

	glBindVertexArray( m_vao );
		glDrawElements( GL_QUADS, 4, GL_UNSIGNED_INT, ( void* )( 0 )  );
    glBindVertexArray( 0 );

	m_shader->Unbind();

	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

//----------------------------------------------------------------------------------

void ShivaGUI::BitmapDrawable::OnSetBounds( float _left, float _top, float _right, float _bottom,  unsigned int _gravity )
{
	float width	= _right - _left;
	float height = _bottom - _top;

	if( m_scaleUp )
	{
		if( m_keepAspectRatio )
		{
			float horizRatio = width / ( float ) m_texWidth;
			float vertRatio = height / ( float ) m_texHeight;

			if( horizRatio > vertRatio )
			{
				m_boundsTop = _top;
				m_boundsBottom = _bottom;

				m_boundsLeft = _left + ( width / 2.0f ) - ( ( ( float )m_texWidth ) * 0.5f * vertRatio );
				m_boundsRight = _left + ( width / 2.0f ) + ( ( ( float )m_texWidth ) * 0.5f * vertRatio );
			}
			else
			{
				m_boundsLeft = _left;
				m_boundsRight = _right;

				m_boundsTop = _top + ( height / 2.0f ) - ( ( ( float )m_texHeight ) * 0.5f * horizRatio );
				m_boundsBottom = _top + ( height / 2.0f ) + ( ( ( float )m_texHeight ) * 0.5f * horizRatio );
			}
		}
		else
		{
			m_boundsLeft = _left;
			m_boundsRight = _right;
			m_boundsTop = _top;
			m_boundsBottom = _bottom;
		}
	}
	else
	{
		if( width > m_texWidth )
		{
			if( ( _gravity == Definitions::CENTRE ) ||
				( ( _gravity & Definitions::HORIZONTAL_MASK ) == Definitions::CENTRE_HORIZONTAL ) )
			{
				m_boundsLeft  = _left + ( width / 2.0f ) - ( ( float )m_texWidth / 2.0f );
				m_boundsRight = _right- ( width / 2.0f ) + ( ( float )m_texWidth / 2.0f );
			}
			else if( ( _gravity & Definitions::HORIZONTAL_MASK ) == Definitions::LEFT )
			{
				m_boundsLeft  = _left;
				m_boundsRight = _left + ( ( float )m_texWidth );
			}
			else if( ( _gravity & Definitions::HORIZONTAL_MASK ) == Definitions::RIGHT )
			{
				m_boundsLeft  = _right -( ( float )m_texWidth );
				m_boundsRight = _right;
			}
		}
		if( height > m_texHeight )
		{
			if( ( _gravity == Definitions::CENTRE ) ||
				( ( _gravity & Definitions::VERTICAL_MASK ) == Definitions::CENTRE_VERTICAL) )
			{
				m_boundsTop		= _top  + ( height / 2.0f ) - ( ( float )m_texHeight / 2.0f );
				m_boundsBottom	= _bottom - ( height / 2.0f ) + ( ( float )m_texHeight / 2.0f );
			}
			else if( ( _gravity & Definitions::VERTICAL_MASK ) == Definitions::TOP )
			{
				m_boundsTop		= _top;
				m_boundsBottom	= _top + ( ( float )m_texHeight );
			}
			else if( ( _gravity & Definitions::VERTICAL_MASK ) == Definitions::BOTTOM )
			{
				m_boundsTop		= _bottom - ( ( float )m_texHeight );
				m_boundsBottom	= _bottom;
			}
		}
	}

	BuildVBOs();
}

//----------------------------------------------------------------------------------
