///-----------------------------------------------------------------------------------------------
/// \file CylinderNode.h
/// \brief Leaf node for a cylinder with its length in the Z-axis
/// \author Leigh McLoughlin, Michelle Wu
/// \date Jan 9, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef CYLINDER_H_
#define CYLINDER_H_

#include <algorithm>
#include <cmath>
#include <sstream>

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
		/// \param [in] _length
		/// \param [in] _radiusX
		/// \param [in] _radiusY
		//----------------------------------------------------------------------------------
		CylinderNode( const float &_length, const float &_radiusX, const float &_radiusY );
		//----------------------------------------------------------------------------------
		/// \brief Get node type
		/// \return "CylinderNode"
		//----------------------------------------------------------------------------------
		virtual std::string GetNodeType() { return "CylinderNode"; }
		//----------------------------------------------------------------------------------
		/// \brief Set cylinder length
		/// \param [in] _value Length
		//----------------------------------------------------------------------------------
		void SetLength( const float &_value ) { m_length = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Set cylinder radius along x- and y-axis
		/// \param [in] _valueX Radius X
		/// \param [in] _valueY Radius Y
		//----------------------------------------------------------------------------------
		void SetRadius( const float &_valueX, const float &_valueY ) { m_radiusX = _valueX; m_radiusY = _valueY; }
		//----------------------------------------------------------------------------------
		/// \brief Get cylinder length
		/// \return m_length
		//----------------------------------------------------------------------------------
		float GetLength() const { return m_length; }
		//----------------------------------------------------------------------------------
		/// \brief Get cylinder radius along x-axis
		/// \return m_radiusX
		//----------------------------------------------------------------------------------
		float GetRadiusX() const { return m_radiusX; }
		//----------------------------------------------------------------------------------
		/// \brief Get cylinder radius along y-axis
		/// \return m_radiusY
		//----------------------------------------------------------------------------------
		float GetRadiusY() const { return m_radiusY; }
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
		/// \brief Set if cylinder represents totem pole
		/// \param [in] _value Either true or false
		//----------------------------------------------------------------------------------
		void SetPole( const bool &_value ) { m_isPole = _value; }  
		//----------------------------------------------------------------------------------
		/// \brief Set if cylinder represents base of totem pole
		/// \param [in] _value Either true or false
		//----------------------------------------------------------------------------------
		void SetBasePole( const bool &_value ) { m_isBase = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Returns true if cylinder represents totem pole
		/// \return m_isPole
		//----------------------------------------------------------------------------------
		bool isPole() const { return m_isPole; }
		//----------------------------------------------------------------------------------
		/// \brief Returns true if cylinder represents base of totem pole
		/// \return m_isBase
		//----------------------------------------------------------------------------------
		bool isBase() const { return m_isBase; }
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
		/// \brief Length
		//----------------------------------------------------------------------------------
		float m_length;
		//----------------------------------------------------------------------------------
		/// \brief Flag to check whether the cylinder represents the totem pole
		//----------------------------------------------------------------------------------
		bool m_isPole;
		//----------------------------------------------------------------------------------
		/// \brief Flag to check whether the cylinder represents base of totem pole
		//----------------------------------------------------------------------------------
		bool m_isBase;
		//----------------------------------------------------------------------------------

	};
}


#endif /* SPHERE_H_ */
