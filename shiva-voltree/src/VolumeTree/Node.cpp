#include "VolumeTree/Node.h"
#include "VolumeRenderer/GLSLRenderer.h"

//----------------------------------------------------------------------------------

VolumeTree::Node::Node()
{
	m_drawBBox = false;
	m_hasBBox = false; 

	m_nContext = 1;

	m_useCache = false;
	m_requiresCache = false;
	m_cacheNumber = -1;
	m_cacheResX = m_cacheResY = m_cacheResZ = 0;
	m_cacheOffsetX = m_cacheOffsetY = m_cacheOffsetZ = 0.0f;
	m_cacheScaleX = m_cacheScaleY = m_cacheScaleZ = 1.0f;

	m_cacheBuilt = false;
	m_cacheDirty = false;
}

//----------------------------------------------------------------------------------

VolumeTree::Node::~Node()
{
	if( m_hasBBox )
	{
		delete m_bboxLinesShader;
		delete m_bboxSidesShader;
	}
	
}

//----------------------------------------------------------------------------------

void VolumeTree::Node::SetUseCache( bool _useCache, unsigned int _cacheID, unsigned int _cacheResX, unsigned int _cacheResY, unsigned int _cacheResZ )
{
	m_cacheDirty = m_cacheDirty || ( _cacheID != m_cacheNumber || m_cacheResX != _cacheResX || m_cacheResY != _cacheResY || m_cacheResZ != _cacheResZ );
	m_useCache = _useCache;
	m_cacheNumber = _cacheID;
	m_cacheResX = _cacheResX;
	m_cacheResY = _cacheResY;
	m_cacheResZ = _cacheResZ;
}

//----------------------------------------------------------------------------------

void VolumeTree::Node::SetDrawBBox( const bool &_value )
{
	m_drawBBox = _value;
}

//----------------------------------------------------------------------------------

void VolumeTree::Node::BuildCaches( GLSLRenderer *_renderer )
{
	if( m_useCache && ( m_cacheNumber >= 0 ) )
	{
		if( ( !m_cacheBuilt ) || m_cacheDirty )
		{
		// Build cache

			// If we have been told to cache the node we will already have been told our resolution
			float boundsX, boundsY, boundsZ;
			GetBoundSizes( &boundsX, &boundsY, &boundsZ );
			unsigned int volX = m_cacheResX, volY = m_cacheResY, volZ = m_cacheResZ;

			//_cacheScaleX = boundsX;
			//_cacheScaleY = boundsY;
			//_cacheScaleZ = boundsZ;

			// Allocate memory
			float *cacheData = new float[ volX * volY * volZ ];

			// Build cache
				// TODO: based on actual bounding box
			
			float minX, maxX, minY, maxY, minZ, maxZ;
			GetBounds( &minX, &maxX, &minY, &maxY, &minZ, &maxZ );

			
			m_cacheOffsetX = -( minX + ( boundsX * 0.5f ) );
			m_cacheOffsetY = -( minY + ( boundsY * 0.5f ) );
			m_cacheOffsetZ = -( minZ + ( boundsZ * 0.5f ) );
			
			m_cacheScaleX = 1.0f / boundsX;
			m_cacheScaleY = 1.0f / boundsY;
			m_cacheScaleZ = 1.0f / boundsZ;


			float startX = minX, startY = minY, startZ = minZ;
			float stepX = boundsX / ( float )volX, stepY = boundsY / ( float )volY, stepZ = boundsZ / ( float )volZ;
			
			PopulateCacheData( &cacheData, startX, startY, startZ, stepX, stepY, stepZ );

			// Pass to renderer
			_renderer->FillCache( m_cacheNumber, cacheData, volX, volY, volZ );

			// Free memory
			delete [] cacheData;

			m_cacheBuilt = true;
			m_cacheDirty = false;
		}
	}
	else
	{
		OnBuildParameters( _renderer );

		for( Node *currentChild = GetFirstChild(); currentChild != NULL; currentChild = GetNextChild( currentChild ) )
		{
			currentChild->BuildCaches( _renderer );
		}
	}
}

//----------------------------------------------------------------------------------

