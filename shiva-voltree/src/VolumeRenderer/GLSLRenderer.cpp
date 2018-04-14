#ifndef PI
#define PI 3.141592653589793238462643383279502884197169399375
#endif

#include "VolumeRenderer/GLSLRenderer.h"

//----------------------------------------------------------------------------------

GLSLRenderer::GLSLRenderer( unsigned int _width, unsigned int _height )
{
	m_functionTree = NULL;
	m_windowWidth = _width;
	m_windowHeight = _height;

	m_fragColour = 0;

	m_objColourR = 0.88f;
	m_objColourG = 0.78f;
	m_objColourB = 0.54f;

	m_angularVelX = m_angularVelY = m_angularVelZ = 0.0f;

	m_stepSize = 0.1f; // 0.03f;
	//m_gradDelta = 0.001f;

	// Set up viewing transformation (matches the mess that's in SHIVA SDFView)
	m_axisX.set( 1, 0, 0, 0 );
	m_axisY.set( 0, 1, 0, 0 );
	m_axisZ.set( 0, 0, 1, 0 );

	m_cam = new Camera();
	m_cam->SetPos( 0.f, 2.f, 3.f );

	m_target.SetPos( 0.f, 0.f, 0.f );
	
	m_cam->LookAt( m_cam->GetCurrent(), m_target.GetCurrent() );

	m_modelViewMatrix = m_cam->GetViewMatrix();
	m_invModelViewMatrix = m_modelViewMatrix;
	cml::inverse( m_invModelViewMatrix );

	m_modelMatrix.identity();
	
	m_localRotX = ( float )( -PI / 2.0f );
	m_localRotY = 0.0f;
	m_localRotZ = 0.0f;
	
	m_worldRotX = m_worldRotY = m_worldRotZ = 0.0f;
	
	m_camAngle = 35.0f;
	m_camNearPlane = 0.1f;
	m_camFarPlane = 100.0f;

	InitialiseCube();

	m_maxCaches = 0;
	m_glCacheTexIDs = NULL;

	//------------------------------------------------------------------------------------------------------------------------------------

	m_maxParametersRaw = 0;
	int rawLimit = 0;
	
	// This returns the number of individual floats that can be used
	glGetIntegerv( GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &rawLimit );
	// Remove a number for those taken up with other things
	// Then divide by 4 because ATI cards treat floats as vec4s
	// This is inaccurate - replacing with a calculation based on the number of currently used things
	int divisor = 1;
	std::string vendorString = ( const char* ) glGetString( GL_VENDOR );
	if( vendorString.find( "ATI" ) !=  std::string::npos )
	{
		// Turns out ATI cards simply get this value wrong and it needs to be divided by 4
		std::cout << "INFO: GLSLRenderer detected an ATI-based graphics card" << std::endl;
		if(  "ATI Mobility Radeon HD 3400 Series" == ( const char* ) glGetString( GL_RENDERER ) )
		{
			// This card is crazy, give is a separate value
			divisor = 16;
			std::cout << "INFO: GLSLRenderer detected an ATI Mobility Radeon HD 3400 Series card, using param divisor: " << divisor << std::endl;
		}
		else
		{
			divisor = 4;
		}
	}
	m_maxParametersRaw = ( rawLimit / divisor ) - 90; //(rawLimit - 16) / (4 * 16);

#if _DEBUG
	std::cout << "INFO: GLSLRenderer, maximum available tree parameters: " << m_maxParametersRaw << std::endl;
#endif 

	m_paramIDCount = 0;
	m_paramUniformListLengths = NULL;
	m_paramUniformData = NULL;
}

//----------------------------------------------------------------------------------

GLSLRenderer::~GLSLRenderer()
{
	// This function clears the cache list
	ReserveCaches( 0 );
	delete [] m_paramUniformListLengths;
	delete [] m_paramUniformData;
	delete m_functionTree;
	delete m_cam;
	delete m_shader;
}

//----------------------------------------------------------------------------------

void GLSLRenderer::SetWindowDimensions( const float &_width, const float &_height )
{
	m_windowWidth = ( unsigned int )_width;
	m_windowHeight = ( unsigned int )_height;

	float aspectRatio = ( ( float ) m_windowWidth ) / ( ( float ) m_windowHeight );
	m_cam->SetProjectionMatrix( m_camAngle, aspectRatio, m_camNearPlane, m_camFarPlane );
}

//----------------------------------------------------------------------------------

void GLSLRenderer::SetTree( VolumeTree::Tree *_tree )
{
	m_functionTree = _tree;
}

//----------------------------------------------------------------------------------

bool GLSLRenderer::Initialise( std::string _vsFile, std::string _fsPartFile )
{
	// Compile shaders
	if( !InitialiseShaders( _vsFile, _fsPartFile ) )
	{
		// No need for error message here, the function will print out specific problems itself
		return false;
	}
	return true;
}


//----------------------------------------------------------------------------------

