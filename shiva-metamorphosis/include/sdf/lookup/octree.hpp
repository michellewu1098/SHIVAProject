#ifndef SDF_LOOKUP_OCTREE_INCLUDED
#define SDF_LOOKUP_OCTREE_INCLUDED

#include <vector>
#include <sdf/core/aabb.hpp>
#include <sdf/core/types.hpp>
#include <sdf/core/mesh.hpp>
#include <sdf/distance/distance_record.hpp>
#include <sdf/lookup/settings.hpp>
#include <sdf/lookup/octnode.hpp>

namespace sdf
{
	namespace detail
	{
		//----------------------------------------------------------------------------------------------------------------------
		/// @class octree "include/sdf/lookup/octree.hpp"
		/// @brief An octree structure to accelerate distance queries
		/// @author Mathieu Sanchez
		/// @version 1.0
		/// @date Last Revision 17/07/11 Initial revision
		//----------------------------------------------------------------------------------------------------------------------
		class octree
		{
		public :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Child index type
			//----------------------------------------------------------------------------------------------------------------------
			typedef octnode::child_index child_index;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief A polygon array contains the list of face indices (so to get the vertex indices you need to use index*3+0 for the first index)
			//----------------------------------------------------------------------------------------------------------------------
			typedef std::vector<index_type> polygon_array;
		public :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Build the two children
			/// @param[in] i_mesh The original mesh
			/// @param[in] i_settings The tree settings
			/// @param[in] i_buckets An array of 8 arrays of polygons
			/// @param[in] i_depth Depth of those children
			/// @param[out] o_childindex First child index , the second is first+1
			//----------------------------------------------------------------------------------------------------------------------
			void build_children(
				const mesh& i_mesh,
				const settings& i_settings,
				const polygon_array i_buckets[],
				const aabb i_boxes[],
				unsigned int i_depth,
				child_index* o_childindex);

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get a node
			/// @param[in] i The node index
			/// @return Constant reference to the node
			//----------------------------------------------------------------------------------------------------------------------
			const octnode& node(unsigned int i) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get a node
			/// @param[in] i The node index
			/// @return Reference to the node
			//----------------------------------------------------------------------------------------------------------------------
			octnode& node(unsigned int i);
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The root of the tree
			/// @return Constant reference to the root
			//----------------------------------------------------------------------------------------------------------------------
			const octnode& root() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The root of the tree
			/// @return Reference to the root
			//----------------------------------------------------------------------------------------------------------------------
			octnode& root();
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Read only access to the polygon list
			/// @return A constant reference to the polygon list
			//----------------------------------------------------------------------------------------------------------------------
			const polygon_array& array() const;

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Build the tree from a mesh
			/// @param[in] i_mesh The mesh
			/// @param[in] i_settings The heurisitic settings 
			//----------------------------------------------------------------------------------------------------------------------
			void build(const mesh& i_mesh, const settings& i_settings);

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The shortest distance to the mesh
			/// @param[in] i_mesh The mesh
			/// @param[in] i_position The position to look from
			/// @param[out] o_closest_record The closest record to the mesh from the position
			/// @param[out] o_face_id The face id of the closest face
			//----------------------------------------------------------------------------------------------------------------------
			void operator()(const mesh& i_mesh, const point3d& i_position, distance_record* o_closest_record, unsigned int* o_face_id) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Append a list of polygons to the "super" list
			/// @param[in] i_polygons The polygons to add
			/// @param[out] o_offset The offset of the polygons added in the super array
			/// @param[out] o_num_elements the number of elements added (i_polygons.size())
			//----------------------------------------------------------------------------------------------------------------------
			void append_polygons(
				const polygon_array& i_polygons,
				index_type* o_offset,
				index_type* o_num_elements);
		private :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Check if the tree is ready to be queried
			/// @return True if it has been built and initialized, false otherwise
			//----------------------------------------------------------------------------------------------------------------------
			bool ready_to_query() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Check if the tree contains the list of polygons
			/// @param[in] i_polygons The polygon list
			/// @return True if all the polygons are included, false otherwise
			//----------------------------------------------------------------------------------------------------------------------
			bool tree_contains(const octnode::polygon_array& i_polygons) const;
		private :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The typedef of the node array
			//----------------------------------------------------------------------------------------------------------------------
			typedef std::vector<octnode> node_pool;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The node pool (array)
			//----------------------------------------------------------------------------------------------------------------------
			node_pool m_nodes;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The raw data - a pool of indices used by the offset grid
			//----------------------------------------------------------------------------------------------------------------------
			polygon_array m_polylist;
		};
	}
}

#endif // SDF_LOOKUP_OCTREE_INCLUDED