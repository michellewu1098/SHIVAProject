///-----------------------------------------------------------------------------------------------
/// \file SphereNode.h
/// \brief Leaf node for a sphere
/// \author Leigh McLoughlin
/// \date Jan 9, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef SPHERE_H_
#define SPHERE_H_

#include "VolumeTree/Node.h"

namespace VolumeTree
{
	class SphereNode : public Node
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Default ctor, creates sphere with radius = 1.0f
		//----------------------------------------------------------------------------------
		SphereNode();
		//----------------------------------------------------------------------------------
		/// \brief Ctor of sphere node
		/// \param [in] radius_x Radius X
		/// \param [in] radius_y Radius Y
		/// \param [in] radius_z Radius Z
		//----------------------------------------------------------------------------------
		SphereNode( float radius_x, float radius_y, float radius_z );
		//----------------------------------------------------------------------------------
		/// \brief Get node type
		/// \return "SphereNode"
		//----------------------------------------------------------------------------------
		virtual std::string GetNodeType() { return "SphereNode"; }
		//----------------------------------------------------------------------------------
		/// \brief Set radius 
		/// \param [in] valueX 
		/// \param [in] valueY
		/// \param [in] valueZ
		//----------------------------------------------------------------------------------
		void SetRadius( const float &valueX, const float &valueY, const float &valueZ ) { _radiusX = valueX; _radiusY = valueY; _radiusZ = valueZ; }
		//----------------------------------------------------------------------------------
		/// \brief Get radius along x-axis
		/// \return _radiusX
		//----------------------------------------------------------------------------------
		float GetRadiusX() const { return _radiusX; }
		//----------------------------------------------------------------------------------
		/// \brief Get radius along y-axis
		/// \return _radiusY
		//----------------------------------------------------------------------------------
		float GetRadiusY() const { return _radiusY; }
		//----------------------------------------------------------------------------------
		/// \brief Get radius along z-axis
		/// \return _radiusZ
		//----------------------------------------------------------------------------------
		float GetRadiusZ() const { return _radiusZ; }
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
		/// \brief Radius Z
		//----------------------------------------------------------------------------------
		float _radiusZ;
		//----------------------------------------------------------------------------------
	};

}


#endif /* SPHERE_H_ */