void VolumeTree::Node::BuildBBoxesVBOs( unsigned int _nContext )
{
	float minX, maxX, minY, maxY, minZ, maxZ;
	GetBounds( &minX, &maxX, &minY, &maxY, &minZ, &maxZ );
	float offset = 0.03f;
	minX -= offset; minY -= offset; minZ -= offset;
	maxX += offset; maxY += offset; maxZ += offset;

	// Bounding box outlines

	float cubeVertices[ 24 * 3 ] = {	minX, minY, minZ,	maxX, minY, minZ, // Red lines
										minX, maxY, minZ,	maxX, maxY, minZ,
										minX, minY, maxZ,	maxX, minY, maxZ,
										minX, maxY, maxZ,	maxX, maxY, maxZ,

										minX, minY, minZ,	minX, maxY, minZ, // Green lines
										maxX, minY, minZ,	maxX, maxY, minZ,
										minX, minY,	maxZ,	minX, maxY, maxZ,
										maxX, minY, maxZ,	maxX, maxY, maxZ,

										minX, minY, minZ,	minX, minY, maxZ, // Blue lines
										minX, maxY, minZ,	minX, maxY, maxZ,
										maxX, minY, minZ,	maxX, minY, maxZ,
										maxX, maxY, minZ,	maxX, maxY, maxZ };

	float cubeColours[ 24 * 3 ] = {	1.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f, // Red
									1.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,
									1.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,
									1.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,

									0.0f, 1.0f, 0.0f, 	0.0f, 1.0f, 0.0f, // Green
									0.0f, 1.0f, 0.0f,	0.0f, 1.0f, 0.0f,
									0.0f, 1.0f, 0.0f, 	0.0f, 1.0f, 0.0f, 
									0.0f, 1.0f, 0.0f,	0.0f, 1.0f, 0.0f,

									0.0f, 0.0f, 1.0f, 	0.0f, 0.0f, 1.0f, // Blue
									0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,
									0.0f, 0.0f, 1.0f, 	0.0f, 0.0f, 1.0f,
									0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f }; 


	glGenBuffers( 1, &m_bboxLinesVertsVBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_bboxLinesVertsVBO );
	glBufferData( GL_ARRAY_BUFFER, 24 * 3 * sizeof( float ), cubeVertices, GL_STATIC_DRAW );

	glGenBuffers( 1, &m_colsVBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_colsVBO );
	glBufferData( GL_ARRAY_BUFFER, 24 * 3 * sizeof( float ), cubeColours, GL_STATIC_DRAW );

	// Bounding box shaded sides
	
	float vertices[ 8 * 3 ] = {	minX, minY, minZ,	maxX, minY, minZ,
								maxX, maxY, minZ,	minX, maxY, minZ,
								minX, minY, maxZ,	maxX, minY, maxZ,
								maxX, maxY, maxZ,	minX, maxY, maxZ };

	unsigned int indices[ 12 * 3 ] = {	0, 1, 2,	0, 2, 3, // front
										4, 5, 6,	4, 6, 7, // back
										1, 5, 6,	1, 6, 2, // right
										4, 0, 3,	4, 3, 7, // left
										3, 2, 6,	3, 6, 7, // top
										0, 1, 5,	0, 5, 4 }; // bottom

	glGenBuffers( 1, &m_bboxSidesVertsVBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_bboxSidesVertsVBO );
	glBufferData( GL_ARRAY_BUFFER, 8 * 3 * sizeof( float ), vertices, GL_STATIC_DRAW );


	glGenBuffers( 1, &m_indicesVBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indicesVBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof( int ), indices, GL_STATIC_DRAW );

	//----------------------------------------------------------------------------------

	// Version with multiple VAOs

	//----------------------------------------------------------------------------------
	
	std::map< unsigned int, GLuint > tmpMap;

	for( unsigned int i = 0; i < _nContext; i++ )
	{
		GLuint tmpVAO[ 2 ];
		glGenVertexArrays( 2, tmpVAO );
		
		tmpMap.insert( std::make_pair( 0, tmpVAO[ 0 ] ) );
		tmpMap.insert( std::make_pair( 1, tmpVAO[ 1 ] ) ); 

		m_vaos.insert( std::make_pair( i, tmpMap ) ); 

		tmpMap.clear();
	}
	
	m_bboxLinesShader = new Shader( "Resources/Shaders/Simple.vert", "Resources/Shaders/Simple.frag" );
	m_bboxLinesShader->init( m_bboxLinesShader->fileRead( "Resources/Shaders/Simple.vert" ), m_bboxLinesShader->fileRead( "Resources/Shaders/Simple.frag" ) );
	glBindAttribLocation( m_bboxLinesShader->getID(), 0, "vPosition" );
	glBindAttribLocation( m_bboxLinesShader->getID(), 1, "vColours" );
	m_bboxLinesShader->link();

	m_bboxSidesShader = new Shader( "Resources/Shader/Colour.vert", "Resources/Shaders/Colour.frag" );
	m_bboxSidesShader->init( m_bboxSidesShader->fileRead( "Resources/Shaders/Colour.vert" ), m_bboxSidesShader->fileRead( "Resources/Shaders/Colour.frag" ) );
	glBindAttribLocation( m_bboxSidesShader->getID(), 0, "vPosition" );
	m_bboxSidesShader->link();
}

