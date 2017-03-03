#include "GUI/Drawables/NinePatch.h"

//----------------------------------------------------------------------------------

ShivaGUI::NinePatch::NinePatch()
{
	m_contentLeftProp = 0.0f;
	m_contentRightProp = 1.0f;
	m_contentTopProp = 0.0f;
	m_contentBottomProp = 1.0f;
	m_centreLeftBounds = m_centreRightBounds = m_centreTopBounds = m_centreBottomBounds = 0.0f;
	m_fixedX = m_fixedY = false;

	glGenVertexArrays( 1, &m_vao );

	BuildVBOs();

	m_shader = new Utility::GPUProgram();
	m_shader->Create( "Resources/Shaders/Drawable", Utility::GPUProgram::VERTEX_AND_FRAGMENT );
	
	m_projMat.identity();
	m_mvMat.identity();
}

//----------------------------------------------------------------------------------

ShivaGUI::NinePatch::~NinePatch()
{
	glDeleteVertexArrays( 1, &m_vao );
	delete m_shader;
}

//----------------------------------------------------------------------------------

void ShivaGUI::NinePatch::SetTexID( unsigned int _value )
{
	m_texID = _value;
	// Not very efficient, but we're not into rendering yet and this is easier for the moment...
	glBindTexture( GL_TEXTURE_2D, m_texID );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_texWidth );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_texHeight );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

//----------------------------------------------------------------------------------

void ShivaGUI::NinePatch::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources )
{

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( currentAttribute->Name() == std::string( "src" ) )
		{
			std::string filename( currentAttribute->Value() );

			// TODO: specifying the directory should *really* not be done here
			SetTexID( _resources->GetBitmap( std::string( "Resources/Drawables/" ) + filename ) );
		}

		else if( currentAttribute->Name() == std::string( "content_left" ) )
		{
			float value = ( float ) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			m_contentLeftProp = value;
		}
		else if( currentAttribute->Name() == std::string( "content_right" ) )
		{
			float value = ( float ) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			m_contentRightProp = value;
		}
		else if( currentAttribute->Name() == std::string( "content_top" ) )
		{
			float value = ( float ) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			m_contentTopProp = value;
		}
		else if( currentAttribute->Name() == std::string("content_bottom" ) )
		{
			float value = ( float ) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			m_contentBottomProp = value;
		}

		else if( currentAttribute->Name() == std::string( "centre_left" ) )
		{
			float value = ( float ) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			m_centreLeftProp = value;
		}
		else if( currentAttribute->Name() == std::string( "centre_right" ) )
		{
			float value = ( float ) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			m_centreRightProp = value;
		}
		else if( currentAttribute->Name() == std::string( "centre_top" ) )
		{
			float value = ( float ) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			m_centreTopProp = value;
		}
		else if( currentAttribute->Name() == std::string( "centre_bottom" ) )
		{
			float value = ( float ) currentAttribute->DoubleValue();
			if( value < 0.0f )
				value = 0.0f;
			else if( value > 1.0f )
				value = 1.0f;
			m_centreBottomProp = value;
		}

		else if( ( std::string )( "fixedX" ) == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				m_fixedX = true;
			else if( value == "false" )
				m_fixedX = false;
		}
		else if( ( std::string )( "fixedY" ) == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				m_fixedY = true;
			else if( value == "false" )
				m_fixedY = false;
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::NinePatch::GetContentBounds( float &_left, float &_top, float &_right, float &_bottom )
{
	_left	= m_contentLeftBounds;
	_right	= m_contentRightBounds;
	_top	= m_contentTopBounds;
	_bottom	= m_contentBottomBounds;
}

//----------------------------------------------------------------------------------

int ShivaGUI::NinePatch::GetNativeWidthFromContent( int _contentWidth )
{
	// is multiplying by the texWidth wrong??
	int value = ( int )( ( m_contentLeftProp + ( 1.0f - m_contentRightProp ) ) * ( float ) m_texWidth ) + _contentWidth;
	//std::cout<<"INFO: NinePatch::GetNativeWidthFromContent, contentWidth = "<<contentWidth<<" result = "<< value<<std::endl;
	return value;
}

//----------------------------------------------------------------------------------

int ShivaGUI::NinePatch::GetNativeHeightFromContent( int _contentHeight )
{
	int value = ( int )( ( m_contentTopProp + ( 1.0f - m_contentBottomProp ) ) * ( float ) m_texHeight ) + _contentHeight;
	//std::cout<<"INFO: NinePatch::GetNativeHeightFromContent, _contentTopProp = "<<_contentTopProp<<" _contentBottomProp = "<< _contentBottomProp<<" texHeight = "<<_texHeight<<" contentHeight = "<<contentHeight<<" result = "<< value<<std::endl;
	return value;
}

//----------------------------------------------------------------------------------

void ShivaGUI::NinePatch::BuildVBOs()
{
	float verts[ 16 * 2 ] = {	m_boundsLeft, m_boundsTop,
								m_centreLeftBounds, m_boundsTop,
								m_centreRightBounds, m_boundsTop,
								m_boundsRight, m_boundsTop,

								m_boundsLeft, m_centreTopBounds,
								m_centreLeftBounds, m_centreTopBounds,
								m_centreRightBounds, m_centreTopBounds,
								m_boundsRight, m_centreTopBounds,
									 
								m_boundsLeft, m_centreBottomBounds,
								m_centreLeftBounds, m_centreBottomBounds,
								m_centreRightBounds, m_centreBottomBounds,
								m_boundsRight, m_centreBottomBounds,
									 
								m_boundsLeft, m_boundsBottom,
								m_centreLeftBounds, m_boundsBottom,
								m_centreRightBounds, m_boundsBottom,
								m_boundsRight, m_boundsBottom };

		float uvs[ 16 * 2 ] = { 0.0f, 0.0f,
								m_centreLeftProp, 0.0f,
								m_centreRightProp, 0.0f,
								1.0f, 0.0f,

								0.0f, m_centreTopProp,
								m_centreLeftProp, m_centreTopProp,
								m_centreRightProp, m_centreTopProp,
								1.0f, m_centreTopProp,
									 
								0.0f, m_centreBottomProp,
								m_centreLeftProp, m_centreBottomProp,
								m_centreRightProp, m_centreBottomProp,
								1.0f, m_centreBottomProp,
									 
								0.0f, 1.0f,
								m_centreLeftProp, 1.0f,
								m_centreRightProp, 1.0f,
								1.0f, 1.0f };
		
		int indices[ 9 * 4 ] = {  0,  1,  5,  4,
								  1,  2,  6,  5,
								  2,  3,  7,  6,
								  4,  5,  9,  8,
								  5,  6, 10,  9,
								  6,  7, 11, 10,
								  8,  9, 13, 12,
								  9, 10, 14, 13,
								  10, 11, 15, 14 };

		GLuint vertVBO, uvsVBO, indicesVBO;

		glGenBuffers( 1, &vertVBO );
		glGenBuffers( 1, &uvsVBO );
		glGenBuffers( 1, &indicesVBO );

		glBindBuffer( GL_ARRAY_BUFFER, vertVBO );
		glBufferData( GL_ARRAY_BUFFER, 16 * 2 * sizeof( float ), verts, GL_STATIC_DRAW );
		
		glBindBuffer( GL_ARRAY_BUFFER, uvsVBO );
		glBufferData( GL_ARRAY_BUFFER, 16 * 2 * sizeof( float ), uvs, GL_STATIC_DRAW );
		
		glBindVertexArray( m_vao );

		glEnableVertexAttribArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, vertVBO );
		glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, ( GLfloat* )NULL );

		glEnableVertexAttribArray( 1 );
		glBindBuffer( GL_ARRAY_BUFFER, uvsVBO );
		glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, ( GLfloat* )NULL );

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indicesVBO );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 9 * 4 * sizeof( int ), indices, GL_STATIC_DRAW );

		glBindVertexArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

