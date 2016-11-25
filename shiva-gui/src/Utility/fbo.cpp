
#include <GL/GLee.h>
#include <GL/glu.h>

#include "Utility/fbo.h"

#include <iostream>
#include <map>


namespace FrameBufferObjects
{
		/// This map associates the slot enum with the actual openGL 'enum'
	std::map<slot,unsigned int> GL_slot_map;

		/// Populates the slot map with the slot-GLenum key pairs
	void GL_slot_map_init();

		/// Whether the slot map has been populated (i.e. GL_slot_map_init has been called)
	bool GL_slot_map_is_initialised = false;

		/// Utility function to convert an openGL slot definition to a slot definition
	GLenum get_slot_GL(slot);
}


void FrameBufferObjects::GL_slot_map_init()
{
		// Populate that horrible map:
	GL_slot_map[FBO_COLOUR_ATTACHMENT0]		= GL_COLOR_ATTACHMENT0_EXT;
	GL_slot_map[FBO_COLOUR_ATTACHMENT1]		= GL_COLOR_ATTACHMENT1_EXT;
	GL_slot_map[FBO_COLOUR_ATTACHMENT2]		= GL_COLOR_ATTACHMENT2_EXT;
	GL_slot_map[FBO_COLOUR_ATTACHMENT3]		= GL_COLOR_ATTACHMENT3_EXT;
	GL_slot_map[FBO_COLOUR_ATTACHMENT4]		= GL_COLOR_ATTACHMENT4_EXT;
	GL_slot_map[FBO_COLOUR_ATTACHMENT5]		= GL_COLOR_ATTACHMENT5_EXT;
	GL_slot_map[FBO_COLOUR_ATTACHMENT6]		= GL_COLOR_ATTACHMENT6_EXT;
	GL_slot_map[FBO_COLOUR_ATTACHMENT7]		= GL_COLOR_ATTACHMENT7_EXT;
	GL_slot_map[FBO_COLOUR_ATTACHMENT8]		= GL_COLOR_ATTACHMENT8_EXT;
	GL_slot_map[FBO_COLOUR_ATTACHMENT9]		= GL_COLOR_ATTACHMENT9_EXT;
	GL_slot_map[FBO_COLOUR_ATTACHMENT10]		= GL_COLOR_ATTACHMENT10_EXT;
	GL_slot_map[FBO_COLOUR_ATTACHMENT11]		= GL_COLOR_ATTACHMENT11_EXT;
	GL_slot_map[FBO_COLOUR_ATTACHMENT12]		= GL_COLOR_ATTACHMENT12_EXT;
	GL_slot_map[FBO_COLOUR_ATTACHMENT13]		= GL_COLOR_ATTACHMENT13_EXT;
	GL_slot_map[FBO_COLOUR_ATTACHMENT14]		= GL_COLOR_ATTACHMENT14_EXT;
	GL_slot_map[FBO_COLOUR_ATTACHMENT15]		= GL_COLOR_ATTACHMENT15_EXT;
	GL_slot_map[FBO_DEPTH_ATTACHMENT]		= GL_DEPTH_ATTACHMENT_EXT;
	GL_slot_map[FBO_STENCIL_ATTACHMENT]		= GL_STENCIL_ATTACHMENT_EXT;

	GL_slot_map_is_initialised = true;
}

GLenum FrameBufferObjects::get_slot_GL(slot input)
{
	if( !GL_slot_map_is_initialised )
		GL_slot_map_init();
	return GL_slot_map[input];
}



void FrameBufferObjects::attachment::attach(config *in_config, slot in_slot)
{

	is_bound = true;
	current_slot = in_slot;
	current_config = in_config;

	bind();

/*
	if( !is_bound )
	{
		current_slot = in_slot;
		current_config = in_config;
		// Bind the attachment
		bind();
	}
	else // Attachment is currently bound to a slot
	{
			// If the attachment isn't currently bound to the right slot
		if( current_slot != in_slot )
		{
				// If the binding was from a previous configuration
			if( current_config != in_config )
			{
				// Unbind from old slot, bind to new one
				bind();
			}
			else
			{
				// The attachment has been asked to bind itself to a different slot from within the same configuration
				// This is bad, and means the configuration hasn't been set up correctly

				// Retrieve reference to logfile
				nLog::cLog &log=nLog::getLog();

				log<<"FBO ERROR: configuration: attachment is being asked to bind itself to multiple slots"<<std::endl;

				// We are treating this as an error, and will unbind the attachment
				unbind();
				return;
			}
		}
		// else do nothing - the attachment is already bound to the right place
	}
	is_bound = true;
	current_slot = in_slot;
	current_config = in_config;
*/
}












