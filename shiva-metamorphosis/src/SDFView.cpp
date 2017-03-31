#include "SDFView.h"
#include "GUI/GUIController.h"
#include "vol_metamorph.h"


#ifndef PI
#define PI 3.141592653589793238462643383279502884
#endif

//----------------------------------------------------------------------------------

SDFView::SDFView()
{
	GLenum err = glGetError();
	if( err != GL_NO_ERROR)
	{
		std::cerr << " SDFView::Ctor, OpenGL Error at start: " << gluErrorString( err ) << std::endl;
	}

	m_boundsLeft = m_boundsTop = m_boundsRight = m_boundsBottom = m_windowWidth = m_windowHeight = 0;

	m_rotationSpeed = 1.0f;
	m_pauseRotation = false;

	// TODO: hopefully a lot of these viewing-related things can be given as parameters with the XML stuff

	m_axisX.set( 1.f, 0.f, 0.f, 0.f );
	m_axisY.set( 0.f, 1.f, 0.f, 0.f );
	m_axisZ.set( 0.f, 0.f, 1.f, 0.f );

	// TODO: fix all camera stuff...

	m_cam = new Camera();
	m_cam->SetPos( 0.f, 2.f, 3.f );

	m_target.SetPos( 0.f, 0.f, 0.f );

	//_cameraPosition.set(0,2,3);
	//_targetPosition.set(0,0,0);
	//cml::vector3f up;
	//up.cardinal(1);
	//matrix_look_at_RH( m_modelViewMatrix, _cameraPosition, _targetPosition, up);

	m_cam->LookAt( m_cam->GetCurrent(), m_target.GetCurrent() );
	m_modelViewMatrix = m_cam->GetViewMatrix();

	m_projectionMatrix.identity();
	m_invModelViewMatrix = m_modelViewMatrix;
	cml::inverse( m_invModelViewMatrix );

	m_modelMatrix.identity();

	//cml::matrix_perspective_RH(_projectionMatrix,-1.0f,1.0f,-1.0f,1.0f,-1.0f,100.0f, cml::z_clip_neg_one);
	
	m_localRotX = -PI / 2.0f;
	m_localRotY = 0.0f;
	m_localRotZ = 0.0f;

	m_worldRotX = m_worldRotY = m_worldRotZ = 0.0f;
	
	m_camAngle = 35.0f;
	m_camNearPlane = 0.1f;
	m_camFarPlane = 100.0f;

	BuildVBOs();

	// Create SDF shader
	m_SDFShader = new Utility::GPUProgram();
	// TODO: pass in program name from xml?
	m_SDFShader->Create( "Resources/Shaders/morph", Utility::GPUProgram::VERTEX_AND_FRAGMENT );

	m_parameterValues[ 0 ] = 0.0f;
	m_parameterValues[ 1 ] = 32.0f; // TODO: put this in xml and allow to be tweaked - was at 32
	m_parameterValues[ 2 ] = 0.01f;
	m_parameterValues[ 3 ] = 1.0f;

	m_blendValue = 0.0f;

	m_colourR = m_colourG = m_colourB = 1.0f;
	m_colourSet = 0.0f;

	err = glGetError();
	if( err != GL_NO_ERROR )
	{
		std::cerr << "SDFView::Ctor, OpenGL Error at end before load volumes: " << gluErrorString( err ) << std::endl;
	}

	m_dataLoader = new VolumeLoader();
	m_volATicket = 0;
	m_volBTicket = 0;
	m_texVolA = 0;
	m_texVolB = 0;

	m_loadingDrawable = NULL;
}

//----------------------------------------------------------------------------------

SDFView::~SDFView()
{
	delete m_SDFShader;

	glDeleteVertexArrays( 1, &m_cubeVAO );

	delete m_dataLoader;
	delete m_loadingDrawable;
	delete m_cam;
}

 //----------------------------------------------------------------------------------

void SDFView::BuildVBOs()
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

	GLuint cubeIndices[ 24 ] = {   0, 1, 2, 3,
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
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof( GLuint ), cubeIndices, GL_STATIC_DRAW );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

}

