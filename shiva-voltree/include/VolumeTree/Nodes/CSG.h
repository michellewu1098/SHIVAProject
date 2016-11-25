/*
 * CSG.h
 *
 *  Created on: Jan 9, 2013
 *      Author: leigh
 */

#ifndef CSG_H_
#define CSG_H_

#include "VolumeTree/Node.h"

namespace VolumeTree
{
	/// Leaf node for a sphere
	class CSGNode : public Node
	{
	public:
		enum CSGType
		{
			CSG_UNION,
			CSG_INTERSECTION,
			CSG_SUBTRACTION
		};

		CSGNode();
		CSGNode(Node *childA, Node *childB);
		virtual ~CSGNode();
		
		virtual std::string GetNodeType(){return "CSGNode";}

		CSGType GetCSGType() {return _CSGType;}
		void SetCSGType(CSGType value) {_CSGType = value;}

		/// Samples the function at a specific point
		float GetFunctionValue(float x, float y, float z);

		/// Returns a GLSL-compatible string for the function
		std::string GetFunctionGLSLString(bool callCache, std::string samplePosStr);

		void SetChildA(Node *child){_childA=child;}
		void SetChildB(Node *child){_childB=child;}

		
		virtual Node* GetFirstChild();
		virtual Node* GetNextChild(Node *previousChild);
		
		virtual unsigned int GetNodeCost() {return 2;}

		virtual void GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ);

	protected:
		Node *_childA, *_childB;

		CSGType _CSGType;
	};

}


#endif /* CSG_H_ */
