#include <sdf/lookup/grid_offset_cell.hpp>
#include <assert.h>

//----------------------------------------------------------------------------------------------------------------------
sdf::detail::grid_offset_cell::grid_offset_cell():m_offset(0),m_size(0){}

//----------------------------------------------------------------------------------------------------------------------
sdf::detail::grid_offset_cell::grid_offset_cell(index_type i_offset, index_type i_num) : m_offset(i_offset), m_size(i_num){}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::grid_offset_cell::set(index_type i_offset, index_type i_num)
{
	 m_offset=i_offset;
	 m_size=i_num;
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::grid_offset_cell::is_empty() const 
{ 
	return m_size==0; 
}

//----------------------------------------------------------------------------------------------------------------------
sdf::index_type sdf::detail::grid_offset_cell::size() const 
{ 
	return m_size; 
}

//----------------------------------------------------------------------------------------------------------------------
sdf::index_type sdf::detail::grid_offset_cell::operator[](index_type i) const
{
	assert(i<m_size);
	return m_offset+i;
}