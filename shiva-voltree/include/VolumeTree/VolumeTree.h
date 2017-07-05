///-----------------------------------------------------------------------------------------------
/// \file VolumeTree.h
/// \brief Holds a tree and provides some extra operations for it
/// \author Leigh McLoughlin, Michelle Wu
/// \date Jan 9, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef VOLUMETREE_H_
#define VOLUMETREE_H_

#include <boost/filesystem.hpp>
#include <stack>
#include <queue>

#include "VolumeTree/CachingPolicies/CachingPolicy.h"
#include "Utility/tinyxml.h"

#include "vol_totem.h"

class GLSLRenderer;

namespace VolumeTree
{
	class Tree
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		Tree();
		//----------------------------------------------------------------------------------
		/// \brief Ctor
		/// \param [in] _rootNode
		//----------------------------------------------------------------------------------
		Tree( Node *_rootNode );
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~Tree();
		//----------------------------------------------------------------------------------
		/// \brief Set root node
		/// \param [in] _rootNode
		//----------------------------------------------------------------------------------
		void SetRoot( Node *_rootNode ) { m_rootNode = _rootNode; }
		//----------------------------------------------------------------------------------
		/// \brief Get root node
		/// \return _rootNode
		//----------------------------------------------------------------------------------
		Node* GetRoot() const { return m_rootNode; }
		//----------------------------------------------------------------------------------
		/// \brief Load .vol file function 
		/// \param [in] _filename
		//----------------------------------------------------------------------------------
		bool Load( const char* _filename );
		//----------------------------------------------------------------------------------
		/// \brief Save .xml file (internal format)
		/// \param [in] _filename File name
		//----------------------------------------------------------------------------------
		void SaveXML( std::string _filename );
		//----------------------------------------------------------------------------------
		/// \brief Import model from .xml file
		/// \param [in] _filename File name
		//----------------------------------------------------------------------------------
		bool ImportXML( const char* _filename );
		//----------------------------------------------------------------------------------
		/// \brief test if the model is printable
		/// \param [in] bIncludePole Flag to check if the pole is included
		/// \param [in] bIncludeBase Flag to check if the base of the totem is included
		//----------------------------------------------------------------------------------
		bool IsPrintable( bool bIncludePole = true, bool bIncludeBase = true);
		//----------------------------------------------------------------------------------
		/// \brief Import model from .vol file
		/// \param [in] _filename File name
		//----------------------------------------------------------------------------------
		bool ImportVol( const char* _filename );
		//----------------------------------------------------------------------------------
		/// \brief Save .vol file function
		/// \param [in] _filename File name
		//----------------------------------------------------------------------------------
		bool Save( std::string _filename );
		//----------------------------------------------------------------------------------
		/// \brief Save .obj file function
		/// \param [in] _filename File name
		//----------------------------------------------------------------------------------
		bool SaveMesh( std::string _filename, bool bIncludePole = true, bool bIncludeBase = true );
		//----------------------------------------------------------------------------------
		/// \brief Calculate bounding box
		//----------------------------------------------------------------------------------
		void CalcBoundingBox();
		//----------------------------------------------------------------------------------
		/// \brief Get bounding box
		/// \param [in] _minvals
		/// \param [in] _maxvals
		//----------------------------------------------------------------------------------
		void GetBoundingBox( float *_minvals, float *_maxvals );
		//----------------------------------------------------------------------------------
		/// \brief Get bounding box size
		/// \param [in] -boxSize
		//----------------------------------------------------------------------------------
		void GetBoundingBoxSize( float *_boxSize );
		//----------------------------------------------------------------------------------
		/// \brief Get bounding box centre
		/// \param [in] _boxSize
		//----------------------------------------------------------------------------------
		void GetBoundingBoxCentre( float *_boxSize );
		//----------------------------------------------------------------------------------
		/// \brief Get bounding box maximum dimension
		//----------------------------------------------------------------------------------
		float GetBoundingBoxMaxDim();
		//----------------------------------------------------------------------------------
		/// \brief Draws any nodes which have the option selected to draw their bounding boxes
		/// \param [in] _proj Projection matrix
		/// \param [in] _mv ModelView matrix
		//----------------------------------------------------------------------------------
		void DrawBBoxes( cml::matrix44f_c &_proj, cml::matrix44f_c &_mv );
		//----------------------------------------------------------------------------------
		/// \brief Draws any nodes which have the option selected to draw their bounding boxes
		/// \param [in] _proj Projection matrix
		/// \param [in] _mv ModelView matrix
		/// \param [in] _context Current OGL context
		//----------------------------------------------------------------------------------
		void DrawBBoxes( cml::matrix44f_c &_proj, cml::matrix44f_c &_mv, unsigned int _context );
		//----------------------------------------------------------------------------------
		/// \brief Samples the function at a specific point
		/// \param [in] _x
		/// \param [in] _y
		/// \param [in] _z
		//----------------------------------------------------------------------------------
		float GetFunctionValue( float _x, float _y, float _z );
		//----------------------------------------------------------------------------------
		/// \brief Returns a GLSL-compatible string for the function including caches
		//----------------------------------------------------------------------------------
		std::string GetCachedFunctionGLSLString();
		//----------------------------------------------------------------------------------
		/// \brief Returns a GLSL-compatible string for the function
		//----------------------------------------------------------------------------------
		std::string GetFunctionGLSLString();
		//----------------------------------------------------------------------------------
		/// \brief Build caches
		/// \param [in] _policy Determines which nodes should be cached
		/// \param [in] _renderer Holds the actual caches
		//----------------------------------------------------------------------------------
		void BuildCaches( CachingPolicy *_policy, GLSLRenderer *_renderer );
		//----------------------------------------------------------------------------------
		/// \brief Update parameters
		/// \param [in] _renderer
		//----------------------------------------------------------------------------------
		void UpdateParameters( GLSLRenderer *_renderer );
		//----------------------------------------------------------------------------------
		/// \brief Get tot number of nodes
		//----------------------------------------------------------------------------------
		int GetNumNodes();
		//----------------------------------------------------------------------------------
		/// \brief Retrieves the i'th node in a consistent flattened traversal
		/// \param [in] _index
		//----------------------------------------------------------------------------------
		Node* GetNode( int _index );
		//----------------------------------------------------------------------------------
		/// \brief Get node depth in the tree
		/// \param [in] _index
		//----------------------------------------------------------------------------------
		int GetNodeDepth( int _index );
		//----------------------------------------------------------------------------------
		/// \brief Get tree reversed
		//----------------------------------------------------------------------------------
		std::queue< Node* > GetReverseTree();
		//----------------------------------------------------------------------------------

