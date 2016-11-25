#include "GLVolumeTexture.h"

#include <GL/glu.h>
#include <iostream>

GLVolumeTexture::GLVolumeTexture() : m_texId(-1) {}


bool GLVolumeTexture::update(const sdf::grid& i_volume)
{
	GLenum err = glGetError();
	if( err != GL_NO_ERROR)
	{
		std::cerr<<"ERROR: GLVolumeTexture, OpenGL Error before loading volume: "<< gluErrorString(err)<<std::endl;
	}


	if (m_texId!=(GLuint)(-1))
	{
		std::cout<<"INFO: deleting existing volume OpenGL texture"<<std::endl;
		//glBindTexture(GL_TEXTURE_3D, m_texId);
		glBindTexture(GL_TEXTURE_3D, 0);
		glDeleteTextures(1, &m_texId);
		//std::cout<<"value of m_texId is now: "<<m_texId<<std::endl;
		m_texId = -1;
		//glBindTexture(GL_TEXTURE_3D, 0);
	}

	m_width = i_volume.width();
	m_height = i_volume.height();
	m_depth = i_volume.depth();

	std::cout<<"INFO: volume dimensions: "<<m_width<<"x"<<m_height<<"x"<<m_depth<<std::endl;
	//std::cout<<"INFO: value: "<<i_volume.raw_data()[m_width]<<std::endl;

	glGenTextures(1, &m_texId);
	glBindTexture(GL_TEXTURE_3D, m_texId);

	std::cout<<"INFO: volume loading into OpenGL Texture ID: "<<m_texId<<std::endl;

	if( m_texId==(GLuint)(-1) )
	{
		std::cerr<<"WARNING: OpenGL could not generate new texture ID for volume"<<std::endl;
		return false;
	}

	// Clamp to edges
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// Trilinear filtering
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/*
	int numValues = m_width*m_height*m_depth;
	float *testValues = new float[numValues];
	for( int i = 0; i < numValues; ++i )
	{
		testValues[i] = (float)i/(float)numValues;
	}
	*/

	glTexImage3D(GL_TEXTURE_3D, 0, GL_ALPHA32F_ARB, m_depth, m_height, m_width, 0, GL_ALPHA, GL_FLOAT,(const char*)i_volume.raw_data());

	//delete [] testValues;

	unbind();

	err = glGetError();
	if( err != GL_NO_ERROR)
	{
		std::cerr<<"ERROR: GLVolumeTexture, OpenGL Error after loading volume: "<< gluErrorString(err)<<std::endl;
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
		//glBindTexture(GL_TEXTURE_3D, m_texId);
		glDeleteTextures(1, &m_texId);
		glBindTexture(GL_TEXTURE_3D, 0);
	}
}
