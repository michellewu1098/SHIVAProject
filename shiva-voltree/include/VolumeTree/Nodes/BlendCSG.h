/*
 * BlendCSG.h
 *
 *  Created on: Jun 13, 2013
 *      Author: leigh
 */

#ifndef BLENDCSG_H_
#define BLENDCSG_H_

#include "VolumeTree/Nodes/CSG.h"

namespace VolumeTree
{
	/// Leaf node for a sphere
	class BlendCSGNode : public CSGNode
	{
	public:
		BlendCSGNode();
		BlendCSGNode(Node *childA, Node *childB);
		virtual ~BlendCSGNode();
		
		virtual std::string GetNodeType(){return "BlendCSGNode";}
		void GetBlendParams(float &a0, float &a1, float &a2) {a0=_a0;a1=_a1;a2=_a2;}
		void SetBlendParams(float a0, float a1, float a2) {_a0=a0;_a1=a1;_a2=a2;}


		/// Samples the function at a specific point
		float GetFunctionValue(float x, float y, float z);

		/// Returns a GLSL-compatible string for the function
		std::string GetFunctionGLSLString(bool callCache, std::string samplePosStr);


		virtual unsigned int GetNodeCost() {return 4;}

		// TODO
		virtual void GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ);

	protected:
		float _a0, _a1, _a2;
	};

}


#endif /* BLENDCSG_H_ */