//----------------------------------------------------------------------------------

void VolumeTree::Node::UpdateBboxesVBOs()
{
	
	float minX, maxX, minY, maxY, minZ, maxZ;
	GetBounds( &minX, &maxX, &minY, &maxY, &minZ, &maxZ );
	float offset = 0.03f;
	minX -= offset; minY -= offset; minZ -= offset;
	maxX += offset; maxY += offset; maxZ += offset;

	float cubeVertices[ 24 * 3 ] = {	minX, minY, minZ,	maxX, minY, minZ, // Red lines
										minX, maxY, minZ,	maxX, maxY, minZ,
										minX, minY, maxZ,	maxX, minY, maxZ,
										minX, maxY, maxZ,	maxX, maxY, maxZ,

										minX, minY, minZ,	minX, maxY, minZ, // Green lines
										maxX, minY, minZ,	maxX, maxY, minZ,
										minX, minY,	maxZ,	minX, maxY, maxZ,
										maxX, minY, maxZ,	maxX, maxY, maxZ,

										minX, minY, minZ,	minX, minY, maxZ, // Blue lines
										minX, maxY, minZ,	minX, maxY, maxZ,
										maxX, minY, minZ,	maxX, minY, maxZ,
										maxX, maxY, minZ,	maxX, maxY, maxZ };

	glBindBuffer( GL_ARRAY_BUFFER, m_bboxLinesVertsVBO );
	glBufferSubData( GL_ARRAY_BUFFER, 0, 24 * 3 * sizeof( float ), cubeVertices );

	float vertices[ 8 * 3 ] = {	minX, minY, minZ,	maxX, minY, minZ,
								maxX, maxY, minZ,	minX, maxY, minZ,
								minX, minY, maxZ,	maxX, minY, maxZ,
								maxX, maxY, maxZ,	minX, maxY, maxZ };

	glBindBuffer( GL_ARRAY_BUFFER, m_bboxSidesVertsVBO );
	glBufferSubData( GL_ARRAY_BUFFER, 0, 8 * 3 * sizeof( float ), vertices );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

}

//----------------------------------------------------------------------------------

void VolumeTree::Node::LoadMatricesToShader( GLuint _shaderID, cml::matrix44f_c &_proj, cml::matrix44f_c &_mv )
{
	// ModelView matrix
	GLint mvLoc = glGetUniformLocation( _shaderID, "u_ModelViewMatrix" );
	if( mvLoc != -1 ) { glUniformMatrix4fv( mvLoc, 1, GL_FALSE, _mv.data() ); }
	else { std::cerr << "Bounding Box: u_ModelViewMatrix not found in shader " << _shaderID << std::endl; }

	// Projection matrix	
	GLint pLoc = glGetUniformLocation( _shaderID, "u_ProjectionMatrix" );
	if( pLoc != -1 ) { glUniformMatrix4fv( pLoc, 1, GL_FALSE, _proj.data() ); }
	else { std::cerr << "Bounding Box: u_ProjectionMatrix not found in shader " << _shaderID << std::endl; }
}

//----------------------------------------------------------------------------------

