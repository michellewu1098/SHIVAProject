#include "VolView.h"

#ifndef PI
#define PI 3.141592653589793238462643383279502884
#endif

//----------------------------------------------------------------------------------

VolView::VolView()
{
	// For mouse rotating:
	m_mousing = false;
	m_mouseLastX = m_mouseLastY = 0;
	m_rotationX = m_rotationZ = 0.0f;
	m_mouseRefX = m_mouseRefY = 0;

	m_mousingTimer = 0.0f;
	m_clickSelectTimeThreshold = 0.5f;
	m_allowClickSelect = false;
	m_selectVecOrigX = m_selectVecOrigY = m_selectVecOrigZ = 0.0f;
	m_selectVecDirX = 0.0f; 
	m_selectVecDirY = 0.0f;
	m_selectVecDirZ = -1.0f;

	m_showCrosshairs = false;
	m_crosshairX = m_crosshairY = 0.5f;
	m_targetSize = 0.1f;

	m_totemController = Totem::Controller::GetInstance();

	m_mainTree = new VolumeTree::Tree();

	m_renderer = new GLSLRenderer( 640, 480 );
	m_renderer->ReserveCaches( 10 );
	m_renderer->SetTree( m_mainTree );

	glGenVertexArrays( 1, &m_crosshairVAO );
	glGenVertexArrays( 1, &m_crosshairCircleVAO );

	m_crosshairShader = new Utility::GPUProgram();
	m_crosshairShader->Create( "Resources/Shaders/Crosshair", Utility::GPUProgram::VERTEX_AND_FRAGMENT );
	glBindAttribLocation( m_crosshairShader->GetProgramID(), 0, "vPosition" );
	m_crosshairShader->LinkProgram();

	m_crosshairCircleShader = new Utility::GPUProgram();
	m_crosshairCircleShader->Create( "Resources/Shaders/Crosshair", Utility::GPUProgram::VERTEX_AND_FRAGMENT );
	glBindAttribLocation( m_crosshairCircleShader->GetProgramID(), 0, "vPosition" );
	m_crosshairCircleShader->LinkProgram();
	
	m_cachePolicy = new VolumeTree::CachingPolicy();
	
	std::cout << "INFO: Building caches, please wait..." << std::endl;
	m_mainTree->BuildCaches( m_cachePolicy, m_renderer );
	
#ifdef _DEBUG
	std::cout << "INFO: Test tree cached GLSL output: " << m_mainTree->GetCachedFunctionGLSLString() << std::endl;
#endif

	if( !m_renderer->Initialise( "Resources/Shaders/VolRenderer.vert", "Resources/Shaders/VolRendererPart.frag" ) )
	{
		std::cerr << "ERROR: Fatal Vol Renderer Initialisation Failure" << std::endl;
	}

	RefreshTree();

	m_commandManager = Totem::CommandManager::GetInstance();
}

//----------------------------------------------------------------------------------

VolView::~VolView()
{
	delete m_mainTree;
	delete m_renderer;
	delete m_cachePolicy;
	delete m_crosshairShader;
	delete m_crosshairCircleShader;
}

//----------------------------------------------------------------------------------

void VolView::Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	View::Layout( _left, _top, _right, _bottom, _windowWidth, _windowHeight );

	m_boundsLeft = _left;
	m_boundsRight = _right;
	m_boundsTop = _top;
	m_boundsBottom = _bottom;
	m_windowWidth = _windowWidth;
	m_windowHeight = _windowHeight;

	m_renderer->SetWindowDimensions( ( float )( _right - _left ), ( float )( _bottom - _top ) );

	//_renderer->SetWindowDimensions( ( float )( windowWidth), ( float )( windowHeight ) );

//	float aspectRatio = ((float)right-left) / ((float)bottom-top);
//	cml::matrix_perspective_xfov_RH(_projectionMatrix, _cameraAngle, aspectRatio, _cameraNearPlane, _cameraFarPlane, cml::z_clip_neg_one);

}

//----------------------------------------------------------------------------------

