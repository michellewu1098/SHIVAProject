/*
 * GLSLRenderer.cpp
 *
 *  Created on: Jan 11, 2013
 *      Author: leigh
 */

#include <GL/GLee.h>
#include <GL/glu.h>
#include <iostream>
#include <fstream>
#include <sstream>

#ifndef PI
#define PI 3.141592653589793238462643383279502884197169399375
#endif

#include "VolumeRenderer/GLSLRenderer.h"

GLSLRenderer::GLSLRenderer(unsigned int width, unsigned int height)
{
	_functionTree = NULL;
	_windowWidth = width;
	_windowHeight = height;

	_projMatrixUniform  = _mvMatrixUniform = _invViewMatrixUniform = _vertSize = _vertPos = 0;
	_fragColour = 0;

	_objectColourR = 0.88f;
	_objectColourG = 0.78f;
	_objectColourB = 0.54f;

	_angularVelocityX = _angularVelocityY = _angularVelocityZ = 0.0f;

	_stepsize = 0.1f;

	// Set up viewing transformation (matches the mess that's in SHIVA SDFView)
		_axisX.set(1,0,0,0);
		_axisY.set(0,1,0,0);
		_axisZ.set(0,0,1,0);

		// TODO: fix all camera stuff...
		_cameraPosition.Set(0,2,3);
		_targetPosition.Set(0,0,0);
		cml::vector3f up;
		up.cardinal(1);
		matrix_look_at_RH(_modelviewMatrix, _cameraPosition.GetCurrent(), _targetPosition.GetCurrent(), up);
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

	InitialiseCube();

	_maxCaches = 0;
	_glCacheTexIDs = NULL;
	_fragCache0 = _fragCache1 = _fragCache2 = _fragCache3 = 0;

	_maxParametersRaw = 0;
	int rawLimit = 0;
	// This returns the number of individual floats that can be used
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &rawLimit);
	// Remove a number for those taken up with other things
	// Then divide by 4 because ATI cards treat floats as vec4s
	// This is inaccurate - replacing with a calculation based on the number of currently used things
	int divisor = 1;
	std::string vendorString = (const char*) glGetString(GL_VENDOR);
	if( vendorString.find( "ATI" ) !=  std::string::npos )
	{
		// Turns out ATI cards simply get this value wrong and it needs to be divided by 4
		std::cout<<"INFO: GLSLRenderer detected an ATI-based graphics card"<<std::endl;
		if(  "ATI Mobility Radeon HD 3400 Series" == (const char*) glGetString(GL_RENDERER) )
		{
			// This card is crazy, give is a separate value
			divisor = 16;
			std::cout<<"INFO: GLSLRenderer detected an ATI Mobility Radeon HD 3400 Series card, using param divisor: "<<divisor<<std::endl;
		}
		else
		{
			divisor = 4;
		}
	}
	_maxParametersRaw = (rawLimit/divisor) - 90;//(rawLimit - 16) / (4 * 16);

	std::cout<<"INFO: GLSLRenderer, maximum available tree parameters: "<<_maxParametersRaw<<std::endl;

	_paramIDcount = 0;
	_parameterUniformListLengths = NULL;
	_parameterUniformData = NULL;
}

GLSLRenderer::~GLSLRenderer()
{
	// This function clears the cache list
	ReserveCaches(0);
	delete [] _parameterUniformListLengths;
	delete [] _parameterUniformData;
}


void GLSLRenderer::SetWindowDimensions(float width, float height)
{
	_windowWidth=width;
	_windowHeight=height;

	//std::cout<<"INFO: GLSLRenderer new window dimensions: "<<width<<"x"<<height<<std::endl;
	
	cml::matrix_perspective_RH(_projectionMatrix,width,height,-1.0f,100.0f, cml::z_clip_neg_one);
}


void GLSLRenderer::SetTree(VolumeTree::Tree *tree)
{
	_functionTree = tree;
}

bool GLSLRenderer::Initialise(std::string vertFile, std::string fragFilePart)
{

	// Compile shaders
	if( !InitialiseShaders(vertFile,fragFilePart) )
	{
		// No need for error message here, the function will print out specific problems itself
		return false;
	}

	return true;
}

