/*

#include "GLVolumeTexture.h"

#include <GL/glu.h>
#include <iostream>

GLVolumeTexture::GLVolumeTexture() : m_texId(-1) {}


bool GLVolumeTexture::update(const sdf::grid& i_volume)
{
	GLenum err = glGetError();
	if( err != GL_NO_ERROR)
	{
		std::cerr<<" GLVolumeTexture, OpenGL Error before loading volume: "<< gluErrorString(err)<<std::endl;
	}


	if (m_texId!=(GLuint)(-1))
	{
			glBindTexture(GL_TEXTURE_3D, m_texId);
			glDeleteTextures(1, &m_texId);
			glBindTexture(GL_TEXTURE_3D, 0);
	}

	m_width = i_volume.width();
	m_height = i_volume.height();
	m_depth = i_volume.depth();

	glGenTextures(1, &m_texId);
	glBindTexture(GL_TEXTURE_3D, m_texId);

	// Clamp to edges
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// Trilinear filtering
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_ALPHA32F_ARB, m_depth, m_height, m_width, 0, GL_ALPHA, GL_FLOAT,(const char*)i_volume.raw_data());

	unbind();

	err = glGetError();
	if( err != GL_NO_ERROR)
	{
		std::cerr<<" GLVolumeTexture, OpenGL Error after loading volume: "<< gluErrorString(err)<<std::endl;
	}

	return true;
}


void GLVolumeTexture::bind()
{
	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, m_texId);
}


void GLVolumeTexture::unbind()
{
	glDisable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);
}

bool GLVolumeTexture::valid()
{
	return m_texId!=(GLuint)-1;
}

GLVolumeTexture::~GLVolumeTexture()
{
	if (m_texId!=(GLuint)(-1))
	{
		glBindTexture(GL_TEXTURE_3D, m_texId);
		glDeleteTextures(1, &m_texId);
		glBindTexture(GL_TEXTURE_3D, 0);
	}
}

*/