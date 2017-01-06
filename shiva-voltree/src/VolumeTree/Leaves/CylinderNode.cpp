#include "VolumeTree/Leaves/CylinderNode.h"

//----------------------------------------------------------------------------------

VolumeTree::CylinderNode::CylinderNode()
{
	_radiusX = _radiusY = 1.0f;
	_length = 1.0f;
}

//----------------------------------------------------------------------------------

VolumeTree::CylinderNode::CylinderNode( float length, float radiusX, float radiusY )
{
	_length = length;
	_radiusX = radiusX;
	_radiusY = radiusY;
}

//----------------------------------------------------------------------------------

float VolumeTree::CylinderNode::GetFunctionValue( float x, float y, float z )
{
	//float value = (_radius*_radius) - (x*x) - (y*y);
	float value = 1.0f - pow( x / _radiusX, 2.0f ) - pow( y / _radiusY, 2.0f );

	float lowerZ = -_length * 0.5f;
	float upperZ = _length * 0.5f;

	//value = std::min(value,z - lowerZ);
	//value = std::min(value,upperZ - z);

	// R-intersection: f1+f2-sqrt(f1^2+f2^2)
	float f1 = value;
	float f2 = upperZ - z;
	value = f1 + f2 - sqrt( pow( f1, 2 ) + pow( f2, 2 ) );

	f1 = value;
	f2 = z - lowerZ;
	value = f1 + f2 - sqrt( pow( f1, 2 ) + pow( f2, 2 ) );

	return value;
}

//----------------------------------------------------------------------------------

std::string VolumeTree::CylinderNode::GetFunctionGLSLString( bool callCache, std::string samplePosStr )
{
	std::stringstream functionString;
	functionString << "Cylinder(" << samplePosStr << "," << _length << "," << _radiusX << "," << _radiusY << ")";

	return functionString.str();
}

//----------------------------------------------------------------------------------

void VolumeTree::CylinderNode::GetBounds( float *minX, float *maxX, float *minY,float *maxY, float *minZ, float *maxZ )
{
	*minX = - _radiusX;
	*maxX = _radiusX;
	
	*minY = - _radiusY;
	*maxY = _radiusY;
	
	*minZ = -( _length * 0.5f );
	*maxZ = _length * 0.5f;
}

//----------------------------------------------------------------------------------