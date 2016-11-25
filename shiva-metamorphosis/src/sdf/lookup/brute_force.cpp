#include <sdf/lookup/brute_force.hpp>
#include <assert.h>

//----------------------------------------------------------------------------------------------------------------------
sdf::brute_force::brute_force() : m_mesh(0) {}

//----------------------------------------------------------------------------------------------------------------------
void sdf::brute_force::initialize(const mesh& i_mesh)
{
	m_mesh = &i_mesh;
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::brute_force::operator()(const point3d& i_position, distance_record* o_closest_record, unsigned int* o_face_id) const
{
	assert(m_mesh);

	unsigned int& face_id = *o_face_id;
	distance_to_triangle dtt(i_position);
	distance_record& closest_record = *o_closest_record;
	// Need to move this away
	for (unsigned int i=0;i<m_mesh->num_faces();i++)
	{
		const index_type offset = i*3;
		const index_type& index_a = m_mesh->index_at(offset+0);
		const index_type& index_b = m_mesh->index_at(offset+1);
		const index_type& index_c = m_mesh->index_at(offset+2);

		const point3d& a = m_mesh->vertex_at(index_a);
		const point3d& b = m_mesh->vertex_at(index_b);
		const point3d& c = m_mesh->vertex_at(index_c);

		distance_record record;

		dtt(a,b,c,&record);

		if (record<closest_record)
		{
			closest_record = record;
			face_id = i;
		}
	}
}