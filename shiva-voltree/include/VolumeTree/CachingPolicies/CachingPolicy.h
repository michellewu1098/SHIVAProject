//------------------------------------------------------------------------------------
/// \file CachingPolicy.h
/// \brief The base class is useable but only caches nodes that say they require it
/// \author Leigh McLoughlin
/// \version 1.0
/// \date May 23, 2013
//------------------------------------------------------------------------------------

#ifndef CACHINGPOLICY_H
#define CACHINGPOLICY_H

#include "VolumeTree/Node.h"

// GLSLRenderer.h includes this file
class GLSLRenderer;

namespace VolumeTree
{
	class Tree;

	class CachingPolicy
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		CachingPolicy();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~CachingPolicy();
		//----------------------------------------------------------------------------------
		/// \brief This must determine which (if any) nodes in the tree should be cached
		/// It should examine the node sub-tree and tell individual nodes to cache their sub-trees (and what resolution cache to use)
		/// It can use information from the renderer to figure out the optimal use of caching and cache sizes etc
		/// \param [in] _rootNode
		/// \param [in] _renderer
		//----------------------------------------------------------------------------------
		virtual void Process( Node *_rootNode, GLSLRenderer *_renderer );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// Tree traversing
		/// \param [in] _currentNode
		/// \param [in] _renderer
		/// \param [in] _cacheDensity
		/// \param [in] _maxNodeVoxels
		/// \param [in] _currentCacheID
		//----------------------------------------------------------------------------------
		unsigned int TreeTraverse( Node *_currentNode, GLSLRenderer *_renderer, float _cacheDensity, unsigned int _maxNodeVoxels, unsigned int _currentCacheID );
		//----------------------------------------------------------------------------------

	};
}

#endif // CACHINGPOLICY_H_ 
