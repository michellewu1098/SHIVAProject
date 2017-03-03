///-----------------------------------------------------------------------------------------------
/// \file CubeNode.h
/// \brief Leaf node for a cube
/// \author Leigh McLoughlin
/// \date Jun 12, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef CUBENODE_H_
#define CUBENODE_H_

#include <sstream>
#include <cmath>
#include <algorithm>

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
		/// \param [in] _length
		//----------------------------------------------------------------------------------
		CubeNode( const float &_length );
		//----------------------------------------------------------------------------------
		/// \brief Ctor for box with different side lengths
		/// \param [in] _lengthX
		/// \param [in] _lengthY
		/// \param [in] _lengthZ
		//----------------------------------------------------------------------------------
		CubeNode( const float &_lengthX, const float &_lengthY, const float &_lengthZ );
		//----------------------------------------------------------------------------------
		/// \brief Get node type
		/// \return "CubeNode"
		//----------------------------------------------------------------------------------
		virtual std::string GetNodeType() { return "CubeNode"; }
		//----------------------------------------------------------------------------------
		/// \brief Set length of cube
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetLength( const float &_value ) { m_lengthX = m_lengthY = m_lengthZ = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Set lengths of cube
		/// \param [in] _valueX
		/// \param [in] _valueY
		/// \param [in] _valueZ
		//----------------------------------------------------------------------------------
		void SetLength( const float &_valueX, const float &_valueY, const float &_valueZ) { m_lengthX = _valueX; m_lengthY = _valueY; m_lengthZ = _valueZ; }
		//----------------------------------------------------------------------------------
		/// \brief Get length along x-axis
		/// \return m_lengthX
		//----------------------------------------------------------------------------------
		float GetLengthX() const { return m_lengthX; }
		//----------------------------------------------------------------------------------
		/// \brief Get length along y-axis
		/// \return m_lengthY
		//----------------------------------------------------------------------------------
		float GetLengthY() const { return m_lengthY; }
		//----------------------------------------------------------------------------------
		/// \brief Get length along z-axis
		/// \return m_lengthZ
		//----------------------------------------------------------------------------------
		float GetLengthZ() const { return m_lengthZ; }
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
		/// \return 15
		//----------------------------------------------------------------------------------
		virtual unsigned int GetNodeCost() { return 15; }
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
		/// \brief CSG Intersection
		/// \param [in] _f1
		/// \param [in] _f2
		//----------------------------------------------------------------------------------
		float CSG_Intersect( float _f1, float _f2 );
		//----------------------------------------------------------------------------------
		/// \brief Length along x-axis
		//----------------------------------------------------------------------------------
		float m_lengthX;
		//----------------------------------------------------------------------------------
		/// \brief Length along y-axis
		//----------------------------------------------------------------------------------
		float m_lengthY;
		//----------------------------------------------------------------------------------
		/// \brief Length along z-axis
		//----------------------------------------------------------------------------------
		float m_lengthZ;
		//----------------------------------------------------------------------------------
	
	};
}

#endif /* CONENODE_H_ */
