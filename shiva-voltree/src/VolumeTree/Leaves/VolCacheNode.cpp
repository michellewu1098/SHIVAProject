
#include "VolumeTree/Leaves/VolCacheNode.h"

#include <iostream>
#include <algorithm>
#include <cmath>

#include "vol_metamorph.h"
#include "vol_totem.h"

VolumeTree::VolCacheNode::VolCacheNode()
{
	_requiresCache = true;
	_boundsMinX = _boundsMaxX = _boundsMinY = _boundsMaxY = _boundsMinZ = _boundsMaxZ = 0.0f;
	_cachedFunction = NULL;
	_volCacheNode = NULL;
}

VolumeTree::VolCacheNode::VolCacheNode( totemio::CacheNode *nodeIn )
{
	_volCacheNode = nodeIn;
	_requiresCache = true;
	_boundsMinX = _boundsMaxX = _boundsMinY = _boundsMaxY = _boundsMinZ = _boundsMaxZ = 0.0f;
	_cachedFunction = NULL;

	// Just get bounds for now
	if( _volCacheNode != NULL )
	{
		float *bounds = NULL;//new float[27];

		bool result = _volCacheNode->getBoundingBox(&bounds);
		if( result )
		{
			// For now enlarge bbox by 10%
			// TODO: ensure negative shell around contents
			float lengthX = bounds[3] - bounds[0];
			float lengthY = bounds[4] - bounds[1];
			float lengthZ = bounds[5] - bounds[2];

			_boundsMinX = bounds[0] - lengthX * 0.1f;
			_boundsMaxX = bounds[3] + lengthX * 0.1f;
			_boundsMinY = bounds[1] - lengthY * 0.1f;
			_boundsMaxY = bounds[4] + lengthY * 0.1f;
			_boundsMinZ = bounds[2] - lengthZ * 0.1f;
			_boundsMaxZ = bounds[5] + lengthZ * 0.1f;

			totemio::freePointer(&bounds);
		}
	}
}



VolumeTree::VolCacheNode::VolCacheNode(std::string filename)
{
	_volCacheNode = NULL;
	_requiresCache = true;
	_filename = filename;
	_boundsMinX = _boundsMaxX = _boundsMinY = _boundsMaxY = _boundsMinZ = _boundsMaxZ = 0.0f;
	_cachedFunction = NULL;
	// Just get bounds for now

	float *bounds = NULL;
	float *data = NULL;//new float[27];

	open_params params;
	params.m_fit_to_box = false;
	params.m_calculate_bbox = true;
	params.m_fill_array = false;
	params.m_exact_bbox = false;
	
	if( !openVol(filename.c_str(), 3,3,3, params, &bounds, &data ) )
	{
		std::cerr<<"WARNING: Could not open VOL file: "<<filename<<std::endl;
	}
	else
	{
		_boundsMinX = bounds[0];
		_boundsMaxX = bounds[3];
		_boundsMinY = bounds[1];
		_boundsMaxY = bounds[4];
		_boundsMinZ = bounds[2];
		_boundsMaxZ = bounds[5];
	}

	//delete [] bounds;
	//delete [] data;
	freePt(&bounds);
	freePt(&data);
}


VolumeTree::VolCacheNode::~VolCacheNode()
{
	if( _volCacheNode != NULL )
	{
		totemio::freeTree(_volCacheNode);
		totemio::freePointer(&_cachedFunction);
	}
	else if( _cachedFunction != NULL )
	{
		freePt(&_cachedFunction);
	}
}

float VolumeTree::VolCacheNode::GetFunctionValue(float x, float y, float z)
{
	// TODO: use cached function
	// Transform coords
	x = (x + _cacheOffsetX) * _cacheScaleX + 0.5f;
	y = (y + _cacheOffsetY) * _cacheScaleY + 0.5f;
	z = (z + _cacheOffsetZ) * _cacheScaleZ + 0.5f;

	// TODO: tri-lerp
	// For now, just use nearest filtering:

	x = std::max(x,0.0f);
	y = std::max(y,0.0f);
	z = std::max(z,0.0f);

	x = std::min(x,1.0f);
	y = std::min(y,1.0f);
	z = std::min(z,1.0f);

	x = (x*(float)_cacheResX);
	y = (y*(float)_cacheResY);
	z = (z*(float)_cacheResZ);

	float scrap;
	x = modf(x,&scrap) < 0.5 ? floor(x) : ceil(x);
	y = modf(y,&scrap) < 0.5 ? floor(y) : ceil(y);
	z = modf(z,&scrap) < 0.5 ? floor(z) : ceil(z);

	return SampleCacheFunction((unsigned int)x,(unsigned int)y,(unsigned int)z);
}