void GLSLRenderer::LoadMatricesToShader()
{
	GLint mvLoc = glGetUniformLocation( m_shader->getID(), "u_ModelViewMatrix" );
	if( mvLoc != -1 ) { glUniformMatrix4fv( mvLoc, 1, GL_FALSE, m_modelViewMatrix.data() ); }
	else { std::cerr << "u_ModelViewMatrix not found in shader " << m_shader->getID() << std::endl; }

	// Inverse ModelView matrix
	GLint imvLoc = glGetUniformLocation( m_shader->getID(), "u_InverseModelViewMatrix" );
	if( imvLoc != -1 ) { glUniformMatrix4fv( imvLoc, 1, GL_FALSE, m_invModelViewMatrix.data() ); }
	else { std::cerr << "GLSLRenderer: u_InverseModelViewMatrix not found in shader " << m_shader->getID() << std::endl; }

	// Projection matrix	
	GLint pLoc = glGetUniformLocation( m_shader->getID(), "u_ProjectionMatrix" );
	if( pLoc != -1 ) { glUniformMatrix4fv( pLoc, 1, GL_FALSE, m_cam->GetProjectionMatrix().data() ); }
	else { std::cerr << "u_ProjectionMatrix not found in shader " << m_shader->getID() << std::endl; }

}

//----------------------------------------------------------------------------------

void GLSLRenderer::Draw()
{
	/*float aspectRatio = ( ( float ) m_windowWidth ) / ( ( float ) m_windowHeight );
	m_cam->SetProjectionMatrix( m_camAngle, aspectRatio, m_camNearPlane, m_camFarPlane );*/

	// Enable depth test
	glEnable( GL_DEPTH_TEST );
	
	// Bind shader
	m_shader->bind();

	LoadMatricesToShader();
	
	if( m_fragColour >= 0 )
	{
		glUniform3f( glGetUniformLocation( m_shader->getID(), "objectcolour" ), m_objColourR, m_objColourG, m_objColourB );
	}

	float extents[ 3 ] = { 0.5f, 0.5f, 0.5f };
	float centre[ 3 ] = { 0.0f, 0.0f, 0.0f };
	float boundsMin[ 3 ] = { -0.5f, -0.5f, -0.5f };
	float boundsMax[ 3 ] = { 0.5f, 0.5f, 0.5f };
		
	if( m_functionTree != NULL )
	{
		m_functionTree->GetBoundingBox( boundsMin, boundsMax );
		m_functionTree->GetBoundingBoxCentre( centre );
		m_functionTree->GetBoundingBoxSize( extents );
		
		// For some reason the extents need to be half their values
		for( unsigned int i = 0; i < 3; i++ )
			extents[ i ] = 0.5f * extents[ i ];
	}
		
	glUniform3fv( glGetUniformLocation( m_shader->getID(), "boxSize" ), 1, extents );
	glUniform3fv( glGetUniformLocation( m_shader->getID(), "boxCentre" ), 1, centre );

	glUniform3fv( glGetUniformLocation( m_shader->getID(), "fragboundmin" ), 1, boundsMin );
	glUniform3fv( glGetUniformLocation( m_shader->getID(), "fragboundmax" ), 1, boundsMax );
	glUniform1f( glGetUniformLocation( m_shader->getID(), "stepsize" ), m_stepSize );
	glUniform1f( glGetUniformLocation( m_shader->getID(), "gradDelta" ), m_gradDelta );

	glUniform1i( glGetUniformLocation( m_shader->getID(), "CacheTexture0" ), 0 );
	glUniform1i( glGetUniformLocation( m_shader->getID(), "CacheTexture1" ), 1 );
	glUniform1i( glGetUniformLocation( m_shader->getID(), "CacheTexture2" ), 2 );
	glUniform1i( glGetUniformLocation( m_shader->getID(), "CacheTexture3" ), 3 );

	BindParametersToGL();

	glBindVertexArray( m_cubeVAO );

	// Bind Cache Textures
	for( unsigned int currentCache = 0; currentCache < m_maxCaches; ++currentCache )
	{
		if( m_glCacheTexIDs[ currentCache ] >= 0 )
		{
			glActiveTexture( GL_TEXTURE0 + currentCache );
			glEnable( GL_TEXTURE_3D );
			glBindTexture( GL_TEXTURE_3D, m_glCacheTexIDs[ currentCache ] );
		}
	}

	// Draw cube using index array
	glDrawElements( GL_QUADS, 24, GL_UNSIGNED_INT, 0 );

	glBindVertexArray( 0 );

	// Unbind Cache Textures
	for( unsigned int currentCache = 0; currentCache < m_maxCaches; ++currentCache )
	{
		glActiveTexture( GL_TEXTURE0 + currentCache );
		glDisable( GL_TEXTURE_3D );
		glBindTexture( GL_TEXTURE_3D, 0 );
	}
	
	glActiveTexture( GL_TEXTURE0 );

	// Unbind shader
	m_shader->unbind();

	if( m_functionTree != NULL )
	{
		m_functionTree->DrawBBoxes( m_cam->GetProjectionMatrix(), m_modelViewMatrix );
	}

	// Disable depth test
	glDisable( GL_DEPTH_TEST );

}

//----------------------------------------------------------------------------------

