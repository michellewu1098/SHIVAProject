/*
 * Node.h
 *
 *  Created on: Jan 9, 2013
 *      Author: leigh
 */

#ifndef NODE_H_
#define NODE_H_

#include <string>

// GLSLRenderer.h includes this file
class GLSLRenderer;

namespace VolumeTree
{
	/// Base Node class
	class Node
	{
	public:
		Node();
		virtual ~Node();

		std::string GetIDString() {return _IDString;}
		void SetIDString( std::string value ) {_IDString = value;}

		virtual std::string GetNodeType(){return "Node";}

		/// Samples the function at a specific point
		virtual float GetFunctionValue( float x, float y, float z ) = 0;

		/// Returns a GLSL-compatible string for the function
		/// Cached nodes will give a cache instruction instead of a full subtree
		/// Default behaviour is to call GetFunctionGLSLString() so nodes with children
		/// will need to override this function to call GetCachedFunctionGLSLString() on children
		/// \param samplePosStr The string to use if the function text needs the sample position. This must be passed to children
		virtual std::string GetCachedFunctionGLSLString( std::string samplePosStr );

		/// Returns a GLSL-compatible string for the function
		/// If the node has children, it must recursively build the string with those
		/// If callCache is set, it must call GetCachedFunctionGLSLString() on its children instead of the uncached version
		/// \param samplePosStr The string to use if the function text needs the sample position. This must be passed to children
		virtual std::string GetFunctionGLSLString(bool callCache, std::string samplePosStr ) = 0;

		/// This instruction is recursively passed down the tree
		/// If a caching node is reached, it builds a cache of its sub-tree and returns (to prevent sub-nodes from caching too)
		/// Nodes are explicitly told that they must cache their sub-trees in a previous step
		/// This function also triggers parameters to be created, but not for nodes that are cached
		void BuildCaches(GLSLRenderer *renderer);

		void SetDrawBBox(bool value) {_drawBBox=value;}
		bool GetDrawBBox() {return _drawBBox;}
		/// Recursively draws bounding boxes of node tree
		void DrawBBoxes();

		/// For performing a left-first tree traversal
		/// If you give it a desiredNode of -1 it will recurse entire structure, thus giving you the total size in currentCount
		Node* RecursiveGetNode(int desiredNode, int &currentCount, int &currentDepth);

		// For use by the CachingPolicy:

			virtual Node* GetFirstChild();
			virtual Node* GetNextChild(Node *previousChild);

			virtual void SetUseCache(bool useCache, unsigned int cacheID, unsigned int cacheResX, unsigned int cacheResY, unsigned int cacheResZ );
			bool GetUseCache() {return _useCache;}

			bool GetRequiresCache() {return _requiresCache;}
			void SetRequiresCache(bool value) {_requiresCache=value;}

			/// Finds out how many nodes require caching in the node's sub-tree
			/// The transparent determines if the node processes its subtree (true if it does)
			unsigned int GetSubtreeNumCacheRequires(bool transparent);

			/// Adds all bounding volumes together of cache nodes
			/// Used to work out resolution of cache
			float GetSubtreeCacheVolume();

			/// For a cache node, this will return the total volume of the subtree
			/// For a non-cache node it will return ? 
			//virtual float GetNodeBoundVolume();

			virtual unsigned int GetNodeCost() = 0;
			// Returns the total cost of itself and its sub-tree
			// If the node is caching (either is already told to use cache or requires cache) it returns just the caching cost
			unsigned int GetSubtreeCost();

			void GetBoundSizes(float *x, float *y, float *z);
			virtual void GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ) = 0;
			//virtual void GetNodeBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ) = 0;

			void UpdateParameters( GLSLRenderer *renderer );

	protected:

		std::string _IDString;

		bool _drawBBox;

		/// If the node has been told to cache it's sub-tree
		bool _useCache;

		/// If the node itself says it needs to be cached
		bool _requiresCache;

		int _cacheNumber;

		/// True if the cache has been built
		bool _cacheBuilt;

		/// True if the cache needs to be rebuilt
		bool _cacheDirty;

		unsigned int _cacheResX, _cacheResY, _cacheResZ;

		float _cacheOffsetX, _cacheOffsetY, _cacheOffsetZ;
		float _cacheScaleX, _cacheScaleY, _cacheScaleZ;

		virtual void PopulateCacheData(float **data, float startX, float startY, float startZ, float stepX, float stepY, float stepZ);

		/// Node is expected to register any parameters it wants to use
		virtual void OnBuildParameters( GLSLRenderer *renderer ) {}
		/// Node is expected to tell the renderer the values of its parameters
		virtual void OnUpdateParameters( GLSLRenderer *renderer ) {}

	};

}


#endif /* NODE_H_ */
