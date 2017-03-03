///-----------------------------------------------------------------------------------------------
/// \file SphereNode.h
/// \brief Leaf node for a sphere
/// \author Leigh McLoughlin
/// \date Jan 9, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef SPHERE_H_
#define SPHERE_H_

#include <cmath>
#include <sstream>

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
		/// \param [in] _radiusX Radius X
		/// \param [in] _radiusY Radius Y
		/// \param [in] _radiusZ Radius Z
		//----------------------------------------------------------------------------------
		SphereNode( const float &_radiusX, const float &_radiusY, const float &_radiusZ );
		//----------------------------------------------------------------------------------
		/// \brief Get node type
		/// \return "SphereNode"
		//----------------------------------------------------------------------------------
		virtual std::string GetNodeType() { return "SphereNode"; }
		//----------------------------------------------------------------------------------
		/// \brief Set radius 
		/// \param [in] _valueX 
		/// \param [in] _valueY
		/// \param [in] _valueZ
		//----------------------------------------------------------------------------------
		void SetRadius( const float &_valueX, const float &_valueY, const float &_valueZ ) { m_radiusX = _valueX; m_radiusY = _valueY; m_radiusZ = _valueZ; }
		//----------------------------------------------------------------------------------
		/// \brief Get radius along x-axis
		/// \return m_radiusX
		//----------------------------------------------------------------------------------
		float GetRadiusX() const { return m_radiusX; }
		//----------------------------------------------------------------------------------
		/// \brief Get radius along y-axis
		/// \return m_radiusY
		//----------------------------------------------------------------------------------
		float GetRadiusY() const { return m_radiusY; }
		//----------------------------------------------------------------------------------
		/// \brief Get radius along z-axis
		/// \return m_radiusZ
		//----------------------------------------------------------------------------------
		float GetRadiusZ() const { return m_radiusZ; }
		//----------------------------------------------------------------------------------
		/// \brief Samples the function at a specific point
		/// \param [in] _x X coord
		/// \param [in] _y Y coord
		/// \param [in] _z Z coord
		//----------------------------------------------------------------------------------
		float GetFunctionValue( float _x, float _y, float _z );
		//----------------------------------------------------------------------------------
		/// \brief Returns a GLSL-compatible string for the function
		/// \param [in] _callCache
		/// \param [in] _samplePosStr 
		//----------------------------------------------------------------------------------
		std::string GetFunctionGLSLString( bool _callCache, std::string _samplePosStr );
		//----------------------------------------------------------------------------------
		/// \brief Get node cost
		/// \return 5
		//----------------------------------------------------------------------------------
		virtual unsigned int GetNodeCost() { return 5; }
		//----------------------------------------------------------------------------------
		/// \brief Get boundaries
		/// \param [out] _minX
		/// \param [out] _maxX
		/// \param [out] _minY
		/// \param [out] _maxY
		/// \param [out] _minZ
		/// \param [out] _maxZ
		//----------------------------------------------------------------------------------
		virtual void GetBounds( float *_minX, float *_maxX, float *_minY, float *_maxY, float *_minZ, float *_maxZ );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Radius X
		//----------------------------------------------------------------------------------
		float m_radiusX;
		//----------------------------------------------------------------------------------
		/// \brief Radius Y
		//----------------------------------------------------------------------------------
		float m_radiusY;
		//----------------------------------------------------------------------------------
		/// \brief Radius Z
		//----------------------------------------------------------------------------------
		float m_radiusZ;
		//----------------------------------------------------------------------------------
	};
}

#endif /* SPHERE_H_ */
