#ifndef SDF_SIGN_NO_SIGN_INCLUDED
#define SDF_SIGN_NO_SIGN_INCLUDED

#include <sdf/core/types.hpp>
#include <sdf/core/mesh.hpp>
#include <sdf/distance/distance_to_triangle.hpp>

namespace sdf
{
	
	//----------------------------------------------------------------------------------------------------------------------
	/// @class no_sign "include/sdf/sign/no_sign.hpp"
	/// @brief No sign (returns 1 always)
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	class no_sign
	{
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Initialize the object
		/// @param[in] i_mesh The mesh *UNUSED*
		//----------------------------------------------------------------------------------------------------------------------
		void initialize(const mesh& /*i_mesh*/) {}
		void force_init(const mesh& /*i_mesh*/) {}
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the sign of the field (1)
		/// @param[in] i_position Tne point in space to compute the distance from *UNUSED*
		/// @param[in] i_closest_record The hit record from the distance query *UNUSED*
		/// @param[in] i_face_id The face id of the closest face *UNUSED*
		/// @return 1
		//----------------------------------------------------------------------------------------------------------------------
		float operator()(const point3d& /*i_position*/, const distance_record& /*i_closest_record*/, unsigned int /*i_face_id*/) const { return 1; }
		std::size_t memory_usage() const { return 0; }
	};
}

#endif /* SDF_SIGN_NO_SIGN_INCLUDED */