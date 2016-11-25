/*
 * VolumeTree.h
 *
 *  Created on: Jan 9, 2013
 *      Author: leigh
 */

#ifndef VOLUMETREE_H_
#define VOLUMETREE_H_

#include "VolumeTree/Node.h"
#include "VolumeTree/CachingPolicies/CachingPolicy.h"

#include "vol_totem.h"

class GLSLRenderer;

namespace VolumeTree
{
	/// Holds a tree and provides some extra operations for it
	class Tree
	{
	public:
		Tree();
		Tree(Node *rootNode);
		~Tree();

		void SetRoot(Node *rootNode) {_rootNode=rootNode;}
		Node* GetRoot() {return _rootNode;}

		// TODO
		bool Load(std::string filename);

		// TODO
		bool Save(std::string filename);

		void CalcBoundingBox();
		void GetBoundingBox( float *minvals, float *maxvals);
		void GetBoundingBoxSize( float *boxSize);
		void GetBoundingBoxCentre( float *boxSize);
		float GetBoundingBoxMaxDim();

		/// Draws any nodes which have the option selected to draw their bounding boxes
		void DrawBBoxes();

		/// Samples the function at a specific point
		float GetFunctionValue(float x, float y, float z);

		/// Returns a GLSL-compatible string for the function including caches
		std::string GetCachedFunctionGLSLString();

		/// Returns a GLSL-compatible string for the function
		std::string GetFunctionGLSLString();

		/// The CachingPolicy determines which nodes should be cached
		/// The renderer holds the actual caches
		void BuildCaches(CachingPolicy *policy, GLSLRenderer *renderer);

		void UpdateParameters( GLSLRenderer *renderer );

		int GetNumNodes();
		/// Retrieves the i'th node in a consistent flattened traversal
		Node* GetNode(int index);
		int GetNodeDepth(int index);


	protected:

		Node *_rootNode;
		
		float *_bboxExtents;
		float *_bboxCentre;
		float *_bboxBoundsMin;
		float *_bboxBoundsMax;

		/// Recursively builds node tree
		Node* BuildImportNode(totemio::TotemNode*);

		/// Recursively builds node tree
		totemio::TotemNode* BuildExportNode(Node *currentNode);

		/// Used as node ID for exporting
		unsigned int _exportID;

		/// Returns a string for exporting for when the node's name doesn't matter
		/// Increments _exportID, which should be set to zero when starting to export a new file
		std::string GetExportNodeID( Node *currentNode = NULL );
	};

}


#endif /* VOLUMETREE_H_ */
