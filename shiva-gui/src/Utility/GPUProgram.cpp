#include "Utility/GPUProgram.h"

#include <fstream>
#include <iostream>

//----------------------------------------------------------------------------------

Utility::GPUProgram::GPUProgram()
{
	m_program = 0;
	m_vs = NULL;
	m_fs = NULL;
}

//----------------------------------------------------------------------------------

Utility::GPUProgram::~GPUProgram()
{
	m_vs->DetachShader( m_program );
	m_fs->DetachShader( m_program );

	if ( !m_vs )
		delete m_vs;
	if ( !m_fs )
		delete m_fs;

	glDeleteProgram( m_program );

	m_program = 0;
	m_vs = NULL;
	m_fs = NULL;
}

//----------------------------------------------------------------------------------

bool Utility::GPUProgram::ValidateProgram( GLuint _program, GLuint _vs, GLuint _fs )
{
	GLint isLinked = 0;
	glGetProgramiv( _program, GL_LINK_STATUS, ( int* )&isLinked );

	if( isLinked == GL_FALSE )
	{
		GLint maxLength = 0;
        glGetProgramiv( _program, GL_INFO_LOG_LENGTH, &maxLength );

        char* infoLog = new char[ maxLength ];
        glGetProgramInfoLog( _program, maxLength, &maxLength, infoLog );
       
		if( maxLength > 0 ) {
            std::cerr << "Program " << _program << " link error: " << infoLog << std::endl;
        }
        
		//We don't need the program anymore.
        glDeleteProgram( _program );
        //Don't leak shaders either.
        glDeleteShader( _vs );
        glDeleteShader( _fs);
		delete[] infoLog;
        return false;
     }
    return true;
}
//----------------------------------------------------------------------------------

bool Utility::GPUProgram::Create( const std::string _programFileName, ProgramType _type )
{
	GLenum err = glGetError();
	if( err != GL_NO_ERROR ) {
		std::cerr << "ERROR: Utility::GPUProgram::Create() " << _programFileName << ", OpenGL Error at entrance to function: " << gluErrorString( err ) << std::endl;
		//return false;
	}

	// #1 Create shader program

	m_program = glCreateProgram();

	err = glGetError();
	if( err != GL_NO_ERROR ) {
		std::cerr << "ERROR: Utility::GPUProgram::Create() " << _programFileName << ", Could not create program object. Error: " << gluErrorString( err ) << std::endl;
		return false;
	}

	// #2 Create, compile and attach the appropriate shader objs to the shader program

	if ( _type == VERTEX || _type == VERTEX_AND_FRAGMENT )
	{
		m_vs = new GPUShader( GPUShader::VERTEX );
		m_vs->LoadFromFile( std::string( _programFileName + std::string( ".vert" ) ) );
		m_vs->CompileShader( m_program );
	}

	if ( _type == FRAGMENT || _type == VERTEX_AND_FRAGMENT )
	{
		m_fs = new GPUShader( GPUShader::FRAGMENT );
		m_fs->LoadFromFile( std::string( _programFileName + std::string( ".frag" ) ) );
		m_fs->CompileShader( m_program );
	}

	// #3 Link shader program

	//glLinkProgram( m_program );

	//// #4 Check that shader link phase completed successfully

	//if( !ValidateProgram( m_program, m_vs->GetShaderHandle(), m_fs->GetShaderHandle() ) ) {
	//	return false;
	//}

	return true;
}

bool Utility::GPUProgram::LinkProgram()
{
	// #3 Link shader program

	glLinkProgram( m_program );

	// #4 Check that shader link phase completed successfully

	if( !ValidateProgram( m_program, m_vs->GetShaderHandle(), m_fs->GetShaderHandle() ) ) {
		return false;
	}
	
	return true;
}

//----------------------------------------------------------------------------------

void Utility::GPUProgram::Bind()
{
	glUseProgram( m_program );
}

//----------------------------------------------------------------------------------

void Utility::GPUProgram::Unbind()
{
	glUseProgram( 0 );
}

//----------------------------------------------------------------------------------