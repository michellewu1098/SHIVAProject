#ifndef SDF_MESH_INCLUDED
#define SDF_MESH_INCLUDED

#include <sdf/core/aabb.hpp>
#include <sdf/core/types.hpp>
#include <sdf/core/obj_file.hpp>
#include <vector>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class mesh "include/sdf/core/mesh.hpp"
	/// @brief A simple mesh class (rather polygon soup)
	///		   It is merely a data holder
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	class mesh
	{
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Default constructor (empty)
		//----------------------------------------------------------------------------------------------------------------------
		mesh();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructing the mesh from a file (will guess according to extension)
		/// @param[in] i_filename Path to the file
		//----------------------------------------------------------------------------------------------------------------------
		mesh(const std::string& i_filename);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructor from 2 arrays
		/// @param[in] i_vertices Vertices
		/// @param[in] i_indices Indices
		//----------------------------------------------------------------------------------------------------------------------
		mesh(const vertex_array& i_vertices, const index_array& i_indices);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructor from [begin,end[ pointers
		/// @param[in] i_vertex_start The first vertex 
		/// @param[in] i_vertex_end The end marker pointer
		/// @param[in] i_index_start The first index
		/// @param[in] i_index_end The end marker index
		//----------------------------------------------------------------------------------------------------------------------
		mesh(const vertex_type* i_vertex_start, const vertex_type* i_vertex_end,
			const index_type* i_index_start, const index_type* i_index_end);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Destructor
		//----------------------------------------------------------------------------------------------------------------------
		~mesh();

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructing the mesh from a .obj file
		/// @param[in] i_filename Path to the file
		/// @return True if everything went well, otherwise, mesh is empty and returns false
		//----------------------------------------------------------------------------------------------------------------------
		bool load_from_obj(const std::string& i_filename, aabb* o_box = 0);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Export the current mesh to an object file
		/// @param[in] i_filename Path to the file
		/// @return True if everything went well, otherwise, no file created
		//----------------------------------------------------------------------------------------------------------------------
		bool export_to_file(const std::string& i_filename) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the number of indices
		/// @return The number of indices
		//----------------------------------------------------------------------------------------------------------------------
		unsigned int num_indices() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the number of faces
		/// @return The number of faces
		//----------------------------------------------------------------------------------------------------------------------
		unsigned int num_faces() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the number of vertices
		/// @return The number of vertices
		//----------------------------------------------------------------------------------------------------------------------
		unsigned int num_vertices() const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the source of the mesh (file)
		/// @return Constant reference to the source
		//----------------------------------------------------------------------------------------------------------------------
		const std::string& source() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief If the mesh was loaded "manually" no source can be identified, then it has no known source
		/// @return True if it has a known source, false otherwise
		//----------------------------------------------------------------------------------------------------------------------
		bool known_source() const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the vertex array
		/// @return A constant reference to the vertex array
		//----------------------------------------------------------------------------------------------------------------------
		const vertex_array& vertices() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the vertex array
		/// @return A reference to the vertex array
		//----------------------------------------------------------------------------------------------------------------------
		vertex_array& vertices();

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the index array
		/// @return A constant reference to the index array
		//----------------------------------------------------------------------------------------------------------------------
		const index_array& indices() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the index array
		/// @return A reference to the index array
		//----------------------------------------------------------------------------------------------------------------------
		index_array& indices();

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get a vertex from the vertex buffer
		/// @param[in] i_index Index in the vertex array
		/// @return A constant reference to the vertex
		//----------------------------------------------------------------------------------------------------------------------
		const vertex_type& vertex_at(unsigned int i_index) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the vertex from index i_index (in the index buffer)
		/// @param[in] i_index Index in the index buffer
		/// @return Constant reference to a vertex
		//----------------------------------------------------------------------------------------------------------------------
		const vertex_type& vertex(unsigned int i_index) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get an index from the index buffer
		/// @param[in] i_index Index in the index array
		/// @return A constant reference to the index
		//----------------------------------------------------------------------------------------------------------------------
		const  index_type&  index_at(unsigned int i_index) const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get face vertices from a face index
		/// @param[in] i_face_id Index of the face
		/// @param[out] o_first Will put the first vertex in this
		/// @param[out] o_second Will put the second vertex in this
		/// @param[out] o_third Will put the third vertex in this
		//----------------------------------------------------------------------------------------------------------------------
		void get_face_vertices(unsigned int i_face_id, point3d* o_first, point3d* o_second, point3d* o_third) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get face indices from a face index
		/// @param[in] i_face_id Index of the face
		/// @param[out] o_first Will put the first index in this
		/// @param[out] o_second Will put the second index in this
		/// @param[out] o_third Will put the third index in this
		//----------------------------------------------------------------------------------------------------------------------
		void get_face_indices (unsigned int i_face_id, index_type* o_first, index_type* o_second, index_type* o_third) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the bounding box of the mesh
		/// @param[out] o_box The mesh's bounding box
		//----------------------------------------------------------------------------------------------------------------------
		void bounding_box(aabb* o_box) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief An empty mesh is a mesh with no faces
		/// @return True if the mesh is empty, false otherwise
		//----------------------------------------------------------------------------------------------------------------------
		bool empty() const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the memory used by the mesh
		/// @return Cumulated memory (allocated memory by the arrays)
		//----------------------------------------------------------------------------------------------------------------------
		std::size_t memory_usage() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Print to std::cout the details of the mesh (size and numbers)
		//----------------------------------------------------------------------------------------------------------------------
		void print_details() const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Append a vertex 
		/// @param[in] i_vertex The vertex to add
		/// @return The index of the new vertex
		//----------------------------------------------------------------------------------------------------------------------
		index_type append(const vertex_type& i_vertex);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Add a new face to the mesh
		/// @param[in] i_indexA Index to the first vertex
		/// @param[in] i_indexB Index to the second vertex
		/// @param[in] i_indexC Index to the third vertex
		/// @return The face id added
		//----------------------------------------------------------------------------------------------------------------------
		index_type append(index_type i_indexA, index_type i_indexB, index_type i_indexC);
	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Filename the mesh comes from - empty if loaded manually
		//----------------------------------------------------------------------------------------------------------------------
		std::string m_source;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The vertices
		//----------------------------------------------------------------------------------------------------------------------
		vertex_array m_vertices;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The indices
		//----------------------------------------------------------------------------------------------------------------------
		index_array m_indices;
	};
}

#endif // SDF_MESH_INCLUDED