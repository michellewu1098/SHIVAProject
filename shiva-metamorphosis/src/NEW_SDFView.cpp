#include "SDFView.h"

//////////////////////////////////////////////////////////////////////////
#include <GL/GLee.h>
#include <GL/glu.h>
#include <cmath>
#include <iostream>
#include <sdf/discretization/discretized_field.hpp>

#include "vol_metamorph.h"

#ifndef PI
#define PI 3.141592653589793238462643383279502884
#endif

SDFView::SDFView()
{
	GLenum err = glGetError();
	if( err != GL_NO_ERROR)
	{
		std::cerr<<" SDFView::Ctor, OpenGL Error at start: "<< gluErrorString(err)<<std::endl;
	}

	_boundsLeft = _boundsTop = _boundsRight = _boundsBottom = _windowWidth = _windowHeight = 0;

	_rotationSpeed = 1.0f;

	_pauseRotation = false;

	// TODO: hopefully a lot of these viewing-related things can be given as parameters with the XML stuff

	_axisX.set(1,0,0,0);
	_axisY.set(0,1,0,0);
	_axisZ.set(0,0,1,0);

	// TODO: fix all camera stuff...
	_cameraPosition.set(0,2,3);
	_targetPosition.set(0,0,0);
	cml::vector3f up;
	up.cardinal(1);
	matrix_look_at_RH(_modelviewMatrix, _cameraPosition, _targetPosition, up);
	_invModelview = _modelviewMatrix;
	cml::inverse(_invModelview);

	_worldMatrix.identity();

	cml::matrix_perspective_RH(_projectionMatrix,-1.0f,1.0f,-1.0f,1.0f,-1.0f,100.0f, cml::z_clip_neg_one);
	_rotationX = -PI/2.0f;
	_rotationY = 0.0f;
	_rotationZ = 0.0f;
	_rotationWorldX = _rotationWorldY = _rotationWorldZ = 0.0f;
	_cameraAngle = 35.0f;
	_cameraNearPlane = 0.1f;
	_cameraFarPlane = 20.0f;

	// Cube data
	float cubeVertices[8*3] = { -1.0f, 1.0f,-1.0f,
								-1.0f, 1.0f, 1.0f,
								 1.0f, 1.0f, 1.0f,
								 1.0f, 1.0f,-1.0f,
								-1.0f,-1.0f,-1.0f,
								-1.0f,-1.0f, 1.0f,
								 1.0f,-1.0f, 1.0f,
								 1.0f,-1.0f,-1.0f };
	int cubeIndices[24] = {   0,1,2,3,
							  0,3,7,4,
							  0,4,5,1,
							  1,5,6,2,
							  3,2,6,7,
							  7,6,5,4};

	// Create vertex buffer object
	glGenBuffers(1,&_cubeVertexVBO);
	glGenBuffers(1,&_cubeIndexVBO);
	// Bind the buffer, ready for transfer
	glBindBuffer(GL_ARRAY_BUFFER,_cubeVertexVBO);
	// Transfer data
	glBufferData(GL_ARRAY_BUFFER, 8*3*sizeof(float), cubeVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_cubeIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24*sizeof(int), cubeIndices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);




	_sdfProgram = new Utility::GPUProgram();
	// TODO: pass in program name from xml?
	_sdfProgram->Create("Resources/Shaders/morph",Utility::GPUProgram::VERTEX_AND_FRAGMENT);
	//_sdfProgram->Create("Resources/Shaders/sdfViewShader",Utility::GPUProgram::VERTEX_AND_FRAGMENT);

	_projMatrixUniform =    new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetVertexShader(),"projectionMatrix");
	_mvMatrixUniform =      new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetVertexShader(),"ViewMatrix");
	_invViewMatrixUniform = new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetVertexShader(),"invView");
	_vertSize =             new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetVertexShader(),"vertsize");
	_vertPos =              new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetVertexShader(),"vertposition");

	_fragboundmin = new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetFragmentShader(),"fragboundmin");
	_fragboundmax = new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetFragmentShader(),"fragboundmax");
	//_parameters =   new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetFragmentShader(),"parameters");
	//_colour =       new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetFragmentShader(),"colour");
	_texture0 =     new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetFragmentShader(),"volume1");
	_texture1 =     new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetFragmentShader(),"volume2");
	_blendUniform = new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetFragmentShader(),"time");

	
	_a0Uniform = new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetFragmentShader(),"a0");
	_a1Uniform = new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetFragmentShader(),"a1");
	_a2Uniform = new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetFragmentShader(),"a2");
	_a3Uniform = new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetFragmentShader(),"a3");
	_clipdistUniform = new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetFragmentShader(),"clipdist");
	_timegapUniform = new Utility::GPUVariable(_sdfProgram,_sdfProgram->GetFragmentShader(),"timegap");



	_sdfProgram->Off();

	_parameterValues[0] = 0.0f;
	_parameterValues[1] = 32.0f; // TODO: put this in xml and allow to be tweaked - was at 32
	_parameterValues[2] = 0.01f;
	_parameterValues[3] = 1.0f;

	_blendValue = 0.0f;

	_colourR = _colourG = _colourB = 1.0f;

	err = glGetError();
	if( err != GL_NO_ERROR)
	{
		std::cerr<<" SDFView::Ctor, OpenGL Error at end before load volumes: "<< gluErrorString(err)<<std::endl;
	}

