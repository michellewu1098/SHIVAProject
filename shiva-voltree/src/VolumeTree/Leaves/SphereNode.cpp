#include "VolumeTree/Leaves/SphereNode.h"

//----------------------------------------------------------------------------------

VolumeTree::SphereNode::SphereNode()
{
	m_radiusX = m_radiusY = m_radiusZ = 1.0f;
}

//----------------------------------------------------------------------------------

VolumeTree::SphereNode::SphereNode( const float &_radiusX, const float &_radiusY, const float &_radiusZ )
{
	m_radiusX = _radiusX;
	m_radiusY = _radiusY;
	m_radiusZ = _radiusZ;
}

//----------------------------------------------------------------------------------

float VolumeTree::SphereNode::GetFunctionValue( float _x, float _y, float _z )
{

	return 1.0f - pow( _x / m_radiusX, 2.0f ) - pow( _y / m_radiusY, 2.0f ) - pow( _z / m_radiusZ, 2.0f );
/*
	float diffX = fabs(x-_centreX);
	float diffY = fabs(y-_centreY);
	float diffZ = fabs(z-_centreZ);
	return 1.0f - pow(diffX/_radius,2.0f) - pow(diffY/_radius,2.0f) - pow(diffZ/_radius,2.0f);
*/
/*	float diffX = fabs(x-_centreX);
	float diffY = fabs(y-_centreY);
	float diffZ = fabs(z-_centreZ);
	return (_radius*_radius) - (diffX*diffX) - (diffY*diffY) - (diffZ*diffZ);
*/
	//return _radius - sqrt( pow(x-_centreX,2) + pow(y-_centreY,2) + pow(z-_centreZ,2) );
}

//----------------------------------------------------------------------------------

std::string VolumeTree::SphereNode::GetFunctionGLSLString( bool _callCache, std::string _samplePosStr )
{
	std::stringstream functionString;
	functionString << "Sphere(" << _samplePosStr << ",vec3(" << m_radiusX << "," << m_radiusY << "," << m_radiusZ << "))";

	return functionString.str();
}

//----------------------------------------------------------------------------------

void VolumeTree::SphereNode::GetBounds( float *_minX, float *_maxX, float *_minY, float *_maxY, float *_minZ, float *_maxZ )
{
	*_minX = - m_radiusX;
	*_maxX =   m_radiusX;

	*_minY = - m_radiusY;
	*_maxY =   m_radiusY;

	*_minZ = - m_radiusZ;
	*_maxZ =   m_radiusZ;
	/*
	*minX = _centreX - _radius;
	*maxX = _centreX + _radius;
	
	*minY = _centreY - _radius;
	*maxY = _centreY + _radius;
	
	*minZ = _centreZ - _radius;
	*maxZ = _centreZ + _radius;
	*/
}

//----------------------------------------------------------------------------------