void GLSLRenderer::Draw( unsigned int _context )
{
	/*float aspectRatio = ( ( float ) m_windowWidth ) / ( ( float ) m_windowHeight );
	m_cam->SetProjectionMatrix( m_camAngle, aspectRatio, m_camNearPlane, m_camFarPlane );*/

	// Enable depth test
	glEnable( GL_DEPTH_TEST );
	//glDepthRange( 0.1, 100.0 );
	// Bind shader
	m_shader->bind();

	LoadMatricesToShader();

	if( m_fragColour >= 0 )
	{
		glUniform3f( glGetUniformLocation( m_shader->getID(), "objectcolour" ), m_objColourR, m_objColourG, m_objColourB );
	}

	float extents[ 3 ] = { 0.5f, 0.5f, 0.5f };
	float centre[ 3 ] = { 0.0f, 0.0f, 0.0f };
	float boundsMin[ 3 ] = { -0.5f, -0.5f, -0.5f };
	float boundsMax[ 3 ] = { 0.5f, 0.5f, 0.5f };
		
	if( m_functionTree != NULL )
	{
		m_functionTree->GetBoundingBox( boundsMin, boundsMax );
		m_functionTree->GetBoundingBoxCentre( centre );
		m_functionTree->GetBoundingBoxSize( extents );
		
		// For some reason the extents need to be half their values
		for( unsigned int i = 0; i < 3; i++ )
			extents[ i ] = 0.5f * extents[ i ];
	}
		
	glUniform3fv( glGetUniformLocation( m_shader->getID(), "boxSize" ), 1, extents );
	glUniform3fv( glGetUniformLocation( m_shader->getID(), "boxCentre" ), 1, centre );

	glUniform3fv( glGetUniformLocation( m_shader->getID(), "fragboundmin" ), 1, boundsMin );
	glUniform3fv( glGetUniformLocation( m_shader->getID(), "fragboundmax" ), 1, boundsMax );
	glUniform1f( glGetUniformLocation( m_shader->getID(), "stepsize" ), m_stepSize );
	glUniform1f( glGetUniformLocation( m_shader->getID(), "gradDelta" ), m_gradDelta );

	glUniform1i( glGetUniformLocation( m_shader->getID(), "CacheTexture0" ), 0 );
	glUniform1i( glGetUniformLocation( m_shader->getID(), "CacheTexture1" ), 1 );
	glUniform1i( glGetUniformLocation( m_shader->getID(), "CacheTexture2" ), 2 );
	glUniform1i( glGetUniformLocation( m_shader->getID(), "CacheTexture3" ), 3 );

	BindParametersToGL();

	glBindVertexArray( m_cubeVAO );

	// Bind Cache Textures
	for( unsigned int currentCache = 0; currentCache < m_maxCaches; ++currentCache )
	{
		if( m_glCacheTexIDs[ currentCache ] >= 0 )
		{
			glActiveTexture( GL_TEXTURE0 + currentCache );
			glEnable( GL_TEXTURE_3D );
			glBindTexture( GL_TEXTURE_3D, m_glCacheTexIDs[ currentCache ] );
		}
	}

	// Draw cube using index array
	glDrawElements( GL_QUADS, 24, GL_UNSIGNED_INT, 0 );

	glBindVertexArray( 0 );

	// Unbind Cache Textures
	for( unsigned int currentCache = 0; currentCache < m_maxCaches; ++currentCache )
	{
		glActiveTexture( GL_TEXTURE0 + currentCache );
		glDisable( GL_TEXTURE_3D );
		glBindTexture( GL_TEXTURE_3D, 0 );
	}
	
	glActiveTexture( GL_TEXTURE0 );

	// Unbind shader
	m_shader->unbind();

	if( m_functionTree != NULL )
	{
		m_functionTree->DrawBBoxes( m_cam->GetProjectionMatrix(), m_modelViewMatrix, _context );
	}

	// Disable depth test
	glDisable( GL_DEPTH_TEST );

}

//----------------------------------------------------------------------------------

void GLSLRenderer::Update( float _deltaTs )
{
	float rotOffsetX = m_angularVelX * _deltaTs;
	float rotOffsetY = m_angularVelY * _deltaTs;
	float rotOffsetZ = m_angularVelZ * _deltaTs;

	AddWorldRotationOffsetRads( 0.0f, 0.0f, rotOffsetZ );
	AddWorldRotationOffsetRads( 0.0f, rotOffsetY, 0.0f );
	AddWorldRotationOffsetRads( rotOffsetX, 0.0f, 0.0f );

	m_cam->Update( _deltaTs );
	m_target.Update( _deltaTs );

	m_cam->LookAt( m_cam->GetCurrent(), m_target.GetCurrent() );
	m_cam->Rotate( m_localRotX, m_localRotY, m_localRotZ );


	cml::matrix44f_c temp;
	
	// Code to make object rotate about the aim point
		
	cml::matrix44f_c preTemp;
	preTemp.identity();
	cml::matrix_set_translation( preTemp, m_target.GetCurrent().data()[ 0 ], m_target.GetCurrent().data()[ 2 ], m_target.GetCurrent().data()[ 1 ] );
	
	temp.identity();
	cml::matrix_rotation_axis_angle( temp, cml::vector3f( m_axisX.data() ), m_worldRotX );
	m_modelMatrix = m_modelMatrix * temp;
	m_worldRotX = 0.0f;


	temp.identity();
	cml::matrix_rotation_axis_angle( temp, cml::vector3f( m_axisY.data() ), m_worldRotY );
	m_modelMatrix = m_modelMatrix * temp;
	m_worldRotY = 0.0f;

	temp.identity();
	cml::matrix_rotation_axis_angle( temp, cml::vector3f( m_axisZ.data() ), m_worldRotZ );
	m_modelMatrix = m_modelMatrix * temp;
	m_worldRotZ = 0.0f;

	
	// Code to make object rotate about the centre of its bounding box
		
	cml::matrix44f_c postTemp;
	postTemp.identity();
	cml::matrix_set_translation( postTemp, -m_target.GetCurrent().data()[ 0 ], -m_target.GetCurrent().data()[ 2 ], -m_target.GetCurrent().data()[ 1 ] );
	
	m_modelViewMatrix = m_cam->GetViewMatrix() * preTemp * m_modelMatrix * postTemp;
	m_invModelViewMatrix = m_modelViewMatrix;
	m_invModelViewMatrix = cml::inverse( m_invModelViewMatrix );

	m_functionTree->UpdateParameters( this );
}

