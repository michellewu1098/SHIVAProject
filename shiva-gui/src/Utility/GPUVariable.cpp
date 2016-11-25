#include "Utility/GPUVariable.h"

//////////////////////////////////////////////////////////////////////////
#include <GL/GLee.h>
#include <GL/glu.h>
#include <iostream>

Utility::GPUVariable::GPUVariable(GPUProgram *program, GPUShader *shader, std::string variableName)
{
	_ID = -1;
	_object = 0;

	_object = program->GetProgramID();//glCreateProgramObjectARB();
	//glUseProgramObjectARB(_object);
	//shader->AttachObject(_object);

	GLenum err = glGetError();
	if( err != GL_NO_ERROR)
		std::cerr<<"ERROR: GPUVariable Ctor: OpenGL Error: "<< gluErrorString(err)<<std::endl;

	//glLinkProgramARB(_object);
	//glUseProgramObjectARB(_object);
	_ID = glGetUniformLocationARB(_object, variableName.c_str());
}

Utility::GPUVariable::~GPUVariable()
{

}

void Utility::GPUVariable::UniformInt(int value)
{
	glUniform1iARB(_ID, value);
}

void Utility::GPUVariable::UniformFloat(float value)
{
	glUniform1fARB(_ID, value);
}

void Utility::GPUVariable::UniformFloat3(float *value)
{
	glUniform3fvARB(_ID, 1, value);
}

void Utility::GPUVariable::UniformFloat4(float *value)
{
	glUniform4fvARB(_ID, 1, value);
}

void Utility::GPUVariable::UniformMatrix4(float *value)
{
	glUniformMatrix4fvARB(_ID, 1, GL_TRUE, value);
}
