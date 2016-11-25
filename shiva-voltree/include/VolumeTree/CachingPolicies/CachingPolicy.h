/*
 * CachingPolicy.h
 *
 *  Created on: May 23, 2013
 *      Author: leigh
 */

#ifndef CACHINGPOLICY_H
#define CACHINGPOLICY_H

#include <string>

#include "VolumeTree/Node.h"

// GLSLRenderer.h includes this file
class GLSLRenderer;

namespace VolumeTree
{
	class Tree;

	/// The base class is useable but only caches nodes that say they require it
	class CachingPolicy
	{
	public:
		CachingPolicy();
		virtual ~CachingPolicy();

		/// This must determine which (if any) nodes in the tree should be cached
		/// It should examine the node sub-tree and tell individual nodes to cache their sub-trees (and what resolution cache to use)
		/// It can use information from the renderer to figure out the optimal use of caching and cache sizes etc
		virtual void Process( Node *, GLSLRenderer *);

	protected:

		unsigned int TreeTraverse( Node *, GLSLRenderer *, float cacheDensity, unsigned int maxNodeVoxels, unsigned int currentCacheID);
	};

}


#endif /* NODE_H_ */
