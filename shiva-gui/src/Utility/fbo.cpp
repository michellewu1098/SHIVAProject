#include "Utility/fbo.h"


namespace FrameBufferObjects
{
	//----------------------------------------------------------------------------------
	/// \brief This map associates the slot enum with the actual openGL 'enum'
	//----------------------------------------------------------------------------------
	std::map< Slot, unsigned int > GLSlotMap;
	//----------------------------------------------------------------------------------
	/// \brief Populates the slot map with the slot-GLenum key pairs
	//----------------------------------------------------------------------------------
	void InitGLSlotMap();
	//----------------------------------------------------------------------------------
	/// \brief Whether the slot map has been populated (i.e. GL_slot_map_init has been called)
	//----------------------------------------------------------------------------------
	bool GLSlotMapIsInitialised = false;
	//----------------------------------------------------------------------------------
	/// \brief Utility function to convert an openGL slot definition to a slot definition
	//----------------------------------------------------------------------------------
	GLenum GetSlotGL( Slot );
	//----------------------------------------------------------------------------------

}

//----------------------------------------------------------------------------------

void FrameBufferObjects::InitGLSlotMap()
{
	// Populate that horrible map:
	GLSlotMap[ FBO_COLOUR_ATTACHMENT0 ]	 = GL_COLOR_ATTACHMENT0_EXT;
	GLSlotMap[ FBO_COLOUR_ATTACHMENT1 ]	 = GL_COLOR_ATTACHMENT1_EXT;
	GLSlotMap[ FBO_COLOUR_ATTACHMENT2 ]	 = GL_COLOR_ATTACHMENT2_EXT;
	GLSlotMap[ FBO_COLOUR_ATTACHMENT3 ]	 = GL_COLOR_ATTACHMENT3_EXT;
	GLSlotMap[ FBO_COLOUR_ATTACHMENT4 ]	 = GL_COLOR_ATTACHMENT4_EXT;
	GLSlotMap[ FBO_COLOUR_ATTACHMENT5 ]	 = GL_COLOR_ATTACHMENT5_EXT;
	GLSlotMap[ FBO_COLOUR_ATTACHMENT6 ]	 = GL_COLOR_ATTACHMENT6_EXT;
	GLSlotMap[ FBO_COLOUR_ATTACHMENT7 ]	 = GL_COLOR_ATTACHMENT7_EXT;
	GLSlotMap[ FBO_COLOUR_ATTACHMENT8 ]	 = GL_COLOR_ATTACHMENT8_EXT;
	GLSlotMap[ FBO_COLOUR_ATTACHMENT9 ]	 = GL_COLOR_ATTACHMENT9_EXT;
	GLSlotMap[ FBO_COLOUR_ATTACHMENT10 ] = GL_COLOR_ATTACHMENT10_EXT;
	GLSlotMap[ FBO_COLOUR_ATTACHMENT11 ] = GL_COLOR_ATTACHMENT11_EXT;
	GLSlotMap[ FBO_COLOUR_ATTACHMENT12 ] = GL_COLOR_ATTACHMENT12_EXT;
	GLSlotMap[ FBO_COLOUR_ATTACHMENT13 ] = GL_COLOR_ATTACHMENT13_EXT;
	GLSlotMap[ FBO_COLOUR_ATTACHMENT14 ] = GL_COLOR_ATTACHMENT14_EXT;
	GLSlotMap[ FBO_COLOUR_ATTACHMENT15 ] = GL_COLOR_ATTACHMENT15_EXT;
	GLSlotMap[ FBO_DEPTH_ATTACHMENT ]	 = GL_DEPTH_ATTACHMENT_EXT;
	GLSlotMap[ FBO_STENCIL_ATTACHMENT ]	 = GL_STENCIL_ATTACHMENT_EXT;

	GLSlotMapIsInitialised = true;
}

//----------------------------------------------------------------------------------

GLenum FrameBufferObjects::GetSlotGL( Slot _input)
{
	if( !GLSlotMapIsInitialised )
		InitGLSlotMap();
	return GLSlotMap[ _input ];
}

//----------------------------------------------------------------------------------

