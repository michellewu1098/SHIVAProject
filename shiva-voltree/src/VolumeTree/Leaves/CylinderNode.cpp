#include "VolumeTree/Leaves/CylinderNode.h"

//----------------------------------------------------------------------------------

VolumeTree::CylinderNode::CylinderNode()
{
	m_radiusX = m_radiusY = 1.0f;
	m_length = 1.0f;
	m_isPole = false;
}

//----------------------------------------------------------------------------------

VolumeTree::CylinderNode::CylinderNode( const float &_length, const float &_radiusX, const float &_radiusY )
{
	m_length = _length;
	m_radiusX = _radiusX;
	m_radiusY = _radiusY;
	m_isPole = false;
}

//----------------------------------------------------------------------------------

float VolumeTree::CylinderNode::GetFunctionValue( float _x, float _y, float _z )
{
	//float value = (_radius*_radius) - (x*x) - (y*y);
	float value = 1.0f - pow( _x / m_radiusX, 2.0f ) - pow( _y / m_radiusY, 2.0f );

	float lowerZ = -m_length * 0.5f;
	float upperZ = m_length * 0.5f;

	//value = std::min(value,z - lowerZ);
	//value = std::min(value,upperZ - z);

	// R-intersection: f1+f2-sqrt(f1^2+f2^2)
	float f1 = value;
	float f2 = upperZ - _z;
	value = f1 + f2 - sqrt( pow( f1, 2 ) + pow( f2, 2 ) );

	f1 = value;
	f2 = _z - lowerZ;
	value = f1 + f2 - sqrt( pow( f1, 2 ) + pow( f2, 2 ) );

	return value;
}

//----------------------------------------------------------------------------------

std::string VolumeTree::CylinderNode::GetFunctionGLSLString( bool _callCache, std::string _samplePosStr )
{
	std::stringstream functionString;
	functionString << "Cylinder(" << _samplePosStr << "," << m_length << "," << m_radiusX << "," << m_radiusY << ")";

	return functionString.str();
}

//----------------------------------------------------------------------------------

void VolumeTree::CylinderNode::GetBounds( float *_minX, float *_maxX, float *_minY, float *_maxY, float *_minZ, float *_maxZ )
{
	*_minX = -m_radiusX;
	*_maxX = m_radiusX;
	
	*_minY = -m_radiusY;
	*_maxY = m_radiusY;
	
	*_minZ = -( m_length * 0.5f );
	*_maxZ = m_length * 0.5f;
}

//----------------------------------------------------------------------------------