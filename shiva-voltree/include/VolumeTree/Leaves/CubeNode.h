/*
 * CubeNode.h
 *
 *  Created on: Jun 12, 2013
 *      Author: leigh
 */

#ifndef CUBENODE_H_
#define CUBENODE_H_

#include "VolumeTree/Node.h"

namespace VolumeTree
{
	/// Leaf node for a cube
	class CubeNode : public Node
	{
	public:
		CubeNode();
		CubeNode(float length);
		CubeNode(float lengthX, float lengthY, float lengthZ);
		
		virtual std::string GetNodeType(){return "CubeNode";}

		void SetLength(float value) {_lengthX = _lengthY = _lengthZ = value;}
		void SetLength(float valueX, float valueY, float valueZ) {_lengthX = valueX;_lengthY = valueY;_lengthZ = valueZ;}
		void SetLengthX( const float &valueX ) { _lengthX = valueX; }
		void SetLengthY( const float &valueY ) { _lengthY = valueY; }
		void SetLengthZ( const float &valueZ ) { _lengthZ = valueZ; }
		float GetLengthX() {return _lengthX;}
		float GetLengthY() {return _lengthY;}
		float GetLengthZ() {return _lengthZ;}

		/// Samples the function at a specific point
		float GetFunctionValue(float x, float y, float z);

		/// Returns a GLSL-compatible string for the function
		std::string GetFunctionGLSLString(bool callCache, std::string samplePosStr);

		
		virtual unsigned int GetNodeCost() {return 15;}

		virtual void GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ);

	protected:

		float CSG_Intersect(float f1, float f2);

		float _lengthX, _lengthY, _lengthZ;
	};

}


#endif /* CONENODE_H_ */