void CheckAttributesUniforms( GLuint _shaderID )
{
	GLint maxLength, nAttribs;
	glGetProgramiv( _shaderID, GL_ACTIVE_ATTRIBUTES, &nAttribs );
	glGetProgramiv( _shaderID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength );

	GLchar* name = ( GLchar* )malloc( maxLength );

	GLint written, size, location;
	GLenum type;
	printf( "Index | Name\n" );
	printf( "-----------------------------------------\n" );
	for( int i = 0; i < nAttribs; i++ )
	{
		glGetActiveAttrib( _shaderID, i, maxLength, &written, &size, &type, name );
		location = glGetAttribLocation( _shaderID, name );
		printf( "%-5d | %s\n", location, name );
	}
	
	free( name );

	//----------------------------------------------------------------------------------

	GLint nUniforms, maxLen;
	glGetProgramiv( _shaderID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen );
	glGetProgramiv( _shaderID, GL_ACTIVE_UNIFORMS, &nUniforms );

	GLchar* name2 = ( GLchar* )malloc( maxLen );
	GLint size2, location2;
	GLsizei written2;
	GLenum type2;

	printf( "Location | Name\n" );
	printf( "-----------------------------------------\n" );
	for( int i = 0; i < nUniforms; ++i )
	{
		glGetActiveUniform( _shaderID, i , maxLen, &written2, &size2, &type2, name2 );
		location2 = glGetUniformLocation( _shaderID, name2 );
		printf( "%-8d | %s\n", location2, name2 );
	}
	
	free( name2 );
}

//----------------------------------------------------------------------------------

void VolumeTree::Node::DrawBBoxes(  cml::matrix44f_c &_proj, cml::matrix44f_c &_mv )
{
	if( m_hasBBox && m_drawBBox )
	{
		UpdateBboxesVBOs();
	
		glEnable( GL_DEPTH_TEST );
		glDepthMask( GL_FALSE ); // Disables writing to the depth buffer so that bounding boxes don't hide each other
		
		// Draw outline of bounding box
		m_bboxLinesShader->bind();

#if _DEBUG
		//CheckAttributesUniforms( m_bboxLinesShader->getID() );
#endif
		LoadMatricesToShader( m_bboxLinesShader->getID(), _proj, _mv );
		
		glBindVertexArray( m_vaos[ 0 ][ 0 ] );

		glEnableVertexAttribArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, m_bboxLinesVertsVBO );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ( GLfloat* ) NULL );

		glEnableVertexAttribArray( 1 );
		glBindBuffer( GL_ARRAY_BUFFER, m_colsVBO );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, ( GLfloat* ) NULL );

		glBindVertexArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		glBindVertexArray( m_vaos[ 0 ][ 0 ] );
			glLineWidth( 5.0f );
			glDrawArrays( GL_LINES, 0, 24 );
		glBindVertexArray( 0 );
		
		m_bboxLinesShader->unbind();

		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );


		// Draw shaded sides of bounding box

		m_bboxSidesShader->bind();
		
		LoadMatricesToShader( m_bboxSidesShader->getID(), _proj, _mv );
		glUniform4f( glGetUniformLocation( m_bboxSidesShader->getID(), "u_Colour" ), 1.0f, 0.f, 0.f, 0.1f );

#if _DEBUG
		//CheckAttributesUniforms( m_bboxSidesShader->getID() );
#endif

		glBindVertexArray( m_vaos[ 0 ][ 1 ] );

		glEnableVertexAttribArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, m_bboxSidesVertsVBO );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ( GLfloat* ) NULL );

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indicesVBO );

		glBindVertexArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		glBindVertexArray( m_vaos[ 0 ][ 1 ] );
			glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );
			glDrawArrays( GL_POINTS, 0, 1 );
		glBindVertexArray( 0 );
		
		m_bboxSidesShader->unbind();
	
		glDisable( GL_CULL_FACE );
		glDisable( GL_BLEND );	

		glDepthMask( GL_TRUE );
		glDisable( GL_DEPTH_TEST );
		
	}

	for( Node *currentChild = GetFirstChild(); currentChild != NULL; currentChild = GetNextChild( currentChild ) )
	{
		currentChild->DrawBBoxes( _proj, _mv );
	}
}

//----------------------------------------------------------------------------------

