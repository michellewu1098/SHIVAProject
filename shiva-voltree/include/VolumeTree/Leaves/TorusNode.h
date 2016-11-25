/*
 * TorusNode.h
 *
 *  Created on: Jun 11, 2013
 *      Author: leigh
 */

#ifndef TORUSNODE_H_
#define TORUSNODE_H_

#include "VolumeTree/Node.h"

namespace VolumeTree
{
	/// Leaf node for a torus with its length in the Y-axis
	class TorusNode : public Node
	{
	public:
		TorusNode();
		TorusNode(float sweepRadius, float circleRadius);
		
		virtual std::string GetNodeType(){return "TorusNode";}

		void SetSweepRadius(float value) {_sweepRadius = value;}
		void SetCircleRadius(float value) {_circleRadius = value;}
		float GetSweepradius() {return _sweepRadius;}
		float GetCircleRadius() {return _circleRadius;}

		/// Samples the function at a specific point
		float GetFunctionValue(float x, float y, float z);

		/// Returns a GLSL-compatible string for the function
		std::string GetFunctionGLSLString(bool callCache, std::string samplePosStr);

		
		virtual unsigned int GetNodeCost() {return 5;}

		virtual void GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ);

	protected:

		float _circleRadius, _sweepRadius;
	};

}


#endif /* TORUSNODE_H_ */