void FrameBufferObjects::Attachment::Attach( Config *_inConfig, Slot _inSlot )
{

	m_isBound = true;
	m_currentSlot = _inSlot;
	m_currentConfig = _inConfig;

	Bind();
}

//----------------------------------------------------------------------------------

FrameBufferObjects::TexAttachment::TexAttachment( TexFormat _inFormat, unsigned int _texID ) : Attachment()
{
	m_texID = _texID;
	m_targetFormat = _inFormat;
}

//----------------------------------------------------------------------------------

FrameBufferObjects::TexAttachment::~TexAttachment()
{
	// Unbind the attachment
	Unbind();
}

//----------------------------------------------------------------------------------

void FrameBufferObjects::TexAttachment::Bind()
{
}

//----------------------------------------------------------------------------------

void FrameBufferObjects::TexAttachment::Unbind()
{
	// Unbind the attachment

//	if( is_bound )
//		glFramebufferTexture2DEXT();
	
	m_isBound = false;
	m_currentSlot = FBO_NULL_ATTACHMENT;
	m_currentConfig = NULL;
}

//----------------------------------------------------------------------------------

FrameBufferObjects::TexAttachment2D::TexAttachment2D( TexFormat _inFormat, unsigned int _texID) : TexAttachment( _inFormat, _texID )
{
}

//----------------------------------------------------------------------------------

void FrameBufferObjects::TexAttachment2D::Bind()
{
	if( m_texID > 0 )
	{
		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GetSlotGL( m_currentSlot), m_targetFormat, m_texID, 0 );
	}
}

//----------------------------------------------------------------------------------

void FrameBufferObjects::TexAttachment2D::Unbind()
{
	if( m_isBound )
		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GetSlotGL( m_currentSlot ), m_targetFormat, 0, 0 );

	m_isBound = false;
	m_currentSlot = FBO_NULL_ATTACHMENT;
	m_currentConfig = NULL;
}

//----------------------------------------------------------------------------------

FrameBufferObjects::TexAttachment3D::TexAttachment3D( unsigned int _texID ) : TexAttachment( FBO_TEXTURE_3D, _texID )
{
	m_zOffset = 0;
}

//----------------------------------------------------------------------------------

void FrameBufferObjects::TexAttachment3D::Bind()
{
	if( m_texID > 0 )
	{
		glFramebufferTexture3DEXT( GL_FRAMEBUFFER_EXT, GetSlotGL( m_currentSlot), m_targetFormat, m_texID, 0, m_zOffset );
	}
}

//----------------------------------------------------------------------------------

void FrameBufferObjects::TexAttachment3D::Unbind()
{
	if( m_isBound )
		glFramebufferTexture3DEXT( GL_FRAMEBUFFER_EXT, GetSlotGL( m_currentSlot ), m_targetFormat, 0, 0, m_zOffset );

	m_isBound = false;
	m_currentSlot = FBO_NULL_ATTACHMENT;
	m_currentConfig = NULL;
}

//----------------------------------------------------------------------------------

FrameBufferObjects::BufAttachment::BufAttachment( BufFormat _inFormat, unsigned int _sizeW, unsigned int _sizeH ) : Attachment()
{
	m_bufferID = 0;

	// Generate the actual buffer, storing our handle to it in the ID
	glGenRenderbuffersEXT( 1, &m_bufferID );

	// Bind the buffer
	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_bufferID );

	// Specify its format and dimensions
	glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, _inFormat, _sizeW, _sizeH );

	// Unbind it, as we will not use it yet
	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );
}

//----------------------------------------------------------------------------------

FrameBufferObjects::BufAttachment::~BufAttachment()
{
	// Unbind the attachment
	Unbind();

	if( m_bufferID > 0 )
		glDeleteRenderbuffersEXT( 1, &m_bufferID );
}

//----------------------------------------------------------------------------------

void FrameBufferObjects::BufAttachment::Bind()
{
	if( m_bufferID > 0 )
	{
		glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_bufferID );
		glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GetSlotGL( m_currentSlot ), GL_RENDERBUFFER_EXT, m_bufferID );
	}
}

//----------------------------------------------------------------------------------

