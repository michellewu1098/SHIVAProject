
/*
 * TorusNode.cpp
 *
 *  Created on: Jun 11, 2013
 *      Author: leigh
 */

#include "VolumeTree/Leaves/TorusNode.h"
#include <sstream>
#include <cmath>
#include <algorithm>

VolumeTree::TorusNode::TorusNode()
{
	_circleRadius = 1.0f;
	_sweepRadius = 1.0f;
}

VolumeTree::TorusNode::TorusNode(float sweepRadius, float circleRadius)
{
	_circleRadius = circleRadius;
	_sweepRadius = sweepRadius;
}

float VolumeTree::TorusNode::GetFunctionValue(float x, float y, float z)
{
	return (_circleRadius*_circleRadius)-(x*x)-(y*y)-(z*z)-(_sweepRadius*_sweepRadius)+2.0*_sweepRadius*sqrt( (x*x)+(z*z) );

	// 'normalised' version
	//return _circleRadius - sqrt((x*x)+(y*y)+(z*z)+(_sweepRadius*_sweepRadius)-2.0*_sweepRadius*sqrt( (x*x)+(z*z) ));
}

std::string VolumeTree::TorusNode::GetFunctionGLSLString(bool callCache, std::string samplePosStr)
{
	std::stringstream functionString;
	functionString<<"Torus("<<samplePosStr<<","<< _circleRadius <<","<<_sweepRadius<<")";

	return functionString.str();
}

void VolumeTree::TorusNode::GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ)
{
	*minX = - (_sweepRadius + _circleRadius);
	*maxX = (_sweepRadius + _circleRadius);
	
	*minY = -(_sweepRadius + _circleRadius);
	*maxY = (_sweepRadius + _circleRadius);

	*minZ = - _circleRadius;
	*maxZ = _circleRadius;
}
