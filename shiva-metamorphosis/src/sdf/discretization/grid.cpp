#include <sdf/discretization/grid.hpp>
#include <assert.h>
#include <math.h>

//----------------------------------------------------------------------------------------------------------------------
sdf::grid::grid()
:   m_dimension_x(0),
	m_dimension_y(0),
	m_dimension_z(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
sdf::grid::grid(
				unsigned int i_dimx,
				unsigned int i_dimy,
				unsigned int i_dimz, 
				float i_default)
	:   m_dimension_x(i_dimx),
		m_dimension_y(i_dimy),
		m_dimension_z(i_dimz), 
		m_data(i_dimx*i_dimy*i_dimz,i_default)
{
}

//----------------------------------------------------------------------------------------------------------------------
const float& sdf::grid::operator[](unsigned int i_index) const
{
	return m_data[i_index];
}

//----------------------------------------------------------------------------------------------------------------------
const float& sdf::grid::operator()(unsigned int i_x, unsigned int i_y, unsigned int i_z) const
{
	return m_data[raw_index(i_x,i_y,i_z)];
}

//----------------------------------------------------------------------------------------------------------------------
float& sdf::grid::operator()(unsigned int i_x, unsigned int i_y, unsigned int i_z)
{
	return m_data[raw_index(i_x,i_y,i_z)];
}

//----------------------------------------------------------------------------------------------------------------------
const float* sdf::grid::raw_data() const
{
	return &m_data[0];
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int sdf::grid::num_elements() const
{
	return m_dimension_x*m_dimension_y*m_dimension_z;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int sdf::grid::width() const
{
	return m_dimension_x;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int sdf::grid::height() const
{
	return m_dimension_y;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int sdf::grid::depth() const
{
	return m_dimension_z;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int sdf::grid::raw_index(unsigned int i_x, unsigned int i_y, unsigned int i_z) const
{
	//const unsigned int x_stride = 1;
	const unsigned int y_stride = m_dimension_x;
	const unsigned int z_stride = m_dimension_x*m_dimension_y;
	return i_x+i_y*y_stride+i_z*z_stride;
}

//----------------------------------------------------------------------------------------------------------------------
float sdf::grid::difference(const grid& i_other) const
{
	assert(m_dimension_x==i_other.m_dimension_x);
	assert(m_dimension_y==i_other.m_dimension_y);
	assert(m_dimension_z==i_other.m_dimension_z);

	
	float diff(0);
	for (unsigned int k=0;k<m_dimension_z;k++)
		for (unsigned int j=0;j<m_dimension_y;j++)
			for (unsigned int i=0;i<m_dimension_x;i++)
			{
				const float current = m_data[raw_index(i,j,k)];
				const float other = i_other(i,j,k);
				if (current!=other)
					diff+=fabs(current-other);
			}

	return diff;
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::grid::operator==(const grid& i_other) const
{
	if (
		m_dimension_x!=i_other.m_dimension_x ||
		m_dimension_y!=i_other.m_dimension_y ||
		m_dimension_z!=i_other.m_dimension_z)
		return false;
	return difference(i_other)==0;
	
}


//----------------------------------------------------------------------------------------------------------------------
void sdf::grid::serialize(binary_file_out& o_file) const
{
    o_file(m_dimension_x);
    o_file(m_dimension_y);
    o_file(m_dimension_z);
	unsigned int size = (unsigned int)m_data.size();
	o_file(size);
	o_file(&m_data[0],size);
    //o_file(m_data);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::grid::unserialize(binary_file_in& o_file)
{
    o_file(&m_dimension_x);
    o_file(&m_dimension_y);
    o_file(&m_dimension_z);
    //o_file(&m_data);
	unsigned int size(0);
	o_file(&size);
	m_data.resize(size);
	o_file(&m_data[0],size);
}