FrameBufferObjects::tex_attachment::tex_attachment(tex_format format_in,unsigned int texture_ID) : attachment()
{
	tex_ID = texture_ID;

	target_format = format_in;
}



FrameBufferObjects::tex_attachment::~tex_attachment()
{
	// Unbind the attachment
	unbind();
}




void FrameBufferObjects::tex_attachment::bind()
{
}

void FrameBufferObjects::tex_attachment::unbind()
{
	// Unbind the attachment

//	if( is_bound )
//		glFramebufferTexture2DEXT();


	is_bound = false;
	current_slot = FBO_NULL_ATTACHMENT;
	current_config = NULL;
}




FrameBufferObjects::tex_attachment_2D::tex_attachment_2D(tex_format format_in,unsigned int texture_ID) : tex_attachment(format_in,texture_ID)
{
}

void FrameBufferObjects::tex_attachment_2D::bind()
{
	if( tex_ID > 0 )
	{
//		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, get_slot_GL(current_slot), target_format, 0, 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, get_slot_GL(current_slot), target_format, tex_ID, 0);
	}
}

void FrameBufferObjects::tex_attachment_2D::unbind()
{
	if( is_bound )
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, get_slot_GL(current_slot), target_format, 0, 0);

	is_bound = false;
	current_slot = FBO_NULL_ATTACHMENT;
	current_config = NULL;
}





FrameBufferObjects::tex_attachment_3D::tex_attachment_3D(unsigned int texture_ID) : tex_attachment(FBO_TEXTURE_3D,texture_ID)
{
	zoffset = 0;
}

void FrameBufferObjects::tex_attachment_3D::bind()
{
	if( tex_ID > 0 )
	{
//		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, get_slot_GL(current_slot), target_format, 0, 0);
		glFramebufferTexture3DEXT(GL_FRAMEBUFFER_EXT, get_slot_GL(current_slot), target_format, tex_ID, 0,zoffset);

/*
		GLenum err = glGetError();
		if(err != GL_NO_ERROR)
		{
			std::cerr<<" GL Error in tex_attachment_3D::bind(), after glFramebufferTexture3DEXT: "<< gluErrorString(err)<<std::endl;
		}
*/
	}
}

void FrameBufferObjects::tex_attachment_3D::unbind()
{
	if( is_bound )
		glFramebufferTexture3DEXT(GL_FRAMEBUFFER_EXT, get_slot_GL(current_slot), target_format, 0, 0,zoffset);

	is_bound = false;
	current_slot = FBO_NULL_ATTACHMENT;
	current_config = NULL;
}








FrameBufferObjects::buf_attachment::buf_attachment( buf_format in_format, unsigned int size_w, unsigned int size_h ) : attachment()
{
	buffer_ID = 0;

		// Generate the actual buffer, storing our handle to it in the ID
	glGenRenderbuffersEXT(1,&buffer_ID);

		// Bind the buffer
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, buffer_ID);

		// Specify its format and dimensions
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,in_format,size_w,size_h);

		// Unbind it, as we will not use it yet
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
}

FrameBufferObjects::buf_attachment::~buf_attachment()
{
	// Unbind the attachment
	unbind();

	if( buffer_ID > 0 )
		glDeleteRenderbuffersEXT(1,&buffer_ID);
}

void FrameBufferObjects::buf_attachment::bind()
{
	if( buffer_ID > 0 )
	{
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, buffer_ID);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,get_slot_GL(current_slot),GL_RENDERBUFFER_EXT,buffer_ID);
	}
}

void FrameBufferObjects::buf_attachment::unbind()
{
	// Unbind the attachment

	if( is_bound )
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,get_slot_GL(current_slot),GL_RENDERBUFFER_EXT,0);

	is_bound = false;
	current_slot = FBO_NULL_ATTACHMENT;
	current_config = NULL;
}







