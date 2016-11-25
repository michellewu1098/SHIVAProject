/*
 * CSG.h
 *
 *  Created on: Jan 9, 2013
 *      Author: leigh
 */

#include "VolumeTree/Nodes/CSG.h"
#include <sstream>
#include <iostream>
#include <algorithm>

#include <cmath>


VolumeTree::CSGNode::CSGNode()
{
	_childA = _childB = NULL;
	_CSGType = CSG_UNION;
}

VolumeTree::CSGNode::CSGNode(Node *childA, Node *childB)
{
	_childA = childA;
	_childB = childB;
	_CSGType = CSG_UNION;
}

VolumeTree::CSGNode::~CSGNode()
{
	delete _childA;
	delete _childB;
}

float VolumeTree::CSGNode::GetFunctionValue(float x, float y, float z)
{
	//return std::max<float>(_childA->GetFunctionValue(x,y,z),_childB->GetFunctionValue(x,y,z));//fmax(_childA->GetFunctionValue(x,y,z),_childB->GetFunctionValue(x,y,z));

	// TODO: CSG types other than union!!!

	// R-intersection: f1+f2-sqrt(f1^2+f2^2)
	float f1 = _childA->GetFunctionValue(x,y,z);
	float f2 = _childB->GetFunctionValue(x,y,z);
	return f1+f2+sqrt( pow(f1,2)+pow(f2,2));

	// TODO: functions for other operation types
}

std::string VolumeTree::CSGNode::GetFunctionGLSLString(bool callCache, std::string samplePosStr)
{
	if( _childA != NULL && _childB != NULL )
	{
		// Just Union for this test
		std::stringstream functionString;
		/*if( callCache )
		{
			functionString<<"CSG_Union("<<_childA->GetCachedFunctionGLSLString(samplePosStr)<<","<<_childB->GetCachedFunctionGLSLString(samplePosStr)<<")";
		}
		else */

		if( _CSGType == CSG_UNION )
		{
			functionString<<"CSG_Union(";
		}
		else if( _CSGType == CSG_SUBTRACTION )
		{
			functionString<<"CSG_Subtract(";
		}
		else if( _CSGType == CSG_INTERSECTION )
		{
			functionString<<"CSG_Intersect(";
		}

		if( callCache )
		{
			functionString<<_childA->GetCachedFunctionGLSLString(samplePosStr)<<","<<_childB->GetCachedFunctionGLSLString(samplePosStr)<<")";
		}
		else
		{
			functionString<<_childA->GetFunctionGLSLString(false,samplePosStr)<<","<<_childB->GetFunctionGLSLString(false,samplePosStr)<<")";
		}


		/*
		if( _CSGType == CSG_UNION )
		{
			functionString<<"CSG_Union("<<_childA->GetFunctionGLSLString(false,samplePosStr)<<","<<_childB->GetFunctionGLSLString(false,samplePosStr)<<")";
		}
		else if( _CSGType == CSG_SUBTRACTION )
		{
			functionString<<"CSG_Subtract("<<_childA->GetFunctionGLSLString(false,samplePosStr)<<","<<_childB->GetFunctionGLSLString(false,samplePosStr)<<")";
		}
		else if( _CSGType == CSG_INTERSECTION )
		{
			functionString<<"CSG_Intersect("<<_childA->GetFunctionGLSLString(false,samplePosStr)<<","<<_childB->GetFunctionGLSLString(false,samplePosStr)<<")";
		}
		*/

		return functionString.str();
	}
	else
	{
		std::cerr<<"WARNING: CSGNode has one or more invalid children"<<std::endl;
		return "-1";
	}
}

VolumeTree::Node* VolumeTree::CSGNode::GetFirstChild()
{
	if( _childA != NULL )
	{
		return _childA;
	}
	else
		return _childB;
}

VolumeTree::Node* VolumeTree::CSGNode::GetNextChild(Node *previousChild)
{
	if( previousChild == NULL )
	{
		return GetFirstChild();
	}
	else if( previousChild == _childA )
	{
		return _childB;
	}
	return NULL;
}

void VolumeTree::CSGNode::GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ)
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
