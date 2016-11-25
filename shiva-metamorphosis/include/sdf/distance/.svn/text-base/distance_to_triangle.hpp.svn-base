#ifndef SDF_DISTANCE_TO_TRIANGLE_INCLUDED
#define SDF_DISTANCE_TO_TRIANGLE_INCLUDED

#include <sdf/core/types.hpp>
#include <sdf/core/feature_type.hpp>
#include <sdf/distance/distance_record.hpp>

#undef max

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class distance_to_triangle "include/sdf/distance/distance_to_triangle.hpp"
	/// @brief A class to compute the distance from a point to a triangle
	///		   The distance to a triangle is the shortest distance to a triangle
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	class distance_to_triangle
	{
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Default constructor - origin point is set to (0,0,0)
		//----------------------------------------------------------------------------------------------------------------------
		distance_to_triangle();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructor
		/// @param[in] i_position The reference position to measure distances to
		//----------------------------------------------------------------------------------------------------------------------
		distance_to_triangle(const point3d& i_position);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The distance operator to a triangle
		/// @param[in] i_p1 The first vertex of the triangle
		/// @param[in] i_p2 The second vertex of the triangle
		/// @param[in] i_p3 The thrid vertex of the triangle
		/// @param[out] o_record The distance record
		/// @return The distance squared
		//----------------------------------------------------------------------------------------------------------------------
		inline float operator()(const point3d& i_p1, const point3d& i_p2, const point3d& i_p3, distance_record* o_record) const { return barycentric_method(i_p1,i_p2,i_p3,o_record); };
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the distance record (feature type, distance squared, closest point) by projection and barycentric coordinates
		/// @param[in] i_p1 The first vertex of the triangle
		/// @param[in] i_p2 The second vertex of the triangle
		/// @param[in] i_p3 The thrid vertex of the triangle
		/// @param[out] o_record The distance record
		/// @return The distance squared
		//----------------------------------------------------------------------------------------------------------------------
		float barycentric_method(const point3d& i_p1, const point3d& i_p2, const point3d& i_p3, distance_record* o_record) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the distance record (feature type, distance squared, closest point) by calculus (finding the minimum of the distance function)
		/// @param[in] i_p1 The first vertex of the triangle
		/// @param[in] i_p2 The second vertex of the triangle
		/// @param[in] i_p3 The thrid vertex of the triangle
		/// @param[out] o_record The distance record
		/// @return The distance squared
		//----------------------------------------------------------------------------------------------------------------------
		float calculus_method(const point3d& i_p1, const point3d& i_p2, const point3d& i_p3, distance_record* o_record) const;
	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The reference position
		//----------------------------------------------------------------------------------------------------------------------
		point3d m_position;
	};
}

#endif /* SDF_DISTANCE_TO_TRIANGLE_INCLUDED */