FrameBufferObjects::config::config()
{

}


FrameBufferObjects::config::~config()
{

}


unsigned int FrameBufferObjects::config::bind(unsigned int previous_config_bits)
{
	unsigned int current_config_bits = 0;

	std::map<slot,attachment*>::iterator it = attachments.begin();

	while( it != attachments.end() )
	{
			// Bind the attachment to the point specified
		(*it).second->attach(this,(*it).first);

			// The slot type is a bitmask for the different positions
		current_config_bits |= (*it).first;

		++it;
	}

	// The current_config now contains information on which slots are currently bound
	// Need to compare this to the previous configuration, to find any slots that were previously bound, but are no longer bound:

	for( unsigned int temp = 1; temp <= FBO_STENCIL_ATTACHMENT; temp = temp << 1 )
	{
		if( (~(temp & current_config_bits)) & (temp & previous_config_bits) )
		{
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,get_slot_GL((slot)temp),GL_RENDERBUFFER_EXT,0);
		}
	}

	return current_config_bits;
}

void FrameBufferObjects::config::attach(slot in_slot, attachment *in_attachment)
{

	attachments[in_slot] = in_attachment;

}






FrameBufferObjects::FBO::FBO()
{
	previous_config_bits = 0;
	FBO_ID = 0;
	glGenFramebuffersEXT(1,&FBO_ID);


}

FrameBufferObjects::FBO::~FBO()
{
	if( FBO_ID != 0 )
	{
			// Unbind the frame buffer object
		unbind();
			// Delete the frame buffer object
		glDeleteFramebuffersEXT(1,&FBO_ID);
	}
}

bool FrameBufferObjects::FBO::check()
{
	bool ret = true;

	// retrieve reference to logfile
//	nLog::cLog &log=nLog::getLog();

	// First, check for any openGL errors
	GLenum err = glGetError();
	if( err != GL_NO_ERROR)
	{
		ret = false;
		std::cerr<<" FrameBufferObjects::FBO::check(), OpenGL Error: "<< gluErrorString(err)<<std::endl;
//		log<<" FrameBufferObjects::FBO::check(), OpenGL Error: "<< gluErrorString(err)<<std::endl;
	}

	GLenum check = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(check)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			// everything is okay
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			std::cerr<<"FBO ERROR: incomplete"<<std::endl;
//			log<<"FBO ERROR: incomplete"<<std::endl;
			ret = false;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			std::cerr<<"FBO ERROR: missing attachment"<<std::endl;
//			log<<"FBO ERROR: missing attachment"<<std::endl;
			ret = false;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			std::cerr<<"FBO ERROR: incorrect dimensions"<<std::endl;
//			log<<"FBO ERROR: incorrect dimensions"<<std::endl;
			ret = false;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			std::cerr<<"FBO ERROR: incomplete formats"<<std::endl;
//			log<<"FBO ERROR: incomplete formats"<<std::endl;
			ret = false;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			std::cerr<<"FBO ERROR: incomplete draw buffer"<<std::endl;
//			log<<"FBO ERROR: incomplete draw buffer"<<std::endl;
			ret = false;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			std::cerr<<"FBO ERROR: incomplete read buffer"<<std::endl;
//			log<<"FBO ERROR: incomplete read buffer"<<std::endl;
			ret = false;
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			std::cerr<<"FBO ERROR: frame buffer format is not supported"<<std::endl;
//			log<<"FBO ERROR: frame buffer format is not supported"<<std::endl;
			ret = false;
			break;
		default:
			std::cerr<<"FBO ERROR"<<std::endl;
//			log<<"FBO ERROR"<<std::endl;
			ret = false;
			break;
	}

	return ret;
}

bool FrameBufferObjects::FBO::bind(config *in_config)
{

	if( in_config == NULL || FBO_ID == 0 )
		return false;

		// Bind the FBO
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,FBO_ID);

		// Bind the configuration, passing it in the previous configuration bits, storing the returned configuration for next time
	previous_config_bits = in_config->bind(previous_config_bits);

	#ifdef _DEBUG
	return true;
//	return check();
	#else
	return true;
	#endif
}

void FrameBufferObjects::FBO::unbind()
{
		// There is no unbind function, so have to bind zero
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
}