void FrameBufferObjects::BufAttachment::Unbind()
{
	// Unbind the attachment

	if( m_isBound )
		glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GetSlotGL( m_currentSlot ), GL_RENDERBUFFER_EXT, 0 );

	m_isBound = false;
	m_currentSlot = FBO_NULL_ATTACHMENT;
	m_currentConfig = NULL;
}

//----------------------------------------------------------------------------------

FrameBufferObjects::Config::Config()
{ }

//----------------------------------------------------------------------------------

FrameBufferObjects::Config::~Config()
{ }

//----------------------------------------------------------------------------------

unsigned int FrameBufferObjects::Config::Bind( unsigned int _prevConfigBits )
{
	unsigned int currentConfigBits = 0;

	std::map< Slot, Attachment* >::iterator it = m_attachments.begin();

	while( it != m_attachments.end() )
	{
			// Bind the attachment to the point specified
		( *it ).second->Attach( this, ( *it ).first );

			// The slot type is a bitmask for the different positions
		currentConfigBits |= ( *it ).first;

		++it;
	}

	// The current_config now contains information on which slots are currently bound
	// Need to compare this to the previous configuration, to find any slots that were previously bound, but are no longer bound:

	for( unsigned int temp = 1; temp <= FBO_STENCIL_ATTACHMENT; temp = temp << 1 )
	{
		if( ( ~( temp & currentConfigBits ) ) & ( temp & _prevConfigBits ) )
		{
			glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );
			glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GetSlotGL( ( Slot )temp ), GL_RENDERBUFFER_EXT, 0 );
		}
	}

	return currentConfigBits;
}

//----------------------------------------------------------------------------------

void FrameBufferObjects::Config::Attach( Slot _inSlot, Attachment *_inAttachment )
{
	m_attachments[ _inSlot ] = _inAttachment;
}

//----------------------------------------------------------------------------------

FrameBufferObjects::FBO::FBO()
{
	m_prevConfigBits = 0;
	m_fboID = 0;
	glGenFramebuffersEXT( 1, &m_fboID );
}

//----------------------------------------------------------------------------------

FrameBufferObjects::FBO::~FBO()
{
	if( m_fboID != 0 )
	{
		// Unbind the frame buffer object
		Unbind();
		// Delete the frame buffer object
		glDeleteFramebuffersEXT( 1, &m_fboID );
	}
}

//----------------------------------------------------------------------------------

bool FrameBufferObjects::FBO::Check()
{
	bool ret = true;

	// First, check for any openGL errors
	GLenum err = glGetError();
	if( err != GL_NO_ERROR )
	{
		ret = false;
		std::cerr << " FrameBufferObjects::FBO::check(), OpenGL Error: " << gluErrorString( err ) << std::endl;
	}

	GLenum check = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
	switch( check )
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			// everything is okay
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			std::cerr << "FBO ERROR: incomplete" << std::endl;
			ret = false;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			std::cerr << "FBO ERROR: missing attachment" << std::endl;
			ret = false;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			std::cerr << "FBO ERROR: incorrect dimensions" << std::endl;
			ret = false;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			std::cerr << "FBO ERROR: incomplete formats" << std::endl;
			ret = false;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			std::cerr << "FBO ERROR: incomplete draw buffer" << std::endl;
			ret = false;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			std::cerr << "FBO ERROR: incomplete read buffer" << std::endl;
			ret = false;
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			std::cerr << "FBO ERROR: frame buffer format is not supported" << std::endl;
			ret = false;
			break;
		default:
			std::cerr << "FBO ERROR" << std::endl;
			ret = false;
			break;
	}

	return ret;
}

//----------------------------------------------------------------------------------

bool FrameBufferObjects::FBO::Bind( Config *_inConfig )
{

	if( _inConfig == NULL || m_fboID == 0 )
		return false;

	// Bind the FBO
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_fboID );

	// Bind the configuration, passing it in the previous configuration bits, storing the returned configuration for next time
	m_prevConfigBits = _inConfig->Bind( m_prevConfigBits );

	#ifdef _DEBUG
	return true;
	#else
	return true;
	#endif
}

//----------------------------------------------------------------------------------

void FrameBufferObjects::FBO::Unbind()
{
	// There is no unbind function, so have to bind zero
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

//----------------------------------------------------------------------------------

