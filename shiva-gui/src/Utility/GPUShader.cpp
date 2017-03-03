#include "Utility/GPUShader.h"

#include <fstream>
#include <iostream>


Utility::GPUShader::GPUShader( ShaderType _type )
{
	m_type = _type;
	m_handle = 0;
}

Utility::GPUShader::~GPUShader()
{
	glDeleteShader( m_handle );
}

//----------------------------------------------------------------------------------

bool Utility::GPUShader::Load( std::string _source )
{
	m_source = _source;
	const char * pSourceCopy = m_source.c_str();
	if( m_type == FRAGMENT ) {
		m_handle = glCreateShader( GL_FRAGMENT_SHADER );
	}	
	else if( m_type == VERTEX ) {
		m_handle = glCreateShader( GL_VERTEX_SHADER );
		GLenum err = glGetError();
	}
	else return false;

	glShaderSource( m_handle, 1, &pSourceCopy, NULL );
	return true;
}

//----------------------------------------------------------------------------------

bool Utility::GPUShader::LoadFromFile( std::string _fileName )
{
	std::ifstream ifs( _fileName.c_str() );
	std::string str( ( std::istreambuf_iterator< char >( ifs ) ), std::istreambuf_iterator< char >() );
	return Load( str );
}

//----------------------------------------------------------------------------------

bool Utility::GPUShader::CompileShader( GLuint _programID )
{
	// http://www.opengl.org/wiki/OpenGL_Shading_Language#Error_Checking
	glCompileShader( m_handle );

	int isCompiled;
	glGetShaderiv( m_handle, GL_COMPILE_STATUS, &isCompiled );

	if( isCompiled == 0 )
	{
		int logLength = -1;
		int maxLength( 0 );
		glGetShaderiv( m_handle, GL_INFO_LOG_LENGTH, &maxLength );

		if ( maxLength != 0 )
		{
			char *infoLog = new char[ maxLength ];

			glGetShaderInfoLog( m_handle, maxLength, &logLength, infoLog );

			std::cerr << "GLSL Compiler error: " << infoLog << std::endl;

			delete [] infoLog;
		}
		else {
			std::cerr << "Aaaaaaaargh!" << std::endl;
			return false;
		}
	}

	glAttachShader( _programID, m_handle );
	return true;
}

//----------------------------------------------------------------------------------

void Utility::GPUShader::DetachShader( GLuint _programID )
{
	glDetachShader( _programID, m_handle );
}