//----------------------------------------------------------------------------------

void SDFView::Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	m_boundsLeft = _left;
	m_boundsRight = _right;
	m_boundsTop = _top;
	m_boundsBottom = _bottom;
	m_windowWidth = _windowWidth;
	m_windowHeight = _windowHeight;

	float aspectRatio = ( float )( ( _right - _left ) / ( _bottom - _top ) );
	//float aspectRatio = ( ( float ) m_windowWidth ) / ( ( float ) m_windowHeight );
	
	//cml::matrix_perspective_xfov_RH( m_projectionMatrix, m_camAngle, aspectRatio, m_camNearPlane, m_camFarPlane, cml::z_clip_neg_one );
	//int viewport[4]; // Shouldn't really do this, but temporarily it's fine
	//glGetIntegerv( GL_VIEWPORT, viewport );

	//float aspectRatio = ( float )viewport[ 2 ] / ( float )viewport[ 3 ];
	//m_cam->SetProjectionMatrix( m_camAngle, aspectRatio, m_camNearPlane, m_camFarPlane );

	if( m_loadingDrawable != NULL )
		m_loadingDrawable->SetBounds( _left, _top, _right, _bottom );
}

//----------------------------------------------------------------------------------

void SDFView::Inflate( TiXmlElement *_xmlElement, ShivaGUI::ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "SDFView_";
	View::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( ( std::string( "cameraPositionX" ) == currentAttribute->Name() ) || ( _themePrefix + "cameraPositionX" == currentAttribute->Name() ) )
		{
			//_cameraPosition.data()[ 0 ] = currentAttribute->DoubleValue();
			m_cam->SetPosX( currentAttribute->DoubleValue() );
		}
		else if( ( std::string( "cameraPositionY" ) == currentAttribute->Name() ) || ( _themePrefix + "cameraPositionY" == currentAttribute->Name() ) )
		{
			//_cameraPosition.data()[ 1 ] = currentAttribute->DoubleValue();
			m_cam->SetPosY( currentAttribute->DoubleValue() );
		}
		else if( ( std::string( "cameraPositionZ" ) == currentAttribute->Name() ) ||  ( _themePrefix + "cameraPositionZ" == currentAttribute->Name() ) )
		{
			//_cameraPosition.data()[ 2 ] = currentAttribute->DoubleValue();
			m_cam->SetPosZ( currentAttribute->DoubleValue() );
		}

		else if( ( std::string( "cameraTargetX" ) == currentAttribute->Name() ) || ( _themePrefix + "cameraTargetX" == currentAttribute->Name() ) )
		{
			m_target.SetPosX( currentAttribute->DoubleValue() );
			//_targetPosition.data()[ 0 ] = currentAttribute->DoubleValue();
		}
		else if( ( std::string( "cameraTargetY" ) == currentAttribute->Name() ) || ( _themePrefix + "cameraTargetY" == currentAttribute->Name() ) )
		{
			m_target.SetPosY( currentAttribute->DoubleValue() );
			//_targetPosition.data()[ 1 ] = currentAttribute->DoubleValue();
		}
		else if( ( std::string( "cameraTargetZ" ) == currentAttribute->Name() ) || ( _themePrefix + "cameraTargetZ" == currentAttribute->Name() ) )
		{
			m_target.SetPosZ( currentAttribute->DoubleValue() );
			//_targetPosition.data()[ 2 ] = currentAttribute->DoubleValue();
		}
		else if( ( std::string( "quality" ) == currentAttribute->Name() ) )
		{
			SetQuality( currentAttribute->DoubleValue() );
		}
		else if( ( std::string( "loadingDrawable" ) == currentAttribute->Name() ) || ( _themePrefix + "loadingDrawable" == currentAttribute->Name() ) )
		{
			std::string resourceName( _resources->GetInflationAttribute( currentAttribute->Value() ) );
			m_loadingDrawable = _resources->GetDrawable( resourceName );
		}
	}
	
	// TODO: deflation attributes
}

//----------------------------------------------------------------------------------