//	_load_volume_from_file("Resources/Models/buddha.bdf",0);
//	_load_volume_from_file("Resources/Models/dragon.bdf",1);
	//_load_volume_from_file("Resources/signbreakerMVC.bdf",0);
	//_load_volume_from_file("Resources/signbreakerSOFTMESH.bdf",1);
}

SDFView::~SDFView()
{
	delete _sdfProgram;
	delete _projMatrixUniform;
	delete _mvMatrixUniform;
	delete _invViewMatrixUniform;
	delete _vertSize;
	delete _vertPos;
	delete _fragboundmin;
	delete _fragboundmax;
	//delete _parameters;
	//delete _colour;
	delete _texture0;
	delete _texture1;
	delete _blendUniform;
	delete _a0Uniform;
	delete _a1Uniform;
	delete _a2Uniform;
	delete _a3Uniform;
	delete _clipdistUniform;
	delete _timegapUniform;

	glDeleteBuffers(1,&_cubeVertexVBO);
	glDeleteBuffers(1,&_cubeIndexVBO);
}

void SDFView::Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight)
{
	_boundsLeft = left;
	_boundsRight = right;
	_boundsTop = top;
	_boundsBottom = bottom;
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;

	float aspectRatio = ((float)right-left) / ((float)bottom-top);
	cml::matrix_perspective_xfov_RH(_projectionMatrix, _cameraAngle, aspectRatio, _cameraNearPlane, _cameraFarPlane, cml::z_clip_neg_one);

}

void SDFView::Inflate(TiXmlElement *xmlElement, ShivaGUI::ResourceManager *resources, std::string themePrefix, bool rootNode )
{
	if( themePrefix.empty() )
		themePrefix = "SDFView_";
	View::Inflate(xmlElement,resources,themePrefix,rootNode);

	for( TiXmlAttribute *currentAttribute = xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
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
	}
}



void SDFView::AddWorldRotationOffsetDegs(float rotX, float rotY, float rotZ)
{
	AddWorldRotationOffsetRads((2.0f*PI)*(rotX/360.0f), (2.0f*PI)*(rotY/360.0f), (2.0f*PI)*(rotZ/360.0f));
}