//----------------------------------------------------------------------------------

void GLSLRenderer::SetStepsize( const float &_stepsize )
{
	m_stepSize = _stepsize;
	m_gradDelta = _stepsize * 0.08f;

#ifdef _DEBUG
	std::cout << "INFO: GLSLRenderer SetStepsize() stepsize: " << m_stepSize << " gradDelta: " << m_gradDelta << std::endl;
#endif
}

//----------------------------------------------------------------------------------

void GLSLRenderer::ResetWorldRotation()
{
	m_worldRotX = m_worldRotY = m_worldRotZ = 0.0f;
	m_localRotX = -PI / 2.0f;
	m_localRotY = 0.0f;
	m_localRotZ = 0.0f;
	m_angularVelX = m_angularVelY = m_angularVelZ = 0.0f;

	m_axisX.set( 1, 0, 0, 0 );
	m_axisY.set( 0, 1, 0, 0 );
	m_axisZ.set( 0, 0, 1, 0 );
	
	m_cam->LookAt( m_cam->GetCurrent(), m_target.GetCurrent() );
	m_modelViewMatrix = m_cam->GetViewMatrix();

	m_invModelViewMatrix = m_modelViewMatrix;
	cml::inverse( m_invModelViewMatrix );

	m_modelMatrix.identity();
}

//----------------------------------------------------------------------------------

void GLSLRenderer::AddWorldRotationOffsetDegs( const float &_rotX, const float &_rotY, const float &_rotZ )
{
	AddWorldRotationOffsetRads( ( 2.0f * PI ) * ( _rotX / 360.0f ), ( 2.0f * PI ) * ( _rotY / 360.0f ), ( 2.0f * PI ) * ( _rotZ / 360.0f ) );
}

//----------------------------------------------------------------------------------

void GLSLRenderer::AddWorldRotationOffsetRads( const float &_rotX, const float &_rotY, const float &_rotZ )
{
	m_worldRotX = std::fmod( m_worldRotX + _rotX,( float )( 2.0f * PI ) );
	m_worldRotY = std::fmod( m_worldRotY + _rotY,( float )( 2.0f * PI ) );
	m_worldRotZ = std::fmod( m_worldRotZ + _rotZ,( float )( 2.0f * PI ) );

	cml::matrix44f_c temp;
	cml::matrix_rotation_axis_angle( temp, cml::vector3f( m_axisX.data() ), _rotX );
	m_axisX = m_axisX * temp;
	m_axisY = m_axisY * temp;
	m_axisZ = m_axisZ * temp;

	temp.identity();
	cml::matrix_rotation_axis_angle( temp, cml::vector3f( m_axisY.data() ), _rotY );
	m_axisX = m_axisX * temp;
	m_axisY = m_axisY * temp;
	m_axisZ = m_axisZ * temp;

	temp.identity();
	cml::matrix_rotation_axis_angle( temp, cml::vector3f( m_axisZ.data() ), _rotZ );
	m_axisX = m_axisX * temp;
	m_axisY = m_axisY * temp;
	m_axisZ = m_axisZ * temp;
}

//----------------------------------------------------------------------------------

void GLSLRenderer::SetWorldAngularVelocityDegs( const float &_rotX, const float &_rotY, const float &_rotZ )
{
	SetWorldAngularVelocityRads( ( 2.0f * PI ) * ( _rotX / 360.0f ), ( 2.0f * PI ) * ( _rotY / 360.0f ), ( 2.0f * PI ) * ( _rotZ / 360.0f ) );
}

//----------------------------------------------------------------------------------

void GLSLRenderer::SetWorldAngularVelocityRads( const float &_rotX, const float &_rotY, const float &_rotZ )
{
	m_angularVelX = _rotX;
	m_angularVelY = _rotY;
	m_angularVelZ = _rotZ;
}

//----------------------------------------------------------------------------------