void GLSLRenderer::Draw()
{
	// TODO: fix all camera stuff...
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//glOrtho(-2.0f,2.0f,-2.0f,2.0f,1.0f,100.0f);
	//glLoadMatrixf(_projectionMatrix.data());
	float aspectRatio = ((float)_windowWidth) / ((float)_windowHeight);
	gluPerspective(_cameraAngle, aspectRatio, _cameraNearPlane, _cameraFarPlane);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(_modelviewMatrix.data());

	// Enable depth test
	glEnable(GL_DEPTH_TEST);


		// Testing code, draws a line along the unprojected selection point
		#ifdef _DEBUG
		glBegin(GL_LINES);
			glColor3f(1.0f,0.0f,0.0f);
			glVertex3f(_temp1X,_temp1Y,_temp1Z);
			glColor3f(0.0f,0.0f,0.0f);
			glVertex3f(_temp2X,_temp2Y,_temp2Z);
		glEnd();
		#endif


		// Bind Shaders
		glUseProgramObjectARB(_GPUProgram);

		glUniformMatrix4fv(_projMatrixUniform, 1, GL_TRUE, _projectionMatrix.data() );
		glUniformMatrix4fv(_mvMatrixUniform, 1, GL_TRUE, _modelviewMatrix.data() );
		glUniformMatrix4fv(_invViewMatrixUniform, 1, GL_TRUE, _invModelview.data() );

		if( _fragColour >= 0 )
		{
			glUniform3f(_fragColour, _objectColourR, _objectColourG, _objectColourB);
		}

		float extents[3] = {0.5f,0.5f,0.5f};
		float centre[3] = {0.0f,0.0f,0.0f};
		float boundsMin[3] = {-0.5f,-0.5f,-0.5f};
		float boundsMax[3] = {0.5f,0.5f,0.5f};
		

		if( _functionTree != NULL )
		{
			_functionTree->GetBoundingBox(boundsMin,boundsMax);
			_functionTree->GetBoundingBoxCentre(centre);
			_functionTree->GetBoundingBoxSize(extents);
			//std::cout<<"bounds min: "<<boundsMin[0]<<" "<<boundsMin[1]<<" "<<boundsMin[2]<<std::endl;
			// For some reason the extents need to be half their values
			for( unsigned int i = 0; i < 3; i++ )
				extents[i] = 0.5f * extents[i];
		}
		

		glUniform3fv(_vertSize, 1, extents);
		glUniform3fv(_vertPos, 1, centre);

		glUniform3fv(_fragboundmin,1,boundsMin);
		glUniform3fv(_fragboundmax,1,boundsMax);
		glUniform1f(_fragStepsize,_stepsize);
		glUniform1f(_fragGradDelta,_gradDelta);

		glUniform1i(_fragCache0, 0);
		glUniform1i(_fragCache1, 1);
		glUniform1i(_fragCache2, 2);
		glUniform1i(_fragCache3, 3);

		BindParametersToGL();

			// Bind VBOs
			glBindBufferARB(GL_ARRAY_BUFFER, _cubeVertexVBO);
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, _cubeIndexVBO);

				// Bind Cache Textures
				for( unsigned int currentCache = 0; currentCache < _maxCaches; ++currentCache )
				{
					if( _glCacheTexIDs[currentCache] >= 0 )
					{
						glActiveTexture(GL_TEXTURE0+currentCache);
						glEnable(GL_TEXTURE_3D);
						glBindTexture(GL_TEXTURE_3D, _glCacheTexIDs[currentCache]);
					}
				}

					// Activate array
					glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(3, GL_FLOAT, 0, 0);

						// Draw cube using index array
						glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, 0);

					// Deactivate array
					glDisableClientState(GL_VERTEX_ARRAY);

				// Unbind Cache Textures
				for( unsigned int currentCache = 0; currentCache < _maxCaches; ++currentCache )
				{
					glActiveTexture(GL_TEXTURE0+currentCache);
					glDisable(GL_TEXTURE_3D);
					glBindTexture(GL_TEXTURE_3D, 0);
				}
				glActiveTexture(GL_TEXTURE0);

			// Unbind VBOs
			glBindBufferARB(GL_ARRAY_BUFFER, 0);
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Unbind Shaders
		glUseProgramObjectARB(0);

	if( _functionTree != NULL )
	{
		_functionTree->DrawBBoxes();
	}

	// Disable depth test
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}

void GLSLRenderer::Update(float deltaTs)
{
	float rotOffsetX = _angularVelocityX * deltaTs;
	float rotOffsetY = _angularVelocityY * deltaTs;
	float rotOffsetZ = _angularVelocityZ * deltaTs;

//	AddWorldRotationOffsetRads(rotOffsetX,rotOffsetY,rotOffsetZ);	
	AddWorldRotationOffsetRads(0.0f,0.0f,rotOffsetZ);
	AddWorldRotationOffsetRads(0.0f,rotOffsetY,0.0f);
	AddWorldRotationOffsetRads(rotOffsetX,0.0f,0.0f);

	_cameraPosition.Update(deltaTs);
	_targetPosition.Update(deltaTs);

	// Again, just copied from SHIVA SDFView
	cml::vector3f up;
	up.cardinal(1);

	/*
	std::cout<<"INFO: GLSLRenderer, current camera position: "<<_cameraPosition.Print()<<std::endl;
	std::cout<<"INFO: GLSLRenderer, current target position: "<<_targetPosition.Print()<<std::endl;
	
	float centre[4] = {0.0f,0.0f,0.0f,0.0f};
	_functionTree->GetBoundingBoxCentre(centre);
	std::cout<<"INFO: GLSLRenderer, current BBox centre: "<<centre[0]<<" "<<centre[1]<<" "<<centre[2]<<std::endl;
	*/

	cml::matrix_look_at_RH(_modelviewMatrix, _cameraPosition.GetCurrent(), _targetPosition.GetCurrent(), up);

	cml::matrix_rotate_about_local_x(_modelviewMatrix,_rotationX);
	cml::matrix_rotate_about_local_y(_modelviewMatrix,_rotationY);
	cml::matrix_rotate_about_local_z(_modelviewMatrix,_rotationZ);


	cml::matrix44f_c temp;

	/*
	{
		// Code to make object rotate about the centre of its bounding box
		
		float centre[4] = {0.0f,0.0f,0.0f,0.0f};
		_functionTree->GetBoundingBoxCentre(centre);

		temp.identity();
		cml::matrix_set_translation(temp,centre[0],centre[1],centre[2]);
		_worldMatrix = _worldMatrix * temp;
	}
	*/
	
	
		// Code to make object rotate about the aim point
		
		cml::matrix44f_c preTemp;
		preTemp.identity();
		cml::matrix_set_translation(preTemp,_targetPosition.GetCurrent().data()[0],_targetPosition.GetCurrent().data()[2],_targetPosition.GetCurrent().data()[1]);
		//_worldMatrix = _worldMatrix * temp;
	
	
	

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

	/*
	{
		// Code to make object rotate about the centre of its bounding box
		
		float centre[3] = {0.0f,0.0f,0.0f};
		_functionTree->GetBoundingBoxCentre(centre);

		temp.identity();
		cml::matrix_set_translation(temp,-centre[0],-centre[1],-centre[2]);
		_worldMatrix = _worldMatrix * temp;
	}
	*/
	
	
		// Code to make object rotate about the centre of its bounding box
		
		cml::matrix44f_c postTemp;
		postTemp.identity();
		cml::matrix_set_translation(postTemp,-_targetPosition.GetCurrent().data()[0],-_targetPosition.GetCurrent().data()[2],-_targetPosition.GetCurrent().data()[1]);
		//_worldMatrix = _worldMatrix * temp;
	
	


	_modelviewMatrix = _modelviewMatrix * preTemp * _worldMatrix * postTemp;

	_invModelview = _modelviewMatrix;
	_invModelview=cml::inverse(_invModelview);

	_functionTree->UpdateParameters( this );
}

