/*
 * Node.cpp
 *
 *  Created on: Jan 9, 2013
 *      Author: leigh
 */

#include <GL/GLee.h>
#include <GL/gl.h>
#include "VolumeTree/Node.h"
#include "VolumeRenderer/GLSLRenderer.h"

VolumeTree::Node::Node()
{
	_drawBBox = false;

	_useCache = false;
	_requiresCache = false;
	_cacheNumber = -1;
	_cacheResX = _cacheResY = _cacheResZ = 0;
	_cacheOffsetX = _cacheOffsetY = _cacheOffsetZ = 0.0f;
	_cacheScaleX = _cacheScaleY = _cacheScaleZ = 1.0f;

	_cacheBuilt = false;
	_cacheDirty = false;
}

VolumeTree::Node::~Node()
{
}

void VolumeTree::Node::SetUseCache(bool useCache, unsigned int cacheID, unsigned int cacheResX, unsigned int cacheResY, unsigned int cacheResZ )
{
	_cacheDirty = _cacheDirty || ( cacheID != _cacheNumber || _cacheResX != cacheResX || _cacheResY != cacheResY || _cacheResZ != cacheResZ );
	_useCache = useCache;
	_cacheNumber = cacheID;
	_cacheResX = cacheResX;
	_cacheResY = cacheResY;
	_cacheResZ = cacheResZ;
}

void VolumeTree::Node::BuildCaches(GLSLRenderer *renderer)
{
	if( _useCache && (_cacheNumber>=0) )
	{
		if( (!_cacheBuilt) || _cacheDirty )
		{
		// Build cache

			// If we have been told to cache the node we will already have been told our resolution
			float boundsX, boundsY, boundsZ;
			GetBoundSizes(&boundsX, &boundsY, &boundsZ);
			unsigned int volX = _cacheResX, volY = _cacheResY, volZ = _cacheResZ;
			//renderer->GetCacheSize(boundsX,boundsY,boundsZ, &volX,&volY,&volZ);

			//_cacheScaleX = boundsX;
			//_cacheScaleY = boundsY;
			//_cacheScaleZ = boundsZ;

			// Allocate memory
			float *cacheData = new float[volX*volY*volZ];

			// Build cache
				// TODO: based on actual bounding box
			
			float minX, maxX, minY, maxY, minZ, maxZ;
			GetBounds(&minX,&maxX, &minY,&maxY, &minZ,&maxZ);

			
			_cacheOffsetX = -(minX + (boundsX*0.5f));
			_cacheOffsetY = -(minY + (boundsY*0.5f));
			_cacheOffsetZ = -(minZ + (boundsZ*0.5f));
			
			_cacheScaleX = 1.0f/boundsX;
			_cacheScaleY = 1.0f/boundsY;
			_cacheScaleZ = 1.0f/boundsZ;


			float startX = minX, startY = minY, startZ = minZ;
			float stepX = boundsX/(float)volX, stepY = boundsY/(float)volY, stepZ = boundsZ/(float)volZ;
			
			PopulateCacheData(&cacheData,startX,startY,startZ,stepX,stepY,stepZ);

			// Pass to renderer
			renderer->FillCache(_cacheNumber, cacheData, volX,volY,volZ);

			// Free memory
			delete [] cacheData;

			_cacheBuilt = true;
			_cacheDirty = false;
		}
	}
	else
	{
		OnBuildParameters(renderer);

		for( Node *currentChild = GetFirstChild(); currentChild != NULL; currentChild = GetNextChild(currentChild) )
		{
			currentChild->BuildCaches(renderer);
		}
	}
}