void GLSLRenderer::Unproject(	float _mouseX, float _mouseY, int _viewOriginX, int _viewOriginY, int _viewWidth, int _viewHeight,
								float &_originX, float &_originY, float &_originZ, float &_dirX, float &_dirY, float &_dirZ )
{
	/* *** Don't touch this, it works! *** */

	float *tempFloat = m_modelViewMatrix.data();
	double *mview = new double[ 16 ];
	for( int i = 0; i < 16; i++ )
		mview[ i ] = tempFloat[ i ];

	float aspectRatio = ( ( float ) m_windowWidth ) / ( ( float ) m_windowHeight );
	//gluPerspective(_cameraAngle, aspectRatio, _cameraNearPlane, _cameraFarPlane);
	float f = cos( ( m_camAngle * 0.5f ) * PI / 180.0f ) / sin( ( m_camAngle * 0.5f ) * PI / 180.0f );// trig identity for cotangent
	double *proj = new double[ 16 ];
	for( int i = 0; i < 16; i++ )
		proj[ i ] = 0.0;
	proj[ 0 ] = f / aspectRatio;
	proj[ 5 ] = f;
	proj[ 10 ] = ( m_camFarPlane + m_camNearPlane ) / ( m_camNearPlane - m_camFarPlane );
	proj[ 14 ] = ( 2.0 * m_camFarPlane * m_camNearPlane ) / ( m_camNearPlane - m_camFarPlane );
	proj[ 11 ] = -1.0;

	int *view = new int[ 4 ];
	view[ 0 ] = _viewOriginX;
	view[ 1 ] = _viewOriginY;
	view[ 2 ] = _viewWidth;
	view[ 3 ] = _viewHeight;

	double objX, objY, objZ;

	int result = gluUnProject( _mouseX,  _mouseY, 0.0, mview,  proj,  view,  &objX,  &objY,  &objZ );

	if( result == GLU_FALSE )
		std::cerr << "WARNING: GLSLRenderer::Unproject failure of first unproject operation" << std::endl;

	_originX = objX;
	_originY = objY;
	_originZ = objZ;

	result = gluUnProject( _mouseX,  _mouseY, 1.0, mview,  proj,  view,  &objX,  &objY,  &objZ );

	if( result == GLU_FALSE )
		std::cerr << "WARNING: GLSLRenderer::Unproject failure of second unproject operation" << std::endl;

	cml::vector3f tempVec( objX - _originX, objY - _originY, objZ - _originZ );
	tempVec.normalize();

	_dirX = tempVec.data()[ 0 ];
	_dirY = tempVec.data()[ 1 ];
	_dirZ = tempVec.data()[ 2 ];

	delete [] mview;
	delete [] proj;
	delete [] view;

}

//----------------------------------------------------------------------------------

void GLSLRenderer::ZoomCamera( const float &_value )
{
	cml::vector3f aimPos = m_cam->GetAim();
	aimPos[ 2 ] *= _value;

	m_cam->SetAim( aimPos );
}

//----------------------------------------------------------------------------------

void GLSLRenderer::SetCameraAngle( const float &_angleDegrees )
{
	m_camAngle = _angleDegrees;
	RefreshCameraPos();
}

//----------------------------------------------------------------------------------

float GLSLRenderer::GetCameraDistance()
{
	return ( m_cam->GetCurrent() - m_target.GetCurrent() ).length();
}

//----------------------------------------------------------------------------------

void GLSLRenderer::ReserveCaches( unsigned int numCaches )
{
	if( m_glCacheTexIDs != NULL )
	{
		for( unsigned int i = 0; i < m_maxCaches; ++i )
		{
			FreeCache(i);
		}
		delete [] m_glCacheTexIDs;
	}

	m_maxCaches = numCaches;
	if( numCaches == 0 )
	{
		m_glCacheTexIDs = NULL;
	}
	else
	{
		m_glCacheTexIDs = new int[ m_maxCaches ];
		for( unsigned int i = 0; i < m_maxCaches; ++i )
		{
			m_glCacheTexIDs[ i ] = -1;
		}
	}
}

//----------------------------------------------------------------------------------

unsigned int GLSLRenderer::GetMaxCachingVoxels()
{
	// TODO
	return 3 * 256 * 256 * 256;
}

//----------------------------------------------------------------------------------

void GLSLRenderer::FillCache( unsigned int _cacheID, float* _data, unsigned int _sizeX, unsigned int _sizeY, unsigned int _sizeZ )
{
	if( _cacheID > m_maxCaches )
	{
		std::cerr << "WARNING: attempting to assign too many caches" << std::endl;
		return;
	}
	
	unsigned int texID = m_glCacheTexIDs[ _cacheID ];
	// Check to see if cache already exists
	if( m_glCacheTexIDs[ _cacheID ] >= 0 )
	{
		glDeleteTextures( 1, &texID );
	}

	// Generate OpenGL texture
	glGenTextures( 1, &texID );
	glBindTexture( GL_TEXTURE_3D, texID );

	// Clamp to edges
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	// Trilinear filtering
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// Transfer data to OpenGL
	glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA32F, _sizeX, _sizeY, _sizeZ, 0, GL_ALPHA, GL_FLOAT, _data );

	glBindTexture( GL_TEXTURE_3D, 0 );

	m_glCacheTexIDs[ _cacheID ] = texID;
}

//----------------------------------------------------------------------------------

void GLSLRenderer::FreeCache( unsigned int _cacheID )
{
	if( _cacheID > m_maxCaches )
	{
		std::cerr << "WARNING: attempting to free cache with ID beyond maxCaches" << std::endl;
		return;
	}
	
	unsigned int texID = m_glCacheTexIDs[ _cacheID ];
	if( m_glCacheTexIDs[ _cacheID ] >= 0 )
	{
		glDeleteTextures( 1, &texID );
	}
	m_glCacheTexIDs[ _cacheID ] = -1;
}

//----------------------------------------------------------------------------------

