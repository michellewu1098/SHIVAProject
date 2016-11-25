#include <sdf/lookup/bvh_accelerated.hpp>
#include <assert.h>

//----------------------------------------------------------------------------------------------------------------------
sdf::bvh_accelerated::bvh_accelerated() : m_mesh(0) {}

//----------------------------------------------------------------------------------------------------------------------
void sdf::bvh_accelerated::initialize(const mesh& i_mesh)
{
	m_mesh = &i_mesh;
	detail::settings stt(26,2,16,3);
	m_tree.build(i_mesh,stt);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::bvh_accelerated::initialize(const mesh& i_mesh, const detail::settings& i_settings)
{
	assert(i_settings.max_depth()<detail::bvh_branch::maximum_depth);
	m_mesh = &i_mesh;
	m_tree.build(i_mesh,i_settings);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::bvh_accelerated::operator()(const point3d& i_position, distance_record* o_closest_record, unsigned int* o_face_id) const
{
	assert(m_mesh);
	m_tree(*m_mesh,i_position,o_closest_record,o_face_id);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::bvh_accelerated::operator()(const point3d i_positions[], distance_record o_closest_record[], unsigned int o_face_id[]) const
{
	assert(m_mesh);
#ifdef PACKETS
	m_tree(*m_mesh,i_positions,o_closest_record,o_face_id);
#else
	for(int i=0;i<8;i++)
		m_tree(*m_mesh,i_positions[i],&o_closest_record[i],&o_face_id[i]);
#endif
}