void SDFView::AddWorldRotationOffsetDegs( const float &_rotX, const float &_rotY, const float &_rotZ )
{
	AddWorldRotationOffsetRads( ( 2.0f * PI ) *( _rotX / 360.0f ), ( 2.0f * PI ) * ( _rotY / 360.0f ), ( 2.0f * PI ) * ( _rotZ / 360.0f ) );
}

//----------------------------------------------------------------------------------

void SDFView::AddWorldRotationOffsetRads( const float &_rotX, const float &_rotY, const float &_rotZ )
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

void SDFView::AddRotationOffsetDegs( const float &_rotX, const float &_rotY, const float &_rotZ )
{
	AddRotationOffsetRads( ( 2.0f * PI ) * ( _rotX / 360.0f ), ( 2.0f * PI ) * ( _rotY / 360.0f ), ( 2.0f * PI ) * ( _rotZ / 360.0f ) );
}

//----------------------------------------------------------------------------------

void SDFView::AddRotationOffsetRads( const float &_rotX, const float &_rotY, const float &_rotZ )
{
	m_localRotX = std::fmod( m_localRotX + _rotX, ( float )( 2.0f * PI ) );
	m_localRotY = std::fmod( m_localRotY + _rotY, ( float )( 2.0f * PI ) );
	m_localRotZ = std::fmod( m_localRotZ + _rotZ, ( float )( 2.0f * PI ) );
}

//----------------------------------------------------------------------------------

void SDFView::SetQuality( const float &_value )
{
	m_parameterValues[ 1 ] = _value;
	if( m_parameterValues[ 1 ] < 1.0f )
		m_parameterValues[ 1 ] = 1.0f;
}

//----------------------------------------------------------------------------------

void SDFView::Update( float _deltaTs, ShivaGUI::GUIController *_guiController )
{
	if( m_volATicket && m_dataLoader->IsJobReady( m_volATicket ) )
	{
		m_dataLoader->GetJobToGL( m_volATicket, m_texVolA);
		m_dataLoader->GetJobBounds( m_volATicket, m_boundMin[ 0 ], m_boundMax[ 0 ] );
		m_volATicket = 0;
	}
	if( m_volBTicket && m_dataLoader->IsJobReady( m_volBTicket ) )
	{
		m_dataLoader->GetJobToGL( m_volBTicket, m_texVolB );
		m_dataLoader->GetJobBounds( m_volBTicket, m_boundMin[ 1 ], m_boundMax[ 1 ] );
		m_volBTicket = 0;
	}

	if( !m_pauseRotation )
	{
		m_localRotZ -= _deltaTs * m_rotationSpeed;
		if( m_localRotZ > ( 2.0f * PI ) )
		{
			double temp;
			m_localRotZ = modf( m_localRotZ / ( 2.0f * PI ), &temp );
		}
	}

	//cml::vector3f up;
	//up.cardinal( 1 );
	
	//cml::matrix_look_at_RH( m_modelViewMatrix, _cameraPosition, _targetPosition, up );

	m_cam->LookAt( m_cam->GetCurrent(), m_target.GetCurrent() );
	m_cam->Rotate( m_localRotX, m_localRotY, m_localRotZ );

	cml::matrix44f_c temp;
	
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


	//m_modelViewMatrix = m_modelViewMatrix * m_modelMatrix;
	m_modelViewMatrix = m_cam->GetViewMatrix() * m_modelMatrix;
	m_invModelViewMatrix = m_modelViewMatrix;
	m_invModelViewMatrix = cml::inverse( m_invModelViewMatrix );
}

//----------------------------------------------------------------------------------