void SDFView::AddWorldRotationOffsetRads(float rotX, float rotY, float rotZ)
{

	_rotationWorldX = std::fmod(_rotationWorldX + rotX,(float)(2.0f*PI));
	_rotationWorldY = std::fmod(_rotationWorldY + rotY,(float)(2.0f*PI));
	_rotationWorldZ = std::fmod(_rotationWorldZ + rotZ,(float)(2.0f*PI));


	cml::matrix44f_c temp;
	cml::matrix_rotation_axis_angle(temp, cml::vector3f(_axisX.data()), rotX);
//	cml::matrix_rotate_about_local_x(temp,rotX);
	_axisX = _axisX * temp;
	_axisY = _axisY * temp;
	_axisZ = _axisZ * temp;

	temp.identity();
	cml::matrix_rotation_axis_angle(temp, cml::vector3f(_axisY.data()), rotY);
	//cml::matrix_rotate_about_local_y(temp,rotY);
	_axisX = _axisX * temp;
	_axisY = _axisY * temp;
	_axisZ = _axisZ * temp;

	temp.identity();
	cml::matrix_rotation_axis_angle(temp, cml::vector3f(_axisZ.data()), rotZ);
	//cml::matrix_rotate_about_local_z(temp,rotZ);
	_axisX = _axisX * temp;
	_axisY = _axisY * temp;
	_axisZ = _axisZ * temp;


/*
	cml::matrix44f_c temp;
	temp.identity();
	cml::matrix_rotation_axis_angle(temp, cml::vector3f(1,0,0), rotX);
	_worldMatrix = _worldMatrix * temp;


	temp.identity();
	cml::matrix_rotation_axis_angle(temp, cml::vector3f(0,1,0), rotY);
	_worldMatrix = _worldMatrix * temp;

	temp.identity();
	cml::matrix_rotation_axis_angle(temp, cml::vector3f(0,0,1), rotZ);
	_worldMatrix = _worldMatrix * temp;
*/


	/*
	cml::matrix_rotate_about_local_x(_worldMatrix,rotX);
	cml::matrix_rotate_about_local_y(_worldMatrix,rotY);
	cml::matrix_rotate_about_local_z(_worldMatrix,rotZ);
	*/
}

void SDFView::AddRotationOffsetDegs(float rotX, float rotY, float rotZ)
{
	AddRotationOffsetRads((2.0f*PI)*(rotX/360.0f), (2.0f*PI)*(rotY/360.0f), (2.0f*PI)*(rotZ/360.0f));
}

void SDFView::AddRotationOffsetRads(float rotX, float rotY, float rotZ)
{
	_rotationX = std::fmod(_rotationX + rotX,(float)(2.0f*PI));
	_rotationY = std::fmod(_rotationY + rotY,(float)(2.0f*PI));
	_rotationZ = std::fmod(_rotationZ + rotZ,(float)(2.0f*PI));
}

void SDFView::SetQuality( float value )
{
	_parameterValues[1] = value;
	if( _parameterValues[1] < 1.0f )
		_parameterValues[1] = 1.0f;
}

