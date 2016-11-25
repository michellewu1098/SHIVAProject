#include <sdf/lookup/grid_cell.hpp>
#include <assert.h>


//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::grid_cell::add(index_type i_polygon)
{
	m_polygons.push_back(i_polygon);
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::grid_cell::is_empty() const
{
	return m_polygons.size()==0;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int sdf::detail::grid_cell::size() const
{
	return static_cast<unsigned int>(m_polygons.size());
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::index_type& sdf::detail::grid_cell::operator[](unsigned int i) const
{
	assert(i<size());
	return m_polygons[i];
}