void SDFView::LoadMatricesToShader()
{
	GLint mvLoc = glGetUniformLocation( m_SDFShader->GetProgramID(), "u_ModelViewMatrix" );
	if( mvLoc != -1 ) { glUniformMatrix4fv( mvLoc, 1, GL_FALSE, m_modelViewMatrix.data() ); }
	else { std::cerr << "u_ModelViewMatrix not found in shader " << m_SDFShader->GetProgramID() << std::endl; }

	// Inverse ModelView matrix
	GLint imvLoc = glGetUniformLocation( m_SDFShader->GetProgramID(), "u_InverseModelViewMatrix" );
	if( imvLoc != -1 ) { glUniformMatrix4fv( imvLoc, 1, GL_FALSE, m_invModelViewMatrix.data() ); }
	else { std::cerr << "u_InverseModelViewMatrix not found in shader " << m_SDFShader->GetProgramID() << std::endl; }

	// Projection matrix	
	GLint pLoc = glGetUniformLocation( m_SDFShader->GetProgramID(), "u_ProjectionMatrix" );
	//if( pLoc != -1 ) { glUniformMatrix4fv( pLoc, 1, GL_FALSE, m_cam->GetProjectionMatrix().data() ); }
	if( pLoc != -1 ) { glUniformMatrix4fv( pLoc, 1, GL_FALSE, m_projectionMatrix.data() ); }
	else { std::cerr << "u_ProjectionMatrix not found in shader " << m_SDFShader->GetProgramID() << std::endl; }
}

//----------------------------------------------------------------------------------

void SDFView::Draw()
{
	View::Draw();
	if( !m_visible )
		return;

	if( ( m_volATicket != 0 ) || ( m_volBTicket != 0 ) )
	{
		if( m_loadingDrawable != NULL )
			m_loadingDrawable->Draw();
		return;
	}

	int viewport[4]; 
	glGetIntegerv( GL_VIEWPORT, viewport );

	// Set glViewport to extent of View
	glViewport( m_boundsLeft, m_windowHeight - m_boundsBottom, m_boundsRight - m_boundsLeft, m_boundsBottom - m_boundsTop );

	//float aspectRatio = ( float )viewport[ 2 ] / ( float )viewport[ 3 ];

	float aspectRatio = ( ( float )m_boundsRight - m_boundsLeft ) / ( ( float )m_boundsBottom - m_boundsTop );
	//m_cam->SetProjectionMatrix( m_camAngle, aspectRatio, m_camNearPlane, m_camFarPlane );

	cml::matrix_perspective_yfov_RH( m_projectionMatrix, cml::rad(m_camAngle), aspectRatio, m_camNearPlane, m_camFarPlane, cml::z_clip_neg_one );

	//glLoadMatrixf( m_cam->GetProjectionMatrix().data() );
	//gluPerspective( m_camAngle, aspectRatio, m_camNearPlane, m_camFarPlane);


	// Enable depth test
	glEnable( GL_DEPTH_TEST );

	// Bind Shaders

	m_SDFShader->Bind();

	LoadMatricesToShader();

	cml::vector3f extent = ( m_boundMax[ 0 ] - m_boundMin[ 0 ] ) * 0.5f;
	cml::vector3f centre = ( m_boundMax[ 0 ] + m_boundMin[ 0 ] ) * 0.5f;

	glUniform3fv( glGetUniformLocation( m_SDFShader->GetProgramID(), "boxSize" ), 1, extent.data() );
	glUniform3fv( glGetUniformLocation( m_SDFShader->GetProgramID(), "boxCentre" ), 1, centre.data() );

	glUniform4fv( glGetUniformLocation( m_SDFShader->GetProgramID(), "inputColour" ), 1, cml::vector4f( m_colourR, m_colourG, m_colourB, m_colourSet ).data() );

	glUniform3fv( glGetUniformLocation( m_SDFShader->GetProgramID(), "fragboundmin" ), 1, m_boundMin[ 0 ].data() );
	glUniform3fv( glGetUniformLocation( m_SDFShader->GetProgramID(), "fragboundmax" ), 1, m_boundMax[ 0 ].data() );
	glUniform1i( glGetUniformLocation( m_SDFShader->GetProgramID(), "volume1" ), 0 );
	glUniform1i( glGetUniformLocation( m_SDFShader->GetProgramID(), "volume2" ), 1 );

	glUniform1f( glGetUniformLocation( m_SDFShader->GetProgramID(), "time" ), m_blendValue );

	glUniform1f( glGetUniformLocation( m_SDFShader->GetProgramID(), "a0" ), 5.0f );
	glUniform1f( glGetUniformLocation( m_SDFShader->GetProgramID(), "a1" ), 5.0f );
	glUniform1f( glGetUniformLocation( m_SDFShader->GetProgramID(), "a2" ), 5.0f );
	glUniform1f( glGetUniformLocation( m_SDFShader->GetProgramID(), "a3" ), 2.5f );

	glUniform1f( glGetUniformLocation( m_SDFShader->GetProgramID(), "clipdist" ), 0.15f );
	glUniform1f( glGetUniformLocation( m_SDFShader->GetProgramID(), "timegap" ), 0.5f );


	glBindVertexArray( m_cubeVAO );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_3D, m_texVolA );
				
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_3D, m_texVolB );
				
	// Draw cube using index array
	glDrawElements( GL_QUADS, 24, GL_UNSIGNED_INT, 0 );

	glBindVertexArray( 0 );
	
	glBindTexture( GL_TEXTURE_3D, 0 );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_3D, 0 );

	// Unbind shaders
	m_SDFShader->Unbind();

	// Disable depth test
	glDisable( GL_DEPTH_TEST );

	// Restore GL Viewport to previous state
	glViewport( 0, 0, viewport[ 2 ], viewport[ 3 ] );
}

