#include "VolumeRenderer/Shader.h"

//------------------------------------------------------------------------------------

Shader::Shader() : m_init( false ),
                   m_id( 0 ),
                   m_vs( 0 ),
                   m_fs( 0 )
{
}

//------------------------------------------------------------------------------------

Shader::Shader( std::string _vsFile, std::string _fsFile ) : m_init( false ),
                                                             m_id( 0 ),
                                                             m_vs( 0 ),
                                                             m_fs( 0 )
{
	m_vsFileName = _vsFile.c_str();
	m_fsFileName = _fsFile.c_str();
}

//------------------------------------------------------------------------------------

void Shader::setFileNames( std::string _vsFile, std::string _fsFile )
{
	m_vsFileName = _vsFile.c_str();
	m_fsFileName = _fsFile.c_str();
}

//------------------------------------------------------------------------------------

std::string Shader::fileRead( const char *_fileName )
{
	std::ifstream fileStream( _fileName );
	return std::string( std::istreambuf_iterator< char >( fileStream ), std::istreambuf_iterator< char >() );
}

//------------------------------------------------------------------------------------

bool Shader::validateShader( GLuint _shader, const char* _file )
{
    GLint isCompiled = 0;
    glGetShaderiv( _shader, GL_COMPILE_STATUS, &isCompiled );
    if( isCompiled == GL_FALSE )
    {
        GLint maxLength = 0;
        glGetShaderiv( _shader, GL_INFO_LOG_LENGTH, &maxLength );

        // The maxLength includes the NULL character
        char* errorLog = new char[ maxLength ];
        glGetShaderInfoLog( _shader, maxLength, &maxLength, errorLog );
        if ( maxLength > 0 ) {
            std::cerr << "Shader " << _shader << " (" << (_file?_file:"") << ") compile error: " << errorLog << std::endl;
        }
        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader( _shader ); // Don't leak the shader.
		delete[] errorLog;
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------------

bool Shader::validateProgram( GLuint _program, GLuint _vs, GLuint _fs )
{
    GLint isLinked = 0;
    glGetProgramiv( _program, GL_LINK_STATUS, (int *)&isLinked );
    if( isLinked == GL_FALSE )
    {
        GLint maxLength = 0;
        glGetProgramiv( _program, GL_INFO_LOG_LENGTH, &maxLength );

        //The maxLength includes the NULL character
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

//------------------------------------------------------------------------------------

void Shader::init( std::string _vsFile, std::string _fsFile )
{
    if( m_init ) { destroy(); }

    // #1 Create a shader object
    // #2 Compile shader source into the object
    // #3 Verify that shader compiled successfully

    m_vs = glCreateShader( GL_VERTEX_SHADER );
    m_fs = glCreateShader( GL_FRAGMENT_SHADER );

	const char* vsText = _vsFile.c_str();
    const char* fsText = _fsFile.c_str();

	if (vsText == NULL || fsText == NULL) {
        std::cerr << "Either vertex shader or fragment shader file not found." << std::endl;
        return;
    }

    glShaderSource( m_vs, 1, &vsText, NULL );
    glShaderSource( m_fs, 1, &fsText, NULL );

	m_vsSourceFile = _vsFile;
	m_fsSourceFile = _fsFile;

    glCompileShader( m_vs );
    // Determine if the compilation was successful
    if( !validateShader( m_vs, m_vsFileName ) ) {
        return;
    }

    glCompileShader( m_fs );
    if( !validateShader( m_fs, m_fsFileName ) ) {
        return;
    }

    // #1 Create a shader program
    // #2 Attach the appropriate shader objs to the shader program
    // #3 Link the shader program
    // #4 Verify that shader link phase completed successfully
    // #5 Use shader for vertex or fragment processing

    m_id = glCreateProgram();

    glAttachShader( m_id, m_vs );
    glAttachShader( m_id, m_fs );

    glLinkProgram( m_id );
    if (!validateProgram( m_id, m_vs, m_fs ) ) {
        return;
    }
    m_init = true;
}

//------------------------------------------------------------------------------------

void Shader::destroy()
{
    if( m_init )
    {
        glDetachShader( m_id, m_fs );
        glDetachShader( m_id, m_vs );

        glDeleteShader( m_fs );
        glDeleteShader( m_vs );

        glDeleteProgram( m_id );

        m_id = 0;
        m_vs = 0;
        m_fs = 0;
        m_init = false;
    }
}

//------------------------------------------------------------------------------------

Shader::~Shader()
{
    destroy();
}

//------------------------------------------------------------------------------------

void Shader::bind() const
{
    if( m_init )
    {
        glUseProgram( m_id );
    }
}

//------------------------------------------------------------------------------------

void Shader::unbind() const
{
    if( m_init)
    {
        glUseProgram( 0 );
    }
}

//------------------------------------------------------------------------------------

