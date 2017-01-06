
#include "VolumeTree/Leaves/CubeNode.h"
#include <sstream>
#include <cmath>
#include <algorithm>

//----------------------------------------------------------------------------------

VolumeTree::CubeNode::CubeNode()
{
	_lengthX = _lengthY = _lengthZ = 1.0f;
}

//----------------------------------------------------------------------------------

VolumeTree::CubeNode::CubeNode( float length )
{
	_lengthX = _lengthY = _lengthZ = length;
}

//----------------------------------------------------------------------------------

VolumeTree::CubeNode::CubeNode( float lengthX, float lengthY, float lengthZ )
{
	_lengthX = lengthX;
	_lengthY = lengthY;
	_lengthZ = lengthZ;
}

//----------------------------------------------------------------------------------

float VolumeTree::CubeNode::GetFunctionValue( float x, float y, float z )
{
	float lowerX = -_lengthX * 0.5;
	float upperX = _lengthX * 0.5;
	float lowerY = -_lengthX * 0.5;
	float upperY = _lengthX * 0.5;
	float lowerZ = -_lengthX * 0.5;
	float upperZ = _lengthX * 0.5;
	
	float value = CSG_Intersect( upperZ - z, z - lowerZ );
	value = CSG_Intersect( value, upperY - y );
	value = CSG_Intersect( value, y - lowerY );
	value = CSG_Intersect( value, upperX - x );
	value = CSG_Intersect( value, x - lowerX );
	
	return value;
}

//----------------------------------------------------------------------------------

float VolumeTree::CubeNode::CSG_Intersect( float f1, float f2 )
{
	return f1 + f2 - sqrt( pow( f1, 2 ) + pow( f2, 2 ) );
}

//----------------------------------------------------------------------------------

std::string VolumeTree::CubeNode::GetFunctionGLSLString( bool callCache, std::string samplePosStr )
{
	std::stringstream functionString;
	functionString << "Cube(" << samplePosStr << ",vec3(" << _lengthX << "," << _lengthY << "," << _lengthZ << "))";

	return functionString.str();
}

//----------------------------------------------------------------------------------

void VolumeTree::CubeNode::GetBounds( float *minX, float *maxX, float *minY, float *maxY, float *minZ, float *maxZ )
{
	*minX = -_lengthX * 0.5f;
	*maxX = _lengthX * 0.5f;
	
	*minY = - _lengthY * 0.5f;
	*maxY = _lengthY * 0.5f;
	 
	*minZ = -_lengthZ * 0.5f;
	*maxZ = _lengthZ * 0.5f;
}

//----------------------------------------------------------------------------------