void VolView::Inflate( TiXmlElement *_xmlElement, ShivaGUI::ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "VolView_";
	View::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );

	//for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	//{
		/*
		if( (std::string("cameraPositionX") == currentAttribute->Name()) || (themePrefix+"cameraPositionX" == currentAttribute->Name()) )
		{
			_cameraPosition.data()[0] = currentAttribute->DoubleValue();
		}
		else if( (std::string("cameraPositionY") == currentAttribute->Name()) || (themePrefix+"cameraPositionY" == currentAttribute->Name()) )
		{
			_cameraPosition.data()[1] = currentAttribute->DoubleValue();
		}
		else if( (std::string("cameraPositionZ") == currentAttribute->Name()) ||  (themePrefix+"cameraPositionZ" == currentAttribute->Name()) )
		{
			_cameraPosition.data()[2] = currentAttribute->DoubleValue();
		}

		else if( (std::string("cameraTargetX") == currentAttribute->Name()) || (themePrefix+"cameraTargetX" == currentAttribute->Name()) )
		{
			_targetPosition.data()[0] = currentAttribute->DoubleValue();
		}
		else if( (std::string("cameraTargetY") == currentAttribute->Name()) || (themePrefix+"cameraTargetY" == currentAttribute->Name()) )
		{
			_targetPosition.data()[1] = currentAttribute->DoubleValue();
		}
		else if( (std::string("cameraTargetZ") == currentAttribute->Name()) || (themePrefix+"cameraTargetZ" == currentAttribute->Name()) )
		{
			_targetPosition.data()[2] = currentAttribute->DoubleValue();
		}
		else if( (std::string("quality") == currentAttribute->Name()) )
		{
			SetQuality( currentAttribute->DoubleValue() );
		}
		*/
	//}
}

//----------------------------------------------------------------------------------

TiXmlElement* VolView::Deflate( ShivaGUI::ResourceManager *_resources )
{
	TiXmlElement *xmlNode = View::Deflate( _resources );
	xmlNode->SetValue( "VolView" );

	return xmlNode;
}

//----------------------------------------------------------------------------------

void VolView::Update( float _deltaTs, ShivaGUI::GUIController *_guiController )
{
	m_renderer->Update( _deltaTs );
	if( m_mousing )
		m_mousingTimer += _deltaTs;
}

//----------------------------------------------------------------------------------

void VolView::LoadMatricesToShader( GLuint _shaderID, cml::matrix44f_c _proj, cml::matrix44f_c _mv )
{
	// ModelView matrix
	GLint mvLoc = glGetUniformLocation( _shaderID, "u_ModelViewMatrix" );
	if( mvLoc != -1 ) { glUniformMatrix4fv( mvLoc, 1, GL_FALSE, _mv.data() ); }
	else { std::cerr << "u_ModelViewMatrix not found in shader " << _shaderID << std::endl; }


	// Projection matrix	
	GLint pLoc = glGetUniformLocation( _shaderID, "u_ProjectionMatrix" );
	if( pLoc != -1 ) { glUniformMatrix4fv( pLoc, 1, GL_FALSE, _proj.data() ); }
	else { std::cerr << "u_ProjectionMatrix not found in shader " << _shaderID << std::endl; }
}

//----------------------------------------------------------------------------------

void VolView::BuildCrosshairVBOs() 
{
	float verts[ 4 * 3 ] = { 0.0f, m_crosshairY, -2.0f,
							 1.0f, m_crosshairY, -2.0f, 
							 m_crosshairX, 0.0f, -2.0f,
							 m_crosshairX, 1.0f, -2.0f };
	
	GLuint vertVBO;
	glGenBuffers( 1, &vertVBO );

	glBindBuffer( GL_ARRAY_BUFFER, vertVBO );
	glBufferData( GL_ARRAY_BUFFER, 4 * 3 * sizeof( float ), verts, GL_STATIC_DRAW );
	
	glBindVertexArray( m_crosshairVAO );

	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, vertVBO );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ( GLfloat* )NULL );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

//----------------------------------------------------------------------------------

void VolView::BuildCrosshairCircleVBOs()
{
	float camDist = m_renderer->GetCameraDistance();
	float scaleFactor = m_targetSize * 57.0f * ( 1.0f / camDist );
	
	std::vector< float > verts;

	for( float i = 0.0f; i < PI * 2.0f; i += 0.2f )
	{
		verts.push_back( m_crosshairX + ( scaleFactor * cos( i ) ) );
		verts.push_back( m_crosshairY + ( scaleFactor * sin( i ) ) );
		verts.push_back( -2.0f );
	}

	GLuint vertVBO;
	glGenBuffers( 1, &vertVBO );

	glBindBuffer( GL_ARRAY_BUFFER, vertVBO );
	glBufferData( GL_ARRAY_BUFFER, verts.size() * sizeof( float ), &verts[ 0 ], GL_STATIC_DRAW );
	
	glBindVertexArray( m_crosshairCircleVAO );

	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, vertVBO );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ( GLfloat* )NULL );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

//----------------------------------------------------------------------------------