void GLSLRenderer::SetStepsize(float stepsize)
{
	_stepsize = stepsize;
	_gradDelta = stepsize * 0.08f;
	std::cout<<"INFO: GLSLRenderer SetStepsize() stepsize: "<<_stepsize<<" gradDelta: "<<_gradDelta<<std::endl;
}

void GLSLRenderer::ResetWorldRotation()
{
	_rotationWorldX = _rotationWorldY = _rotationWorldZ = 0.0f;
	_rotationX = -PI/2.0f;
	_rotationY = 0.0f;
	_rotationZ = 0.0f;
	_angularVelocityX = _angularVelocityY = _angularVelocityZ = 0.0f;

	_axisX.set(1,0,0,0);
	_axisY.set(0,1,0,0);
	_axisZ.set(0,0,1,0);
	
	cml::vector3f up;
	up.cardinal(1);
	matrix_look_at_RH(_modelviewMatrix, _cameraPosition.GetCurrent(), _targetPosition.GetCurrent(), up);
	_invModelview = _modelviewMatrix;
	cml::inverse(_invModelview);

	_worldMatrix.identity();
}

void GLSLRenderer::AddWorldRotationOffsetDegs(float rotX, float rotY, float rotZ)
{
	AddWorldRotationOffsetRads((2.0f*PI)*(rotX/360.0f), (2.0f*PI)*(rotY/360.0f), (2.0f*PI)*(rotZ/360.0f));
}

void GLSLRenderer::AddWorldRotationOffsetRads(float rotX, float rotY, float rotZ)
{
	_rotationWorldX = std::fmod(_rotationWorldX + rotX,(float)(2.0f*PI));
	_rotationWorldY = std::fmod(_rotationWorldY + rotY,(float)(2.0f*PI));
	_rotationWorldZ = std::fmod(_rotationWorldZ + rotZ,(float)(2.0f*PI));

	cml::matrix44f_c temp;
	cml::matrix_rotation_axis_angle(temp, cml::vector3f(_axisX.data()), rotX);
	_axisX = _axisX * temp;
	_axisY = _axisY * temp;
	_axisZ = _axisZ * temp;

	temp.identity();
	cml::matrix_rotation_axis_angle(temp, cml::vector3f(_axisY.data()), rotY);
	_axisX = _axisX * temp;
	_axisY = _axisY * temp;
	_axisZ = _axisZ * temp;

	temp.identity();
	cml::matrix_rotation_axis_angle(temp, cml::vector3f(_axisZ.data()), rotZ);
	_axisX = _axisX * temp;
	_axisY = _axisY * temp;
	_axisZ = _axisZ * temp;
}

void GLSLRenderer::SetWorldAngularVelocityDegs(float rotX, float rotY, float rotZ)
{
	SetWorldAngularVelocityRads((2.0f*PI)*(rotX/360.0f), (2.0f*PI)*(rotY/360.0f), (2.0f*PI)*(rotZ/360.0f));
}

void GLSLRenderer::SetWorldAngularVelocityRads(float rotX, float rotY, float rotZ)
{
	_angularVelocityX = rotX;
	_angularVelocityY = rotY;
	_angularVelocityZ = rotZ;
}


