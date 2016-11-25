
/*
 * ConeNode.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: leigh
 */

#include "VolumeTree/Leaves/ConeNode.h"
#include <sstream>
#include <cmath>
#include <algorithm>

VolumeTree::ConeNode::ConeNode()
{
	_radius = 1.0f;
	_length = 1.0f;
}

VolumeTree::ConeNode::ConeNode(float length, float radius)
{
	_length = length;
	_radius = radius;
}

float VolumeTree::ConeNode::GetFunctionValue(float x, float y, float z)
{
	// From Hyperfun: (z-z0)^2-((x-x0)/R)^2-((y-y0)/R)^2
	// (z)^2-((x)/R)^2-((y)/R)^2

	float lowerZ = -_length*0.5f;
	float upperZ = _length*0.5f;

	float radius = _radius * (1.0f/_length);
	float value = pow(z-upperZ,2) - pow(x/radius,2) - pow(y/radius,2);

	// R-intersection: f1+f2-sqrt(f1^2+f2^2)
	float f1 = value;
	float f2 = upperZ-z;
	value = f1+f2-sqrt( pow(f1,2)+pow(f2,2));

	f1 = value;
	f2 = z-lowerZ;
	value = f1+f2-sqrt( pow(f1,2)+pow(f2,2));

	return value;
}

std::string VolumeTree::ConeNode::GetFunctionGLSLString(bool callCache, std::string samplePosStr)
{
	std::stringstream functionString;
	functionString<<"Cone("<<samplePosStr<<","<< _length <<","<<_radius<<")";

	return functionString.str();
}

void VolumeTree::ConeNode::GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ)
{
	*minX = - _radius;
	*maxX = _radius;
	
	*minY = - _radius;
	*maxY = _radius;
	
	*minZ = -(_length*0.5f);
	*maxZ = _length*0.5f;
}
