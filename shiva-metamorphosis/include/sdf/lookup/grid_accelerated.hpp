#ifndef SDF_LOOKUP_GRID_ACCELERATED_INCLUDED
#define SDF_LOOKUP_GRID_ACCELERATED_INCLUDED

#include <sdf/lookup/regular_grid.hpp>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class grid_accelerated "include/sdf/lookup/grid_accelerated.hpp"
	/// @brief Using a uniform grid it will look for the shortest distance to the mesh
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	class grid_accelerated
	{
	public :
		typedef detail::cell_weights parameters;
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Default constructor - sets invalid attributes
		//----------------------------------------------------------------------------------------------------------------------
		grid_accelerated();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Initialize the tree (save the mesh) using default settings (0.5, 0.5 weights)
		/// @param[in] i_mesh The polygonal mesh
		//----------------------------------------------------------------------------------------------------------------------
		void initialize(const mesh& i_mesh);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Initialize the tree (save the mesh)
		/// @param[in] i_mesh The polygonal mesh
		/// @param[in] i_weights The cell weighting for the cell size , alpha is the weight of max size, and beta the average size
		//----------------------------------------------------------------------------------------------------------------------
		void initialize(const mesh& i_mesh, const detail::cell_weights& i_weights);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the shortest distance to the mesh
		/// @param[in] i_position Tne point in space to compute the distance from
		/// @param[out] o_closest_record A record of the closest hit (feature type, distance)
		/// @param[out] o_face_id The face id will be filled in there
		//----------------------------------------------------------------------------------------------------------------------
		void operator()(const point3d& i_position, distance_record* o_closest_record, unsigned int* o_face_id) const;

                const detail::regular_grid& grid() const { return m_grid; }
	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief A pointer to the mesh - pointer is only valid after initialize has been called with appropriate mesh
		//----------------------------------------------------------------------------------------------------------------------
		const mesh* m_mesh;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief A uniform grid to accelerate lookups
		//----------------------------------------------------------------------------------------------------------------------
		detail::regular_grid m_grid;
	};
}

#endif /* SDF_LOOKUP_GRID_ACCELERATED_INCLUDED */