void GLSLRenderer::Unproject(	float mouseX,float mouseY,
								int viewOriginX, int viewOriginY, int viewWidth, int viewHeight,
								float &originX,float &originY,float &originZ,
								float &dirX,float &dirY, float &dirZ)
{
/*
	#ifdef _DEBUG
	std::cout<<"INFO: GLSLRenderer::Unproject( mouseX = "<<mouseX<<" mouseY = "<<mouseY<<" viewOriginX = "<<viewOriginX<<" viewOriginY = "<<viewOriginY<<" viewWidth = "<<viewWidth<<" viewHeight = "<<viewHeight<<std::endl;
	#endif
*/

/*
	cml::vector4f mousePosNear( (2.0f*(mouseX-(float)viewOriginX))/((float)viewWidth) - 1.0f, (2.0f*(mouseY-(float)viewOriginY))/((float)viewHeight) - 1.0f, (2.0f*0.0f)-1.0f, 1.0f );
	cml::vector4f mousePosFar( (2.0f*(mouseX-(float)viewOriginX))/((float)viewWidth) - 1.0f, (2.0f*(mouseY-(float)viewOriginY))/((float)viewHeight) - 1.0f, (2.0f*1.0f)-1.0f, 1.0f );

	#ifdef _DEBUG
		std::cout<<"INFO: GLSLRenderer::Unproject, mousePosNear: "<<mousePosNear.data()[0]<<" "<<mousePosNear.data()[1]<<" "<<mousePosNear.data()[2]<<" "<<std::endl;
	#endif

	// Build projection matrix
	float aspectRatio = ((float)_windowWidth) / ((float)_windowHeight);
	//gluPerspective(_cameraAngle, aspectRatio, _cameraNearPlane, _cameraFarPlane);
	float f = cos(_cameraAngle*PI/360.0f)/sin(_cameraAngle*PI/360.0f);// trig identity for cotangent
	cml::matrix44f_c projMat;
	for( int i = 0; i < 4; i++ )
		for( int j = 0; j < 4; j++ )
			projMat(i,j) = 0.0f;
	projMat(0,0) = f / aspectRatio;
	projMat(1,1) = f;
	projMat(2,2) = (_cameraFarPlane + _cameraNearPlane) / (_cameraNearPlane - _cameraFarPlane);
	//projMat(2,2) = (10.0f + 1.0f) / (1.0f - 10.0f);
	//projMat(2,3) = (2.0 * 10.0f * 1.0f) / (1.0f - 10.0f);
	projMat(3,2) = (2.0 * _cameraFarPlane * _cameraNearPlane) / (_cameraNearPlane - _cameraFarPlane);
	projMat(2,3) = -1.0;
	//invProj = cml::inverse(invProj);

	cml::matrix44f_c invPM = projMat * _modelviewMatrix;
	invPM = cml::inverse(invPM);

	//cml::vector4f outputNear = _invModelview * invProj * mousePosNear;
	cml::vector4f outputNear = invPM * mousePosNear;
	//cml::vector4f outputFar = _invModelview * invProj * mousePosFar;
	cml::vector4f outputFar = invPM * mousePosFar;

	//outputNear.data()[2] = -outputNear.data()[2];
	//outputFar.data()[2] = -outputFar.data()[2];

	originX = outputNear.data()[0];
	originY = outputNear.data()[1];
	originZ = outputNear.data()[2];

	cml::vector4f tempVec(outputFar - outputNear);
	tempVec.data()[3] = 0.0f;
	tempVec.normalize();

	dirX = tempVec.data()[0];
	dirY = tempVec.data()[1];
	dirZ = tempVec.data()[2];

	
	_temp1X = originX; _temp1Y = originY; _temp1Z = originZ; _temp2X = outputFar.data()[0]; _temp2Y = outputFar.data()[1]; _temp2Z = outputFar.data()[2];
*/

	/* *** Don't touch this, it works! *** */

	float *tempFloat = _modelviewMatrix.data();
	double *mview = new double[16];
	for( int i = 0; i < 16; i++ )
		mview[i] = tempFloat[i];

	float aspectRatio = ((float)_windowWidth) / ((float)_windowHeight);
	//gluPerspective(_cameraAngle, aspectRatio, _cameraNearPlane, _cameraFarPlane);
	float f = cos((_cameraAngle*0.5f)*PI/180.0f)/sin((_cameraAngle*0.5f)*PI/180.0f);// trig identity for cotangent
	double *proj = new double[16];
	for( int i = 0; i < 16; i++ )
		proj[i] = 0.0;
	proj[0] = f / aspectRatio;
	proj[5] = f;
	proj[10] = (_cameraFarPlane + _cameraNearPlane) / (_cameraNearPlane - _cameraFarPlane);
	proj[14] = (2.0 * _cameraFarPlane * _cameraNearPlane) / (_cameraNearPlane - _cameraFarPlane);
	proj[11] = -1.0;

	int *view = new int[4];
	view[0] = viewOriginX;
	view[1] = viewOriginY;
	view[2] = viewWidth;
	view[3] = viewHeight;

	double objX, objY, objZ;

	int result = gluUnProject( mouseX,  mouseY, 0.0, mview,  proj,  view,  &objX,  &objY,  &objZ);

	if( result == GLU_FALSE )
		std::cerr<<"WARNING: GLSLRenderer::Unproject failure of first unproject operation"<<std::endl;

	originX = objX;
	originY = objY;
	originZ = objZ;

	result = gluUnProject( mouseX,  mouseY, 1.0, mview,  proj,  view,  &objX,  &objY,  &objZ);

	if( result == GLU_FALSE )
		std::cerr<<"WARNING: GLSLRenderer::Unproject failure of second unproject operation"<<std::endl;

	cml::vector3f tempVec(objX-originX, objY-originY, objZ-originZ);
	tempVec.normalize();

	dirX = tempVec.data()[0];
	dirY = tempVec.data()[1];
	dirZ = tempVec.data()[2];


	_temp1X = originX; _temp1Y = originY; _temp1Z = originZ; _temp2X = objX; _temp2Y = objY; _temp2Z = objZ;

	delete [] mview;
	delete [] proj;
	delete [] view;

}


