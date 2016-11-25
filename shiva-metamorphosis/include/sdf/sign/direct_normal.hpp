#ifndef SDF_SIGN_DIRECT_NORMAL_INCLUDED
#define SDF_SIGN_DIRECT_NORMAL_INCLUDED

#include <sdf/core/types.hpp>
#include <sdf/core/mesh.hpp>
#include <sdf/distance/distance_to_triangle.hpp>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class direct_normal "include/sdf/sign/direct_normal.hpp"
	/// @brief To compute the sign of the distance field
	///        It is the simplest implementation to compute the sign
	///		   It only uses the dot product from the point to the closest point on the mesh
	///		   This technique is not handle sharp edges correctly
	///		   angle_weighted_average is a better solution
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	class direct_normal
	{
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Default constructor
		//----------------------------------------------------------------------------------------------------------------------
		direct_normal();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Initialize the object
		/// @param[in] i_mesh The mesh - it will keep a pointer to it
		//----------------------------------------------------------------------------------------------------------------------
		void initialize(const mesh& i_mesh);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the sign of the field 
		/// @param[in] i_position Tne point in space to compute the distance from
		/// @param[in] i_closest_record The hit record from the distance query
		/// @param[in] i_face_id The face id of the closest face
		/// @return -1 or 1. It is the value the distance should be multiplied by.
		//----------------------------------------------------------------------------------------------------------------------
		float operator()(const point3d& i_position, const distance_record& i_closest_record, unsigned int i_face_id) const;
		std::size_t memory_usage() const { return 0; }
	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief A pointer to the mesh - pointer is only valid after initialize has been called with appropriate mesh
		//----------------------------------------------------------------------------------------------------------------------
		const mesh* m_mesh;
	};
}

#endif // SDF_SIGN_DIRECT_NORMAL_INCLUDED