bool GLSLRenderer::InitialiseShaders( std::string _vsFile, std::string _fsPartFile )
{
	m_shader = new Shader( _vsFile, _fsPartFile );

	// Read in VERTEX SHADER file 
	m_vertShaderString = m_shader->fileRead( _vsFile.c_str() );
	
	// Read in FRAGMENT SHADER (first part)
	m_fragShaderString = m_shader->fileRead( _fsPartFile.c_str() );

	return RebuildTree();
}

//----------------------------------------------------------------------------------

bool GLSLRenderer::RebuildTree()
{
	#ifdef _DEBUG
	{
		GLenum err = glGetError();
		if( err != GL_NO_ERROR)
		{
			std::cerr<<"ERROR: GLSLRenderer::RebuildTree() OpenGL Error before rebuilding tree: "<<gluErrorString(err)<<std::endl;
		}
	}
	#endif
	
	// Make sure the uniform lists are up to date
	m_functionTree->UpdateParameters( this );
	BuildParameterUniformLists();

	// Make sure the tree's cached bounding box size is up-to-date
	m_functionTree->CalcBoundingBox();
	
	// Change camera's position and aiming point
	RefreshCameraPos();

	// FRAGMENT SHADER (update)
	// Retrieve function string from VolumeTree
	std::string functionString = m_functionTree->GetCachedFunctionGLSLString();
	//std::string functionString = m_functionTree->GetFunctionGLSLString();

	// Append function string to first fragment shader part
	std::string fullFragShaderString = GetParameterGLSLDeclaration();
	fullFragShaderString.append( m_fragShaderString );
	fullFragShaderString.append( functionString );

	#ifdef _DEBUG
		std::cout << "INFO: GLSLRenderer::RebuildTree(): function string for fragment shader is now: \n" << functionString << std::endl;
	#endif

	// Finally initialise our shader program
	m_shader->init( m_vertShaderString, fullFragShaderString );
	glBindAttribLocation( m_shader->getID(), 0, "vPosition" );
	m_shader->link();

	return true;
}

//----------------------------------------------------------------------------------

unsigned int GLSLRenderer::GetNumUsedRawParameters()
{
	unsigned int paramSizeCount = 0;
	for( std::vector< TreeParameter >::iterator it = m_paramList.begin(); it != m_paramList.end(); ++it )
	{
		// based on number of vec4s
		if( (unsigned int) it->m_type <= 3 )
		{
			paramSizeCount += 4;
		}
		else
		{
			paramSizeCount += 16;
		}
	}
	return paramSizeCount;
}

//----------------------------------------------------------------------------------

bool GLSLRenderer::ParametersStillAvailable()
{
	return GetNumUsedRawParameters() < m_maxParametersRaw;
}

//----------------------------------------------------------------------------------

unsigned int GLSLRenderer::NewParameter( ParameterType _type )
{
	// Note that our ID approach is to assume a uint has enough IDs without keeping track of freed ones
	// This means over 4billion IDs so we're probably ok

	// Need to prevent allocating too many params when we don't really have the space
	if( ( GetNumUsedRawParameters() + 16 ) >= m_maxParametersRaw )
	{
		std::cerr << "WARNING: GLSL Renderer: Maximum tree parameters reached, cannot allocate new parameter. MaxParametersRaw: " << m_maxParametersRaw << std::endl;
		return 0;
	}
	m_paramIDCount++;
	m_paramList.push_back( TreeParameter( _type, m_paramIDCount ) );
	return m_paramIDCount;
}

//----------------------------------------------------------------------------------

void GLSLRenderer::DeleteParameter( unsigned int _paramID )
{
	if( _paramID == 0 )
	{
		return;
	}
	// Note that our ID approach is to assume a uint has enough IDs without keeping track of freed ones
	// This means over 4billion IDs so we're probably ok
	if( _paramID == m_paramIDCount )
	{
		m_paramIDCount--;
	}

	std::vector< TreeParameter >::iterator it = GetParameter( _paramID );
	if( it != m_paramList.end() )
	{
		delete [] it->m_data;
		m_paramList.erase( it );
	}
	else
	{
		std::cerr << "WARNING: GLSLRenderer DeleteParameter() cannot find parameter of ID: " << _paramID << std::endl;
	}
}

//----------------------------------------------------------------------------------

void GLSLRenderer::SetParameter( unsigned int _paramID, float* _data )
{
	std::vector< TreeParameter >::iterator it = GetParameter( _paramID );
	if( it != m_paramList.end() )
	{
		it->SetData( _data );
	}
	else
	{
		std::cerr << "WARNING: GLSLRenderer DeleteParameter() cannot find parameter of ID: " << _paramID << std::endl;
	}
}


//----------------------------------------------------------------------------------

void GLSLRenderer::TreeParameter::SetData( float *_dataIn )
{
	int dataLength = GLSLRenderer::GetParameterTypeDatasize( m_type );
	if( m_data == NULL )
	{
		m_data = new float[ dataLength ];
	}
	for( int i = 0; i < dataLength; i++ )
	{
		m_data[ i ] = _dataIn[ i ];
	}
}

//----------------------------------------------------------------------------------

unsigned int GLSLRenderer::GetParameterTypeDatasize( ParameterType _value )
{
	unsigned int datasize = ( ( unsigned int ) _value ) + 1;
	if( _value == MAT3 )
	{
		return 9;
	}
	if( _value == MAT4 )
	{
		return 16;
	}
	return datasize;
}

//----------------------------------------------------------------------------------

