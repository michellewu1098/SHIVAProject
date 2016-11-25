#ifndef SDF_LOOKUP_OCTNODE_INCLUDED
#define SDF_LOOKUP_OCTNODE_INCLUDED

#include <vector>
#include <sdf/core/aabb.hpp>
#include <sdf/core/types.hpp>
#include <sdf/core/mesh.hpp>
#include <sdf/distance/distance_record.hpp>
#include <sdf/lookup/settings.hpp>

namespace sdf
{
	namespace detail
	{
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Forward declaration of the octree
		//----------------------------------------------------------------------------------------------------------------------
		class octree;

		//----------------------------------------------------------------------------------------------------------------------
		/// @class oct_node "include/sdf/lookup/octnode.hpp"
		/// @brief A node of the octree. The node contains the primitives and eventually other nodes.
		///			It tries to keep it to a small amount of memory and keep it pointer-less
		/// @author Mathieu Sanchez
		/// @version 1.0
		/// @date Last Revision 17/07/11 Initial revision
		//----------------------------------------------------------------------------------------------------------------------
		class octnode
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
			/// @brief An array of axis aligned bounding boxes
			//----------------------------------------------------------------------------------------------------------------------
			typedef std::vector<aabb> box_array;
		public :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Default constructor
			//----------------------------------------------------------------------------------------------------------------------
			octnode();

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief A leaf is a node with no children (no sub nodes)
			/// @return True if this node is a leaf, false otherwise
			//----------------------------------------------------------------------------------------------------------------------
			bool is_leaf() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief A node is a node with children (sub nodes)
			/// @return True if this node is a node, false otherwise
			//----------------------------------------------------------------------------------------------------------------------
			bool is_node() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Equivalent to is_node
			/// @return True if this node is a node, false otherwise
			//----------------------------------------------------------------------------------------------------------------------
			bool has_children() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Set the children first index. The second child is startindex+1, and a node contains either no or 8 children
			/// @param[in] startindex The index of the first child
			//----------------------------------------------------------------------------------------------------------------------
			void set_children(child_index startindex);

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the bounding box of this node
			/// @return A constant reference to the box
			//----------------------------------------------------------------------------------------------------------------------
			const aabb& get_bounding_box() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the child index
			/// @param[in] i Index of the children in the node (from 0 to 7 inclusive)
			/// @return The absolute child index for the node array
			//----------------------------------------------------------------------------------------------------------------------
			child_index operator[](unsigned int i) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the child index
			/// @param[in] i Index of the children in the node (from 0 to 7 inclusive)
			/// @return The absolute child index for the node array
			//----------------------------------------------------------------------------------------------------------------------
			child_index child(unsigned int i) const;

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Build this node (recursively)
			/// @param io_octree The tree it belongs to. It is io because the tree may allocate new nodes (and invalidate this instance by deleting this memory)
			/// @param[in] i_mesh The original mesh
			/// @param[in] i_box The bounding box of this node
			/// @param[in] i_polygons The polygon list this node has to handle
			/// @param[in] i_depth Current depth of recursion, starts at 0, finishes at max_depth (from heuristic settings)
			/// @param[in] i_settings The heuristic settings containing the max depth, the optimum number of polygons per leaf etc...
			//----------------------------------------------------------------------------------------------------------------------
			void build(octree* io_octree, const mesh& i_mesh, const aabb& i_box, const polygon_array& i_polygons, unsigned int i_depth, const settings& i_settings);

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Shortest distance to the mesh lookup
			/// @param[in] i_octree Octree for children lookup - everything is const (pointer and data)
			/// @param[in] i_mesh Mesh to lookup, this will be used for vertices - it is important that the mesh is exactly the same as the one use in build
			/// @param[in] i_position Position to look from
			/// @param[out] io_closest_record This is the current distance record, if a closer record is found, it will update this param
			/// @param[out] o_face_id The face id of the closest distance record
			//----------------------------------------------------------------------------------------------------------------------
			inline void operator()(const octree* const i_octree, const mesh& i_mesh, const point3d& i_position, distance_record* io_closest_record, unsigned int* o_face_id) const { sorted_look(i_octree, i_mesh, i_position, io_closest_record, o_face_id); }
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Shortest distance to the mesh lookup - brute force is a naive way
			/// @param[in] i_octree Octree for children lookup - everything is const (pointer and data)
			/// @param[in] i_mesh Mesh to lookup, this will be used for vertices - it is important that the mesh is exactly the same as the one use in build
			/// @param[in] i_position Position to look from
			/// @param[out] io_closest_record This is the current distance record, if a closer record is found, it will update this param
			/// @param[out] o_face_id The face id of the closest distance record
			//----------------------------------------------------------------------------------------------------------------------
			void brute_force(const octree* const i_octree, const mesh& i_mesh, const point3d& i_position, distance_record* io_closest_record, unsigned int* o_face_id) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Shortest distance to the mesh lookup - sorts the boxes from closest to furthest 
			/// @param[in] i_octree Octree for children lookup - everything is const (pointer and data)
			/// @param[in] i_mesh Mesh to lookup, this will be used for vertices - it is important that the mesh is exactly the same as the one use in build
			/// @param[in] i_position Position to look from
			/// @param[out] io_closest_record This is the current distance record, if a closer record is found, it will update this param
			/// @param[out] o_face_id The face id of the closest distance record
			//----------------------------------------------------------------------------------------------------------------------
			void sorted_look(const octree* const i_octree, const mesh& i_mesh, const point3d& i_position, distance_record* io_closest_record, unsigned int* o_face_id) const;

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The first polygon index in the master array
			/// @return First polygon index
			//----------------------------------------------------------------------------------------------------------------------
			index_type polygon_start() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The end polygon index in the master array
			/// @return End polygon index (end marker, not included)
			//----------------------------------------------------------------------------------------------------------------------
			index_type polygon_end() const;
		private :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Decide if this node should become a leaf - Called by the build function
			/// @param[in] i_depth Current build depth
			/// @param[in] i_polygon The polygon list of this node
			/// @param[in] i_settings The heuristics of this tree
			/// @return True if it should stop the recursion, false otherwise
			//----------------------------------------------------------------------------------------------------------------------
			bool has_reached_leaf(unsigned int i_depth, const polygon_array& i_polygon, const settings& i_settings) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Build the box from a polygon list - Called by the build function
			/// @param[in] i_mesh The original mesh
			/// @param[in] i_polygon The polygon list
			/// @param[out] o_box The box containing all the polygons
			//----------------------------------------------------------------------------------------------------------------------
			void build_box(const mesh& i_mesh, const polygon_array& i_polygon, aabb* o_box) const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Set this node as a leaf - Called by the build function
			/// @param io_octree The tree it belongs to. 
			/// @param[in] i_polygon The polygon list of this node
			//----------------------------------------------------------------------------------------------------------------------
			void set_leaf(octree* io_tree, const polygon_array& i_polygon);
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Set this node as a node (with children) - Called by the build function
			/// @param io_octree The tree it belongs to. It is io because the tree may allocate new nodes (and invalidate this instance by deleting this memory)
			/// @param[in] i_mesh The original mesh
			/// @param[in] i_polygon The polygon list of this node
			/// @param[in] i_depth Current build depth
			/// @param[in] i_settings The heuristics of this tree
			//----------------------------------------------------------------------------------------------------------------------
			void set_node(octree* io_tree, const mesh& i_mesh, const polygon_array& i_polygons,	unsigned int i_depth, const settings& i_settings);
		private :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The bounding box of this node
			//----------------------------------------------------------------------------------------------------------------------
			aabb m_box;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The children index, first child is at m_children, last is at m_children+7
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

#endif // SDF_LOOKUP_OCTNODE_INCLUDED