void VolumeTree::Node::DrawBBoxes()
{
	if( _drawBBox )
	{
		// TODO: this is temporary code, must replace from immediate mode
		float minX, maxX, minY, maxY, minZ, maxZ;
		GetBounds(&minX,&maxX, &minY,&maxY, &minZ,&maxZ);
		float offset = 0.03f;
		minX -= offset; minY -= offset; minZ -= offset;
		maxX += offset; maxY += offset; maxZ += offset;


		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE); // Disables writing to the depth buffer so that bounding boxes don't hide each other

		glLineWidth(5.0f);
		glBegin(GL_LINES);
			glColor3f(1.0f,0.0f,0.0f);
			glVertex3f(minX,minY,minZ);	glVertex3f(maxX,minY,minZ);
			glVertex3f(minX,maxY,minZ);	glVertex3f(maxX,maxY,minZ);
			glVertex3f(minX,minY,maxZ);	glVertex3f(maxX,minY,maxZ);
			glVertex3f(minX,maxY,maxZ);	glVertex3f(maxX,maxY,maxZ);

			glColor3f(0.0f,1.0f,0.0f);
			glVertex3f(minX,minY,minZ);	glVertex3f(minX,maxY,minZ);
			glVertex3f(maxX,minY,minZ);	glVertex3f(maxX,maxY,minZ);
			glVertex3f(minX,minY,maxZ);	glVertex3f(minX,maxY,maxZ);
			glVertex3f(maxX,minY,maxZ);	glVertex3f(maxX,maxY,maxZ);
			
			glColor3f(0.0f,0.0f,1.0f);
			glVertex3f(minX,minY,minZ);	glVertex3f(minX,minY,maxZ);
			glVertex3f(minX,maxY,minZ);	glVertex3f(minX,maxY,maxZ);
			glVertex3f(maxX,minY,minZ);	glVertex3f(maxX,minY,maxZ);
			glVertex3f(maxX,maxY,minZ);	glVertex3f(maxX,maxY,maxZ);

		glEnd();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_SRC_COLOR, GL_ONE);
		//glBlendFuncSeparate(GL_SRC_COLOR, GL_ONE, GL_DST_ALPHA, GL_ONE);

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		//glFrontFace(GL_CW);
		
		glColor4f(1.0f,0.0f,0.0f,0.1f);
		glBegin(GL_QUADS);

			glVertex3f(minX,minY,minZ);	glVertex3f(maxX,minY,minZ);
			glVertex3f(maxX,maxY,minZ); glVertex3f(minX,maxY,minZ);

			glVertex3f(minX,minY,maxZ);	glVertex3f(maxX,minY,maxZ);
			glVertex3f(maxX,maxY,maxZ); glVertex3f(minX,maxY,maxZ);

		
			glVertex3f(minX,minY,maxZ);	glVertex3f(minX,maxY,maxZ);
			glVertex3f(minX,maxY,minZ); glVertex3f(minX,minY,minZ);

			glVertex3f(maxX,minY,maxZ);	glVertex3f(maxX,maxY,maxZ);
			glVertex3f(maxX,maxY,minZ); glVertex3f(maxX,minY,minZ);
				

			glVertex3f(minX,minY,minZ);	glVertex3f(minX,minY,maxZ);
			glVertex3f(maxX,minY,maxZ); glVertex3f(maxX,minY,minZ);

			glVertex3f(minX,maxY,minZ);	glVertex3f(minX,maxY,maxZ);
			glVertex3f(maxX,maxY,maxZ); glVertex3f(maxX,maxY,minZ);

		glEnd();

		glDisable(GL_CULL_FACE);

		glDisable(GL_BLEND);
		
		glDepthMask(GL_TRUE);
		glDisable(GL_DEPTH_TEST);
	}

	for( Node *currentChild = GetFirstChild(); currentChild != NULL; currentChild = GetNextChild(currentChild) )
	{
		currentChild->DrawBBoxes();
	}
}

VolumeTree::Node* VolumeTree::Node::RecursiveGetNode(int desiredNode, int &currentCount, int &currentDepth)
{
	if( desiredNode == currentCount )
	{
		std::cout<<"INFO: recursive get has found node "<<desiredNode<<std::endl;
		return this;
	}

	currentDepth++;
	Node *currentChild = GetFirstChild();
	while( currentChild != NULL )
	{
		currentCount++;
		Node *result = currentChild->RecursiveGetNode(desiredNode, currentCount, currentDepth);
		if( result != NULL )
		{
			return result;
		}
		currentChild = GetNextChild(currentChild);
	}
	currentDepth--;
	return NULL;
}

