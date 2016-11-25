#ifndef SDF_CORE_OBJ_FILE_INCLUDED
#define SDF_CORE_OBJ_FILE_INCLUDED

#include <sdf/core/types.hpp>
#include <sdf/core/aabb.hpp>
#include <vector>
#include <iostream>
#include <sstream>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class obj_file "include/sdf/core/obj_file.hpp"
	/// @brief A class to read .obj files. From the file it will only read vertices and indices
	///			Smoothing groups, normals, texture coordinates will be ignored.
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	class obj_file
	{
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructor
		//----------------------------------------------------------------------------------------------------------------------
		obj_file() : m_enable_comments(true) {}
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Read an obj file from a filename
		/// @param[in] i_filename The path to the obj file
		/// @param[out] o_vb The vertex buffer
		/// @param[out] o_ib The index buffer
		/// @return True if it managed to read the file, false otherwise
		//----------------------------------------------------------------------------------------------------------------------
		bool read(const std::string& i_filename, vertex_array* o_vb, index_array* o_ib, aabb* o_box = 0) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Read an obj file from a stream
		/// @param[in] i_sstream The stream
		/// @param[out] o_vb The vertex buffer
		/// @param[out] o_ib The index buffer
		/// @return True if it managed to read the file, false otherwise
		//----------------------------------------------------------------------------------------------------------------------
		bool read(std::stringstream& i_sstream, vertex_array* o_vb, index_array* o_ib, aabb* o_box = 0) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Write an obj file 
		/// @param[in] i_filename The path to the file
		/// @param[in] i_vb Vertex buffer of the object
		/// @param[in] i_ib Index buffer of the object
		/// @return True if it managed to write the file, false otherwise
		//----------------------------------------------------------------------------------------------------------------------
		bool write(const std::string& i_filename, const vertex_array& i_vb, const index_array& i_ib) const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Display comments in the .obj file while reading the mesh
		/// @param[in] i_enable Enable or disable comments
		//----------------------------------------------------------------------------------------------------------------------
		void enable_comments(bool i_enable);
	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Read a vertex from a stream
		/// @param io_stream The stream to read from
		/// @param[out] o_vertex The vertex to fill, it needs at least 3 consecutive floats
		//----------------------------------------------------------------------------------------------------------------------
		void read_vertex(std::stringstream& io_stream, float* o_vertex) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Read a face from a stream
		/// @param io_stream The stream to read from
		/// @param[out] o_indices An array with at least 4 elements
		/// @param[out] o_nAttrib The number of attributes read in - normally 3
		//----------------------------------------------------------------------------------------------------------------------
		void read_face  (std::stringstream& io_stream, index_type o_indices[], unsigned char& o_nAttrib) const;
	private :
		bool m_enable_comments;
	};
}

#endif // SDF_CORE_OBJ_FILE_INCLUDED
