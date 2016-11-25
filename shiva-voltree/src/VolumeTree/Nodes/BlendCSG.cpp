/*
 * BlendCSG.h
 *
 *  Created on: Jan 9, 2013
 *      Author: leigh
 */

#include "VolumeTree/Nodes/BlendCSG.h"
#include <sstream>
#include <iostream>
#include <algorithm>

#include <cmath>


VolumeTree::BlendCSGNode::BlendCSGNode()
{
	_childA = _childB = NULL;
	_a0 = 0.1f;
	_a1 = 1.0f;
	_a2 = 1.0f;
}

VolumeTree::BlendCSGNode::BlendCSGNode(Node *childA, Node *childB)
{
	_childA = childA;
	_childB = childB;
	_a0 = 0.1f;
	_a1 = 1.0f;
	_a2 = 1.0f;
}

VolumeTree::BlendCSGNode::~BlendCSGNode()
{
}

float VolumeTree::BlendCSGNode::GetFunctionValue(float x, float y, float z)
{
	//return std::max<float>(_childA->GetFunctionValue(x,y,z),_childB->GetFunctionValue(x,y,z));//fmax(_childA->GetFunctionValue(x,y,z),_childB->GetFunctionValue(x,y,z));

	// TODO: CSG types other than union!!!

	// R-intersection: f1+f2-sqrt(f1^2+f2^2)
	float f1 = _childA->GetFunctionValue(x,y,z);
	float f2 = _childB->GetFunctionValue(x,y,z);
	float value = f1+f2+sqrt( pow(f1,2)+pow(f2,2));

	value += _a0 / (1.0f+pow(f1/_a1,2) + pow(f2/_a2,2));
	return value;
}

std::string VolumeTree::BlendCSGNode::GetFunctionGLSLString(bool callCache, std::string samplePosStr)
{
	if( _childA != NULL && _childB != NULL )
	{

		std::stringstream functionString;
		if( _CSGType == CSG_UNION )
		{
			functionString<<"BlendCSG_Union(";
		}
		else if( _CSGType == CSG_SUBTRACTION )
		{
			functionString<<"BlendCSG_Subtract(";
		}
		else if( _CSGType == CSG_INTERSECTION )
		{
			functionString<<"BlendCSG_Intersect(";
		}

		if( callCache )
		{
			functionString<<_childA->GetCachedFunctionGLSLString(samplePosStr)<<","<<_childB->GetCachedFunctionGLSLString(samplePosStr)<<","<<_a0<<","<<_a1<<","<<_a2<<")";
		}
		else
		{
			functionString<<_childA->GetFunctionGLSLString(false,samplePosStr)<<","<<_childB->GetFunctionGLSLString(false,samplePosStr)<<","<<_a0<<","<<_a1<<","<<_a2<<")";
		}

/*
		if( callCache )
		{
			functionString<<"BlendCSG_Union("<<_childA->GetCachedFunctionGLSLString(samplePosStr)<<","<<_childB->GetCachedFunctionGLSLString(samplePosStr)<<","<<_a0<<","<<_a1<<","<<_a2<<")";
		}
		else
		{
			functionString<<"BlendCSG_Union("<<_childA->GetFunctionGLSLString(false,samplePosStr)<<","<<_childB->GetFunctionGLSLString(false,samplePosStr)<<","<<_a0<<","<<_a1<<","<<_a2<<")";
		}
*/
		return functionString.str();
	}
	else
	{
		std::cerr<<"WARNING: BlendCSGNode has one or more invalid children"<<std::endl;
		return "-1";
	}
}



void VolumeTree::BlendCSGNode::GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ)
{
	if( _childA != NULL && _childB != NULL )
	{
		// This is the normal case

		if( _CSGType == CSG_SUBTRACTION )
		{
			_childA->GetBounds(minX,maxX,minY,maxY,minZ,maxZ);
			//std::cout<<"INFO: ********************** subtraction"<<std::endl;
		}
		else
		{
			// We cannot tell intersection bounds just from the bounding boxes of our children
			// So we will just treat it the same as a union
			// TODO: fix this

			float AminX, AmaxX, AminY, AmaxY, AminZ, AmaxZ;
			float BminX, BmaxX, BminY, BmaxY, BminZ, BmaxZ;
			_childA->GetBounds(&AminX, &AmaxX, &AminY, &AmaxY, &AminZ, &AmaxZ);
			_childB->GetBounds(&BminX, &BmaxX, &BminY, &BmaxY, &BminZ, &BmaxZ);

			*minX = std::min<float>(AminX,BminX);
			*maxX = std::max<float>(AmaxX,BmaxX);
			*minY = std::min<float>(AminY,BminY);
			*maxY = std::max<float>(AmaxY,BmaxY);
			*minZ = std::min<float>(AminZ,BminZ);
			*maxZ = std::max<float>(AmaxZ,BmaxZ);
		}

		float lengthX = (*maxX) - (*minX);
		float lengthY = (*maxY) - (*minY);
		float lengthZ = (*maxZ) - (*minZ);

		*minX -= std::max(0.0f,_a0*lengthX*0.20f);
		*maxX += std::max(0.0f,_a0*lengthX*0.20f);
		*minY -= std::max(0.0f,_a0*lengthY*0.20f);
		*maxY += std::max(0.0f,_a0*lengthY*0.20f);
		*minZ -= std::max(0.0f,_a0*lengthZ*0.20f);
		*maxZ += std::max(0.0f,_a0*lengthZ*0.20f);

	}
	else if( _childA != NULL && _childB == NULL )
	{
		_childA->GetBounds(minX,maxX,minY,maxY,minZ,maxZ);
	}
	else if( _childA == NULL && _childB != NULL )
	{
		_childB->GetBounds(minX,maxX,minY,maxY,minZ,maxZ);
	}
	else
	{
		// Results undefined
		*minX = 0.0f;
		*maxX = 0.0f;
		*minY = 0.0f;
		*maxY = 0.0f;
		*minZ = 0.0f;
		*maxZ = 0.0f;
	}
}




/*
TODO
void VolumeTree::BlendCSGNode::GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ)
{
	if( _childA != NULL && _childB != NULL )
	{
		// This is the normal case

		float AminX, AmaxX, AminY, AmaxY, AminZ, AmaxZ;
		float BminX, BmaxX, BminY, BmaxY, BminZ, BmaxZ;
		_childA->GetBounds(&AminX, &AmaxX, &AminY, &AmaxY, &AminZ, &AmaxZ);
		_childB->GetBounds(&BminX, &BmaxX, &BminY, &BmaxY, &BminZ, &BmaxZ);

		*minX = std::min<float>(AminX,BminX);
		*maxX = std::max<float>(AmaxX,BmaxX);
		*minY = std::min<float>(AminY,BminY);
		*maxY = std::max<float>(AmaxY,BmaxY);
		*minZ = std::min<float>(AminZ,BminZ);
		*maxZ = std::max<float>(AmaxZ,BmaxZ);
	}
	else if( _childA != NULL && _childB == NULL )
	{
		_childA->GetBounds(minX,maxX,minY,maxY,minZ,maxZ);
	}
	else if( _childA == NULL && _childB != NULL )
	{
		_childB->GetBounds(minX,maxX,minY,maxY,minZ,maxZ);
	}
	else
	{
		// Results undefined
		*minX = 0.0f;
		*maxX = 0.0f;
		*minY = 0.0f;
		*maxY = 0.0f;
		*minZ = 0.0f;
		*maxZ = 0.0f;
	}
}
*/