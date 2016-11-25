#include "Utility/GPUProgram.h"

//////////////////////////////////////////////////////////////////////////
#include <GL/GLee.h>
#include <GL/glu.h>
#include <fstream>
#include <iostream>

Utility::GPUProgram::GPUProgram()
{
	_program = 0;
	_vertexShader = NULL;
	_fragmentShader = NULL;
}

Utility::GPUProgram::~GPUProgram()
{
	if (!_program)
		glDeleteObjectARB(_program);
	if (!_vertexShader)
		delete _fragmentShader;
	if (!_fragmentShader)
		delete _fragmentShader;
}

bool Utility::GPUProgram::Create(const std::string programFileName, ProgramType type)
{
	GLenum err = glGetError();
	if( err != GL_NO_ERROR)
	{
		std::cerr<<"ERROR: Utility::GPUProgram::Create() "<<programFileName<<", OpenGL Error at entrance to function: "<< gluErrorString(err)<<std::endl;
		return false;
	}

	_program = glCreateProgramObjectARB();

	err = glGetError();
	if( err != GL_NO_ERROR)
	{
		std::cerr<<"ERROR: Utility::GPUProgram::Create() "<<programFileName<<", Could not create program object. Error: "<< gluErrorString(err)<<std::endl;
		return false;
	}

	if (type == VERTEX || type == VERTEX_AND_FRAGMENT)
	{
		_vertexShader = new GPUShader(GPUShader::VERTEX);
		_vertexShader->LoadFromFile(std::string(programFileName + std::string(".vert")));
		_vertexShader->CompileLink(_program);
	}

	if (type == FRAGMENT || type == VERTEX_AND_FRAGMENT)
	{
		_fragmentShader = new GPUShader(GPUShader::FRAGMENT);
		_fragmentShader->LoadFromFile(std::string(programFileName + std::string(".frag")));
		_fragmentShader->CompileLink(_program);
	}

	glLinkProgramARB(_program);

	GLint progLinkSuccess;
	glGetObjectParameterivARB(_program, GL_OBJECT_LINK_STATUS_ARB, &progLinkSuccess);
	if (!progLinkSuccess)
	{
		std::cerr<<"ERROR: Shader"<<programFileName<<" could not be linked"<<std::endl;
		//std::string szOut = std::string("Shader ") + programFileName + std::string(" could not be linked");
		//MessageBox(NULL, szOut.c_str(), "Error", MB_OK);
		//printInfoLog(pProgram);
		return false;
	}
	return true;
}

void Utility::GPUProgram::On()
{
	glUseProgramObjectARB(_program);
}

void Utility::GPUProgram::Off()
{
	glUseProgramObjectARB(0);
}
