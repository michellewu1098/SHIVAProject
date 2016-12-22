#include "VolView.h"

#ifndef PI
#define PI 3.141592653589793238462643383279502884
#endif

//----------------------------------------------------------------------------------

VolView::VolView()
{
	// For mouse rotating:
	_mousing = false;
	_mouseLastX = _mouseLastY = 0;
	_rotationX =_rotationZ = 0.0f;
	_mouseRefX = _mouseRefY = 0;

	_mousingTimer = 0.0f;
	_clickSelectTimeThreshold = 0.5f;
	_allowClickSelect = false;
	_selectVecOrigX = _selectVecOrigY = _selectVecOrigZ = 0.0f;
	_selectVecDirX = 0.0f;_selectVecDirY = 0.0f;_selectVecDirZ = -1.0f;

	_showCrosshairs = false;
	_crosshairX = _crosshairY = 0.5f;
	_targetSize = 0.1f;

	_totemController = Totem::Controller::GetInstance();

	_mainTree = new VolumeTree::Tree();

	_renderer = new GLSLRenderer( 640, 480 );
	_renderer->ReserveCaches( 10 );
	_renderer->SetTree( _mainTree );
	
	_cachePolicy = new VolumeTree::CachingPolicy();
	
	std::cout << "INFO: Building caches, please wait..." << std::endl;
	_mainTree->BuildCaches( _cachePolicy, _renderer );
	
	std::cout << "INFO: Test tree cached GLSL output: " << _mainTree->GetCachedFunctionGLSLString() << std::endl;

	if( !_renderer->Initialise( "Resources/Shaders/VolRenderer.vert", "Resources/Shaders/VolRendererPart.frag" ) )
	{
		std::cerr << "ERROR: Fatal Vol Renderer Initialisation Failure" << std::endl;
	}

	RefreshTree();
}

//----------------------------------------------------------------------------------

VolView::~VolView()
{
	delete _mainTree;
	delete _renderer;
	delete _cachePolicy;
}

//----------------------------------------------------------------------------------

void VolView::Layout( int left, int top, int right, int bottom, int windowWidth, int windowHeight )
{
	View::Layout( left, top, right, bottom, windowWidth, windowHeight );

	_boundsLeft = left;
	_boundsRight = right;
	_boundsTop = top;
	_boundsBottom = bottom;
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;

	_renderer->SetWindowDimensions( ( float )( right - left ), ( float )( bottom - top ) );

//	float aspectRatio = ((float)right-left) / ((float)bottom-top);
//	cml::matrix_perspective_xfov_RH(_projectionMatrix, _cameraAngle, aspectRatio, _cameraNearPlane, _cameraFarPlane, cml::z_clip_neg_one);

}

//----------------------------------------------------------------------------------

void VolView::Inflate( TiXmlElement *xmlElement, ShivaGUI::ResourceManager *resources, std::string themePrefix, bool rootNode )
{
	if( themePrefix.empty() )
		themePrefix = "VolView_";
	View::Inflate( xmlElement, resources, themePrefix, rootNode );

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
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
	}
}

//----------------------------------------------------------------------------------

TiXmlElement* VolView::Deflate( ShivaGUI::ResourceManager *resources )
{
	TiXmlElement *xmlNode = View::Deflate( resources );
	xmlNode->SetValue( "VolView" );

	return xmlNode;
}

//----------------------------------------------------------------------------------

void VolView::Update( float deltaTs, ShivaGUI::GUIController *guiController )
{
	_renderer->Update( deltaTs );
	if( _mousing )
		_mousingTimer += deltaTs;
}

//----------------------------------------------------------------------------------

