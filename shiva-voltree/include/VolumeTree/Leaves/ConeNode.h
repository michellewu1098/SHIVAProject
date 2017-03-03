///-----------------------------------------------------------------------------------------------
/// \file ConeNode.h
/// \brief Leaf node for a cone
/// \author Leigh McLoughlin
/// \date Jun 12, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef CONENODE_H_
#define CONENODE_H_

#include <sstream>
#include <cmath>
#include <algorithm>

#include "VolumeTree/Node.h"

namespace VolumeTree
{
	class ConeNode : public Node
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		ConeNode();
		//----------------------------------------------------------------------------------
		/// \brief Ctor passing lenght and radius of cone
		/// \param [in] _length
		/// \param [in] _radius
		//----------------------------------------------------------------------------------
		ConeNode( const float &_length, const float &_radius );
		//----------------------------------------------------------------------------------
		/// \brief Get node type
		/// \return "ConeNode"
		//----------------------------------------------------------------------------------
		virtual std::string GetNodeType() { return "ConeNode"; }
		//----------------------------------------------------------------------------------
		/// \brief Set cone length
		/// \param [in] _value Length value
		//----------------------------------------------------------------------------------
		void SetLength( const float &_value ) { m_length = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Set cone radius
		/// \param [in] _value Radius value
		//----------------------------------------------------------------------------------
		void SetRadius( const float &_value ) { m_radius = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Returns length of cone
		/// \return m_length
		//----------------------------------------------------------------------------------
		float GetLength() const { return m_length; }
		//----------------------------------------------------------------------------------
		/// \brief Returns radius of cone
		/// \return m_radius
		//----------------------------------------------------------------------------------
		float GetRadius() const { return m_radius; }
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
		/// \brief Cone radius
		//----------------------------------------------------------------------------------
		float m_radius;
		//----------------------------------------------------------------------------------
		/// \brief Cone length
		//----------------------------------------------------------------------------------
		float m_length;
		//----------------------------------------------------------------------------------

	};
}


#endif /* CONENODE_H_ */
