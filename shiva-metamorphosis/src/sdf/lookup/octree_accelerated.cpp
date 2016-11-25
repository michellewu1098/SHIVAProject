#include <sdf/lookup/octree_accelerated.hpp>
#include <assert.h>

//----------------------------------------------------------------------------------------------------------------------
sdf::octree_accelerated::octree_accelerated() : m_mesh(0) {}

//----------------------------------------------------------------------------------------------------------------------
void sdf::octree_accelerated::initialize(const mesh& i_mesh)
{
	m_mesh = &i_mesh;
	detail::settings stt(12,10,15,3);
	m_tree.build(i_mesh,stt);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::octree_accelerated::initialize(const mesh& i_mesh, const detail::settings& i_settings)
{
	m_mesh = &i_mesh;
	m_tree.build(i_mesh,i_settings);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::octree_accelerated::operator()(const point3d& i_position, distance_record* o_closest_record, unsigned int* o_face_id) const
{
	assert(m_mesh);
	m_tree(*m_mesh,i_position,o_closest_record,o_face_id);
}