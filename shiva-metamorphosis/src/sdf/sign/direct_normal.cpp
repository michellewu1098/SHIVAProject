#include <sdf/sign/direct_normal.hpp>
#include <assert.h>


//----------------------------------------------------------------------------------------------------------------------
sdf::direct_normal::direct_normal() : m_mesh(0) {}

//----------------------------------------------------------------------------------------------------------------------
void sdf::direct_normal::initialize(const mesh& i_mesh)
{
	m_mesh = &i_mesh;
	assert(m_mesh);
}

//----------------------------------------------------------------------------------------------------------------------
float sdf::direct_normal::operator ()(const point3d& i_position, const distance_record& closest_record, unsigned int face_id) const
{
	assert(m_mesh);

	float sign(0.f);
	{
		const index_type& index_a = m_mesh->index_at(face_id*3+0);
		const index_type& index_b = m_mesh->index_at(face_id*3+1);
		const index_type& index_c = m_mesh->index_at(face_id*3+2);

		const point3d& a = m_mesh->vertex_at(index_a);
		const point3d& b = m_mesh->vertex_at(index_b);
		const point3d& c = m_mesh->vertex_at(index_c);
		
		const vector3d ab = b-a;
		const vector3d ac = c-a;


		vector3d normal;
		normal.cross(ab,ac);

		// This assert makes sure the triangle is not flat
		// The length of a cross product is 
		// the area of the parallelogram craeted by the two vectors
		// If the normal length is null, the dot product would have no real meaning
		assert(normal.length_squared()!=0);

		const vector3d ptb = i_position-closest_record.closest_point();

		sign = 1.f;
		if (ptb.dot(normal)<=0)
		{
			sign=-1.f;
		}
	}
	// Because it is a multiplier, we should only return one or the other
	assert(sign==-1.f || sign==1.f); 
	return sign;
}