		// 3D PRINTING 

		//----------------------------------------------------------------------------------
		void Print();
		//----------------------------------------------------------------------------------


	protected:


		//----------------------------------------------------------------------------------
		/// \brief Root node
		//----------------------------------------------------------------------------------
		Node *m_rootNode;
		//----------------------------------------------------------------------------------
		/// \brief Bounding box extents
		//----------------------------------------------------------------------------------
		float *m_bboxExtents;
		//----------------------------------------------------------------------------------
		/// \brief Bounding box centre
		//----------------------------------------------------------------------------------
		float *m_bboxCentre;
		//----------------------------------------------------------------------------------
		/// \brief Bounding box min
		//----------------------------------------------------------------------------------
		float *m_bboxBoundsMin;
		//----------------------------------------------------------------------------------
		/// \brief Bounding box max
		//----------------------------------------------------------------------------------
		float *m_bboxBoundsMax;
		//----------------------------------------------------------------------------------
		/// \brief Recursively builds node tree
		/// \param [in] node
		//----------------------------------------------------------------------------------
		Node* BuildImportNode( totemio::TotemNode* );
		//----------------------------------------------------------------------------------
		/// \brief Recursively builds node tree
		/// \param [in] currentNode
		//----------------------------------------------------------------------------------
		totemio::TotemNode* BuildExportNode( Node *currentNode );
		//----------------------------------------------------------------------------------
		/// \brief Export node tree as .xml file
		/// \param [in] _currentNode
		/// \param [in] _root Root element in the xml file 
		//----------------------------------------------------------------------------------
		void ExportToXML( Node *_currentNode, TiXmlElement *_root );
		//----------------------------------------------------------------------------------
		/// \brief Import model from .xml file
		/// \param [in] _root Root element in the xml file
		//----------------------------------------------------------------------------------
		Node* ImportFromXML( TiXmlElement *_root );
		//----------------------------------------------------------------------------------
		/// \brief Used as node ID for exporting
		//----------------------------------------------------------------------------------
		unsigned int m_exportID;
		//----------------------------------------------------------------------------------
		/// \brief Returns a string for exporting for when the node's name doesn't matter
		/// Increments _exportID, which should be set to zero when starting to export a new file
		/// \param [in] currentNode
		//----------------------------------------------------------------------------------
		std::string GetExportNodeID( Node *currentNode = NULL );
		//----------------------------------------------------------------------------------
		

	};
}

#endif /* VOLUMETREE_H_ */
