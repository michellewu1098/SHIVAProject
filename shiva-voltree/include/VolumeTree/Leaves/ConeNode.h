/*
 * ConeNode.h
 *
 *  Created on: Jun 12, 2013
 *      Author: leigh
 */

#ifndef CONENODE_H_
#define CONENODE_H_

#include "VolumeTree/Node.h"

namespace VolumeTree
{
	/// Leaf node for a cylinder with its length in the Z-axis
	class ConeNode : public Node
	{
	public:
		ConeNode();
		ConeNode(float length, float radius);
		
		virtual std::string GetNodeType(){return "ConeNode";}

		void SetLength(float value) {_length = value;}
		void SetRadius(float value) {_radius = value;}
		float GetLength() {return _length;}
		float GetRadius() {return _radius;}

		/// Samples the function at a specific point
		float GetFunctionValue(float x, float y, float z);

		/// Returns a GLSL-compatible string for the function
		std::string GetFunctionGLSLString(bool callCache, std::string samplePosStr);

		
		virtual unsigned int GetNodeCost() {return 5;}

		virtual void GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ);

	protected:

		float _radius, _length;
	};

}


#endif /* CONENODE_H_ */