void GLSLRenderer::ZoomCamera( float value )
{
	cml::vector3f aimPos = _cameraPosition.GetAim();
	aimPos[2] *= value;
	_cameraPosition.SetAim( aimPos );
}

void GLSLRenderer::SetCameraAngle( float angleDeg )
{
	_cameraAngle = angleDeg;
	RefreshCameraPos();
}

float GLSLRenderer::GetCameraDistance()
{
	return (_cameraPosition.GetCurrent() - _targetPosition.GetCurrent()).length();
}


void GLSLRenderer::ReserveCaches(unsigned int numCaches)
{
	if( _glCacheTexIDs != NULL )
	{
		for( unsigned int i = 0; i < _maxCaches; ++i )
		{
			FreeCache(i);
		}
		delete [] _glCacheTexIDs;
	}

	_maxCaches = numCaches;
	if( numCaches == 0 )
	{
		_glCacheTexIDs = NULL;
	}
	else
	{
		_glCacheTexIDs = new int[_maxCaches];
		for( unsigned int i = 0; i < _maxCaches; ++i )
		{
			_glCacheTexIDs[i] = -1;
		}
	}
}

unsigned int GLSLRenderer::GetMaxCachingVoxels()
{
	// TODO
	return 3*256*256*256;
}

void GLSLRenderer::FillCache(unsigned int ID, float *data, unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ)
{
	if( ID > _maxCaches )
	{
		std::cerr<<"WARNING: attempting to assign too many caches"<<std::endl;
		return;
	}
	
	unsigned int texID = _glCacheTexIDs[ID];
	// Check to see if cache already exists
	if( _glCacheTexIDs[ID] >= 0 )
	{
		glDeleteTextures(1,&texID);
	}

	// Generate OpenGL texture
	glGenTextures(1,&texID);
	glBindTexture(GL_TEXTURE_3D, texID);

	// Clamp to edges
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Trilinear filtering
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Transfer data to OpenGL
	glTexImage3D(GL_TEXTURE_3D, 0, GL_ALPHA32F_ARB, sizeX, sizeY, sizeZ, 0, GL_ALPHA, GL_FLOAT,data);

	glBindTexture(GL_TEXTURE_3D, 0);

	_glCacheTexIDs[ID] = texID;
}

void GLSLRenderer::FreeCache(unsigned int ID)
{
	if( ID > _maxCaches )
	{
		std::cerr<<"WARNING: attempting to free cache with ID beyond maxCaches"<<std::endl;
		return;
	}
	
	unsigned int texID = _glCacheTexIDs[ID];
	if( _glCacheTexIDs[ID] >= 0 )
	{
		glDeleteTextures(1,&texID);
	}
	_glCacheTexIDs[ID] = -1;
}




