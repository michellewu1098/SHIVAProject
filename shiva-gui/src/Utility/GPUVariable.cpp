#include "Utility/GPUVariable.h"

//----------------------------------------------------------------------------------

Utility::GPUVariable::GPUVariable( GLuint _programID, std::string _variableName )
{
	m_loc = glGetUniformLocation( _programID, _variableName.c_str() );
	if( m_loc == -1 )
	{
		GLenum err = glGetError();
		if( err != GL_NO_ERROR )
			std::cerr << "ERROR: GPUVariable Ctor: OpenGL Error: " << gluErrorString( err ) << std::endl;
	}
}

//----------------------------------------------------------------------------------

Utility::GPUVariable::~GPUVariable()
{ }

//----------------------------------------------------------------------------------

void Utility::GPUVariable::UniformInt( int _value )
{
	glUniform1i( m_loc, _value );
}

//----------------------------------------------------------------------------------

void Utility::GPUVariable::UniformFloat( float _value )
{
	glUniform1f( m_loc, _value );
}

//----------------------------------------------------------------------------------

void Utility::GPUVariable::UniformFloat3( float *_value )
{
	glUniform3fv( m_loc, 1, _value );
}

//----------------------------------------------------------------------------------

void Utility::GPUVariable::UniformFloat4( float *_value )
{
	glUniform4fv( m_loc, 1, _value );
}

//----------------------------------------------------------------------------------

void Utility::GPUVariable::UniformMatrix4( float *_value )
{
	glUniformMatrix4fv( m_loc, 1, GL_FALSE, _value );
}

//----------------------------------------------------------------------------------