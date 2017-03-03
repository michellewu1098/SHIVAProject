#include "VolumeRenderer/Camera.h"

//----------------------------------------------------------------------------------

Camera::Camera()
{
	m_viewMatrix.identity();
	m_projectionMatrix.identity();
	m_up.cardinal( 1 );
}

//----------------------------------------------------------------------------------

Camera::~Camera()
{}

//----------------------------------------------------------------------------------

void Camera::LookAt( cml::vector3f _eye, cml::vector3f _look )
{
	cml::matrix_look_at_RH( m_viewMatrix, _eye, _look, m_up );
}

//----------------------------------------------------------------------------------

void Camera::SetProjectionMatrix( float _fov, float _aspect, float _zNear, float _zFar )
{
	cml::matrix_perspective_yfov_RH( m_projectionMatrix, cml::rad( _fov ), _aspect, _zNear, _zFar, cml::z_clip_neg_one );
}

//----------------------------------------------------------------------------------

void Camera::Rotate( float _x, float _y, float _z )
{
	cml::matrix_rotate_about_local_x( m_viewMatrix, _x );
	cml::matrix_rotate_about_local_y( m_viewMatrix, _y );
	cml::matrix_rotate_about_local_z( m_viewMatrix, _z );
}

//----------------------------------------------------------------------------------

