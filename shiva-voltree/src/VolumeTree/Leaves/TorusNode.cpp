#include "VolumeTree/Leaves/TorusNode.h"

//----------------------------------------------------------------------------------

VolumeTree::TorusNode::TorusNode()
{
	m_circleRadius = 1.0f;
	m_sweepRadius = 1.0f;
}

//----------------------------------------------------------------------------------

VolumeTree::TorusNode::TorusNode( const float &_sweepRadius, const float &_circleRadius )
{
	m_circleRadius = _circleRadius;
	m_sweepRadius = _sweepRadius;
}

//----------------------------------------------------------------------------------

float VolumeTree::TorusNode::GetFunctionValue( float _x, float _y, float _z )
{
	return ( m_circleRadius * m_circleRadius ) - ( _x * _x ) - ( _y * _y ) - ( _z * _z ) - ( m_sweepRadius * m_sweepRadius ) + 2.0f * m_sweepRadius * sqrt( ( _x * _x ) + ( _z * _z ) );

	// 'normalised' version
	//return m_circleRadius - sqrt( ( _x * _x ) + ( _y * _y ) + ( _z * _z ) + ( m_sweepRadius * m_sweepRadius ) - 2.0f * m_sweepRadius * sqrt( ( _x * _x ) + ( _z * _z ) ) );
}

//----------------------------------------------------------------------------------

std::string VolumeTree::TorusNode::GetFunctionGLSLString( bool _callCache, std::string _samplePosStr )
{
	std::stringstream functionString;
	functionString << "Torus(" << _samplePosStr << "," << m_circleRadius << "," << m_sweepRadius << ")";

	return functionString.str();
}

//----------------------------------------------------------------------------------

void VolumeTree::TorusNode::GetBounds( float *_minX, float *_maxX, float *_minY, float *_maxY, float *_minZ, float *_maxZ )
{
	*_minX = -( m_sweepRadius + m_circleRadius );
	*_maxX = ( m_sweepRadius + m_circleRadius );
	
	*_minY = -( m_sweepRadius + m_circleRadius );
	*_maxY = ( m_sweepRadius + m_circleRadius );

	*_minZ = -m_circleRadius;
	*_maxZ = m_circleRadius;
}

//----------------------------------------------------------------------------------