void SDFView::Update(float deltaTs, ShivaGUI::GUIController *guiController)
{
	if( !_pauseRotation )
	{
		_rotationZ -= deltaTs * _rotationSpeed;
		if( _rotationZ > (2.0f*PI) )
		{
			double temp;
			_rotationZ = modf(_rotationZ / (2.0f*PI),&temp);
		}
	}

	//cml::vector4f up4(0,1,0,0);
	cml::vector3f up;
	up.cardinal(1);
	/*
	 * For rotating the camera after the look_at:
	cml::matrix44f_c temp;
	temp.identity();

	cml::matrix_rotate_about_world_x(temp,_rotationWorldX);
	cml::matrix_rotate_about_world_y(temp,_rotationWorldY);
	cml::matrix_rotate_about_world_z(temp,_rotationWorldZ);
	*/

/*
	cml::matrix44f_c temp;
	temp.identity();
	cml::matrix_rotate_about_local_x(temp,_rotationWorldX);
	cml::matrix_rotate_about_local_y(temp,_rotationWorldY);
	cml::matrix_rotate_about_local_z(temp,_rotationWorldZ);

	cml::vector4f adjCamPos( _cameraPosition[0],_cameraPosition[1],_cameraPosition[2],0 );
	adjCamPos = temp * adjCamPos;
	//cml::vector3f adjCamPos3(adjCamPos.data() );
	//cml::vector3f up( temp * up4 );
	cml::vector3f adjCamPos3(adjCamPos.data());
*/
/*
	cml::vector4f adjCamPos( _cameraPosition[0],_cameraPosition[1],_cameraPosition[2],0 );
	adjCamPos = _worldMatrix * adjCamPos;
	cml::vector3f adjCamPos3(adjCamPos.data());

	cml::vector4f adjUp(0,1,0,0);
	adjUp = _worldMatrix * adjUp;
	up.set(adjUp[0],adjUp[1],adjUp[2]);


	cml::matrix_look_at_RH(_modelviewMatrix, adjCamPos3, _targetPosition, up);
*/
	cml::matrix_look_at_RH(_modelviewMatrix, _cameraPosition, _targetPosition, up);

	cml::matrix_rotate_about_local_x(_modelviewMatrix,_rotationX);
	cml::matrix_rotate_about_local_y(_modelviewMatrix,_rotationY);
	cml::matrix_rotate_about_local_z(_modelviewMatrix,_rotationZ);


	cml::matrix44f_c temp;
	temp.identity();
	cml::matrix_rotation_axis_angle(temp, cml::vector3f(_axisX.data()), _rotationWorldX);
	_worldMatrix = _worldMatrix * temp;
	_rotationWorldX = 0.0f;


	temp.identity();
	cml::matrix_rotation_axis_angle(temp, cml::vector3f(_axisY.data()), _rotationWorldY);
	_worldMatrix = _worldMatrix * temp;
	_rotationWorldY = 0.0f;

	temp.identity();
	cml::matrix_rotation_axis_angle(temp, cml::vector3f(_axisZ.data()), _rotationWorldZ);
	_worldMatrix = _worldMatrix * temp;
	_rotationWorldZ = 0.0f;


	_modelviewMatrix = _modelviewMatrix * _worldMatrix;






	//cml::matrix_rotation_align(_modelviewMatrix,adjCamPos);
/*
	cml::matrix_rotate_about_local_x(_modelviewMatrix,_rotationX);
	cml::matrix_rotate_about_local_y(_modelviewMatrix,_rotationY);
	cml::matrix_rotate_about_local_z(_modelviewMatrix,_rotationZ);
	*/
/*
	cml::matrix44f_c temp;
	temp.identity();
	cml::matrix_rotate_about_local_x(temp,_rotationWorldX);

	cml::matrix44f_c temp2;
	temp2.identity();
	cml::matrix_rotate_about_local_z(temp2,_rotationWorldZ);

	_modelviewMatrix = _modelviewMatrix * temp * temp2;

*/


//	cml::matrix_rotation_euler(temp,_rotationWorldX,_rotationWorldY,_rotationWorldZ,cml::euler_order_zyx);

/*
	cml::matrix44f_c temp;
	//cml::vector3f worldAxis = (_cameraPosition-_targetPosition).normalize();
	//cml::vector3f camX(0,2,3);
	//cml::vector3f worldAxis = (camX-_targetPosition).normalize();
	cml::vector4f camX(1,0,0,0);
	//camX = _modelviewMatrix * camX;
	cml::vector3f camX3(camX.data());
	cml::matrix_rotation_axis_angle(temp, camX3 ,_rotationWorldX);

	//try a matrix that gets rotated / etc at every request??

	cml::matrix44f_c temp2;
	cml::vector4f camZ(0,0,1,0);
	camZ = cml::inverse(temp) * camZ;
	cml::vector3f camZ3(camZ.data());
	cml::matrix_rotation_axis_angle(temp2, camZ3 ,_rotationWorldZ);
*/

/*
 * This seems to control the camera:
	cml::matrix44f_c temp;

	cml::vector4f axisX4(1,0,0,0);
	axisX4 = cml::inverse(_modelviewMatrix) * axisX4;
	cml::vector3f axisX3(axisX4.data());
	cml::matrix_rotation_axis_angle(temp, axisX3 ,_rotationWorldX);
	_worldMatrix = temp * _worldMatrix;

	cml::vector4f axisZ4(0,0,1,0);
	axisZ4 = cml::inverse(_modelviewMatrix) * axisZ4;
	cml::vector3f axisZ3(axisZ4.data());
	cml::matrix_rotation_axis_angle(temp, axisZ3 ,_rotationWorldZ);
	_worldMatrix = temp * _worldMatrix;
*/
/*
	cml::matrix44f_c temp;

	cml::vector4f axisX4(1,0,0,0);
	axisX4 = cml::inverse(_worldMatrix) * axisX4;
	cml::vector3f axisX3(axisX4.data());
	cml::matrix_rotation_axis_angle(temp, axisX3 ,_rotationWorldX);
	_worldMatrix = _worldMatrix * temp;

	cml::vector4f axisZ4(0,0,1,0);
	axisZ4 = cml::inverse(_worldMatrix) * axisZ4;
	cml::vector3f axisZ3(axisZ4.data());
	cml::matrix_rotation_axis_angle(temp, axisZ3 ,_rotationWorldZ);
	_worldMatrix = _worldMatrix * temp;
*/
	/*
	cml::vector3f xvect = (_cameraPosition-_targetPosition).normalize();
	float a = xvect[1];
	xvect[1] = xvect[2];
	xvect[2] = a;
	cml::matrix44f_c temp2;
	cml::matrix_rotation_axis_angle(temp2, xvect ,_rotationWorldY);
	*/
	//_modelviewMatrix = _worldMatrix * _modelviewMatrix;// * temp * temp2;
/*
	cml::matrix_rotate_about_world_x(_modelviewMatrix,_rotationWorldX);
	cml::matrix_rotate_about_world_y(_modelviewMatrix,_rotationWorldY);
	cml::matrix_rotate_about_world_z(_modelviewMatrix,_rotationWorldZ);
*/
	_invModelview = _modelviewMatrix;
	_invModelview=cml::inverse(_invModelview);
}

