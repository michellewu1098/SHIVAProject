/*
 * VolCache.h
 *
 *  Created on: Jun 3, 2013
 *      Author: leigh
 */

#ifndef VOLCACHE_H_
#define VOLCACHE_H_

#include "VolumeTree/Node.h"

//#include "vol_metamorph.h"
//#include "vol_totem.h"
namespace totemio
{
	class CacheNode;
}

namespace VolumeTree
{
	/// Leaf node for a sphere
	class VolCacheNode : public Node
	{
	public:
		VolCacheNode();
		VolCacheNode(std::string filename);
		VolCacheNode( totemio::CacheNode* );
		virtual ~VolCacheNode();
		
		virtual std::string GetNodeType(){return "VolCacheNode";}

		/// Samples the function at a specific point
		float GetFunctionValue(float x, float y, float z);

		/// Returns a GLSL-compatible string for the function
		std::string GetFunctionGLSLString(bool callCache, std::string samplePosStr);

		virtual void SetUseCache(bool useCache, unsigned int cacheID, unsigned int cacheResX, unsigned int cacheResY, unsigned int cacheResZ );

		
		virtual unsigned int GetNodeCost() {return 2;}

		virtual void GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ);

	protected:

		std::string _filename;
		totemio::CacheNode *_volCacheNode;

		float _boundsMinX, _boundsMaxX, _boundsMinY, _boundsMaxY, _boundsMinZ, _boundsMaxZ;
		
		virtual void PopulateCacheData(float **data, float startX, float startY, float startZ, float stepX, float stepY, float stepZ);

		float *_cachedFunction;

		/// Uses nearest sampling 
		float SampleCacheFunction(unsigned int x, unsigned int y, unsigned int z);
	};
}


#endif
