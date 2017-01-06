#include "VolumeTree/Leaves/SphereNode.h"
#include <sstream>
#include <cmath>

//----------------------------------------------------------------------------------

VolumeTree::SphereNode::SphereNode()
{
	_radiusX = _radiusY = _radiusZ = 1.0f;
}

//----------------------------------------------------------------------------------

VolumeTree::SphereNode::SphereNode( float radius_x, float radius_y, float radius_z )
{
	_radiusX = radius_x;
	_radiusY = radius_y;
	_radiusZ = radius_z;
}

//----------------------------------------------------------------------------------

float VolumeTree::SphereNode::GetFunctionValue( float x, float y, float z )
{

	return 1.0f - pow( x / _radiusX, 2.0f ) - pow( y / _radiusY, 2.0f ) - pow( z / _radiusZ, 2.0f );
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

std::string VolumeTree::SphereNode::GetFunctionGLSLString( bool callCache, std::string samplePosStr )
{
	std::stringstream functionString;
	functionString << "Sphere(" << samplePosStr << ",vec3(" << _radiusX << "," << _radiusY << "," << _radiusZ << "))";

	return functionString.str();
}

//----------------------------------------------------------------------------------

void VolumeTree::SphereNode::GetBounds( float *minX, float *maxX, float *minY, float *maxY, float *minZ, float *maxZ )
{
	*minX = - _radiusX;
	*maxX =   _radiusX;

	*minY = - _radiusY;
	*maxY =   _radiusY;

	*minZ = - _radiusZ;
	*maxZ =   _radiusZ;
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
