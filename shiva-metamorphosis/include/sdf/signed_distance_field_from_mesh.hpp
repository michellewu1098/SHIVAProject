#ifndef SDF_SIGNED_DISTANCE_FIELD_FROM_MESH_INCLUDED
#define SDF_SIGNED_DISTANCE_FIELD_FROM_MESH_INCLUDED

#include <assert.h>
#include <math.h>
//#include <boost/utility.hpp>
#include <sdf/core/types.hpp>
#include <sdf/core/mesh.hpp>
#include <sdf/sign/angle_weighted_average.hpp>
#include <sdf/lookup/bvh_accelerated.hpp>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class signed_distance_field_from_mesh "include/sdf/signed_distance_field_from_mesh.hpp"
	/// @brief This should be the main class users should use. 
	///			It takes in a mesh and allows users to query the signed distance to the mesh from any point in space
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	class signed_distance_field_from_mesh// : boost::noncopyable
	{
	public :
		typedef angle_weighted_average sign_computation;
		typedef bvh_accelerated distance_lookup;
		typedef signed_distance_field_from_mesh<T> this_type;

		typedef std::vector<point3d> point_array;
		typedef std::vector<float> scalar_array;

		typedef typename distance_lookup::parameters parameters;
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructor - from a mesh
		/// @param[in] i_mesh The mesh
		//----------------------------------------------------------------------------------------------------------------------
		signed_distance_field_from_mesh(const std::string& i_filename);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Initialize the distance field - It must be called before the operator()
		//----------------------------------------------------------------------------------------------------------------------
		void initialize();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Initialize the distance field - It must be called before the operator()
		/// @param[in] i_params Parameters for the acceleration structure
		//----------------------------------------------------------------------------------------------------------------------
		void initialize(const parameters& i_params);		
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the signed distance from i_position to the mesh
		/// @param[in] i_position A point in space to get the signed distance to the mesh
		/// @return Signed distance to the mesh
		//----------------------------------------------------------------------------------------------------------------------
		float operator()(const point3d& i_position) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Packet queries using an array of positions and filling the distance array. Positions have to be "coherent" i.e. close in space relatively to the closest feature.
		/// @param[in] i_positions The list of positions to packet trace
		/// @param[out] o_distances The distances to the mesh
		/// @return Returns true if the packet was coherent or not. It can be used to dynamically adapt the sampling during run time
		//----------------------------------------------------------------------------------------------------------------------
		bool operator()(const point3d i_positions[], float o_distances[]) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the signed distance from position (x,y,z) to the mesh
		/// @param[in] x point.x
		/// @param[in] y point.y
		/// @param[in] z point.z
		/// @return Signed distance to the mesh
		//----------------------------------------------------------------------------------------------------------------------
		float operator()(float x, float y, float z) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the signed distance from i_position to the mesh
		/// @param[in] i_position A point in space to get the signed distance to the mesh
		/// @param[out] o_face_id The face id of the closest face
		/// @param[out] o_hit_type The feature type - vertex, edge, face
		/// @return Signed distance to the mesh
		//----------------------------------------------------------------------------------------------------------------------
		float operator()(const point3d& i_position, unsigned int* o_face_id, distance_record* o_hit) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the signed distance from all the positions in the array
		/// @param[in] i_positions An array of points to sample
		/// @param[out] o_values Values at the positions
		//----------------------------------------------------------------------------------------------------------------------
		//void operator()(const point_array& i_positions, scalar_array* o_values) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The sign structure
		/// @return The sign computation constant reference
		//----------------------------------------------------------------------------------------------------------------------
		const sign_computation& get_sign() const { return sign; }
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The lookup structure
		/// @return The lookup constant reference
		//----------------------------------------------------------------------------------------------------------------------
		const distance_lookup& get_lookup() const { return lookup; }
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The lookup structure
		/// @return The reference to the mesh 
		//----------------------------------------------------------------------------------------------------------------------
		const mesh& get_mesh() const { return m_mesh; }
		const aabb& get_box() const { return m_box; }
		std::size_t memory_usage() const;
	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The mesh to represent
		//----------------------------------------------------------------------------------------------------------------------
		mesh m_mesh;
		aabb m_box;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Retrieve the sign of the field
		//----------------------------------------------------------------------------------------------------------------------
		sign_computation sign;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief A structure to get the shortest distance to the mesh
		//----------------------------------------------------------------------------------------------------------------------
		distance_lookup lookup;
#ifdef _DEBUG
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Has the object been initialized ?
		//----------------------------------------------------------------------------------------------------------------------
		bool m_initialized;
#endif 
	};

	typedef signed_distance_field_from_mesh<float> sdffmf;
	typedef signed_distance_field_from_mesh<float> sdffmd;
}

#include <sdf/signed_distance_field_from_mesh.inl>


#endif //SDF_SIGNED_DISTANCE_FIELD_FROM_MESH_INCLUDED
