#ifndef SDF_LOOKUP_REGULAR_GRID_INCLUDED
#define SDF_LOOKUP_REGULAR_GRID_INCLUDED

#include <assert.h>
#include <vector>
#include <sdf/core/aabb.hpp>
#include <sdf/core/types.hpp>
#include <sdf/core/mesh.hpp>
#include <sdf/distance/distance_record.hpp>
#include <sdf/distance/distance_to_triangle.hpp>
#include <sdf/lookup/cell_weights.hpp>
#include <sdf/lookup/grid_cell.hpp>
#include <sdf/lookup/grid_offset_cell.hpp>
#include <sdf/lookup/index_3d.hpp>

namespace sdf
{
	namespace detail
	{
		//----------------------------------------------------------------------------------------------------------------------
		/// @class regular_grid "include/sdf/lookup/regular_grid.hpp"
		/// @brief A regular grid is dividing the world into cells (or buckets) of the same size, and puts polygons in them
		///			The lookup is done through a propagation method but a brute force can be used (checking all the cells)
		/// @author Mathieu Sanchez
		/// @version 1.0
		/// @date Last Revision 28/06/11 Initial revision
		//----------------------------------------------------------------------------------------------------------------------
		class regular_grid
		{
		public :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief A polygon array contains the list of face indices (so to get the vertex indices you need to use index*3+0 for the first index)
			//----------------------------------------------------------------------------------------------------------------------
			typedef std::vector<index_type> polygon_array;
		public :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Build the grid
			/// @param[in] i_mesh The mesh the grid builds upon
			//----------------------------------------------------------------------------------------------------------------------
			void build(const mesh& i_mesh, const cell_weights& i_weights);

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Shortest distance to the mesh lookup
			/// @param[in] i_mesh The mesh we are computing the distance to
			/// @param[in] i_position The position from which we want the shortest distance
			/// @param[out] o_closest_record The closest distance (record) to the mesh 
			/// @param[out] o_face_id The face id of the closest face (it could be a face feature)
			//----------------------------------------------------------------------------------------------------------------------
			void operator()(const mesh& i_mesh, const point3d& i_position, distance_record* o_closest_record, unsigned int* o_face_id) const;


			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Number of cells in total
			/// @return Number of cells in X * number of cells in Y * number of cells in Z
			//----------------------------------------------------------------------------------------------------------------------
			unsigned int num_cells() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Access a cell directly 
			/// @param[in] i Absolute index
			/// @return Constant reference to the cell
			//----------------------------------------------------------------------------------------------------------------------
			const grid_offset_cell& operator[](unsigned int i) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the cell size
			/// @return Constant reference to the cell size vector
			//----------------------------------------------------------------------------------------------------------------------
			const vector3d& cell_size() const;
			const vector3d& inv_cell_size() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the centre of a cell
			/// @param[in] i Absolute index
			/// @return The centre position of a cell
			//----------------------------------------------------------------------------------------------------------------------
			point3d cell_centre(unsigned int i) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the bounding box of a cell
			/// @param[in] id Index in 3d of the cell
			/// @param[out] o_box Its corresponding bounding box
			//----------------------------------------------------------------------------------------------------------------------
			void cell_aabb(const index_3d& id, aabb* o_box) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the overall bounding box
			/// @return Constant reference to the grid box
			//----------------------------------------------------------------------------------------------------------------------
			const aabb& grid_box() const;