bool GLSLRenderer::InitialiseShaders(std::string vertFile, std::string fragFilePart)
{
	_GPUProgram = glCreateProgramObjectARB();
	_vertShader = glCreateShaderObjectARB(GL_VERTEX_SHADER);
	_fragShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER);

	glAttachShader(_GPUProgram,_vertShader);
	glAttachShader(_GPUProgram,_fragShader);


	// Vertex Shader

		// Load vertex shader from file into a string
		std::ifstream vertexFileStream(vertFile.c_str());
		std::string vertexShaderString((std::istreambuf_iterator<char>(vertexFileStream)), std::istreambuf_iterator<char>());

		// Send vertex shader string to OpenGL
		const char *vertSourceCopy = vertexShaderString.c_str();
		glShaderSource(_vertShader, 1, &vertSourceCopy, NULL);

		// Compile vertex shader
		if( !CompileShader(_vertShader) )
		{
			std::cerr<<"ERROR: Could not compile vertex shader"<<std::endl;
			return false;
		}

		
		#ifdef _DEBUG
		{
			GLenum err = glGetError();
			if( err != GL_NO_ERROR)
			{
				std::cerr<<"ERROR: GLSLRenderer::InitialiseShaders() OpenGL Error after compiling vertex shader"<<std::endl;
			}
		}
		#endif


	// Fragment Shader

		
		// Load first part of fragment shader from file into a string
		std::ifstream fragFileStream(fragFilePart.c_str());
		_fragShaderString = std::string((std::istreambuf_iterator<char>(fragFileStream)), std::istreambuf_iterator<char>());

		return RebuildTree();
}

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
	_functionTree->UpdateParameters( this );
	BuildParameterUniformLists();

	// Make sure the tree's cached bounding box size is up-to-date
		_functionTree->CalcBoundingBox();
	// Change camera's position and aiming point
		RefreshCameraPos();

	// Fragment Shader

		// Retrieve function string from VolumeTree
		std::string functionString = _functionTree->GetCachedFunctionGLSLString();

		// Append function string to first fragment shader part
		std::string fullFragShaderString = GetParameterGLSLDeclaration();
		fullFragShaderString.append(_fragShaderString);
		fullFragShaderString.append(functionString);

		//#ifdef _DEBUG
			std::cout<<"INFO: GLSLRenderer::RebuildTree(): function string for fragment shader is now: \n"<<functionString<<std::endl;
			//std::cout<<"INFO: GLSLRenderer::RebuildTree(): Fragment shader is now: \n"<<fullFragShaderString<<std::endl;
		//#endif

		// Send fragment shader string to OpenGL
		const char *fragSourceCopy = fullFragShaderString.c_str();
		glShaderSource(_fragShader, 1, &fragSourceCopy, NULL);

		
		#ifdef _DEBUG
		{
			GLenum err = glGetError();
			if( err != GL_NO_ERROR)
			{
				std::cerr<<"ERROR: GLSLRenderer::RebuildTree() OpenGL Error before compiling fragment shader: "<<gluErrorString(err)<<std::endl;
			}
		}
		#endif
		
		

		// Compile fragment shader
		if( !CompileShader(_fragShader) )
		{
			std::cerr<<"ERROR: Could not compile fragment shader"<<std::endl;
			std::cerr<<"SOURCE: <<"<<fullFragShaderString<<std::endl;
			return false;
		}


		#ifdef _DEBUG
		{
			GLenum err = glGetError();
			if( err != GL_NO_ERROR)
			{
				std::cerr<<"ERROR: GLSLRenderer::RebuildTree() OpenGL Error after building fragment shader: "<<gluErrorString(err)<<std::endl;
			}
		}
		#endif
		


	// Link shaders
	glLinkProgramARB(_GPUProgram);

	GLint progLinkSuccess;
	glGetObjectParameterivARB(_GPUProgram, GL_OBJECT_LINK_STATUS_ARB, &progLinkSuccess);
	if( !progLinkSuccess )
	{
		std::cerr<<"ERROR: Shader could not be linked"<<std::endl;
		return false;
	}

	// Get Uniform handles

		_projMatrixUniform =    glGetUniformLocation(_GPUProgram, "projectionMatrix");
		_mvMatrixUniform =      glGetUniformLocation(_GPUProgram, "ViewMatrix");
		_invViewMatrixUniform = glGetUniformLocation(_GPUProgram, "invView");
		_vertSize =             glGetUniformLocation(_GPUProgram, "vertsize");
		_vertPos =              glGetUniformLocation(_GPUProgram, "vertposition");

		_fragboundmin = glGetUniformLocation(_GPUProgram, "fragboundmin");
		_fragboundmax = glGetUniformLocation(_GPUProgram, "fragboundmax");
		_fragStepsize = glGetUniformLocation(_GPUProgram, "stepsize");
		_fragGradDelta = glGetUniformLocation(_GPUProgram, "gradDelta");

		_fragCache0 = glGetUniformLocation(_GPUProgram, "CacheTexture0");
		_fragCache1 = glGetUniformLocation(_GPUProgram, "CacheTexture1");
		_fragCache2 = glGetUniformLocation(_GPUProgram, "CacheTexture2");
		_fragCache3 = glGetUniformLocation(_GPUProgram, "CacheTexture3");

		_fragColour = glGetUniformLocation(_GPUProgram, "objectcolour");


		
		#ifdef _DEBUG
		{
			GLenum err = glGetError();
			if( err != GL_NO_ERROR)
			{
				std::cerr<<"ERROR: GLSLRenderer::RebuildTree() OpenGL Error after linking shaders: "<<gluErrorString(err)<<std::endl;
			}
		}
		#endif
		

	return true;
}