//----------------------------------------------------------------------------------

bool SDFView::HandleEvent( ShivaGUI::InternalEvent* )
{
	return false;
}

//----------------------------------------------------------------------------------

bool SDFView::LoadVolumeFromFile( std::string _filename, bool _objectB, ShivaGUI::GUIController *_guiController )
{
	std::cout << "INFO: *** Loading VOL file: " << _filename << " ***" << std::endl;

	if( !_objectB )
	{
		m_objectAfilename = _filename;
	
		// Initialise openGL texture
		// This is now a managed resource, if it already exists we don't have to reload it
		if( _guiController->GetResources()->GetManagedTexture( _filename, &m_texVolA ) )
		{
			// Resource needs to be loaded
			m_volATicket = m_dataLoader->AddJobLoad( _filename );
		}
		else
		{
			m_boundMin[ 0][ 0 ] = -1.0f;
			m_boundMin[ 0][ 1 ] = -1.0f;
			m_boundMin[ 0][ 2 ] = -1.0f;
			
			m_boundMax[ 0 ][ 0 ] =  1.0f;
			m_boundMax[ 0 ][ 1 ] =  1.0f;
			m_boundMax[ 0 ][ 2 ] =  1.0f;

			std::cout << "INFO: Volume A retrieved from cache" << std::endl;
		}
	}
	else
	{
		m_objectBfilename = _filename;
		
		// Initialise openGL texture
		// This is now a managed resource, if it already exists we don't have to reload it
		if( _guiController->GetResources()->GetManagedTexture( _filename, &m_texVolB ) )
		{
			// Resource needs to be loaded
			m_volBTicket = m_dataLoader->AddJobLoad( _filename );
		}
		else
		{
			m_boundMin[ 1 ][ 0 ] = -1.0f;
			m_boundMin[ 1 ][ 1 ] = -1.0f;
			m_boundMin[ 1 ][ 2 ] = -1.0f;
			
			m_boundMax[ 1 ][ 0 ] =  1.0f;
			m_boundMax[ 1 ][ 1 ] =  1.0f;
			m_boundMax[ 1 ][ 2 ] =  1.0f;

			std::cout << "INFO: Volume B retrieved from cache" << std::endl;
		}
	}
	return true;
}

//----------------------------------------------------------------------------------

bool SDFView::SaveVolumeToFile( std::string _filename )
{
	std::cout << "INFO: *** Saving to VOL file: " << _filename << " ***" << std::endl;

	metamorph_params params;
	params.m_alpha = m_blendValue;
	params.m_type = METAMORPH_LINEAR;

	return saveVol( m_objectAfilename, m_objectBfilename, params, _filename );

}

//----------------------------------------------------------------------------------