void SDFView::Draw()
{
	View::Draw();
	if( !_visible )
		return;

	// Set glViewport to extent of View
	glPushAttrib(GL_VIEWPORT_BIT);
	glPushMatrix();
	glViewport( _boundsLeft, _windowHeight - _boundsBottom, _boundsRight-_boundsLeft, _boundsBottom-_boundsTop );

	// TODO: fix all camera stuff...
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//glOrtho(-2.0f,2.0f,-2.0f,2.0f,1.0f,100.0f);
	//glLoadMatrixf(_projectionMatrix.data());
	float aspectRatio = ((float)_boundsRight-_boundsLeft) / ((float)_boundsBottom-_boundsTop);
	gluPerspective(_cameraAngle, aspectRatio, _cameraNearPlane, _cameraFarPlane);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(_modelviewMatrix.data());

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

		// Bind Shaders
		_sdfProgram->On();
		_projMatrixUniform->UniformMatrix4(_projectionMatrix.data());
		_mvMatrixUniform->UniformMatrix4(_modelviewMatrix.data());
		_invViewMatrixUniform->UniformMatrix4( _invModelview.data() );

		cml::vector3f extent = (_bound_max[0] - _bound_min[0]) *0.5f;
		cml::vector3f centre = (_bound_max[0] + _bound_min[0]) * 0.5f;

		_vertSize->UniformFloat3( extent.data() );
		_vertPos->UniformFloat3( centre.data() );

		_fragboundmin->UniformFloat3( _bound_min[0].data() );
		_fragboundmax->UniformFloat3( _bound_max[0].data() );
		//_parameters->UniformFloat4( _parameterValues.data() );
		//_colour->UniformFloat4( cml::vector4f(_colourR,_colourG,_colourB,1.0f).data() );
		_texture0->UniformInt(0);
		_texture1->UniformInt(1);
		_blendUniform->UniformFloat(_blendValue);
		_a0Uniform->UniformFloat(5.0f);
		_a1Uniform->UniformFloat(5.0f);
		_a2Uniform->UniformFloat(5.0f);
		_a3Uniform->UniformFloat(2.5f);
		_clipdistUniform->UniformFloat(0.15f);
		_timegapUniform ->UniformFloat(0.5f);
		
			// Bind VBOs
			glBindBufferARB(GL_ARRAY_BUFFER, _cubeVertexVBO);
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, _cubeIndexVBO);

				// Activate array
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(3, GL_FLOAT, 0, 0);

				glActiveTexture(GL_TEXTURE0);
				_volume_data[0].bind();
				glActiveTexture(GL_TEXTURE1);
				_volume_data[1].bind();

				//_sdfProgram->
					// Draw cube using index array
					glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, 0);

				//glActiveTexture(GL_TEXTURE1);
				_volume_data[1].unbind();
				glActiveTexture(GL_TEXTURE0);
				_volume_data[0].unbind();

				// Deactivate array
				glDisableClientState(GL_VERTEX_ARRAY);

			// Unbind VBOs
			glBindBufferARB(GL_ARRAY_BUFFER, 0);
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Unbind shaders
		_sdfProgram->Off();

	// Disable depth test
	glDisable(GL_DEPTH_TEST);

	// Temporary code to show coord axes
	//  - need to either disable depth test or make cube draw as GL_LINE_LOOP