std::string GLSLRenderer::GetParameterString( unsigned int _paramID )
{
	if( _paramID == 0 )
	{
		return "";
	}
	std::vector< GLSLRenderer::TreeParameter >::iterator param = GetParameter( _paramID );

	unsigned int uniformPos = GetParameterUniformPos( param );

	std::stringstream paramStream;
	paramStream << GetParameterUniformTypeStr( param->m_type, false );
	paramStream << "[" << uniformPos << "]";

	return paramStream.str();
}

//----------------------------------------------------------------------------------

std::vector<GLSLRenderer::TreeParameter>::iterator GLSLRenderer::GetParameter( unsigned int _id )
{
	unsigned int pos;
	return GetParameter( _id, pos );
}

//----------------------------------------------------------------------------------

std::vector<GLSLRenderer::TreeParameter>::iterator GLSLRenderer::GetParameter( unsigned int _id, unsigned int &_position )
{
	if( _id == 0 )
	{
		return m_paramList.end();
	}
	_position = 0;
	std::vector< TreeParameter >::iterator it = m_paramList.begin();
	while( ( it != m_paramList.end() ) && ( it->m_ID != _id ) )
	{
		++it;
		++_position;
	};
	return it;
}

//----------------------------------------------------------------------------------

unsigned int GLSLRenderer::GetParameterUniformPos( std::vector< TreeParameter >::iterator _param )
{
	unsigned int paramCount = 0;
	for( std::vector< TreeParameter >::iterator it = m_paramList.begin(); ( it != m_paramList.end() ) && ( it != _param ); ++it )
	{
		if( it->m_type == _param->m_type )
		{
			paramCount++;
		}
	}
	return paramCount;
}

//----------------------------------------------------------------------------------

std::string GLSLRenderer::GetParameterUniformTypeStr( GLSLRenderer::ParameterType _type, bool _showDataType )
{
	if( _showDataType )
	{
		if( _type == FLOAT )
		{
			return "float paramFloat";
		}
		if( _type == VEC2 )
		{
			return "vec2 paramVec2";
		}
		if( _type == VEC3 )
		{
			return "vec3 paramVec3";
		}
		if( _type == VEC4 )
		{
			return "vec4 paramVec4";
		}
		if( _type == MAT3 )
		{
			return "mat3 paramMat3";
		}
		if( _type == MAT4 )
		{
			return "mat4 paramMat4";
		}
		return "ERROR! GLSLRenderer::ParameterType Unknown!!!";
	}
	else
	{
		if( _type == FLOAT )
		{
			return "paramFloat";
		}
		if( _type == VEC2 )
		{
			return "paramVec2";
		}
		if( _type == VEC3 )
		{
			return "paramVec3";
		}
		if( _type == VEC4 )
		{
			return "paramVec4";
		}
		if( _type == MAT3 )
		{
			return "paramMat3";
		}
		if( _type == MAT4 )
		{
			return "paramMat4";
		}
		return "ERROR! GLSLRenderer::ParameterType Unknown!!!";
	}
}

//----------------------------------------------------------------------------------

void GLSLRenderer::BuildParameterUniformLists()
{
	// First, work out lengths of the lists
	unsigned int numParamTypes = GetParameterTypeSize();
	if( m_paramUniformListLengths == NULL )
	{
		m_paramUniformListLengths = new unsigned int[ numParamTypes ];
	}
	for( unsigned int i = 0; i < numParamTypes; i++ )
	{
		m_paramUniformListLengths[ i ] = 0;
	}
	for( std::vector< TreeParameter >::iterator it = m_paramList.begin(); it != m_paramList.end(); ++it )
	{
		unsigned int index = GetParameterTypeIndex( it->m_type );
		it->m_shaderUniformOffset = m_paramUniformListLengths[ index ];
		m_paramUniformListLengths[ index ] += 1;
	}

	// Allocate the lists
	if( m_paramUniformData == NULL )
	{
		m_paramUniformData = new float *[ numParamTypes ];

		for( unsigned int i = 0; i < numParamTypes; i++ )
		{
			unsigned int arraySize = m_paramUniformListLengths[ i ] * GetParameterTypeDatasize( static_cast< ParameterType >( i ) );
			m_paramUniformData[ i ] = new float[ arraySize ];
		}
	}
	else
	{
		for( unsigned int i = 0; i < numParamTypes; i++ )
		{
			delete [] m_paramUniformData[ i ];
			unsigned int arraySize = m_paramUniformListLengths[ i ] * GetParameterTypeDatasize( static_cast< ParameterType >( i ) );
			m_paramUniformData[ i ] = new float[ arraySize ];
		}
	}

	// Populate with data
	for( unsigned int typeIndex = 0; typeIndex < numParamTypes; typeIndex++ )
	{
		unsigned int dataIndex = 0;
		unsigned int dataLength = GetParameterTypeDatasize( static_cast< ParameterType >( typeIndex ) );

		// We only just allocated the uniform offsets so the things must be in the same order still
		for( std::vector< TreeParameter >::iterator it = m_paramList.begin(); it != m_paramList.end(); ++it )
		{
			if( it->m_type == static_cast< ParameterType >( typeIndex ) )
			{
				for( unsigned int i = 0; i < dataLength; i++ )
				{
					float dataEntry = 0.0f;
					if( it->m_data != NULL )
					{
						dataEntry = it->m_data[ i ];
					}
					m_paramUniformData[ typeIndex ][ ( dataIndex * dataLength ) + i] = dataEntry;
				}
				dataIndex++;
			}
		}
	}
}

