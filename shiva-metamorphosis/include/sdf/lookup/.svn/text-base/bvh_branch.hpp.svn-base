#ifndef SDF_LOOKUP_BVH_BRANCH_INCLUDED
#define SDF_LOOKUP_BVH_BRANCH_INCLUDED

#include <sdf/core/aabb.hpp>
#include <sdf/core/types.hpp>
#include <sdf/core/mesh.hpp>
#include <sdf/distance/distance_record.hpp>
#include <sdf/distance/distance_to_triangle.hpp>
#include <sdf/lookup/settings.hpp>

namespace sdf
{
	namespace detail
	{
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Forward declaration of the BVH
		//----------------------------------------------------------------------------------------------------------------------
		class bvh;

		//----------------------------------------------------------------------------------------------------------------------
		/// @class bvh_branch "include/sdf/lookup/bvh_branch.hpp"
		/// @brief It is more or less a node. A BVH branch has a bounding box, possible children and a list of polygons
		///			Like all acceleration structures of sdf, it is offset based and relies on the tree to handle memory
		/// @author Mathieu Sanchez
		/// @version 1.0
		/// @date Last Revision 20/07/11 Initial revision
		//----------------------------------------------------------------------------------------------------------------------
		class bvh_branch
		{
		public :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The type defined for indexing children (could be an iterator or something else)
			//----------------------------------------------------------------------------------------------------------------------
			typedef unsigned int child_index;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief A polygon array contains the list of face indices (so to get the vertex indices you need to use index*3+0 for the first index)
			//----------------------------------------------------------------------------------------------------------------------
			typedef std::vector<index_type> polygon_array;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief A polygon array contains the list of face indices (so to get the vertex indices you need to use index*3+0 for the first index)
			//----------------------------------------------------------------------------------------------------------------------
			static const unsigned int maximum_depth = 32;
		public :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Default constructor
			//----------------------------------------------------------------------------------------------------------------------
			bvh_branch();

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief If a branch is empty it means that no polygons are associated (meaning it should have children)
			/// @return True if it is empty, false otherwise
			//----------------------------------------------------------------------------------------------------------------------
			bool is_empty() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Children are children nodes (so this node would be empty)
			/// @return True if it has children, false otherwise
			/// @warning is_empty() should always be equal to has_children() after initialization
			//----------------------------------------------------------------------------------------------------------------------
			bool has_children() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the index (offset) of the child "i" (0 or 1)
			/// @param[in] i Index of the child (first=0, second=1, anything else is invalid)
			/// @return Offset in the node array 
			//----------------------------------------------------------------------------------------------------------------------
			child_index child(unsigned int i) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the number of polygons
			/// @return Number of polygons in this branch 
			//----------------------------------------------------------------------------------------------------------------------
			index_type num_polygons() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Decide whether this branch should be a leaf or a node
			/// @param[in] i_depth Current depth of the tree
			/// @param[in] i_polygons Polygon list of the node
			/// @param[in] i_settings Heuristic settings
			/// @return True if it should be a leaf, false otherwise
			//----------------------------------------------------------------------------------------------------------------------
			bool has_reached_leaf(unsigned int i_depth, const polygon_array& i_polygons, const settings& i_settings) const;
			
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the axis aligned bounding box of the branch
			/// @return The node's box
			//----------------------------------------------------------------------------------------------------------------------
			const aabb& box() const;

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Build the node
			/// @param io_tree The BVH tree (used to create new nodes and add polygons to the list)
			/// @param[in] i_mesh The mesh to process
			/// @param[in] i_settings Heuristic settings
			/// @param[in] i_polygons Polygon list of the node
			/// @param[in] i_depth Current tree depth (default to 0, root build)
			//----------------------------------------------------------------------------------------------------------------------
			void build(bvh& io_tree, const mesh& i_mesh, const settings& i_settings, const polygon_array& i_polygons, unsigned int i_depth = 0);

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Shortest distance to the mesh lookup
			/// @param[in] i_tree BVH tree
			/// @param[in] i_mesh Mesh to lookup, this will be used for vertices - it is important that the mesh is exactly the same as the one use in build
			/// @param[in] i_position Position to look from
			/// @param[out] io_closest_record This is the current distance record, if a closer record is found, it will update this param
			/// @param[out] o_face_id The face id of the closest distance record
			//----------------------------------------------------------------------------------------------------------------------
			inline void operator()(const bvh& i_tree, const mesh& i_mesh, const point3d& i_position, distance_record* io_closest_record, unsigned int* o_face_id) const { recursive_traversal(i_tree,i_mesh,i_position,io_closest_record,o_face_id); };
			void operator()(const bvh& i_tree, const mesh& i_mesh,	const point3d i_position[], distance_record o_closest_record[],	unsigned int o_face_id[]) const;

			bool operator()(const bvh& i_tree, const mesh& i_mesh, const point3d i_triangle[3], const aabb& i_box, const index_type i_ignore[],	index_type* o_face) const;

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Shortest distance to the mesh lookup (Recursive call)
			/// @param[in] i_tree BVH tree
			/// @param[in] i_mesh Mesh to lookup, this will be used for vertices - it is important that the mesh is exactly the same as the one use in build
			/// @param[in] i_position Position to look from
			/// @param[out] io_closest_record This is the current distance record, if a closer record is found, it will update this param
			/// @param[out] o_face_id The face id of the closest distance record
			//----------------------------------------------------------------------------------------------------------------------
			void recursive_traversal(const bvh& i_tree, const mesh& i_mesh, const point3d& i_position, distance_record* io_closest_record, unsigned int* o_face_id) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Shortest distance to the mesh lookup (Stackless call)
			/// @param[in] i_tree BVH tree
			/// @param[in] i_mesh Mesh to lookup, this will be used for vertices - it is important that the mesh is exactly the same as the one use in build
			/// @param[in] i_position Position to look from
			/// @param[out] io_closest_record This is the current distance record, if a closer record is found, it will update this param
			/// @param[out] o_face_id The face id of the closest distance record
			//----------------------------------------------------------------------------------------------------------------------
			void stackless_traversal(const bvh& i_tree, const mesh& i_mesh, const point3d& i_position, distance_record* io_closest_record, unsigned int* o_face_id) const;
		private :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Update shortest distance to the triangles from this node (assuming it is a leaf)
			/// @param[in] i_tree BVH tree
			/// @param[in] i_mesh Mesh to lookup, this will be used for vertices - it is important that the mesh is exactly the same as the one use in build
			/// @param[in] i_position Position to look from
			/// @param[out] io_closest_record This is the current distance record, if a closer record is found, it will update this param
			/// @param[out] o_face_id The face id of the closest distance record
			//----------------------------------------------------------------------------------------------------------------------
			void test_polygons(const bvh& i_tree, const mesh& i_mesh, const point3d& i_position, distance_record* io_closest_record, unsigned int* o_face_id) const;
		private :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The bounding box of this node
			//----------------------------------------------------------------------------------------------------------------------
			aabb m_box;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The children index, first child is at m_children, second is at m_children+1
			//----------------------------------------------------------------------------------------------------------------------
			child_index m_children;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Offset in the array of polygons
			//----------------------------------------------------------------------------------------------------------------------
			index_type m_offset;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Number of elements (size attached to this node)
			//----------------------------------------------------------------------------------------------------------------------
			index_type m_size;
		};
	}
}

#endif /* SDF_LOOKUP_BVH_BRANCH_INCLUDED */