std::string VolumeTree::Node::GetCachedFunctionGLSLString( std::string samplePosStr )
{
	if( _useCache )
	{
		std::stringstream functionString;
		functionString<<"Cache("<<samplePosStr<<",CacheTexture"<<_cacheNumber<<",vec3("<<_cacheOffsetX<<","<<_cacheOffsetY<<","<<_cacheOffsetZ<<"),vec3("<<_cacheScaleX<<","<<_cacheScaleY<<","<<_cacheScaleZ<<"))";

		return functionString.str();
	}
	else
		return GetFunctionGLSLString(true,samplePosStr);
}



VolumeTree::Node* VolumeTree::Node::GetFirstChild()
{
	return NULL;
}

VolumeTree::Node* VolumeTree::Node::GetNextChild(Node *previousChild)
{
	return NULL;
}

unsigned int VolumeTree::Node::GetSubtreeNumCacheRequires(bool transparent)
{
	unsigned int total = _requiresCache ? 1 : 0;
	if( (!transparent) && _requiresCache )
	{
		return 1;
	}

	for( Node *currentChild = GetFirstChild(); currentChild != NULL; currentChild = GetNextChild(currentChild) )
	{
		total += currentChild->GetSubtreeNumCacheRequires(transparent);
	}
	return total;
}

float VolumeTree::Node::GetSubtreeCacheVolume()
{
	float x,y,z;
	GetBoundSizes(&x,&y,&z);

	return x*y*z;
}

unsigned int VolumeTree::Node::GetSubtreeCost()
{
	if( _useCache || _requiresCache )
	{
		return 5;
	}
	int total = GetNodeCost();
	for( Node *currentChild = GetFirstChild(); currentChild != NULL; currentChild = GetNextChild(currentChild) )
	{
		total += currentChild->GetSubtreeCost();
	}
	return total;
}

void VolumeTree::Node::GetBoundSizes(float *x, float *y, float *z)
{
	float minX, maxX, minY, maxY, minZ, maxZ;
	GetBounds(&minX,&maxX, &minY,&maxY, &minZ,&maxZ);

	*x = fabs(maxX-minX);
	*y = fabs(maxY-minY);
	*z = fabs(maxZ-minZ);
}

/*
void VolumeTree::Node::GetSubtreeBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ)
{
	float minX, maxX, minY, maxY, minZ, maxZ;
	GetNodeBounds(&minX,&maxX, &minY,&maxY, &minZ,&maxZ);

	for( Node *currentChild = currentNode->GetFirstChild(); currentChild != NULL; currentChild = currentNode->GetNextChild(currentChild) )
	{
		float childMinX, childMaxX, childMinY, childMaxY, childMinZ, childMaxZ;
		currentChild->GetNodeBounds(&childMinX,&childMaxX, &childMinY,&childMaxY, &childMinZ,&childMaxZ);


	}
	return total;
}
*/

void VolumeTree::Node::PopulateCacheData(float **cacheData, float startX, float startY, float startZ, float stepX, float stepY, float stepZ)
{
	unsigned int volX = _cacheResX, volY = _cacheResY, volZ = _cacheResZ;
	for( unsigned int i = 0; i < volX; i++ )
	{
		float samplePosX = startX + (stepX * (float)i);
		for( unsigned int j = 0; j < volY; j++ )
		{
			float samplePosY = startY + (stepY * (float)j);
			for( unsigned int k = 0; k < volZ; k++ )
			{
				float samplePosZ = startZ + (stepZ * (float)k);

				(*cacheData)[i + j*volX + k*volX*volY] = GetFunctionValue(samplePosX,samplePosY,samplePosZ);
			}
		}
	}
}

void VolumeTree::Node::UpdateParameters( GLSLRenderer *renderer )
{
	OnUpdateParameters( renderer );

	for( Node *currentChild = GetFirstChild(); currentChild != NULL; currentChild = GetNextChild(currentChild) )
	{
		currentChild->UpdateParameters( renderer );
	}
}
