/*
 * Sphere.h
 *
 *  Created on: Jan 9, 2013
 *      Author: leigh
 */

#ifndef SPHERE_H_
#define SPHERE_H_

#include "VolumeTree/Node.h"

namespace VolumeTree
{
	/// Leaf node for a sphere
	class SphereNode : public Node
	{
	public:
		SphereNode();
		SphereNode(float radius_x, float radius_y, float radius_z);
		
		virtual std::string GetNodeType(){return "SphereNode";}

		void SetRadius(float valueX, float valueY, float valueZ) {_radiusX=valueX; _radiusY=valueY; _radiusZ=valueZ;}
		float GetRadiusX() {return _radiusX;}
		float GetRadiusY() {return _radiusY;}
		float GetRadiusZ() {return _radiusZ;}

		/// Samples the function at a specific point
		float GetFunctionValue(float x, float y, float z);

		/// Returns a GLSL-compatible string for the function
		std::string GetFunctionGLSLString(bool callCache, std::string samplePosStr);

		
		virtual unsigned int GetNodeCost() {return 5;}

		virtual void GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ);

	protected:

		float _radiusX, _radiusY, _radiusZ;
	};

}


#endif /* SPHERE_H_ */
