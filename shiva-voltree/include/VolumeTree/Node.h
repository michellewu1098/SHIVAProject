//------------------------------------------------------------------------------------
/// \file Node.h
/// \brief Base node class
/// \author Leigh McLoughlin
/// \version 1.0
//------------------------------------------------------------------------------------

#ifndef NODE_H_
#define NODE_H_

#include <cml/cml.h.>
#include "VolumeRenderer/Shader.h"

// GLSLRenderer.h includes this file
class GLSLRenderer;

namespace VolumeTree
{
	class Node
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		Node();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~Node();
		//----------------------------------------------------------------------------------
		/// \brief Returns node id
		/// \return m_IDString
		//----------------------------------------------------------------------------------
		std::string GetIDString() { return m_IDString; }
		//----------------------------------------------------------------------------------
		/// \brief Set node id string
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetIDString( std::string _value ) { m_IDString = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Returns node type
		/// \return "Node"
		//----------------------------------------------------------------------------------
		virtual std::string GetNodeType() { return "Node"; }
		//----------------------------------------------------------------------------------
		/// \brief Samples the function at a specific point
		/// \param [in] _x 
		/// \param [in] _y
		/// \param [in] _z
		//----------------------------------------------------------------------------------
		virtual float GetFunctionValue( float _x, float _y, float _z ) = 0;
		//----------------------------------------------------------------------------------
		/// \brief Returns a GLSL-compatible string for the function
		/// Cached nodes will give a cache instruction instead of a full subtree
		/// Default behaviour is to call GetFunctionGLSLString() so nodes with children
		/// will need to override this function to call GetCachedFunctionGLSLString() on children
		/// \param [in] _samplePosStr The string to use if the function text needs the sample position. This must be passed to children
		//----------------------------------------------------------------------------------
		virtual std::string GetCachedFunctionGLSLString( std::string _samplePosStr );
		//----------------------------------------------------------------------------------
		/// \brief Returns a GLSL-compatible string for the function
		/// If the node has children, it must recursively build the string with those
		/// If callCache is set, it must call GetCachedFunctionGLSLString() on its children instead of the uncached version
		/// \param [in] _samplePosStr The string to use if the function text needs the sample position. This must be passed to children
		//----------------------------------------------------------------------------------
		virtual std::string GetFunctionGLSLString( bool _callCache, std::string _samplePosStr ) = 0;
		//----------------------------------------------------------------------------------
		/// \brief This instruction is recursively passed down the tree
		/// If a caching node is reached, it builds a cache of its sub-tree and returns (to prevent sub-nodes from caching too)
		/// Nodes are explicitly told that they must cache their sub-trees in a previous step
		/// This function also triggers parameters to be created, but not for nodes that are cached
		/// \param [in] _renderer
		//----------------------------------------------------------------------------------
		void BuildCaches( GLSLRenderer *_renderer );
		//----------------------------------------------------------------------------------
		/// \brief Set whether to draw bbox or not
		/// \param [in] _value Either true or false
		//----------------------------------------------------------------------------------
		void SetDrawBBox( const bool &_value );// { m_drawBBox = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Returns whether bbox is being drawn or not
		/// \return m_drawBBox
		//----------------------------------------------------------------------------------
		bool GetDrawBBox() const { return m_drawBBox; }
		//----------------------------------------------------------------------------------
		/// \brief Recursively draws bounding boxes of node tree using shaders
		/// \param [in] _proj Projection matrix
		/// \param [in] _mv ModelView matrix
		//----------------------------------------------------------------------------------
		void DrawBBoxes( cml::matrix44f_c &_proj, cml::matrix44f_c &_mv );
		//----------------------------------------------------------------------------------
		/// \brief Create bounding box vbos and vao
		//----------------------------------------------------------------------------------
		void BuildBBoxesVBOs();
		//----------------------------------------------------------------------------------
		/// \brief For performing a left-first tree traversal
		/// If you give it a desiredNode of -1 it will recurse entire structure, thus giving you the total size in currentCount
		/// \param [in] _desiredNode
		/// \param [in] _currentCount
		/// \param [in] _currentDepth
		//----------------------------------------------------------------------------------
		Node* RecursiveGetNode( int _desiredNode, int &_currentCount, int &_currentDepth );
		//----------------------------------------------------------------------------------
		// FOR USE BY CACHING POLICY:
		//----------------------------------------------------------------------------------
		/// \brief Returns node's first child
		//----------------------------------------------------------------------------------
		virtual Node* GetFirstChild();
		//----------------------------------------------------------------------------------
		/// \brief Returns node's next child of previous child
		/// \param [in] _previousChild
		//----------------------------------------------------------------------------------
		virtual Node* GetNextChild( Node *_previousChild );
		//----------------------------------------------------------------------------------
		/// \brief Use cache
		/// \param [in] _useCache
		/// \param [in] _cacheID
		/// \param [in] _cacheResX
		/// \param [in] _cacheResY
		/// \param [in] _cacheResZ
		//----------------------------------------------------------------------------------
		virtual void SetUseCache( bool _useCache, unsigned int _cacheID, unsigned int _cacheResX, unsigned int _cacheResY, unsigned int _cacheResZ );
		//----------------------------------------------------------------------------------
		/// \brief Returns whether caching is on or not
		/// \return m_useCache
		//----------------------------------------------------------------------------------
		bool GetUseCache() { return m_useCache; }
		//----------------------------------------------------------------------------------
		/// \brief Returns if caching is required
		/// \return m_requiresCache
		//----------------------------------------------------------------------------------
		bool GetRequiresCache() { return m_requiresCache; }
		//----------------------------------------------------------------------------------
		/// \brief Set if caching is required
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetRequiresCache( bool _value ) { m_requiresCache = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Finds out how many nodes require caching in the node's sub-tree
		/// The transparent determines if the node processes its subtree (true if it does)
		//----------------------------------------------------------------------------------
		unsigned int GetSubtreeNumCacheRequires( bool _transparent );
		//----------------------------------------------------------------------------------
		/// \brief Adds all bounding volumes together of cache nodes
		/// Used to work out resolution of cache
		//----------------------------------------------------------------------------------
		float GetSubtreeCacheVolume();
		//----------------------------------------------------------------------------------
		/// \brief For a cache node, this will return the total volume of the subtree
		/// For a non-cache node it will return ? 
		//----------------------------------------------------------------------------------
		//virtual float GetNodeBoundVolume();
		//----------------------------------------------------------------------------------
		/// \brief Get node cost
		//----------------------------------------------------------------------------------
		virtual unsigned int GetNodeCost() = 0;
		//----------------------------------------------------------------------------------
		// \brief Returns the total cost of itself and its sub-tree
		// If the node is caching (either is already told to use cache or requires cache) it returns just the caching cost
		//----------------------------------------------------------------------------------
		unsigned int GetSubtreeCost();
		//----------------------------------------------------------------------------------
		/// \brief Get boundaries sizes
		/// \param [in] _x
		/// \param [in] _y
		/// \param [in] _z
		//----------------------------------------------------------------------------------
		void GetBoundSizes( float *_x, float *_y, float *_z );
		//----------------------------------------------------------------------------------
		/// \brief Get boundaries
		/// \param [in] _minX
		/// \param [in] _maxX
		/// \param [in] _minY 
		/// \param [in] _maxY
		/// \param [in] _minZ
		/// \param [in] _maxZ
		//----------------------------------------------------------------------------------
		virtual void GetBounds( float *_minX, float *_maxX, float *_minY, float *_maxY, float *_minZ, float *_maxZ ) = 0;
		//----------------------------------------------------------------------------------
		/// \brief Update parameters
		/// \param [in] _renderer
		//----------------------------------------------------------------------------------
		void UpdateParameters( GLSLRenderer *_renderer );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Node id 
		//----------------------------------------------------------------------------------
		std::string m_IDString;
		//----------------------------------------------------------------------------------
		/// \brief Flag to check whether to draw bounding boxes or not
		//----------------------------------------------------------------------------------
		bool m_drawBBox;
		//----------------------------------------------------------------------------------
		/// \brief Vao for drawing outline of bounding box
		//----------------------------------------------------------------------------------
		GLuint m_bboxLinesVAO;
		//----------------------------------------------------------------------------------
		/// \brief Vao for drawing sides of bounding box
		//----------------------------------------------------------------------------------
		GLuint m_bboxSidesVAO;
		//----------------------------------------------------------------------------------
		/// \brief If the node has been told to cache it's sub-tree
		//----------------------------------------------------------------------------------
		bool m_useCache;
		//----------------------------------------------------------------------------------
		/// \brief If the node itself says it needs to be cached
		//----------------------------------------------------------------------------------
		bool m_requiresCache;
		//----------------------------------------------------------------------------------
		/// \brief Cache number
		//----------------------------------------------------------------------------------
		int m_cacheNumber;
		//----------------------------------------------------------------------------------
		/// \brief True if the cache has been built
		//----------------------------------------------------------------------------------
		bool m_cacheBuilt;
		//----------------------------------------------------------------------------------
		/// \brief True if the cache needs to be rebuilt
		//----------------------------------------------------------------------------------
		bool m_cacheDirty;
		//----------------------------------------------------------------------------------
		/// \brief Cache resolution along X
		//----------------------------------------------------------------------------------
		unsigned int m_cacheResX;
		//----------------------------------------------------------------------------------
		/// \brief Cache resolution along Y
		//----------------------------------------------------------------------------------
		unsigned int m_cacheResY;
		//----------------------------------------------------------------------------------
		/// \brief Cache resolution along Z
		//----------------------------------------------------------------------------------
		unsigned int m_cacheResZ;
		//----------------------------------------------------------------------------------
		/// \brief Cache offset along X
		//----------------------------------------------------------------------------------
		float m_cacheOffsetX;
		//----------------------------------------------------------------------------------
		/// \brief Cache offset along Y
		//----------------------------------------------------------------------------------
		float m_cacheOffsetY;
		//----------------------------------------------------------------------------------
		/// \brief Cache offset along Z
		//----------------------------------------------------------------------------------
		float m_cacheOffsetZ;
		//----------------------------------------------------------------------------------
		/// \brief Cache scaling along X
		//----------------------------------------------------------------------------------
		float m_cacheScaleX;
		//----------------------------------------------------------------------------------
		/// \brief Cache scaling along Y
		//----------------------------------------------------------------------------------
		float m_cacheScaleY;
		//----------------------------------------------------------------------------------
		/// \brief Cache scaling along Z
		//----------------------------------------------------------------------------------
		float m_cacheScaleZ;
		//----------------------------------------------------------------------------------
		/// \brief Populate cache data
		/// \param [in] _data
		/// \param [in] _startX
		/// \param [in] _startY
		/// \param [in] _startZ
		/// \param [in] _stepX
		/// \param [in] _stepY
		/// \param [in] _stepZ
		//----------------------------------------------------------------------------------
		virtual void PopulateCacheData( float **_data, float _startX, float _startY, float _startZ, float _stepX, float _stepY, float _stepZ );
		//----------------------------------------------------------------------------------
		/// \brief Node is expected to register any parameters it wants to use
		//----------------------------------------------------------------------------------
		virtual void OnBuildParameters( GLSLRenderer *_renderer ) {}
		//----------------------------------------------------------------------------------
		/// \brief Node is expected to tell the renderer the values of its parameters
		//----------------------------------------------------------------------------------
		virtual void OnUpdateParameters( GLSLRenderer *_renderer ) {}
		//----------------------------------------------------------------------------------
		/// \brief Shader for drawing bounding boxes
		//----------------------------------------------------------------------------------
		Shader* m_bboxShader;
		Shader* m_bboxSidesShader;
		//----------------------------------------------------------------------------------
		/// \brief Vertex shader 
		//----------------------------------------------------------------------------------
		std::string m_vertShaderString;
		//----------------------------------------------------------------------------------
		/// \brief Fragment shader
		//----------------------------------------------------------------------------------
		std::string m_fragShaderString;
		//----------------------------------------------------------------------------------

	};
}

#endif /* NODE_H_ */