void VolumeTree::Node::DrawBBoxes( cml::matrix44f_c &_proj, cml::matrix44f_c &_mv, unsigned int _context )
{
	if( m_hasBBox && m_drawBBox )
	{

#if _DEBUG
		//std::cout<< "#=====================================================#" << std::endl;
		//std::cout<< "CURRENT CONTEXT: " << _context << std::endl;
		//std::cout<< "BBox LINES shader ID: " << m_bboxLinesShader->getID() << std::endl;
		//std::cout<< "BBox SIDES shader ID: " << m_bboxSidesShader->getID() << std::endl;
#endif
		
		UpdateBboxesVBOs();
	
		glEnable( GL_DEPTH_TEST );
		glDepthMask( GL_FALSE ); // Disables writing to the depth buffer so that bounding boxes don't hide each other
		
		// Draw outline of bounding box
		m_bboxLinesShader->bind();
			
#if _DEBUG
		//CheckAttributesUniforms( m_bboxLinesShader->getID() );
#endif
		LoadMatricesToShader( m_bboxLinesShader->getID(), _proj, _mv );

		glBindVertexArray( m_vaos[ _context ][ 0 ] );

		glEnableVertexAttribArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, m_bboxLinesVertsVBO );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ( GLfloat* ) NULL );

		glEnableVertexAttribArray( 1 );
		glBindBuffer( GL_ARRAY_BUFFER, m_colsVBO );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, ( GLfloat* ) NULL );

		glBindVertexArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		glBindVertexArray( m_vaos[ _context ][ 0 ] );
			glLineWidth( 5.0f );
			glDrawArrays( GL_LINES, 0, 24 );
		glBindVertexArray( 0 );
		
		m_bboxLinesShader->unbind();

		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );


		// Draw shaded sides of bounding box

		m_bboxSidesShader->bind();
		
		LoadMatricesToShader( m_bboxSidesShader->getID(), _proj, _mv );
		glUniform4f( glGetUniformLocation( m_bboxSidesShader->getID(), "u_Colour" ), 1.0f, 0.f, 0.f, 0.1f );

#if _DEBUG
		//CheckAttributesUniforms( m_bboxSidesShader->getID() );
#endif
		glBindVertexArray( m_vaos[ _context ][ 1 ] );

		glEnableVertexAttribArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, m_bboxSidesVertsVBO );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ( GLfloat* ) NULL );

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indicesVBO );

		glBindVertexArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		glBindVertexArray( m_vaos[ _context ][ 1 ] );
			glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );
			glDrawArrays( GL_POINTS, 0, 1 );
		glBindVertexArray( 0 );
		
		m_bboxSidesShader->unbind();
	
		glDisable( GL_CULL_FACE );
		glDisable( GL_BLEND );	

		glDepthMask( GL_TRUE );
		glDisable( GL_DEPTH_TEST );
		
	}

	for( Node *currentChild = GetFirstChild(); currentChild != NULL; currentChild = GetNextChild( currentChild ) )
	{
		currentChild->DrawBBoxes( _proj, _mv, _context );
	}
}

//----------------------------------------------------------------------------------

VolumeTree::Node* VolumeTree::Node::RecursiveGetNode( int _desiredNode, int &_currentCount, int &_currentDepth )
{
	if( _desiredNode == _currentCount )
	{
#ifdef _DEBUG
		std::cout << "INFO: recursive get has found node " << _desiredNode << std::endl;
#endif
		return this;
	}

	_currentDepth++;
	Node *currentChild = GetFirstChild();
	while( currentChild != NULL )
	{
		_currentCount++;
		Node *result = currentChild->RecursiveGetNode( _desiredNode, _currentCount, _currentDepth );
		if( result != NULL )
		{
			return result;
		}
		currentChild = GetNextChild( currentChild );
	}
	_currentDepth--;
	return NULL;
}

//----------------------------------------------------------------------------------

