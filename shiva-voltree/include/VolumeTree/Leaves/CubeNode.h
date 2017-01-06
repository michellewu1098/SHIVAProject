///-----------------------------------------------------------------------------------------------
/// \file CubeNode.h
/// \brief Leaf node for a cube
/// \author Leigh McLoughlin
/// \date Jun 12, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef CUBENODE_H_
#define CUBENODE_H_

#include "VolumeTree/Node.h"

namespace VolumeTree
{
	class CubeNode : public Node
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		CubeNode();
		//----------------------------------------------------------------------------------
		/// \brief Ctor for cube with same side length
		/// \param [in] length
		//----------------------------------------------------------------------------------
		CubeNode( float length );
		//----------------------------------------------------------------------------------
		/// \brief Ctor for box with different side lengths
		/// \param [in] lengthX
		/// \param [in] lengthY
		/// \param [in] lengthZ
		//----------------------------------------------------------------------------------
		CubeNode( float lengthX, float lengthY, float lengthZ );
		//----------------------------------------------------------------------------------
		/// \brief Get node type
		/// \return "CubeNode"
		//----------------------------------------------------------------------------------
		virtual std::string GetNodeType(){ return "CubeNode"; }
		//----------------------------------------------------------------------------------
		/// \brief Set length of cube
		/// \param [in] value
		//----------------------------------------------------------------------------------
		void SetLength( const float &value ) { _lengthX = _lengthY = _lengthZ = value; }
		//----------------------------------------------------------------------------------
		/// \brief Set lengths of cube
		/// \param [in] valueX
		/// \param [in] valueY
		/// \param [in] valueZ
		//----------------------------------------------------------------------------------
		void SetLength( const float &valueX, const float &valueY, const float &valueZ) { _lengthX = valueX; _lengthY = valueY; _lengthZ = valueZ; }
		//----------------------------------------------------------------------------------
		/// \brief Get length along x-axis
		/// \return _lengthX
		//----------------------------------------------------------------------------------
		float GetLengthX() const { return _lengthX; }
		//----------------------------------------------------------------------------------
		/// \brief Get length along y-axis
		/// \return _lengthY
		//----------------------------------------------------------------------------------
		float GetLengthY() const { return _lengthY; }
		//----------------------------------------------------------------------------------
		/// \brief Get length along z-axis
		/// \return _lengthZ
		//----------------------------------------------------------------------------------
		float GetLengthZ() const { return _lengthZ; }
		//----------------------------------------------------------------------------------
		/// \brief Samples the function at a specific point
		/// \param [in] x X coord
		/// \param [in] y Y coord
		/// \param [in] z Z coord
		//----------------------------------------------------------------------------------
		float GetFunctionValue( float x, float y, float z );
		//----------------------------------------------------------------------------------
		/// \brief Returns a GLSL-compatible string for the function
		/// \param [in] callCache
		/// \param [in] samplePosStr 
		//----------------------------------------------------------------------------------
		std::string GetFunctionGLSLString( bool callCache, std::string samplePosStr );
		//----------------------------------------------------------------------------------
		/// \brief Get node constant
		/// \return 15
		//----------------------------------------------------------------------------------
		virtual unsigned int GetNodeCost() { return 15; }
		//----------------------------------------------------------------------------------
		/// \brief Get boundaries
		/// \param [out] minX
		/// \param [out] maxX
		/// \param [out] minY
		/// \param [out] maxY
		/// \param [out] minZ
		/// \param [out] maxZ
		//----------------------------------------------------------------------------------
		virtual void GetBounds( float *minX, float *maxX, float *minY, float *maxY, float *minZ, float *maxZ );
		//----------------------------------------------------------------------------------

	protected:

		float CSG_Intersect( float f1, float f2 );
		//----------------------------------------------------------------------------------
		/// \brief Length along x-axis
		//----------------------------------------------------------------------------------
		float _lengthX;
		//----------------------------------------------------------------------------------
		/// \brief Length along y-axis
		//----------------------------------------------------------------------------------
		float _lengthY;
		//----------------------------------------------------------------------------------
		/// \brief Length along z-axis
		//----------------------------------------------------------------------------------
		float _lengthZ;
		//----------------------------------------------------------------------------------
	};

}


#endif /* CONENODE_H_ */
