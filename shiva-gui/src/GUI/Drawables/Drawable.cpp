#include "GUI/Drawables/Drawable.h"

//----------------------------------------------------------------------------------

ShivaGUI::Drawable::Drawable()
{
	m_boundsLeft = m_boundsRight = m_boundsTop = m_boundsBottom = 0.0f;
	m_projMat.identity();
	m_mvMat.identity();
}

//----------------------------------------------------------------------------------

ShivaGUI::Drawable::~Drawable()
{ }

//----------------------------------------------------------------------------------

void ShivaGUI::Drawable::SetBounds( const float &_left, const float &_top, const float &_right, const float &_bottom, const unsigned int &_gravity )
{
	m_boundsLeft = _left;
	m_boundsRight = _right;
	m_boundsTop = _top;
	m_boundsBottom = _bottom;

	OnSetBounds( _left, _top, _right, _bottom, _gravity );
}

//----------------------------------------------------------------------------------

void ShivaGUI::Drawable::GetContentBounds( float &_left, float &_top, float &_right, float &_bottom )
{
	_left = m_boundsLeft;
	_top = m_boundsTop;
	_right = m_boundsRight;
	_bottom = m_boundsBottom;
}

//----------------------------------------------------------------------------------

void ShivaGUI::Drawable::LoadMatricesToShader( GLuint _shaderID, cml::matrix44f_c _proj, cml::matrix44f_c _mv )
{
	// ModelView matrix
	GLint mvLoc = glGetUniformLocation( _shaderID, "u_ModelViewMatrix" );
	if( mvLoc != -1 ) { glUniformMatrix4fv( mvLoc, 1, GL_FALSE, _mv.data() ); }
	else { std::cerr << "u_ModelViewMatrix not found in shader " << _shaderID << std::endl; }


	// Projection matrix	
	GLint pLoc = glGetUniformLocation( _shaderID, "u_ProjectionMatrix" );
	if( pLoc != -1 ) { glUniformMatrix4fv( pLoc, 1, GL_FALSE, _proj.data() ); }
	else { std::cerr << "u_ProjectionMatrix not found in shader " << _shaderID << std::endl; }
}

//----------------------------------------------------------------------------------