void VolView::Draw( unsigned int _context )
{
	View::Draw();
	if( !m_visible )
		return;

	int viewport[4]; // Viewport data needed to restore the viewport after drawing the volume 
	glGetIntegerv( GL_VIEWPORT, viewport );

	//glEnable( GL_SCISSOR_TEST );

	//glScissor( m_boundsLeft, m_windowHeight - m_boundsBottom, m_boundsRight - m_boundsLeft, m_boundsBottom - m_boundsTop );
	

	//glClear( GL_DEPTH_BUFFER_BIT );
	// Set glViewport to extent of View
	//glDisable( GL_SCISSOR_TEST );

	glViewport( m_boundsLeft, m_windowHeight - m_boundsBottom, m_boundsRight - m_boundsLeft, m_boundsBottom - m_boundsTop );

	// Draw the volume view
	
	m_renderer->Draw( _context );
	
	if( m_showCrosshairs )
	{
		cml::matrix44f_c proj, mv;
		proj.identity();
		cml::matrix_orthographic_RH( proj, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 10.0f, cml::z_clip_neg_one );
		mv.identity();

		// Draw crosshair
		//----------------------------------------------------------------------------------

		BuildCrosshairVBOs();

		m_crosshairShader->Bind();

		glUniform3f( glGetUniformLocation( m_crosshairShader->GetProgramID(), "u_Colour" ), 1.0f, 0.0f, 0.0f );
		
		LoadMatricesToShader( m_crosshairShader->GetProgramID(), proj, mv );

		glLineWidth( 5.0f );

		glBindVertexArray( m_crosshairVAO );
			glDrawArrays( GL_LINES, 0, 4 );
		glBindVertexArray( 0 );
		
		m_crosshairShader->Unbind();

		// Draw crosshair circle
		//----------------------------------------------------------------------------------

		BuildCrosshairCircleVBOs();

		m_crosshairCircleShader->Bind();

		glUniform3f( glGetUniformLocation( m_crosshairCircleShader->GetProgramID(), "u_Colour" ), 1.0f, 0.0f, 0.0f );
		
		LoadMatricesToShader( m_crosshairCircleShader->GetProgramID(), proj, mv );

		glBindVertexArray( m_crosshairCircleVAO );
			glDrawArrays( GL_LINE_LOOP, 0, 32 );
		glBindVertexArray( 0 );

		m_crosshairCircleShader->Unbind();
	}

	// Restore GL Viewport to previous state
	glViewport( 0, 0, viewport[2], viewport[3] );
	
}


//----------------------------------------------------------------------------------

void VolView::Draw()
{
	View::Draw();
	if( !m_visible )
		return;

	int viewport[4]; // Viewport data needed to restore the viewport after drawing the volume 
	glGetIntegerv( GL_VIEWPORT, viewport );

	//glEnable( GL_SCISSOR_TEST );

	//glScissor( _boundsLeft, _windowHeight - _boundsBottom, _boundsRight - _boundsLeft, _boundsBottom - _boundsTop );


	//glClear( GL_DEPTH_BUFFER_BIT );
	// Set glViewport to extent of View
	//glDisable( GL_SCISSOR_TEST );

	glViewport( m_boundsLeft, m_windowHeight - m_boundsBottom, m_boundsRight - m_boundsLeft, m_boundsBottom - m_boundsTop );
	// Draw the volume view
	m_renderer->Draw();

	
	if( m_showCrosshairs )
	{
		cml::matrix44f_c proj, mv;
		proj.identity();
		cml::matrix_orthographic_RH( proj, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 10.0f, cml::z_clip_neg_one );
		mv.identity();

		// Draw crosshair
		//----------------------------------------------------------------------------------

		BuildCrosshairVBOs();

		m_crosshairShader->Bind();

		glUniform3f( glGetUniformLocation( m_crosshairShader->GetProgramID(), "u_Colour" ), 1.0f, 0.0f, 0.0f );
		
		LoadMatricesToShader( m_crosshairShader->GetProgramID(), proj, mv );

		glLineWidth( 5.0f );

		glBindVertexArray( m_crosshairVAO );
			glDrawArrays( GL_LINES, 0, 4 );
		glBindVertexArray( 0 );
		
		m_crosshairShader->Unbind();

		// Draw crosshair circle
		//----------------------------------------------------------------------------------

		BuildCrosshairCircleVBOs();

		m_crosshairCircleShader->Bind();

		glUniform3f( glGetUniformLocation( m_crosshairCircleShader->GetProgramID(), "u_Colour" ), 1.0f, 0.0f, 0.0f );
		
		LoadMatricesToShader( m_crosshairCircleShader->GetProgramID(), proj, mv );

		glBindVertexArray( m_crosshairCircleVAO );
			glDrawArrays( GL_LINE_LOOP, 0, 32 );
		glBindVertexArray( 0 );

		m_crosshairCircleShader->Unbind();
	}

	// Restore GL Viewport to previous state
	glViewport( 0, 0, viewport[ 2 ], viewport[ 3 ] );
	
}

