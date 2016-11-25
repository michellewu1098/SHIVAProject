/*
 * CylinderNode.h
 *
 *  Created on: Jun 11, 2013
 *      Author: leigh
 */

#ifndef CYLINDER_H_
#define CYLINDER_H_

#include "VolumeTree/Node.h"

namespace VolumeTree
{
	/// Leaf node for a cylinder with its length in the Z-axis
	class CylinderNode : public Node
	{
	public:
		CylinderNode();
		CylinderNode(float length, float radiusX, float radiusY);
		
		virtual std::string GetNodeType(){return "CylinderNode";}

		void SetLength(float value) {_length = value;}
		void SetRadius(float valueX, float valueY) {_radiusX = valueX; _radiusY = valueY;}
		float GetLength() {return _length;}
		float GetRadiusX() {return _radiusX;}
		float GetRadiusY() {return _radiusY;}

		/// Samples the function at a specific point
		float GetFunctionValue(float x, float y, float z);

		/// Returns a GLSL-compatible string for the function
		std::string GetFunctionGLSLString(bool callCache, std::string samplePosStr);

		
		virtual unsigned int GetNodeCost() {return 5;}

		virtual void GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ);

	protected:

		float _radiusX, _radiusY, _length;
	};

}


#endif /* SPHERE_H_ */
