///-----------------------------------------------------------------------------------------------
/// \file TorusNode.h
/// \brief Leaf node for a torus with its length in the Y-axis
/// \author Leigh McLoughlin
/// \date Jun 11, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef TORUSNODE_H_
#define TORUSNODE_H_

#include <sstream>
#include <cmath>
#include <algorithm>

#include "VolumeTree/Node.h"

namespace VolumeTree
{
	class TorusNode : public Node
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		TorusNode();
		//----------------------------------------------------------------------------------
		/// \brief Ctor passing sweepRadius and circleRadius
		/// \param [in] _sweepRadius
		/// \param [in] _circleRadius
		//----------------------------------------------------------------------------------
		TorusNode( const float &_sweepRadius, const float &_circleRadius );
		//----------------------------------------------------------------------------------
		/// \brief Returns node type
		/// \return "TorusNode"
		//----------------------------------------------------------------------------------
		virtual std::string GetNodeType() { return "TorusNode"; }
		//----------------------------------------------------------------------------------
		/// \brief Set sweep radius
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetSweepRadius( const float &_value ) { m_sweepRadius = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Set circle radius
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetCircleRadius( const float &_value ) { m_circleRadius = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Returns sweep radius
		/// \return m_sweepRadius
		//----------------------------------------------------------------------------------
		float GetSweepradius() const { return m_sweepRadius; }
		//----------------------------------------------------------------------------------
		/// \brief Returns circle radius
		/// \return m_circleRadius
		//----------------------------------------------------------------------------------
		float GetCircleRadius() const { return m_circleRadius; }
		//----------------------------------------------------------------------------------
		/// \brief Samples the function at a specific point
		/// \param [in] _x
		/// \param [in] _y
		/// \param [in] _z
		//----------------------------------------------------------------------------------
		float GetFunctionValue( float _x, float _y, float _z );
		//----------------------------------------------------------------------------------
		/// \brief Returns a GLSL-compatible string for the function
		/// \param [in] _callCache
		/// \param [in] _samplePosStr
		//----------------------------------------------------------------------------------
		std::string GetFunctionGLSLString( bool _callCache, std::string _samplePosStr );
		//----------------------------------------------------------------------------------
		/// \brief Returns node cost
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
		/// \brief Circle radius
		//----------------------------------------------------------------------------------
		float m_circleRadius;
		//----------------------------------------------------------------------------------
		/// \brief Sweep radius
		//----------------------------------------------------------------------------------
		float m_sweepRadius;
		//----------------------------------------------------------------------------------

	};
}

#endif /* TORUSNODE_H_ */
