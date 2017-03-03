#include "VolumeTree/Leaves/ConeNode.h"

//----------------------------------------------------------------------------------

VolumeTree::ConeNode::ConeNode()
{
	m_radius = 1.0f;
	m_length = 1.0f;
}

//----------------------------------------------------------------------------------

VolumeTree::ConeNode::ConeNode( const float &_length, const float &_radius )
{
	m_length = _length;
	m_radius = _radius;
}

//----------------------------------------------------------------------------------

float VolumeTree::ConeNode::GetFunctionValue( float _x, float _y, float _z )
{
	// From Hyperfun: (z-z0)^2-((x-x0)/R)^2-((y-y0)/R)^2
	// (z)^2-((x)/R)^2-((y)/R)^2

	float lowerZ = -m_length * 0.5f;
	float upperZ = m_length * 0.5f;

	float radius = m_radius * ( 1.0f / m_length );
	float value = pow( _z - upperZ, 2 ) - pow( _x / radius, 2 ) - pow( _y / radius, 2 );

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

std::string VolumeTree::ConeNode::GetFunctionGLSLString( bool _callCache, std::string _samplePosStr)
{
	std::stringstream functionString;
	functionString << "Cone(" << _samplePosStr << "," << m_length << "," << m_radius << ")";

	return functionString.str();
}

//----------------------------------------------------------------------------------

void VolumeTree::ConeNode::GetBounds( float *_minX, float *_maxX, float *_minY, float *_maxY, float *_minZ, float *_maxZ )
{
	*_minX = -m_radius;
	*_maxX = m_radius;
	
	*_minY = -m_radius;
	*_maxY = m_radius;
	
	*_minZ = -( m_length * 0.5f );
	*_maxZ = m_length * 0.5f;
}

//----------------------------------------------------------------------------------