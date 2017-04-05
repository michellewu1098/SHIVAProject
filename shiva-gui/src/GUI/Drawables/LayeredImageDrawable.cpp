#include "GUI/Drawables/LayeredImageDrawable.h"

//----------------------------------------------------------------------------------

ShivaGUI::LayeredImageDrawable::LayeredImageDrawable()
{
	m_maxLayerGroups = 4;
	m_scaleUp = true;
	m_isNinePatch = false;
	m_texWidth = m_texHeight = 10;
	m_keepAspectRatio = false;

	m_usingText = false;
	m_nLayers = 0;
	
	m_centreLeftProp = m_centreRightProp = m_centreTopProp = m_centreBottomProp = 0.0f;
	m_centreLeftBounds = m_centreRightBounds = m_centreTopBounds = m_centreBottomBounds = 0.0f;

	m_contentLeftProp = 0.0f;
	m_contentRightProp = 1.0f;
	m_contentTopProp = 0.0f;
	m_contentBottomProp = 1.0f;
	m_contentLeftBounds = m_contentRightBounds = m_contentTopBounds = m_contentBottomBounds = 0.0f;

	// Create VAO for this object
	glGenVertexArrays( 1, &m_vao );

	BuildVBOs();

	m_shader = new Utility::GPUProgram();
	m_shader->Create( "Resources/Shaders/LayeredImageDrawable", Utility::GPUProgram::VERTEX_AND_FRAGMENT );

	m_layerGroups = new LayerGroup[ m_maxLayerGroups ];
	for( unsigned int i = 0; i < m_maxLayerGroups; ++i )
	{
		m_layerGroups[ i ].Init( i, m_shader->GetProgramID() );
	}

	m_projMat.identity();
	m_mvMat.identity();
}

//----------------------------------------------------------------------------------