//----------------------------------------------------------------------------------

std::string GLSLRenderer::GetParameterGLSLDeclaration()
{
	std::stringstream shaderStream;
	unsigned int numParamTypes = GetParameterTypeSize();
	for( unsigned int i = 0; i < numParamTypes; i++ )
	{
		if( m_paramUniformListLengths[ i ] > 0 )
		{
			shaderStream << "uniform " << GetParameterUniformTypeStr( static_cast< ParameterType >( i ), true ) << "[" << m_paramUniformListLengths[ i ] << "];" << std::endl;
		}
	}
	return shaderStream.str();
}

//----------------------------------------------------------------------------------

void GLSLRenderer::BindParametersToGL()
{
	for( unsigned int currentUniformType = 0; currentUniformType < GetParameterTypeSize(); currentUniformType++ )
	{
		unsigned int numUniforms = m_paramUniformListLengths[ currentUniformType ];
		if( numUniforms > 0 )
		{
			ParameterType currentType = static_cast< ParameterType >( currentUniformType );
			int uniformLocation = glGetUniformLocation( m_shader->getID(), GetParameterUniformTypeStr( currentType, false ).c_str() );
			if( uniformLocation >= 0 )
			{
				if( currentType == FLOAT )
				{
					glUniform1fv( uniformLocation, numUniforms, m_paramUniformData[ currentUniformType ] );
				}
				else if( currentType == VEC2 )
				{
					glUniform2fv( uniformLocation, numUniforms, m_paramUniformData[ currentUniformType ] );
				}
				else if( currentType == VEC3 )
				{
					glUniform3fv( uniformLocation, numUniforms, m_paramUniformData[ currentUniformType ] );
				}
				else if( currentType == VEC4 )
				{
					glUniform4fv( uniformLocation, numUniforms, m_paramUniformData[ currentUniformType ] );
				}
				else if( currentType == MAT3 )
				{
					glUniformMatrix3fv( uniformLocation, numUniforms, false, m_paramUniformData[ currentUniformType ] );
				}
				else if( currentType == MAT4 )
				{
					glUniformMatrix4fv( uniformLocation, numUniforms, false, m_paramUniformData[ currentUniformType ] );
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------

void GLSLRenderer::RefreshCameraPos()
{
	float centre[ 3 ] = { 0.0f, 0.0f, 0.0f };
	m_functionTree->GetBoundingBoxCentre( centre );
	float temp = centre[ 1 ];
	centre[ 1 ] = centre[ 2 ];
	centre[ 2 ] = temp;
	m_target.SetAim( cml::vector3f( centre ) );

	float bboxSize[ 3 ] = { 1.0f, 1.0f, 1.0f };
	m_functionTree->GetBoundingBoxSize( bboxSize );
	

	float bboxMaxDim = m_functionTree->GetBoundingBoxMaxDim();
	float camDist = ( bboxMaxDim * 0.5f ) / ( tan( ( 2.0f * PI ) * ( ( 0.4f * m_camAngle ) / 360.0f ) ) );
	m_cam->SetAim( cml::vector3f( 0.0f, centre[ 1 ], camDist ) );

	#ifdef _DEBUG
		std::cout << "INFO: GLSLRenderer::RebuildTree() camera position aim: 0.0f, " << centre[ 1 ] << " " << camDist << std::endl;
		std::cout << "INFO: GLSLRenderer::RebuildTree() camera lookat target position aim: " << centre[ 0 ] << " " << centre[ 1 ] << " " << centre[ 2 ] << std::endl;
	#endif

	m_camNearPlane = camDist * 0.1f;
	m_camFarPlane = camDist * 10.0f; // This can sometimes cause problems if the previous object was very big etc
}

//----------------------------------------------------------------------------------

void GLSLRenderer::InitialiseCube()
{
	// Cube data
	float cubeVertices[ 8 * 3 ] = { -1.0f,  1.0f, -1.0f,
									-1.0f,  1.0f,  1.0f,
									 1.0f,  1.0f,  1.0f,
									 1.0f,  1.0f, -1.0f,
								    -1.0f, -1.0f, -1.0f,
								    -1.0f, -1.0f,  1.0f,
								 	 1.0f, -1.0f,  1.0f,
									 1.0f, -1.0f, -1.0f };

	int cubeIndices[ 24 ] = {   0, 1, 2, 3,
								0, 3, 7, 4,
								0, 4, 5, 1,
								1, 5, 6, 2,
								3, 2, 6, 7,
								7, 6, 5, 4 };


	GLuint vertsVBO;
	glGenBuffers( 1, &vertsVBO );
	glBindBuffer( GL_ARRAY_BUFFER, vertsVBO );
	glBufferData( GL_ARRAY_BUFFER, 8 * 3 * sizeof( float ), cubeVertices, GL_STATIC_DRAW );

	glGenVertexArrays( 1, &m_cubeVAO );
	glBindVertexArray( m_cubeVAO );

	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, vertsVBO );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ( GLfloat* )NULL );

	GLuint indicesVBO;
	glGenBuffers( 1, &indicesVBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indicesVBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof( int ), cubeIndices, GL_STATIC_DRAW );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

}

//----------------------------------------------------------------------------------

