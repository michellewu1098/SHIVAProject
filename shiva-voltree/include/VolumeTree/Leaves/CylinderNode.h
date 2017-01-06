///-----------------------------------------------------------------------------------------------
/// \file CylinderNode.h
/// \brief Leaf node for a cylinder with its length in the Z-axis
/// \author Leigh McLoughlin
/// \date Jan 9, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef CYLINDER_H_
#define CYLINDER_H_

#include <sstream>
#include <cmath>
#include <algorithm>

#include "VolumeTree/Node.h"

namespace VolumeTree
{
	class CylinderNode : public Node
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		CylinderNode();
		//----------------------------------------------------------------------------------
		/// \brief Cylinder ctor that takes as paramater user specified length, radiusX and radiusY
		/// \param [in] length
		/// \param [in] radiusX
		/// \param [in] radiusY
		//----------------------------------------------------------------------------------
		CylinderNode( float length, float radiusX, float radiusY );
		//----------------------------------------------------------------------------------
		/// \brief Get node type
		/// \return "CylinderNode"
		//----------------------------------------------------------------------------------
		virtual std::string GetNodeType() { return "CylinderNode"; }
		//----------------------------------------------------------------------------------
		/// \brief Set cylinder length
		/// \param [in] value Length
		//----------------------------------------------------------------------------------
		void SetLength( const float &value ) { _length = value; }
		//----------------------------------------------------------------------------------
		/// \brief Set cylinder radius along x- and y-axis
		/// \param [in] valueX Radius X
		/// \param [in] valueY Radius Y
		//----------------------------------------------------------------------------------
		void SetRadius( const float &valueX, const float &valueY ) { _radiusX = valueX; _radiusY = valueY; }
		//----------------------------------------------------------------------------------
		/// \brief Get cylinder length
		/// \return _length
		//----------------------------------------------------------------------------------
		float GetLength() const { return _length; }
		//----------------------------------------------------------------------------------
		/// \brief Get cylinder radius along x-axis
		/// \return _radiusX
		//----------------------------------------------------------------------------------
		float GetRadiusX() const { return _radiusX; }
		//----------------------------------------------------------------------------------
		/// \brief Get cylinder radius along y-axis
		/// \return _radiusY
		//----------------------------------------------------------------------------------
		float GetRadiusY() const { return _radiusY; }
		//----------------------------------------------------------------------------------
		/// \brief Samples the function at a specific point
		/// \param [in] x
		/// \param [in] y
		/// \param [in] z
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
		/// \return 5
		//----------------------------------------------------------------------------------
		virtual unsigned int GetNodeCost() { return 5; }
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

		//----------------------------------------------------------------------------------
		/// \brief Radius X
		//----------------------------------------------------------------------------------
		float _radiusX;
		//----------------------------------------------------------------------------------
		/// \brief Radius Y
		//----------------------------------------------------------------------------------
		float _radiusY;
		//----------------------------------------------------------------------------------
		/// \brief Length
		//----------------------------------------------------------------------------------
		float _length;
		//----------------------------------------------------------------------------------
	};

}


#endif /* SPHERE_H_ */
