#ifndef SDF_LOOKUP_OCTREE_ACCELERATED_INCLUDED
#define SDF_LOOKUP_OCTREE_ACCELERATED_INCLUDED

#include <sdf/lookup/octree.hpp>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class octree_accelerated "include/sdf/lookup/octree_accelerated.hpp"
	/// @brief Using an octree it will look for the shortest distance to the mesh
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	class octree_accelerated
	{
	public :
		typedef detail::settings parameters;
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Default constructor - sets invalid attributes
		//----------------------------------------------------------------------------------------------------------------------
		octree_accelerated();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Initialize the tree (save the mesh)
		/// @param[in] i_mesh The polygonal mesh
		//----------------------------------------------------------------------------------------------------------------------
		void initialize(const mesh& i_mesh);
		void initialize(const mesh& i_mesh, const detail::settings& i_settings);
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
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The actual octree
		//----------------------------------------------------------------------------------------------------------------------
		detail::octree m_tree;
	};
}

#endif /* SDF_LOOKUP_OCTREE_ACCELERATED_INCLUDED */