void VolView::Draw()
{
	View::Draw();
	if( !_visible )
		return;

	// Set glViewport to extent of View
	glPushAttrib( GL_VIEWPORT_BIT );
	glPushMatrix();
	glViewport( _boundsLeft, _windowHeight - _boundsBottom, _boundsRight - _boundsLeft, _boundsBottom - _boundsTop );

	_renderer->Draw();

	if( _showCrosshairs )
	{
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();
		glOrtho( 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 10.0f );

		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		glLoadIdentity();

		glLineWidth( 5.0f );
		glColor3f( 1.0f, 0.0f, 0.0f );
		glBegin( GL_LINES );
			glVertex3f( 0.0f, _crosshairY, -2.0f );
			glVertex3f( 1.0f, _crosshairY, -2.0f );

			glVertex3f( _crosshairX, 0.0f, -2.0f );
			glVertex3f( _crosshairX, 1.0f, -2.0f );
		glEnd();

		float camDist = _renderer->GetCameraDistance();
		float scaleFactor = _targetSize * 57.0f * ( 1.0f / camDist );
		glBegin( GL_LINE_LOOP );
			for( float i = 0.0f; i < PI * 2.0f; i += 0.2f )
			{
				glVertex3f( _crosshairX + ( scaleFactor * cos( i ) ), _crosshairY + ( scaleFactor * sin( i ) ), -2.0f );
			}
		glEnd();


		// Restore GL matrixes for GUI system
		glMatrixMode( GL_PROJECTION );
		glPopMatrix();
		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();
	}

	// Restore GL Viewport
	glPopAttrib();
}

//----------------------------------------------------------------------------------