//----------------------------------------------------------------------------------

void ShivaGUI::NinePatch::Draw()
{
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glColor3f( 1.0f, 1.0f, 1.0f );

	m_shader->Bind();

	glActiveTexture( GL_TEXTURE0 );
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, m_texID );

	glUniform1i( glGetUniformLocation( m_shader->GetProgramID(), "tex" ), 0 );

	int viewport[4]; // Shouldn't really do this, but temporarily it's fine
	glGetIntegerv( GL_VIEWPORT, viewport );

	cml::matrix_orthographic_RH( m_projMat, 0.f, ( float )viewport[ 2 ], ( float )viewport[ 3 ], 0.f, -1.f, 1.f, cml::z_clip_neg_one );
	LoadMatricesToShader( m_shader->GetProgramID(), m_projMat, m_mvMat );

	glBindVertexArray( m_vao );
		glDrawElements( GL_QUADS, 9 * 4, GL_UNSIGNED_INT, ( void* )( 0 )  );
	glBindVertexArray( 0 );

	m_shader->Unbind();

	glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

//----------------------------------------------------------------------------------

void ShivaGUI::NinePatch::OnSetBounds( float _left, float _top, float _right, float _bottom, unsigned int _gravity )
{
	float height = _bottom - _top;

	if( m_fixedX )
	{
		m_contentLeftBounds  = _left + ( m_contentLeftProp * m_texWidth );
		m_contentRightBounds = _right - ( ( 1.0f - m_contentRightProp ) * m_texWidth );
		m_centreLeftBounds	 = _left + ( m_centreLeftProp * m_texWidth );
		m_centreRightBounds  = _right - ( ( 1.0f - m_centreRightProp) * m_texWidth );
	}
	else
	{
		m_contentLeftBounds  = _left + ( m_contentLeftProp * m_texWidth );
		m_contentRightBounds = _right - ( ( 1.0f - m_contentRightProp ) * m_texWidth );
		m_centreLeftBounds   = _left + ( m_centreLeftProp * m_texWidth );
		m_centreRightBounds  = _right - ( ( 1.0f - m_centreRightProp ) * m_texWidth );
	}

	if( m_fixedY )
	{
		m_contentTopBounds		= _top  + ( height / 2.0f ) - ( ( float ) m_texHeight / 2.0f );
		m_contentBottomBounds	= _bottom - ( height / 2.0f ) + ( ( float ) m_texHeight / 2.0f );
		m_contentBottomBounds	= _bottom - ( ( 1.0f - m_contentBottomProp ) * m_texHeight );
		m_centreTopBounds		= m_contentTopBounds  + ( m_centreTopProp * m_texHeight / 2.0f );
		m_centreBottomBounds	= m_contentBottomBounds - ( ( 1.0f - m_centreBottomProp ) * m_texHeight / 2.0f );
	}
	else
	{
		m_contentTopBounds		= _top  + ( m_contentTopProp * m_texHeight );
		m_contentBottomBounds	= _bottom - ( ( 1.0f - m_contentBottomProp ) * m_texHeight );
		m_centreTopBounds		= _top  + ( m_centreTopProp * m_texHeight );
		m_centreBottomBounds	= _bottom - ( ( 1.0f - m_centreBottomProp ) * m_texHeight );
	}

	if( m_centreBottomBounds < m_centreTopBounds )
	{
		float average = ( m_centreBottomBounds + m_centreTopBounds ) / 2.0f;
		m_centreBottomBounds = m_centreTopBounds = average;
	}

	BuildVBOs();
}

//----------------------------------------------------------------------------------