//------------------------------------------------------------------------------------
/// @file Camera.h
/// @brief Very basic camera class
/// @author Michelle Wu
/// @version 1.0
//------------------------------------------------------------------------------------

#ifndef CAMERA_H_
#define CAMERA_H_

#include "VolumeRenderer/SpringyVec3.h"

class Camera : public SpringyVec3
{
public:

	//----------------------------------------------------------------------------------
	/// \brief Camera ctor
	//----------------------------------------------------------------------------------
	Camera();
	//----------------------------------------------------------------------------------
	/// \brief Dtor
	//----------------------------------------------------------------------------------
	~Camera();
	//----------------------------------------------------------------------------------
	/// \brief Set the projection matrix
	/// \param [in] _fov Field of view
	/// \param [in] _aspect Aspect ratio (width/height)
	/// \param [in] _zNear Near plane
	/// \param [in] _zFar Far plane
	//----------------------------------------------------------------------------------
	void SetProjectionMatrix( float _fov, float _aspect, float _zNear, float _zFar );
	void SetProjectionMatrixX( float _fov, float _aspect, float _zNear, float _zFar );
	//----------------------------------------------------------------------------------
	/// \brief Returns view matrix
	/// \return m_viewMatrix;
	//----------------------------------------------------------------------------------
	cml::matrix44f_c GetViewMatrix() { return m_viewMatrix; } 
	//----------------------------------------------------------------------------------
	/// \brief Returns projection matrix
	/// \return m_projectionMatrix
	//----------------------------------------------------------------------------------
	cml::matrix44f_c GetProjectionMatrix() { return m_projectionMatrix; }
	//----------------------------------------------------------------------------------
	/// \brief Construct the view matrix base on position of camera and target
	/// \param [in] _eye Camera current position
	/// \param [in] _look Target current position
	//----------------------------------------------------------------------------------
	void LookAt( cml::vector3f _eye, cml::vector3f _look );
	//----------------------------------------------------------------------------------
	/// \brief Rotate camera
	/// \param [in] _x Rotation about x-axis
	/// \param [in] _y Rotation about y-axis
	/// \param [in] _z Rotation about z-axis
	//----------------------------------------------------------------------------------
	void Rotate( float _x, float _y, float _z );
	//----------------------------------------------------------------------------------
	
private:
	
	//----------------------------------------------------------------------------------
	/// \brief View matrix
	//----------------------------------------------------------------------------------
	cml::matrix44f_c m_viewMatrix;
	//----------------------------------------------------------------------------------
	/// \brief Projection matrix
	//----------------------------------------------------------------------------------
	cml::matrix44f_c m_projectionMatrix;
	//----------------------------------------------------------------------------------
	/// \brief Indication of which way is up
	//----------------------------------------------------------------------------------
	cml::vector3f m_up;
	//----------------------------------------------------------------------------------

};

#endif