ShivaGUI::LayeredImageDrawable::~LayeredImageDrawable()
{
	glDeleteVertexArrays( 1, &m_vao );
	delete m_shader;
	delete [] m_layerGroups;
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayeredImageDrawable::AddTextLayer( unsigned int _fontColour )
{
	// Add extra layer with text
	if( m_usingText )
	{
		SetTexID( m_textTextureID, m_nLayers ); 
		SetLayerColour( 1, m_nLayers, 0 );
		SetLayerColour( 1, m_nLayers, 1 );
		SetLayerColour( 1, m_nLayers, 2 );
		SetLayerColour( _fontColour, m_nLayers, 3 );
		m_usingText = false;
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayeredImageDrawable::SetTexID( unsigned int _OpenGLTexID, unsigned int _layerGroup )
{
	if( _layerGroup >= m_maxLayerGroups )
	{
		std::cerr << "WARNING: LayeredImageDrawable given layer group >= _maxLayerGroups" << std::endl;
		return;
	}

	m_layerGroups[ _layerGroup ].SetGLTexID( _OpenGLTexID );

	// Not very efficient, but we're not into rendering yet and this is easier for the moment...
	glBindTexture( GL_TEXTURE_2D, _OpenGLTexID );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_texWidth );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_texHeight );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayeredImageDrawable::GetContentBounds( float &_left, float &_top, float &_right, float &_bottom )
{
	_left	= m_contentLeftBounds;
	_right	= m_contentRightBounds;
	_top	= m_contentTopBounds;
	_bottom	= m_contentBottomBounds;
}

//----------------------------------------------------------------------------------

int ShivaGUI::LayeredImageDrawable::GetNativeWidthFromContent( int _contentWidth )
{
	return ( int )( ( ( m_contentLeftProp + ( 1.0f - m_contentRightProp ) ) * ( float )m_texWidth ) + _contentWidth );
}

//----------------------------------------------------------------------------------

int ShivaGUI::LayeredImageDrawable::GetNativeHeightFromContent( int _contentHeight )
{
	return ( int )( ( ( m_contentTopProp + ( 1.0f - m_contentBottomProp ) ) * ( float )m_texHeight ) + _contentHeight );
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayeredImageDrawable::SetLayerColour( std::string _colourString, unsigned int _group, unsigned int _layer )
{
	unsigned int fontColour = 0;
	std::stringstream colourStream;
	colourStream << std::hex << _colourString;
	colourStream >> fontColour;
	SetLayerColour( fontColour, _group, _layer );
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayeredImageDrawable::SetLayerColour( unsigned int _colour, unsigned int _group, unsigned int _layer )
{
	if( _group >= ( m_maxLayerGroups ) )
	{
		std::cerr << "WARNING: LayeredImageDrawable given layer group >= _maxLayerGroups" << std::endl;
		return;
	}
	if( _layer >= 4 )
	{
		std::cerr << "WARNING: LayeredImageDrawable given layer >= 4" << std::endl;
		return;
	}
	m_layerGroups[ _group ].SetLayerColour( _colour, _layer );
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayeredImageDrawable::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources )
{

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( ( std::string )( "scaleUp" ) == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				m_scaleUp = true;
			else if( value == "false" )
				m_scaleUp = false;
		}
		else if( ( std::string )( "isNinePatch" ) == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				m_isNinePatch = true;
			else if( value == "false" )
				m_isNinePatch = false;
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
		else if( currentAttribute->Name() == std::string( "content_bottom" ) )
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
		else if( ( std::string )( "keepAspectRatio" ) == currentAttribute->Name() )
		{
			std::string value( currentAttribute->Value() );

			if( value == "true" )
				m_keepAspectRatio = true;
			else if( value == "false" )
				m_keepAspectRatio = false;
		}
	}

	// Each layer is a child of the LayeredImageDrawable element
	// Each layer child contains its properties as attributes

	if( m_usingText ) {
		_resources->CreateTextureFromText( true );
		_resources->SetExtraSpace( true );
	}

	unsigned int imageLayerGroup = 0;
	
	for( TiXmlElement *currentChild = _xmlElement->FirstChildElement( "Layer" ); currentChild != NULL; currentChild = currentChild->NextSiblingElement( "Layer" ) )
	{
		for( TiXmlAttribute *currentAttribute = currentChild->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
		{
			if( std::string( "src" ) == currentAttribute->Name() )
			{
				std::string resourceName( currentAttribute->Value() );

				// TODO: specifying the directory should *really* not be done here

				if( m_usingText ) {
					SetTexID( _resources->GetBitmap( std::string( "Resources/Drawables/" ) + resourceName ), imageLayerGroup );
					if( _resources->IsCreatingText() )
					{
						size_t lastdot = resourceName.find_last_of( "." );
						if( lastdot != std::string::npos )
						{	
							resourceName = resourceName.substr( 0, lastdot );
						}
						//m_textTextureID = _resources->GetTexture( std::string( "Text/Resources/Drawables/" ) + resourceName );

						m_textTextureID = _resources->GetTexture(  std::string( "Resources/Drawables/" ) + resourceName + std::string( "_Text" ) );
						if( !m_textTextureID )
						{
							std::cerr << "ERROR: Resource Manager didn't create the text texture." << std::endl;
						}
					}
				}
				else
					SetTexID( _resources->GetBitmap( std::string( "Resources/Drawables/" ) + resourceName ), imageLayerGroup );

			}
			else if( std::string( "colour0" ) == currentAttribute->Name() )
			{
				SetLayerColour( _resources->GetInflationAttribute( currentAttribute->Value() ), imageLayerGroup, 0 );
			}
			else if( std::string( "colour1" ) == currentAttribute->Name() )
			{
				SetLayerColour( _resources->GetInflationAttribute( currentAttribute->Value() ), imageLayerGroup, 1 );
			}
			else if( std::string( "colour2" ) == currentAttribute->Name() )
			{
				SetLayerColour( _resources->GetInflationAttribute( currentAttribute->Value() ), imageLayerGroup, 2 );
			}
			else if( std::string( "colour3" ) == currentAttribute->Name() )
			{
				SetLayerColour( _resources->GetInflationAttribute( currentAttribute->Value() ), imageLayerGroup, 3 );
			}
		}
		++imageLayerGroup;
		
		if( m_usingText )
			_resources->CreateTextureFromText( false );
	}
	
	if( m_usingText )
	{
		_resources->SetExtraSpace( false );
		_resources->ClearText();
		m_nLayers = imageLayerGroup;	
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayeredImageDrawable::Draw()
{
	glDisable( GL_DEPTH_TEST );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	

	glColor4f( 0.f, 1.f, 0.f, 1.f );
	m_shader->Bind();

	int viewport[4]; // Shouldn't really do this, but temporarily it's fine
	glGetIntegerv( GL_VIEWPORT, viewport );

	cml::matrix_orthographic_RH( m_projMat, 0.f, ( float )viewport[ 2 ], ( float )viewport[ 3 ], 0.f, -1.f, 1.f, cml::z_clip_neg_one );
	LoadMatricesToShader( m_shader->GetProgramID(), m_projMat, m_mvMat );

	for( unsigned int i = 0; i < m_maxLayerGroups; i++ )
	{
		m_layerGroups[ i ].Bind();
	}

	glBindVertexArray( m_vao );
    
		if( m_isNinePatch )
			glDrawElements( GL_QUADS, 9 * 4, GL_UNSIGNED_INT, ( void* )( 0 )  );
		else
			glDrawElements( GL_QUADS, 4, GL_UNSIGNED_INT, ( void* )( 0 )  );

    glBindVertexArray( 0 );

	m_shader->Unbind();

	glDisable( GL_BLEND );

	for( unsigned int i = 0; i < m_maxLayerGroups; i++ )
	{
		m_layerGroups[ i ].Unbind();
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayeredImageDrawable::OnSetBounds( float _left, float _top, float _right, float _bottom, unsigned int _gravity )
{
	float width = _right - _left;
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

		if( m_isNinePatch )
		{
			m_contentLeftBounds  = _left + ( m_contentLeftProp * m_texWidth );
			m_contentRightBounds = _right - ( ( 1.0f - m_contentRightProp ) * m_texWidth );
			m_centreLeftBounds   = _left + ( m_centreLeftProp * m_texWidth );
			m_centreRightBounds  = _right - ( ( 1.0f - m_centreRightProp ) * m_texWidth );

			m_contentTopBounds    = _top  + ( m_contentTopProp * m_texHeight );
			m_contentBottomBounds = _bottom - ( ( 1.0f - m_contentBottomProp ) * m_texHeight );
			m_centreTopBounds     = _top  + ( m_centreTopProp * m_texHeight );
			m_centreBottomBounds  = _bottom  - ( ( 1.0f - m_centreBottomProp ) * m_texHeight );

			if( m_centreBottomBounds < m_centreTopBounds )
			{
				float average = ( m_centreBottomBounds + m_centreTopBounds ) / 2.0f;
				m_centreBottomBounds = m_centreTopBounds = average;
			}

			if( m_centreRightBounds < m_centreLeftBounds )
			{
				float average = ( m_centreRightBounds + m_centreLeftBounds ) / 2.0f;
				m_centreRightBounds = m_centreLeftBounds = average;
			}
		}
	}
	else
	{
		if( width > m_texWidth )
		{
			if( ( _gravity == Definitions::CENTRE ) ||
				( ( _gravity & Definitions::HORIZONTAL_MASK ) == Definitions::CENTRE_HORIZONTAL ) )
			{
				m_boundsLeft  = _left  + ( width / 2.0f ) - ( ( float )m_texWidth / 2.0f );
				m_boundsRight = _right - ( width / 2.0f ) + ( ( float )m_texWidth / 2.0f );
			}
			else if( ( _gravity & Definitions::HORIZONTAL_MASK ) == Definitions::LEFT )
			{
				m_boundsLeft  = _left;
				m_boundsRight = _left + ( ( float )m_texWidth );
			}
			else if( ( _gravity & Definitions::HORIZONTAL_MASK ) == Definitions::RIGHT )
			{
				m_boundsLeft  = _right - ( ( float )m_texWidth );
				m_boundsRight = _right;
			}
		}
		if( height > m_texHeight )
		{
			if( ( _gravity == Definitions::CENTRE ) ||
				( ( _gravity & Definitions::VERTICAL_MASK ) == Definitions::CENTRE_VERTICAL ) )
			{
				m_boundsTop    =    _top + ( height / 2.0f ) - ( ( float )m_texHeight / 2.0f );
				m_boundsBottom = _bottom - ( height / 2.0f ) + ( ( float )m_texHeight / 2.0f );
			}
			else if( ( _gravity & Definitions::VERTICAL_MASK ) == Definitions::TOP )
			{
				m_boundsTop = _top;
				m_boundsBottom = _top + ( ( float )m_texHeight );
			}
			else if( ( _gravity & Definitions::VERTICAL_MASK ) == Definitions::BOTTOM )
			{
				m_boundsTop = _bottom - ( ( float )m_texHeight );
				m_boundsBottom = _bottom;
			}
		}
	}
	
	BuildVBOs();
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayeredImageDrawable::BuildVBOs()
{
	if( m_isNinePatch )
	{
		float verts[ 16 * 2 ] = { m_boundsLeft, m_boundsTop,
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
	else
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
}

//----------------------------------------------------------------------------------

ShivaGUI::LayeredImageDrawable::LayerGroup::LayerGroup()
{
	m_texID = 0;
	m_colourUniforms = new int[ 4 ];
	m_layerColours = new float[ 4 * 4 ];
	m_groupNumber = 0;
	m_textureUniform = -1;
	for( unsigned int i = 0; i < 4; ++i )
	{
		m_colourUniforms[ i ] = -1;
		for( unsigned int j = 0; j < 4; ++j )
			m_layerColours[ i * 4 + j ] = 0;
	}
}

//----------------------------------------------------------------------------------

ShivaGUI::LayeredImageDrawable::LayerGroup::~LayerGroup()
{
	delete [] m_colourUniforms;
	delete [] m_layerColours;
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayeredImageDrawable::LayerGroup::SetGLTexID( unsigned int _texID )
{ 
	m_texID = _texID; 
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayeredImageDrawable::LayerGroup::Init( unsigned int _groupNumber, unsigned int _GLSLProgID )
{
	m_groupNumber = _groupNumber;
	std::stringstream texName;
	texName << "ImageLayer" << _groupNumber;
	m_textureUniform = glGetUniformLocation( _GLSLProgID, texName.str().c_str() );

	if( m_textureUniform < 0 )
		std::cout << "WARNING: LayerGroup::Init _textureUniform " << texName.str().c_str() << " = " << m_textureUniform << std::endl;

	for( unsigned int j = 0; j < 4; j++ )
	{
		std::stringstream temp;
		temp << "LayerColour" << _groupNumber << "_" << j;
		m_colourUniforms[ j ] = glGetUniformLocation( _GLSLProgID, temp.str().c_str() );
		if( m_colourUniforms[ j ] < 0 )
		{
			std::cerr << "WARNING: LayerGroup Init could not find GLSL uniform: " << temp.str().c_str() << std::endl;
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayeredImageDrawable::LayerGroup::SetLayerColour( unsigned int _colour, unsigned int _layer )
{
	m_layerColours[ _layer * 4 + 0 ] = ( ( float )( ( _colour & 0xFF000000 ) >> 24 ) ) / 255.0f;
	m_layerColours[ _layer * 4 + 1 ] = ( ( float )( ( _colour & 0x00FF0000 ) >> 16 ) ) / 255.0f;
	m_layerColours[ _layer * 4 + 2 ] = ( ( float )( ( _colour & 0x0000FF00 ) >> 8  ) ) / 255.0f;
	m_layerColours[ _layer * 4 + 3 ] = ( ( float )( _colour & 0x000000FF ) ) / 255.0f;

	//std::cout << "INFO: LayeredImageDrawable setting layer colours to: " << m_layerColours[ layer * 4 + 0 ] << " " << m_layerColours[ layer * 4 + 1 ] << " " << m_layerColours[ layer * 4 + 2 ] << " " << m_layerColours[ layer * 4 + 3 ] << std::endl;
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayeredImageDrawable::LayerGroup::Bind()
{
	if( m_texID != 0 )
	{
		glActiveTexture( GL_TEXTURE0 + m_groupNumber );
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, m_texID );

		if( m_textureUniform >= 0 )
		{
			glUniform1i( m_textureUniform, m_groupNumber );
		}
	}
	else
	{
		glActiveTexture( GL_TEXTURE0 + m_groupNumber );
		glDisable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, 0 );
	}

	for( unsigned int currentLayer = 0; currentLayer < 4; currentLayer++ )
	{
		if( m_colourUniforms[ currentLayer ] >= 0 )
		{
			glUniform4fv( m_colourUniforms[ currentLayer ], 1, &( m_layerColours[ currentLayer * 4 ] ) );
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::LayeredImageDrawable::LayerGroup::Unbind()
{
	glActiveTexture( GL_TEXTURE0 + m_groupNumber );
	glDisable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

//----------------------------------------------------------------------------------