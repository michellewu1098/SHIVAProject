#ifndef SDF_CORE_TRIANGLE_AABB_OVERLAP_INCLUDED
#define SDF_CORE_TRIANGLE_AABB_OVERLAP_INCLUDED

#include <sdf/core/types.hpp>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Triangle-AABB overlap.
	/// @param[in] i_centre Centre of the box
	/// @param[in] i_halfextent The hlaf extent (vector from centre to corner)
	/// @param[in] i_first The first triangle vertex
	/// @param[in] i_second The second triangle vertex
	/// @param[in] i_third The third triangle vertex
	/// @return True if they collide false otherwise
	//----------------------------------------------------------------------------------------------------------------------
	bool triangle_box_overlap(const point3d& i_centre, const vector3d& i_halfextent, const point3d& i_first, const point3d& i_second, const point3d& i_third);
}

#endif /* SDF_CORE_TRIANGLE_AABB_OVERLAP_INCLUDED */