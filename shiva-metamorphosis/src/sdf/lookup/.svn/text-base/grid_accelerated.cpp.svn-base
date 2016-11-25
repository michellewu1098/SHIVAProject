#include <sdf/lookup/grid_accelerated.hpp>
#include <assert.h>

//----------------------------------------------------------------------------------------------------------------------
sdf::grid_accelerated::grid_accelerated() : m_mesh(0) {}

//----------------------------------------------------------------------------------------------------------------------
void sdf::grid_accelerated::initialize(const mesh& i_mesh)
{
	detail::cell_weights weights(0.5f);
	initialize(i_mesh,weights);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::grid_accelerated::initialize(const mesh& i_mesh, const detail::cell_weights& i_weights)
{
	m_mesh = &i_mesh;
	m_grid.build(i_mesh,i_weights);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::grid_accelerated::operator()(const point3d& i_position, distance_record* o_closest_record, unsigned int* o_face_id) const
{
	assert(m_mesh);
	m_grid(*m_mesh,i_position,o_closest_record,o_face_id);
}