#ifndef SDF_DISTANCE_DISTANCE_TO_AABB_INCLUDED
#define SDF_DISTANCE_DISTANCE_TO_AABB_INCLUDED

#include <sdf/core/aabb.hpp>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class distance_to_aabb "include/sdf/distance/distance_to_aabb.hpp"
	/// @brief Distance to an AABB - shortest distance to the box (corner, edge, face)
	///			Right now, it stays at 0 if it is inside the box
	///			For more explanation check the book "Real-Time Collision Detection" by Christer Ericson, 2005 - Morgran Kaufmann
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	class distance_to_aabb
	{
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructor - position set to the origin
		//----------------------------------------------------------------------------------------------------------------------
		distance_to_aabb();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructor with position
		/// @param[in] i_position Position from
		//----------------------------------------------------------------------------------------------------------------------
		distance_to_aabb(const point3d& i_position);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the shortest squared distance to the box from the point
		/// @param[in] i_box The box to test against
		/// @return The squared distance
		//----------------------------------------------------------------------------------------------------------------------
		float operator()(const aabb& i_box) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the shortest squared distance to the box from the point
		/// @param[in] i_box The box to test against
		/// @param[out] o_pos The position on the box 
		//----------------------------------------------------------------------------------------------------------------------
		void  operator()(const aabb& i_box, point3d* o_pos) const;
	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The position to compute distance from
		//----------------------------------------------------------------------------------------------------------------------
		point3d m_position;
	};
}

#endif // SDF_DISTANCE_DISTANCE_TO_AABB_INCLUDED