bool VolView::HandleEvent( ShivaGUI::InternalEvent *eventIn )
{
	if( eventIn->GetType() == ShivaGUI::InternalEvent::POSITIONAL_SELECT )
	{
		if( EventHit( eventIn ) )
		{
			_mousing = true;
			int posX, posY;
			eventIn->GetPosition( posX, posY );
			_mouseLastX = _mouseRefX = posX;
			_mouseLastY = _mouseRefY = posY;
			_mousingTimer = 0.0f;
		}
		else
		{
			_mousing = false;
		}
	}
	else if( eventIn->GetType() == ShivaGUI::InternalEvent::POSITIONAL_DESELECT )
	{
		if( _mousing )
		{
			_mousing = false;
			_rotationX += _mouseLastX - _mouseRefX;
			_rotationZ += _mouseLastY - _mouseRefY;
			int posX,posY;
			eventIn->GetPosition( posX, posY );
			_mouseLastX = _mouseRefX = posX;
			_mouseLastY = _mouseRefY = posY;

			_renderer->SetWorldAngularVelocityDegs( 0.0f, 0.0f, 0.0f );
			//renderer->SetStepsize(stationaryStepsize);

			if( _mousingTimer < _clickSelectTimeThreshold )
			{
				// Unproject and then pass the mouse coords to the _totemController to see if it can use them as a selection
				float xPos = ( float ) _mouseRefX;
				float yPos = ( float ) _mouseRefY;

				#ifdef _DEBUG
					std::cout<<"INFO: Attempting selection, mouse coords: "<<xPos<<" "<<yPos<<std::endl;
				#endif

				_crosshairX = ( xPos - ( float ) _boundsLeft ) / ( ( float ) _boundsRight - _boundsLeft );
				_crosshairY = ( ( float ) _windowHeight - yPos - ( ( float ) _windowHeight - _boundsBottom ) ) / ( ( float ) _boundsBottom - _boundsTop );
				std::cout << "INFO: crosshairY = " << _crosshairY << std::endl;
				_renderer->Unproject( xPos,
									  ( float ) _windowHeight - yPos, 
									  _boundsLeft, 
									  _windowHeight - _boundsBottom, 
									  _boundsRight - _boundsLeft, 
									  _boundsBottom - _boundsTop, 
									  _selectVecOrigX,
									  _selectVecOrigY,
									  _selectVecOrigZ, 
									  _selectVecDirX,
									  _selectVecDirY,
									  _selectVecDirZ );

				#ifdef _DEBUG
					std::cout<<"INFO: Attempting selection, unproject origin: "<<_selectVecOrigX<<" "<<_selectVecOrigY<<" "<<_selectVecOrigZ<<" dir: "<<_selectVecDirX<<" "<<_selectVecDirY<<" "<<_selectVecDirZ<<std::endl;
				#endif

				/*
				Totem::Operations::Drill *drill = new Totem::Operations::Drill();
				drill->SetDrill(_selectVecOrigX,_selectVecOrigY,_selectVecOrigZ, _selectVecDirX,_selectVecDirY,_selectVecDirZ, 100.0f);
				_totemController->AddOperation(drill);
				*/
				if( _allowClickSelect )
				{
					_totemController->SelectIntersectingObject( _selectVecOrigX, _selectVecOrigY, _selectVecOrigZ, _selectVecDirX, _selectVecDirY, _selectVecDirZ );
				}
			}
		}
	}
	else if( eventIn->GetType() == ShivaGUI::InternalEvent::POSITIONAL_DRAG )
	{
		if( _mousing )
		{
			int posX, posY;
			eventIn->GetPosition( posX, posY );
			int diffX = posX - _mouseRefX;
			int diffY = posY - _mouseRefY;

			_renderer->SetWorldAngularVelocityDegs( 0.5f * ( ( float ) diffY ), 0.0f, 0.5f * ( ( float ) diffX ) );
			//renderer->AddWorldRotationOffsetDegs((float)diffY,0.0f,(float)diffX);

			_mouseLastX = posX;
			_mouseLastY = posY;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------

void VolView::SetCameraAngle( float angleDeg )
{
	_renderer->SetCameraAngle( angleDeg );
}

//----------------------------------------------------------------------------------

void VolView::GetSelectionVector( float &originX, float &originY, float &originZ, float &dirX, float &dirY, float &dirZ )
{
	float xPos = ( _crosshairX * ( ( float ) _boundsRight - _boundsLeft ) ) + ( float ) _boundsLeft;
	float yPos = ( ( float ) _windowHeight ) - ( ( _crosshairY * ( ( float ) _boundsBottom - _boundsTop ) ) + ( ( float ) _windowHeight - _boundsBottom ) );

	_renderer->Unproject( xPos, 
						  ( float ) _windowHeight - yPos, 
						  _boundsLeft, 
						  _windowHeight - _boundsBottom, 
						  _boundsRight - _boundsLeft, 
						  _boundsBottom - _boundsTop, 
						  _selectVecOrigX, 
						  _selectVecOrigY, 
						  _selectVecOrigZ, 
						  _selectVecDirX, 
						  _selectVecDirY, 
						  _selectVecDirZ );

	originX = _selectVecOrigX;
	originY = _selectVecOrigY;
	originZ = _selectVecOrigZ;

	dirX = _selectVecDirX;
	dirY = _selectVecDirY;
	dirZ = _selectVecDirZ;
}

//----------------------------------------------------------------------------------

void VolView::ResetWorldRotation()
{
	_renderer->ResetWorldRotation();
}

//----------------------------------------------------------------------------------

void VolView::AddWorldRotationOffsetDegs( float rotX, float rotY, float rotZ )
{
	_renderer->AddWorldRotationOffsetDegs( rotX, rotY, rotZ );
}

//----------------------------------------------------------------------------------

void VolView::AddWorldRotationOffsetRads( float rotX, float rotY, float rotZ )
{
	_renderer->AddWorldRotationOffsetRads( rotX, rotY, rotZ );
}

//----------------------------------------------------------------------------------

void VolView::RefreshTree()
{
	_mainTree->SetRoot( _totemController->GetNodeTree() );
	_mainTree->BuildCaches( _cachePolicy, _renderer );
	_renderer->RebuildTree();
	
	CalcStepsize();
}

//----------------------------------------------------------------------------------

void VolView::RefreshTreeParams()
{
	if( _renderer->ParametersStillAvailable() )
	{
		_mainTree->UpdateParameters( _renderer );
		// This is the best way to see if it failed
		if( _renderer->ParametersStillAvailable() )
		{
			_renderer->BuildParameterUniformLists();
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

void VolView::SetObjectColour( float R, float G, float B )
{
	_renderer->SetObjectColour( R, G, B );
}

//----------------------------------------------------------------------------------

void VolView::NudgeCrosshairs( float x, float y )
{
	_crosshairX += x;
	if( _crosshairX < 0.0f )
		_crosshairX = 0.0f;
	if( _crosshairX > 1.0f )
		_crosshairX = 1.0f;
	_crosshairY += y;
	if( _crosshairY < 0.0f )
		_crosshairY = 0.0f;
	if( _crosshairY > 1.0f )
		_crosshairY = 1.0f;
}

//----------------------------------------------------------------------------------

void VolView::CalcStepsize()
{
	float bboxMaxDim = _mainTree->GetBoundingBoxMaxDim();

	float stepsize = bboxMaxDim / 170.0f;
	std::cout << "INFO: new stepsize: " << stepsize << std::endl;
	_renderer->SetStepsize( stepsize );
}

//----------------------------------------------------------------------------------