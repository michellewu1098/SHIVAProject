#include "VolumeTree/Leaves/CubeNode.h"

//----------------------------------------------------------------------------------

VolumeTree::CubeNode::CubeNode()
{
	m_lengthX = m_lengthY = m_lengthZ = 1.0f;
}

//----------------------------------------------------------------------------------

VolumeTree::CubeNode::CubeNode( const float &_length )
{
	m_lengthX = m_lengthY = m_lengthZ = _length;
}

//----------------------------------------------------------------------------------

VolumeTree::CubeNode::CubeNode( const float &_lengthX, const float &_lengthY, const float &_lengthZ )
{
	m_lengthX = _lengthX;
	m_lengthY = _lengthY;
	m_lengthZ = _lengthZ;
}

//----------------------------------------------------------------------------------

float VolumeTree::CubeNode::GetFunctionValue( float _x, float _y, float _z )
{
	float lowerX = -m_lengthX * 0.5f;
	float upperX = m_lengthX * 0.5f;
	float lowerY = -m_lengthX * 0.5f;
	float upperY = m_lengthX * 0.5f;
	float lowerZ = -m_lengthX * 0.5f;
	float upperZ = m_lengthX * 0.5f;
	
	float value = CSG_Intersect( upperZ - _z, _z - lowerZ );
	value = CSG_Intersect( value, upperY - _y );
	value = CSG_Intersect( value, _y - lowerY );
	value = CSG_Intersect( value, upperX - _x );
	value = CSG_Intersect( value, _x - lowerX );
	
	return value;
}

//----------------------------------------------------------------------------------

float VolumeTree::CubeNode::CSG_Intersect( float _f1, float _f2 )
{
	return _f1 + _f2 - sqrt( pow( _f1, 2 ) + pow( _f2, 2 ) );
}

//----------------------------------------------------------------------------------

std::string VolumeTree::CubeNode::GetFunctionGLSLString( bool _callCache, std::string _samplePosStr )
{
	std::stringstream functionString;
	functionString << "Cube(" << _samplePosStr << ",vec3(" << m_lengthX << "," << m_lengthY << "," << m_lengthZ << "))";

	return functionString.str();
}

//----------------------------------------------------------------------------------

void VolumeTree::CubeNode::GetBounds( float *_minX, float *_maxX, float *_minY, float *_maxY, float *_minZ, float *_maxZ )
{
	*_minX = -m_lengthX * 0.5f;
	*_maxX = m_lengthX * 0.5f;
	
	*_minY = -m_lengthY * 0.5f;
	*_maxY = m_lengthY * 0.5f;
	 
	*_minZ = -m_lengthZ * 0.5f;
	*_maxZ = m_lengthZ * 0.5f;
}

//----------------------------------------------------------------------------------