/*
	glBegin(GL_LINES);
		glColor3f(  0.3f, 0.0f, 0.0f);
		glVertex3f(-0.1f, 0.0f, 0.0f);
		glVertex3f( 0.3f, 0.0f, 0.0f);

		glColor3f(  0.0f, 0.3f, 0.0f);
		glVertex3f( 0.0f,-0.1f, 0.0f);
		glVertex3f( 0.0f, 0.3f, 0.0f);

		glColor3f(  0.0f, 0.0f, 0.3f);
		glVertex3f( 0.0f, 0.0f,-0.1f);
		glVertex3f( 0.0f, 0.0f, 0.3f);
	glEnd();


	glBegin(GL_LINES);
		glColor3f(  1.0f, 0.0f, 0.0f);
		glVertex3f( 0.0f, 0.0f, 0.0f);
		glVertex3fv( _axisX.data() );

		glColor3f(  0.0f, 1.0f, 0.0f);
		glVertex3f( 0.0f, 0.0f, 0.0f);
		glVertex3fv( _axisY.data() );

		glColor3f(  0.0f, 0.0f, 1.0f);
		glVertex3f( 0.0f, 0.0f, 0.0f);
		glVertex3fv( _axisZ.data() );
	glEnd();
*/
	// Restore GL matrixes for GUI system
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	// Restore GL Viewport
	glPopAttrib();
}

bool SDFView::HandleEvent(ShivaGUI::InternalEvent*)
{
	return false;
}

bool SDFView::_load_volume_from_file(const std::string& i_filename, unsigned int i_volume_id)
{
	assert(i_volume_id<number_of_models);
	assert(!i_filename.empty());

	unsigned int _depth, _width, _height;
	_depth = 128;
	_width = 128;
	_height = 128;

	float* bbox = NULL;
	float* _copy;
	if (!openVol(i_filename.c_str(), _depth, _width, _height, &bbox, &_copy))
	{
		std::cout<<"Failed to load .vol file"<<std::endl;
		return false;
	}

	std::cout<<"Unpacked texture "<<i_filename<<": "<<_width<<"x"<<_height<<"x"<<_depth<<" in bounds (";
	std::cout<<bbox[0]<<","<<bbox[1]<<","<<bbox[2]<<")<("<<bbox[3]<<","<<bbox[4]<<","<<bbox[5]<<")"<<std::endl;

	_volume_data[i_volume_id].update(_width, _height, _depth, _copy);

	_bound_min[i_volume_id][0]=bbox[0];
	_bound_min[i_volume_id][1]=bbox[1];
	_bound_min[i_volume_id][2]=bbox[2];
	_bound_max[i_volume_id][0]=bbox[3];
	_bound_max[i_volume_id][1]=bbox[4];
	_bound_max[i_volume_id][2]=bbox[5];

	_volume_data[i_volume_id].unbind();

	GLenum err = glGetError();
	if( err != GL_NO_ERROR)
	{
		std::cerr<<" SDFView OpenGL Error: "<< gluErrorString(err)<<std::endl;
	}

	std::cout<<"   ...done loading volume"<<std::endl;
	freePt(&bbox);
	freePt(&_copy);
	return true;

/*
	sdf::discretized_field df;
	sdf::binary_file_in filein(i_filename);
	if (filein.is_open())
	{
		std::cout<<"Loading Volume to OpenGL: "<<i_filename<<std::endl;
		df.unserialize(filein);
		filein.close();

		std::cout<<"Unpacked texture "<<i_filename<<": "<<df.grid().width()<<"x"<<df.grid().width()<<"x"<<df.grid().width()<<" in bounds (";
		std::cout<<df.minimum()[0]<<","<<df.minimum()[1]<<","<<df.minimum()[2]<<")<("<<df.maximum()[0]<<","<<df.maximum()[1]<<","<<df.maximum()[2]<<")"<<std::endl;

		_volume_data[i_volume_id].update(df.grid());

		_bound_min[i_volume_id][0]=df.minimum()[0];
		_bound_min[i_volume_id][1]=df.minimum()[1];
		_bound_min[i_volume_id][2]=df.minimum()[2];
		_bound_max[i_volume_id][0]=df.maximum()[0];
		_bound_max[i_volume_id][1]=df.maximum()[1];
		_bound_max[i_volume_id][2]=df.maximum()[2];

		_volume_data[i_volume_id].unbind();

		GLenum err = glGetError();
		if( err != GL_NO_ERROR)
		{
			std::cerr<<" SDFView OpenGL Error: "<< gluErrorString(err)<<std::endl;
		}

		std::cout<<"   ...done loading volume"<<std::endl;
		return true;
	}
	return false;
*/
}