std::string VolumeTree::Node::GetCachedFunctionGLSLString( std::string _samplePosStr )
{
	if( m_useCache )
	{
		std::stringstream functionString;
		functionString << "Cache(" << _samplePosStr << ",CacheTexture" << m_cacheNumber << ",vec3(" << m_cacheOffsetX << "," << m_cacheOffsetY << "," << m_cacheOffsetZ << "),vec3(" << m_cacheScaleX << "," << m_cacheScaleY << "," << m_cacheScaleZ << "))";

		return functionString.str();
	}
	else
		return GetFunctionGLSLString( true, _samplePosStr );
}

//----------------------------------------------------------------------------------

VolumeTree::Node* VolumeTree::Node::GetFirstChild()
{
	return NULL;
}

//----------------------------------------------------------------------------------

VolumeTree::Node* VolumeTree::Node::GetNextChild( Node *_previousChild )
{
	return NULL;
}

//----------------------------------------------------------------------------------

unsigned int VolumeTree::Node::GetSubtreeNumCacheRequires( bool _transparent )
{
	unsigned int total = m_requiresCache ? 1 : 0;
	if( ( !_transparent ) && m_requiresCache )
	{
		return 1;
	}

	for( Node *currentChild = GetFirstChild(); currentChild != NULL; currentChild = GetNextChild( currentChild ) )
	{
		total += currentChild->GetSubtreeNumCacheRequires( _transparent );
	}
	return total;
}

//----------------------------------------------------------------------------------

float VolumeTree::Node::GetSubtreeCacheVolume()
{
	float x, y, z;
	GetBoundSizes( &x, &y, &z );

	return x * y * z;
}

//----------------------------------------------------------------------------------

unsigned int VolumeTree::Node::GetSubtreeCost()
{
	if( m_useCache || m_requiresCache )
	{
		return 5;
	}
	int total = GetNodeCost();
	for( Node *currentChild = GetFirstChild(); currentChild != NULL; currentChild = GetNextChild( currentChild ) )
	{
		total += currentChild->GetSubtreeCost();
	}
	return total;
}

//----------------------------------------------------------------------------------

void VolumeTree::Node::GetBoundSizes( float* _x, float* _y, float* _z )
{
	float minX, maxX, minY, maxY, minZ, maxZ;
	GetBounds( &minX, &maxX, &minY, &maxY, &minZ, &maxZ );

	*_x = fabs( maxX - minX );
	*_y = fabs( maxY - minY );
	*_z = fabs( maxZ - minZ );
}

//----------------------------------------------------------------------------------

/*
void VolumeTree::Node::GetSubtreeBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ)
{
	float minX, maxX, minY, maxY, minZ, maxZ;
	GetNodeBounds(&minX,&maxX, &minY,&maxY, &minZ,&maxZ);

	for( Node *currentChild = currentNode->GetFirstChild(); currentChild != NULL; currentChild = currentNode->GetNextChild(currentChild) )
	{
		float childMinX, childMaxX, childMinY, childMaxY, childMinZ, childMaxZ;
		currentChild->GetNodeBounds(&childMinX,&childMaxX, &childMinY,&childMaxY, &childMinZ,&childMaxZ);


	}
	return total;
}
*/

//----------------------------------------------------------------------------------

void VolumeTree::Node::PopulateCacheData( float** _cacheData, float _startX, float _startY, float _startZ, float _stepX, float _stepY, float _stepZ )
{
	unsigned int volX = m_cacheResX, volY = m_cacheResY, volZ = m_cacheResZ;
	for( unsigned int i = 0; i < volX; i++ )
	{
		float samplePosX = _startX + ( _stepX * ( float )i );
		for( unsigned int j = 0; j < volY; j++ )
		{
			float samplePosY = _startY + ( _stepY * ( float )j );
			for( unsigned int k = 0; k < volZ; k++ )
			{
				float samplePosZ = _startZ + ( _stepZ * ( float )k );

				( *_cacheData )[ i + j * volX + k * volX * volY ] = GetFunctionValue( samplePosX, samplePosY, samplePosZ );
			}
		}
	}
}

//----------------------------------------------------------------------------------

void VolumeTree::Node::UpdateParameters( GLSLRenderer *_renderer )
{
	OnUpdateParameters( _renderer );

	for( Node *currentChild = GetFirstChild(); currentChild != NULL; currentChild = GetNextChild( currentChild ) )
	{
		currentChild->UpdateParameters( _renderer );
	}
}

//----------------------------------------------------------------------------------