#ifndef SDF_ANGLE_WEIGHTED_AVERAGE_INCLUDED
#define SDF_ANGLE_WEIGHTED_AVERAGE_INCLUDED

#include <sdf/core/types.hpp>
#include <sdf/core/mesh.hpp>
#include <sdf/distance/distance_to_triangle.hpp>


namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class angle_weighted_average "include/sdf/sign/angle_weighted_average.hpp"
	/// @brief To compute the sign of the distance field
	///        It is an implementation of the paper 
	///		   Signed Distance Computation using the Angle Weighted Pseudo-normal
	///		   By J. Andreas Baerentzen and Henrik Aanaes, 2005
	///		   IEEE Transactions on Visualization and Computer Graphics, Volume 11
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	class angle_weighted_average
	{
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Default constructor
		//----------------------------------------------------------------------------------------------------------------------
		angle_weighted_average();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Initialize the brute force method (save the mesh)
		/// @param[in] i_mesh The polygonal mesh - if the mesh has a source, it will look for a .awn file, if it doesnt exist, it will then create it, if it exists, it will load data from it
		//----------------------------------------------------------------------------------------------------------------------
		void initialize(const mesh& i_mesh);
		void force_init(const mesh& i_mesh);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the sign of the field 
		/// @param[in] i_position Tne point in space to compute the distance from
		/// @param[in] i_closest_record The hit record from the distance query
		/// @param[in] i_face_id The face id of the closest face
		/// @return -1 or 1. It is the value the distance should be multiplied by.
		//----------------------------------------------------------------------------------------------------------------------
		float operator()(const point3d& i_position, const distance_record& i_closest_record, unsigned int i_face_id) const;

		const normal_array& face_normals() const { return m_face_normals; }
		const normal_array& edge_normals() const { return m_edge_normals; }
		const normal_array& vertex_normals() const { return m_vertex_normals; }
		std::size_t memory_usage() const;
	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Check if it has been initialized
		/// @return True if the internal data looks valid, false otherwise
		//----------------------------------------------------------------------------------------------------------------------
		bool is_initialized() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Build the face and vertex normals for later lookups
		//----------------------------------------------------------------------------------------------------------------------
		void build_face_normals();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Build the edge normals for later lookups
		//----------------------------------------------------------------------------------------------------------------------
		void build_edge_normals();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Build the vertex normals for later lookups
		//----------------------------------------------------------------------------------------------------------------------
		void build_vertex_normals();
	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief A pointer to the mesh - pointer is only valid after initialize has been called with appropriate mesh
		//----------------------------------------------------------------------------------------------------------------------
		const mesh* m_mesh;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Precomputed normals of each face (cross product between two edges)
		//----------------------------------------------------------------------------------------------------------------------
		normal_array m_face_normals;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Precomputed vertex normals - a vertex normal is the weigthed average of neighbor faces
		//----------------------------------------------------------------------------------------------------------------------
		normal_array m_vertex_normals;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Precomputed edge normals - an edge normal is the average of each face on each side.
		//----------------------------------------------------------------------------------------------------------------------
		normal_array m_edge_normals;
	};
}

#endif // SDF_ANGLE_WEIGHTED_AVERAGE_INCLUDED