			const unsigned int* offset_size_cells() const { return (const unsigned int*)&(m_grid[0]); }
			const polygon_array& polygons() const { return m_polylist; }
			unsigned int num_polygons() const { return static_cast<unsigned int>(m_polylist.size()); }
			const float* grid_data() const { return (const float*)&m_box; }
			const int* dimensions() const { return m_dim; }
		private :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Access a cell directly 
			/// @param[in] i Absolute index
			/// @return Constant reference to the cell
			//----------------------------------------------------------------------------------------------------------------------
			void get_cell_heuristic(const mesh& i_mesh, aabb* o_mesh, vector3d* o_average, vector3d* o_biggest) const;

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Test if a position is inside the grid
			/// @param[in] i_pos Position to test
			/// @return True if the position is inside the grid, false otherwise
			//----------------------------------------------------------------------------------------------------------------------
			bool is_inside(const point3d& i_pos) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get a position corresponding cell index
			/// @param[in] i_pos The position being tested
			/// @param[out] o_cell_id Get the cell id of the position i_pos
			/// @return True if the cell id is valid, that is, the cell exists
			//----------------------------------------------------------------------------------------------------------------------
			bool get_cell_id(const point3d& i_pos, index_3d* o_cell_id) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the absolute index given an index
			/// @param[in] i_index3d Index of the cell in 3D
			/// @return Absolute index
			//----------------------------------------------------------------------------------------------------------------------
			unsigned int get_absolute_id(const index_3d& i_index3d) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Explore a node to find a closer distance
			/// @param[in] i_mesh The mesh to lookup
			/// @param[in] i_tridist The triangle distance object
			/// @param[in] i_index Index of the node (absolute)
			/// @param[out] o_record The closest match (either unchanged if no closer match, or the closest found in this cell)
			/// @param[out] o_face_id The closest face id (only updated if a closer match was found)
			/// @return True if this node had a shorter distance than before, false otherwise
			//----------------------------------------------------------------------------------------------------------------------
			bool explore_node(const mesh& i_mesh, const distance_to_triangle& i_tridist, unsigned int i_index, distance_record* o_record, unsigned int* o_face_id)const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Look through the grid, going through all the cells
			/// @param[in] i_mesh The mesh to lookup
			/// @param[in] i_position The position to look from
			/// @param[out] o_closest_record The closest match (only updated if a closer match was found)
			/// @param[out] o_face_id The closest face id (only updated if a closer match was found)
			/// @return True if this node had a shorter distance than before, false otherwise
			//----------------------------------------------------------------------------------------------------------------------
			void full_check_lookup(const mesh& i_mesh, const point3d& i_position, distance_record* o_closest_record, unsigned int* o_face_id) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Propagate from the closest cell to the point and "growing" from it in all directions
			/// @param[in] i_mesh The mesh to lookup
			/// @param[in] i_position The position to look from
			/// @param[out] o_closest_record The closest match (only updated if a closer match was found)
			/// @param[out] o_face_id The closest face id (only updated if a closer match was found)
			/// @return True if this node had a shorter distance than before, false otherwise
			//----------------------------------------------------------------------------------------------------------------------
			void propagation_lookup(const mesh& i_mesh, const point3d& i_position, distance_record* o_closest_record, unsigned int* o_face_id) const;

		private :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief typedef grid of cells - the each own a vector, which is used for building the grid, but discarded after
			//----------------------------------------------------------------------------------------------------------------------
			typedef std::vector<grid_cell> cell_grid;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief typedef grid of offset cells - A grid containing offsets to a pool
			//----------------------------------------------------------------------------------------------------------------------
			typedef std::vector<grid_offset_cell> offset_based_grid;

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The grid of cells stored in 1 dimension (for faster lookups and memory coherency)
			//----------------------------------------------------------------------------------------------------------------------
			offset_based_grid m_grid;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The raw data - a pool of indices used by the offset grid
			//----------------------------------------------------------------------------------------------------------------------
			polygon_array m_polylist;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The box of the grid
			//----------------------------------------------------------------------------------------------------------------------
			aabb m_box;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief A single cell size 
			//----------------------------------------------------------------------------------------------------------------------
			vector3d m_cellsize;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The inverse of the cell size (for faster queries)
			//----------------------------------------------------------------------------------------------------------------------
			vector3d m_invcellsize;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Number of cells in each dimension
			//----------------------------------------------------------------------------------------------------------------------
			int m_dim[3];
		};
	}
}

#endif /* SDF_LOOKUP_REGULAR_GRID_INCLUDED */
