#include "Utility/GPUShader.h"

//////////////////////////////////////////////////////////////////////////
#include <GL/GLee.h>
#include <fstream>
#include <iostream>


Utility::GPUShader::GPUShader(ShaderType type)
{
	_type = type;
	_shaderHandle = 0;
}

Utility::GPUShader::~GPUShader()
{
}


bool Utility::GPUShader::Load(std::string source)
{
	_source = source;
	const char * pSourceCopy = _source.c_str();
	if( _type == FRAGMENT )
		_shaderHandle = glCreateShaderObjectARB(GL_FRAGMENT_SHADER);
	else if( _type == VERTEX )
		_shaderHandle = glCreateShaderObjectARB(GL_VERTEX_SHADER);
	else return false;

	glShaderSource(_shaderHandle, 1, &pSourceCopy, NULL);
	return true;
}

bool Utility::GPUShader::LoadFromFile(std::string fileName)
{
	std::ifstream ifs(fileName.c_str());
	std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	return Load(str);
/*

	std::string szBody;

	FILE * pFile = fopen(fileName.c_str(), "r");
	if (!pFile)
		return false;
	char buffer[1024];
	while (fgets(buffer, 1024, pFile) != NULL)
	{
		szBody += buffer;
		szBody += "\n";
	}
	fclose(pFile);

	return Load(szBody);
*/
}

bool Utility::GPUShader::CompileLink(unsigned int programID)
{
	// http://www.opengl.org/wiki/OpenGL_Shading_Language#Error_Checking
	glCompileShader(_shaderHandle);

	int isCompiled;
	glGetShaderiv(_shaderHandle, GL_COMPILE_STATUS, &isCompiled);

	if( isCompiled == 0 )
	{
		int logLength = -1;
		int maxLength(0);
		glGetShaderiv(_shaderHandle, GL_INFO_LOG_LENGTH, &maxLength);

		if (maxLength!=0)
		{
			char *infoLog = new char[maxLength];

			glGetShaderInfoLog(_shaderHandle, maxLength, &logLength, infoLog);

			std::cerr<<"GLSL Compiler error: "<<infoLog<<std::endl;

			delete [] infoLog;
		}
		else
		{
			std::cerr<<"Aaaaaaaargh!"<<std::endl;
		}
	}

	glAttachObjectARB(programID, _shaderHandle);
	return true;
}

bool Utility::GPUShader::AttachObject(unsigned int objectID)
{
	glAttachObjectARB(objectID, _shaderHandle);
	return true;
}
