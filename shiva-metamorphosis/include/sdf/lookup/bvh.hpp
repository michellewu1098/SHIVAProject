#ifndef SDF_LOOKUP_BVH_INCLUDED
#define SDF_LOOKUP_BVH_INCLUDED

#include <vector>
#include <sdf/lookup/bvh_branch.hpp>

namespace sdf
{
	namespace detail
	{
		//----------------------------------------------------------------------------------------------------------------------
		/// @class bvh "include/sdf/lookup/bvh.hpp"
		/// @brief BVH stands for Bounding Volume Hierarchy.
		///			It is similar to a kd-tree, each level splits in two
		///			The lookups are fast for a large number of faces, and fits to the shape better than most other structure
		/// @author Mathieu Sanchez
		/// @version 1.0
		/// @date Last Revision 20/07/11 Initial revision
		//----------------------------------------------------------------------------------------------------------------------
		class bvh
		{
		public :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief A polygon array contains the list of face indices (so to get the vertex indices you need to use index*3+0 for the first index)
			//----------------------------------------------------------------------------------------------------------------------
			typedef std::vector<index_type> polygon_array;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The nodes in one array (for serialization)
			//----------------------------------------------------------------------------------------------------------------------
			typedef std::vector<bvh_branch> m_tree_pool;
		public :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Default constructor
			//----------------------------------------------------------------------------------------------------------------------
			bvh_branch& root();
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Root branch of the tree
			/// @return A constant reference to the root branch
			//----------------------------------------------------------------------------------------------------------------------
			const bvh_branch& root() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Read only access to the polygon list
			/// @return A constant reference to the polygon list
			//----------------------------------------------------------------------------------------------------------------------
			const polygon_array& array() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Read only access to a branch
			/// @param[in] i Raw index in the master array of branches of the branch
			/// @return A constant reference to the branch
			//----------------------------------------------------------------------------------------------------------------------
			const bvh_branch& branch(unsigned int i) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Build the tree if no file exists, otherwise pickup the file, settings will be ignored. If the tree is built, a .bvh file will be written
			/// @param[in] i_mesh The original mesh
			/// @param[in] i_settings The tree settings
			//----------------------------------------------------------------------------------------------------------------------
			void build(
				const mesh& i_mesh,
				const settings& i_settings);
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Build the tree (no file operations)
			/// @param[in] i_mesh The original mesh
			/// @param[in] i_settings The tree settings
			//----------------------------------------------------------------------------------------------------------------------
			void force_build(
				const mesh& i_mesh,
				const settings& i_settings);
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Shortest distance to mesh lookup
			/// @param[in] i_mesh The original mesh
			/// @param[in] i_position THe position to look up from
			/// @param[out] o_closest_record The closest record 
			/// @param[out] o_face_id The face that registered the closest record
			//----------------------------------------------------------------------------------------------------------------------
			void operator()(
				const mesh& i_mesh,
				const point3d& i_position,
				distance_record* o_closest_record,
				unsigned int* o_face_id) const;
			void operator()(
				const mesh& i_mesh,
				const point3d i_position[],
				distance_record o_closest_record[],
				unsigned int o_face_id[]) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Self intersection collisions
			/// @param[in] i_mesh The original mesh
			/// @param[in] i_triangle The three vertices of the triangle
			/// @param[in] i_ignore The vertex indices which have to be excluded from a self intersection
			/// @param[out] o_face The face it is colliding with - it is only one of the faces but most of the time, other faces are also colliding
			/// @return True if a collision was detected false otherwise
			//----------------------------------------------------------------------------------------------------------------------
			bool operator()(
				const mesh& i_mesh,
				const point3d i_triangle[3],
				const index_type i_ignore[],
				index_type* o_face) const;
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
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Build the two children
			/// @param[in] i_mesh The original mesh
			/// @param[in] i_settings The tree settings
			/// @param[in] i_buckets An array of 2 arrays of polygons - for the first and second child
			/// @param[in] i_depth Depth of those children
			/// @param[out] o_childindex First child index , the second is first+1
			//----------------------------------------------------------------------------------------------------------------------
			void build_children(
				const mesh& i_mesh,
				const settings& i_settings,
				const polygon_array i_buckets[],
				unsigned int i_depth,
				bvh_branch::child_index* o_childindex);

			const m_tree_pool& nodes() const { return m_branches; }
			const polygon_array polygons() const { return m_polylist; }
			unsigned int num_polygons() const { return static_cast<unsigned int>(m_polylist.size()); }
			std::size_t memory_usage() const;
		private :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The raw data - a pool of indices used by the offset grid
			//----------------------------------------------------------------------------------------------------------------------
			polygon_array m_polylist;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The tree pool - all the bvh branches in one array
			//----------------------------------------------------------------------------------------------------------------------
			m_tree_pool m_branches;
		};
	}
}

#endif /* SDF_LOOKUP_BVH_INCLUDED */