unsigned int GLSLRenderer::GetNumUsedRawParameters()
{
	unsigned int paramSizeCount = 0;
	for( std::vector<TreeParameter>::iterator it = _paramList.begin(); it != _paramList.end(); ++it )
	{
		// based on number of vec4s
		if( (unsigned int) it->_type <= 3 )
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

bool GLSLRenderer::ParametersStillAvailable()
{
	return GetNumUsedRawParameters() < _maxParametersRaw;
	//return _paramList.size()*4 < _maxParameters;
}

unsigned int GLSLRenderer::NewParameter(ParameterType type)
{
	// Note that our ID approach is to assume a uint has enough IDs without keeping track of freed ones
	// This means over 4billion IDs so we're probably ok

	// Need to prevent allocating too many params when we don't really have the space
	if( (GetNumUsedRawParameters() + 16) >= _maxParametersRaw )
	{
		std::cerr<<"WARNING: GLSL Renderer: Maximum tree parameters reached, cannot allocate new parameter. MaxParametersRaw: "<<_maxParametersRaw<<std::endl;
		return 0;
	}
	_paramIDcount++;
	_paramList.push_back(TreeParameter(type,_paramIDcount));
	return _paramIDcount;
}

void GLSLRenderer::DeleteParameter(unsigned int paramID)
{
	if( paramID == 0 )
	{
		return;
	}
	// Note that our ID approach is to assume a uint has enough IDs without keeping track of freed ones
	// This means over 4billion IDs so we're probably ok
	if( paramID == _paramIDcount )
	{
		_paramIDcount--;
	}

	std::vector<TreeParameter>::iterator it = GetParameter( paramID );
	if( it != _paramList.end() )
	{
		delete [] it->_data;
		_paramList.erase(it);
	}
	else
	{
		std::cerr<<"WARNING: GLSLRenderer DeleteParameter() cannot find parameter of ID: "<<paramID<<std::endl;
	}
}

void GLSLRenderer::SetParameter(unsigned int paramID, float *data)
{
	std::vector<TreeParameter>::iterator it = GetParameter( paramID );
	if( it != _paramList.end() )
	{
		it->SetData(data);
	}
	else
	{
		std::cerr<<"WARNING: GLSLRenderer DeleteParameter() cannot find parameter of ID: "<<paramID<<std::endl;
	}
}

void GLSLRenderer::TreeParameter::SetData(float *dataIn)
{
	int dataLength = GLSLRenderer::GetParameterTypeDatasize( _type );
	if( _data == NULL )
	{
		_data = new float[dataLength];
	}
	for( unsigned int i = 0; i < dataLength; i++ )
	{
		_data[i] = dataIn[i];
	}
}

unsigned int GLSLRenderer::GetParameterTypeDatasize( ParameterType value )
{
	unsigned int datasize = ((unsigned int) value)+1;
	if( value == MAT3 )
	{
		return 9;
	}
	if( value == MAT4 )
	{
		return 16;
	}
	return datasize;
}

std::string GLSLRenderer::GetParameterString(unsigned int paramID)
{
	if( paramID == 0 )
	{
		return "";
	}
	std::vector<GLSLRenderer::TreeParameter>::iterator param = GetParameter( paramID );

	unsigned int uniformPos = GetParameterUniformPos( param );

	std::stringstream paramStream;
	paramStream<< GetParameterUniformTypeStr( param->_type, false );
	paramStream<<"["<<uniformPos<<"]";

	return paramStream.str();
}

std::vector<GLSLRenderer::TreeParameter>::iterator GLSLRenderer::GetParameter(unsigned int id)
{
	unsigned int pos;
	return GetParameter(id,pos);
}

std::vector<GLSLRenderer::TreeParameter>::iterator GLSLRenderer::GetParameter(unsigned int id, unsigned int &position)
{
	if( id == 0 )
	{
		return _paramList.end();
	}
	position = 0;
	std::vector<TreeParameter>::iterator it = _paramList.begin();
	while( (it != _paramList.end()) && (it->_ID != id) )
	{
		++it;
		++position;
	};
	return it;
}

unsigned int GLSLRenderer::GetParameterUniformPos( std::vector<TreeParameter>::iterator param )
{
	unsigned int paramCount = 0;
	for( std::vector<TreeParameter>::iterator it = _paramList.begin(); (it != _paramList.end()) && (it != param); ++it )
	{
		if( it->_type == param->_type )
		{
			paramCount++;
		}
	}
	return paramCount;
}

std::string GLSLRenderer::GetParameterUniformTypeStr( GLSLRenderer::ParameterType type, bool showDataType )
{
	if( showDataType )
	{
		if( type == FLOAT )
		{
			return "float paramFloat";
		}
		if( type == VEC2 )
		{
			return "vec2 paramVec2";
		}
		if( type == VEC3 )
		{
			return "vec3 paramVec3";
		}
		if( type == VEC4 )
		{
			return "vec4 paramVec4";
		}
		if( type == MAT3 )
		{
			return "mat3 paramMat3";
		}
		if( type == MAT4 )
		{
			return "mat4 paramMat4";
		}
		return "ERROR! GLSLRenderer::ParameterType Unknown!!!";
	}
	else
	{
		if( type == FLOAT )
		{
			return "paramFloat";
		}
		if( type == VEC2 )
		{
			return "paramVec2";
		}
		if( type == VEC3 )
		{
			return "paramVec3";
		}
		if( type == VEC4 )
		{
			return "paramVec4";
		}
		if( type == MAT3 )
		{
			return "paramMat3";
		}
		if( type == MAT4 )
		{
			return "paramMat4";
		}
		return "ERROR! GLSLRenderer::ParameterType Unknown!!!";
	}
}

void GLSLRenderer::BuildParameterUniformLists()
{
	// First, work out lengths of the lists
	unsigned int numParamTypes = GetParameterTypeSize();
	if( _parameterUniformListLengths == NULL )
	{
		_parameterUniformListLengths = new unsigned int[numParamTypes];
	}
	for( unsigned int i = 0; i < numParamTypes; i++ )
	{
		_parameterUniformListLengths[i] = 0;
	}
	for( std::vector<TreeParameter>::iterator it = _paramList.begin(); it != _paramList.end(); ++it )
	{
		unsigned int index = GetParameterTypeIndex(it->_type);
		it->_shaderUniformOffset = _parameterUniformListLengths[index];
		_parameterUniformListLengths[index] += 1;
	}

	// Allocate the lists
	if( _parameterUniformData == NULL )
	{
		_parameterUniformData = new float*[numParamTypes];

		for( unsigned int i = 0; i < numParamTypes; i++ )
		{
			unsigned int arraySize = _parameterUniformListLengths[i] * GetParameterTypeDatasize(static_cast<ParameterType>(i));
			_parameterUniformData[i] = new float[arraySize];
		}
	}
	else
	{
		for( unsigned int i = 0; i < numParamTypes; i++ )
		{
			delete [] _parameterUniformData[i];
			unsigned int arraySize = _parameterUniformListLengths[i] * GetParameterTypeDatasize(static_cast<ParameterType>(i));
			_parameterUniformData[i] = new float[arraySize];
		}
	}

	// Populate with data
	for( unsigned int typeIndex = 0; typeIndex < numParamTypes; typeIndex++ )
	{
		unsigned int dataIndex = 0;
		unsigned int dataLength = GetParameterTypeDatasize(static_cast<ParameterType>(typeIndex));

		// We only just allocated the uniform offsets so the things must be in the same order still
		for( std::vector<TreeParameter>::iterator it = _paramList.begin(); it != _paramList.end(); ++it )
		{
			if( it->_type == static_cast<ParameterType>(typeIndex) )
			{
				for( unsigned int i = 0; i < dataLength; i++ )
				{
					float dataEntry = 0.0f;
					if( it->_data != NULL )
					{
						dataEntry = it->_data[i];
					}
					_parameterUniformData[typeIndex][(dataIndex * dataLength) + i] = dataEntry;
				}
				dataIndex++;
			}
		}
	}
}

std::string GLSLRenderer::GetParameterGLSLDeclaration()
{
	std::stringstream shaderStream;
	unsigned int numParamTypes = GetParameterTypeSize();
	for( unsigned int i = 0; i < numParamTypes; i++ )
	{
		if( _parameterUniformListLengths[i] > 0 )
		{
			shaderStream<<"uniform "<<GetParameterUniformTypeStr(static_cast<ParameterType>(i), true)<<"["<<_parameterUniformListLengths[i]<<"];"<<std::endl;
		}
	}
	return shaderStream.str();
}

void GLSLRenderer::BindParametersToGL()
{
	for( unsigned int currentUniformType = 0; currentUniformType < GetParameterTypeSize(); currentUniformType++ )
	{
		unsigned int numUniforms = _parameterUniformListLengths[currentUniformType];
		if( numUniforms > 0 )
		{
			ParameterType currentType = static_cast<ParameterType>(currentUniformType);
			int uniformLocation = glGetUniformLocation(_GPUProgram, GetParameterUniformTypeStr( currentType, false).c_str() );
			if( uniformLocation >= 0 )
			{
				if( currentType == FLOAT )
				{
					glUniform1fv(uniformLocation, numUniforms, _parameterUniformData[currentUniformType] );
				}
				else if( currentType == VEC2 )
				{
					glUniform2fv(uniformLocation, numUniforms, _parameterUniformData[currentUniformType] );
				}
				else if( currentType == VEC3 )
				{
					glUniform3fv(uniformLocation, numUniforms, _parameterUniformData[currentUniformType] );
				}
				else if( currentType == VEC4 )
				{
					glUniform4fv(uniformLocation, numUniforms, _parameterUniformData[currentUniformType] );
				}
				else if( currentType == MAT3 )
				{
					glUniformMatrix3fv(uniformLocation, numUniforms, false, _parameterUniformData[currentUniformType] );
				}
				else if( currentType == MAT4 )
				{
					glUniformMatrix4fv(uniformLocation, numUniforms, false, _parameterUniformData[currentUniformType] );
				}
			}
		}
	}
}

void GLSLRenderer::RefreshCameraPos()
{
	float centre[3] = {0.0f,0.0f,0.0f};
	_functionTree->GetBoundingBoxCentre(centre);
	float temp = centre[1];
	centre[1] = centre[2];
	centre[2] = temp;
	_targetPosition.SetAim(cml::vector3f(centre));

	float bboxSize[3] = {1.0f,1.0f,1.0f};
	_functionTree->GetBoundingBoxSize(bboxSize);
	

	float bboxMaxDim = _functionTree->GetBoundingBoxMaxDim();
	float camDist = (bboxMaxDim*0.5f) / (tan((2.0f*PI)*((0.4f*_cameraAngle)/360.0f)));
	_cameraPosition.SetAim( cml::vector3f(0.0f,centre[1],camDist) );

	#ifdef _DEBUG
		std::cout<<"INFO: GLSLRenderer::RebuildTree() camera position aim: 0.0f, "<<centre[1]<<" "<<camDist<<std::endl;
		std::cout<<"INFO: GLSLRenderer::RebuildTree() camera lookat target position aim: "<<centre[0]<<" "<<centre[1]<<" "<<centre[2]<<std::endl;
	#endif

	_cameraNearPlane = camDist * 0.1f;
	_cameraFarPlane = camDist * 10.0f; // This can sometimes cause problems if the previous object was very big etc
}

void GLSLRenderer::InitialiseCube()
{

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
}

bool GLSLRenderer::CompileShader(unsigned int shaderID)
{
	// http://www.opengl.org/wiki/OpenGL_Shading_Language#Error_Checking
	glCompileShader(shaderID);

	int isCompiled;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);

	if( isCompiled == 0 )
	{
		int logLength = -1;
		int maxLength(0);
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		if (maxLength!=0)
		{
			char *infoLog = new char[maxLength];

			glGetShaderInfoLog(shaderID, maxLength, &logLength, infoLog);

			std::cerr<<"WARNING: GLSL Compiler error: "<<infoLog<<std::endl;

			delete [] infoLog;
		}
		else
		{
			std::cerr<<"ERROR: Aaaaaaaargh!"<<std::endl;
		}
		return false;
	}

	//glAttachObjectARB(_GPUProgram, shaderID);
	return true;
}


