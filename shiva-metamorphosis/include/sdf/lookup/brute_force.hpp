#ifndef SDF_LOOKUP_BRUTE_FORCE_INCLUDED
#define SDF_LOOKUP_BRUTE_FORCE_INCLUDED

#include <sdf/core/types.hpp>
#include <sdf/core/mesh.hpp>
#include <sdf/distance/distance_to_triangle.hpp>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class brute_force "include/sdf/lookup/brute_force.hpp"
	/// @brief A brute force lookup for the closest distance. It is going to loop over them all.
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	class brute_force
	{
	public :
		typedef char parameters;
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Default constructor - sets invalid attributes
		//----------------------------------------------------------------------------------------------------------------------
		brute_force();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Initialize the brute force method (save the mesh)
		/// @param[in] i_mesh The polygonal mesh
		//----------------------------------------------------------------------------------------------------------------------
		void initialize(const mesh& i_mesh);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the shortest distance to the mesh
		/// @param[in] i_position Tne point in space to compute the distance from
		/// @param[out] o_closest_record A record of the closest hit (feature type, distance)
		/// @param[out] o_face_id The face id will be filled in there
		//----------------------------------------------------------------------------------------------------------------------
		void operator()(const point3d& i_position, distance_record* o_closest_record, unsigned int* o_face_id) const;
	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief A pointer to the mesh - pointer is only valid after initialize has been called with appropriate mesh
		//----------------------------------------------------------------------------------------------------------------------
		const mesh* m_mesh;
	};
}

#endif /* SDF_LOOKUP_BRUTE_FORCE_INCLUDED */