std::string VolumeTree::VolCacheNode::GetFunctionGLSLString(bool callCache, std::string samplePosStr)
{
	// Should never get here, this node can only be cached
	std::cerr<<"WARNING: GetFunctionGLSLString() called on VolCacheNode"<<std::endl;
	return "-1.0";
}

void VolumeTree::VolCacheNode::GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ)
{
	*minX = _boundsMinX;
	*maxX = _boundsMaxX;
	*minY = _boundsMinY;
	*maxY = _boundsMaxY;
	*minZ = _boundsMinZ;
	*maxZ = _boundsMaxZ;
}

void VolumeTree::VolCacheNode::SetUseCache(bool useCache, unsigned int cacheID, unsigned int cacheResX, unsigned int cacheResY, unsigned int cacheResZ )
{
	Node::SetUseCache(useCache,cacheID,cacheResX,cacheResY,cacheResZ);
	if( useCache )
	{
		if( _volCacheNode )
		{
			#ifdef _DEBUG
			std::cout<<"INFO: VolCacheNode::SetUseCache resX: "<<cacheResX<<" resY: "<<cacheResY<<" resZ: "<<cacheResZ<<std::endl;
			#endif
			float *bbox = new float[6];
			bbox[0] = _boundsMinX;
			bbox[1] = _boundsMinY;
			bbox[2] = _boundsMinZ;
			bbox[3] = _boundsMaxX;
			bbox[4] = _boundsMaxY;
			bbox[5] = _boundsMaxZ;
			if( !_volCacheNode->generateCache(_cacheResX,_cacheResY,_cacheResZ,bbox,&_cachedFunction ) )
			{
				std::cerr<<"WARNING: Could not build cache from cache node: "<<_volCacheNode->getID()<<std::endl;
			}
		}
		else if( !_filename.empty() )
		{

			float *bounds = NULL;
			_cachedFunction = NULL;

			open_params params;
			params.m_fit_to_box = false;
			params.m_calculate_bbox = false;
			params.m_fill_array = true;
			params.m_exact_bbox = false;

			std::cout<<"INFO: Loading VOL file: "<<_filename<<std::endl;
			if( !openVol(_filename.c_str(), _cacheResX,_cacheResY,_cacheResZ, params, &bounds, &_cachedFunction ) )
			{
				std::cerr<<"WARNING: Could not build cache from VOL file: "<<_filename<<std::endl;
			}
			freePt(&bounds);
		}
	}
}

void VolumeTree::VolCacheNode::PopulateCacheData(float **data, float startX, float startY, float startZ, float stepX, float stepY, float stepZ)
{
	if( _cachedFunction != NULL )
	{
		for( unsigned int i = 0; i < _cacheResX*_cacheResY*_cacheResZ; i++ )
		{
			(*data)[i] = -_cachedFunction[i];
		}
	}
	/*
	float *bounds = NULL;
	_cachedFunction = NULL;
	if( !openVol(_filename.c_str(), _cacheResX,_cacheResY,_cacheResZ, false, &bounds, &_cachedFunction ) )
	{
		std::cerr<<"WARNING: Could not build cache from VOL file: "<<_filename<<std::endl;
	}
	else
	{
		for( unsigned int i = 0; i < _cacheResX*_cacheResY*_cacheResZ; i++ )
		{
			(*data)[i] = -_cachedFunction[i];
		}
	}
	freePt(&bounds);
	*/

	// Unfortunately we need to keep the cache in case a parent node needs caching, in which case we need to be able to sample the function
}

float VolumeTree::VolCacheNode::SampleCacheFunction(unsigned int x, unsigned int y, unsigned int z)
{
	if( _cachedFunction != NULL && (x < _cacheResX) && (y < _cacheResY) && (z < _cacheResZ) )
	{
		return -_cachedFunction[x + y*_cacheResX + z*_cacheResX*_cacheResY];
	}
	return -1.0f;
}