//----------------------------------------------------------------------------------

bool VolView::HandleEvent( ShivaGUI::InternalEvent *_eventIn )
{
	if( _eventIn->GetType() == ShivaGUI::InternalEvent::POSITIONAL_SELECT )
	{
		if( EventHit( _eventIn ) )
		{
			m_mousing = true;
			int posX, posY;
			_eventIn->GetPosition( posX, posY );
			m_mouseLastX = m_mouseRefX = posX;
			m_mouseLastY = m_mouseRefY = posY;
			m_mousingTimer = 0.0f;
		}
		else
		{
			m_mousing = false;
		}
	}
	else if( _eventIn->GetType() == ShivaGUI::InternalEvent::POSITIONAL_DESELECT )
	{
		if( m_mousing )
		{
			m_mousing = false;
			m_rotationX += m_mouseLastX - m_mouseRefX;
			m_rotationZ += m_mouseLastY - m_mouseRefY;
			int posX,posY;
			_eventIn->GetPosition( posX, posY );
			m_mouseLastX = m_mouseRefX = posX;
			m_mouseLastY = m_mouseRefY = posY;

			m_renderer->SetWorldAngularVelocityDegs( 0.0f, 0.0f, 0.0f );
			//renderer->SetStepsize(stationaryStepsize);

			if( m_mousingTimer < m_clickSelectTimeThreshold )
			{
				// Unproject and then pass the mouse coords to the _totemController to see if it can use them as a selection
				float xPos = ( float )m_mouseRefX;
				float yPos = ( float )m_mouseRefY;

				#ifdef _DEBUG
					std::cout << "INFO: Attempting selection, mouse coords: " << xPos << " " << yPos << std::endl;
				#endif

				m_crosshairX = ( xPos - ( float )m_boundsLeft ) / ( ( float )m_boundsRight - m_boundsLeft );
				m_crosshairY = ( ( float )m_windowHeight - yPos - ( ( float )m_windowHeight - m_boundsBottom ) ) / ( ( float ) m_boundsBottom - m_boundsTop );
#ifdef _DEBUG
				std::cout << "INFO: crosshairY = " << m_crosshairY << std::endl;
#endif
				m_renderer->Unproject( xPos,
									  ( float ) m_windowHeight - yPos, 
									  m_boundsLeft, 
									  m_windowHeight - m_boundsBottom, 
									  m_boundsRight - m_boundsLeft, 
									  m_boundsBottom - m_boundsTop, 
									  m_selectVecOrigX,
									  m_selectVecOrigY,
									  m_selectVecOrigZ, 
									  m_selectVecDirX,
									  m_selectVecDirY,
									  m_selectVecDirZ );

				#ifdef _DEBUG
					std::cout << "INFO: Attempting selection, unproject origin: " << m_selectVecOrigX << " " << m_selectVecOrigY << " " << m_selectVecOrigZ << " dir: " << m_selectVecDirX << " " << m_selectVecDirY << " " << m_selectVecDirZ << std::endl;
				#endif

				/*
				Totem::Operations::Drill *drill = new Totem::Operations::Drill();
				drill->SetDrill(_selectVecOrigX,_selectVecOrigY,_selectVecOrigZ, _selectVecDirX,_selectVecDirY,_selectVecDirZ, 100.0f);
				_totemController->AddOperation(drill);
				*/
				if( m_allowClickSelect )
				{
					SelectMouseCommand* selectMouseCmd = new SelectMouseCommand();
					selectMouseCmd->SetSelectionVec( m_selectVecOrigX, m_selectVecOrigY, m_selectVecOrigZ, m_selectVecDirX, m_selectVecDirY, m_selectVecDirZ );
					
					m_commandManager->Execute( selectMouseCmd );
					
					//m_totemController->SelectIntersectingObject( m_selectVecOrigX, m_selectVecOrigY, m_selectVecOrigZ, m_selectVecDirX, m_selectVecDirY, m_selectVecDirZ );
				}
			}
		}
	}
	else if( _eventIn->GetType() == ShivaGUI::InternalEvent::POSITIONAL_DRAG )
	{
		if( m_mousing )
		{
			int posX, posY;
			_eventIn->GetPosition( posX, posY );
			int diffX = posX - m_mouseRefX;
			int diffY = posY - m_mouseRefY;

			m_renderer->SetWorldAngularVelocityDegs( 0.5f * ( ( float )diffY ), 0.0f, 0.5f * ( ( float )diffX ) );
			//renderer->AddWorldRotationOffsetDegs((float)diffY,0.0f,(float)diffX);

			m_mouseLastX = posX;
			m_mouseLastY = posY;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------

void VolView::SetCameraAngle( float _angleDeg )
{
	m_renderer->SetCameraAngle( _angleDeg );
}

//----------------------------------------------------------------------------------

void VolView::GetSelectionVector( float &_originX, float &_originY, float &_originZ, float &_dirX, float &_dirY, float &_dirZ )
{
	float xPos = ( m_crosshairX * ( ( float )m_boundsRight - m_boundsLeft ) ) + ( float )m_boundsLeft;
	float yPos = ( ( float )m_windowHeight ) - ( ( m_crosshairY * ( ( float )m_boundsBottom - m_boundsTop ) ) + ( ( float )m_windowHeight - m_boundsBottom ) );

	m_renderer->Unproject( xPos, 
						  ( float ) m_windowHeight - yPos, 
						  m_boundsLeft, 
						  m_windowHeight - m_boundsBottom, 
						  m_boundsRight - m_boundsLeft, 
						  m_boundsBottom - m_boundsTop, 
						  m_selectVecOrigX, 
						  m_selectVecOrigY, 
						  m_selectVecOrigZ, 
						  m_selectVecDirX, 
						  m_selectVecDirY, 
						  m_selectVecDirZ );

	_originX = m_selectVecOrigX;
	_originY = m_selectVecOrigY;
	_originZ = m_selectVecOrigZ;

	_dirX = m_selectVecDirX;
	_dirY = m_selectVecDirY;
	_dirZ = m_selectVecDirZ;
}

//----------------------------------------------------------------------------------

void VolView::ResetWorldRotation()
{
	m_renderer->ResetWorldRotation();
}

//----------------------------------------------------------------------------------

void VolView::AddWorldRotationOffsetDegs( float _rotX, float _rotY, float _rotZ )
{
	m_renderer->AddWorldRotationOffsetDegs( _rotX, _rotY, _rotZ );
}

//----------------------------------------------------------------------------------

void VolView::AddWorldRotationOffsetRads( float _rotX, float _rotY, float _rotZ )
{
	m_renderer->AddWorldRotationOffsetRads( _rotX, _rotY, _rotZ );
}

//----------------------------------------------------------------------------------

void VolView::RefreshTree()
{
	m_mainTree->SetRoot( m_totemController->GetNodeTree() );
	m_mainTree->BuildCaches( m_cachePolicy, m_renderer );
	m_renderer->RebuildTree();
	
	CalcStepsize();
}

//----------------------------------------------------------------------------------

void VolView::RefreshTreeParams()
{
	if( m_renderer->ParametersStillAvailable() )
	{
		m_mainTree->UpdateParameters( m_renderer );
		// This is the best way to see if it failed
		if( m_renderer->ParametersStillAvailable() )
		{
			m_renderer->BuildParameterUniformLists();
			CalcStepsize();
		}
		else
		{
			RefreshTree();
		}
	}
	else
	{
		RefreshTree();
	}
}

//----------------------------------------------------------------------------------

void VolView::SetObjectColour( float _r, float _g, float _b )
{
	m_renderer->SetObjectColour( _r, _g, _b );
}

//----------------------------------------------------------------------------------

void VolView::NudgeCrosshairs( float _x, float _y )
{
	m_crosshairX += _x;
	if( m_crosshairX < 0.0f )
		m_crosshairX = 0.0f;
	if( m_crosshairX > 1.0f )
		m_crosshairX = 1.0f;
	m_crosshairY += _y;
	if( m_crosshairY < 0.0f )
		m_crosshairY = 0.0f;
	if( m_crosshairY > 1.0f )
		m_crosshairY = 1.0f;
}

//----------------------------------------------------------------------------------

void VolView::CalcStepsize()
{
	float bboxMaxDim = m_mainTree->GetBoundingBoxMaxDim();

	float stepsize = bboxMaxDim / 170.0f;
#ifdef _DEBUG
	std::cout << "INFO: new stepsize: " << stepsize << std::endl;
#endif
	m_renderer->SetStepsize( stepsize );
}

//----------------------------------------------------------------------------------