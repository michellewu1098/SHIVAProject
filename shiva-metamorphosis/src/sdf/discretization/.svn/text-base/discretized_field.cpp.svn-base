#include <sdf/discretization/discretized_field.hpp>
#include <sdf/core/tools.hpp>
#include <assert.h>

//----------------------------------------------------------------------------------------------------------------------
sdf::discretized_field::discretized_field() {}
//----------------------------------------------------------------------------------------------------------------------
sdf::discretized_field::discretized_field(
	const point3d& i_min,
	const point3d& i_max,
	unsigned int i_nx,
	unsigned int i_ny,
	unsigned int i_nz) : m_grid(i_nx,i_ny,i_nz), m_min(i_min), m_max(i_max)
{
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::discretized_field::cell_size(vector3d* o_size) const
{
	const vector3d extent = m_max-m_min;
	const vector3d increment(
		extent[0]/((float)num_cell_x()/*-1.f*/),
		extent[1]/((float)num_cell_y()/*-1.f*/),
		extent[2]/((float)num_cell_z()/*-1.f*/));
	*o_size = increment;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int sdf::discretized_field::num_cells() const
{
	return m_grid.num_elements();
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int sdf::discretized_field::num_cell_x() const
{
	return m_grid.width();
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int sdf::discretized_field::num_cell_y() const
{
	return m_grid.height();
}
//----------------------------------------------------------------------------------------------------------------------
unsigned int sdf::discretized_field::num_cell_z() const
{
	return m_grid.depth();
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::discretized_field::operator==(const discretized_field& i_other) const
{
	return difference(i_other)==0.f;
}
//----------------------------------------------------------------------------------------------------------------------
bool sdf::discretized_field::operator!=(const discretized_field& i_other) const
{
	return difference(i_other)!=0.f;
}

//----------------------------------------------------------------------------------------------------------------------
float sdf::discretized_field::difference(const discretized_field& i_other) const
{
	assert(num_cell_x()==i_other.num_cell_x());
	assert(num_cell_y()==i_other.num_cell_y());
	assert(num_cell_z()==i_other.num_cell_z());

	assert(m_min==i_other.m_min);
	assert(m_max==i_other.m_max);

	return m_grid.difference(i_other.m_grid);
}

//----------------------------------------------------------------------------------------------------------------------
float sdf::discretized_field::element_at(unsigned int i, unsigned int j, unsigned int k) const
{
	return m_grid(i,j,k);
}

//----------------------------------------------------------------------------------------------------------------------
float sdf::discretized_field::nearest(const point3d& /*i_position*/) const
{
	assert(false);
	return 0.f;
}

//----------------------------------------------------------------------------------------------------------------------
float sdf::discretized_field::linear(const point3d& i_position) const
{
        sdf::point3d rpos = (i_position - m_min)/(m_max-m_min);
        rpos[0] = clamp(rpos[0],0.f,1.f);
        rpos[1] = clamp(rpos[1],0.f,1.f);
        rpos[2] = clamp(rpos[2],0.f,1.f);



        const float positionU = rpos[0]*((float)(num_cell_x()-1))-.5f;
        const float positionV = rpos[1]*((float)(num_cell_y()-1))-.5f;
        const float positionW = rpos[2]*((float)(num_cell_z()-1))-.5f;

        const int minCornerX = int(positionU);
        const int minCornerY = int(positionV);
        const int minCornerZ = int(positionW);

        const int maxCornerX = minCornerX+1;
        const int maxCornerY = minCornerY+1;
        const int maxCornerZ = minCornerZ+1;

        const float a = positionU - (float)minCornerX;
        const float b = positionV - (float)minCornerY;
        const float c = positionW - (float)minCornerZ;

        const float level1[] =
        {
                lerp( // Bottom front lerp
                m_grid(minCornerX,minCornerY,minCornerZ),
                m_grid(maxCornerX,minCornerY,minCornerZ),a)  ,
                lerp( // Top front lerp
                m_grid(minCornerX,maxCornerY,minCornerZ),
                m_grid(maxCornerX,maxCornerY,minCornerZ),a)  ,
                lerp( // Bottom back lerp
                m_grid(minCornerX,minCornerY,maxCornerZ),
                m_grid(maxCornerX,minCornerY,maxCornerZ),a)  ,
                lerp( // Top back lerp
                m_grid(minCornerX,maxCornerY,maxCornerZ),
                m_grid(maxCornerX,maxCornerY,maxCornerZ),a)
        };

        const float level2[] =
        {
                lerp(level1[0],level1[1],b), // Front face
                lerp(level1[2],level1[3],b), // Back face
        };

        return lerp(level2[0],level2[1],c);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::discretized_field::gradient(const point3d& i_pos, sdf::point3d* o_gradient) const
{
    float delta = 0.002f;
    point3d ret(
        linear(i_pos+point3d(delta,0.0,0.0)) - linear(i_pos-point3d(delta,0.0,0.0)),
        linear(i_pos+point3d(0.0,delta,0.0)) - linear(i_pos-point3d(0.0,delta,0.0)),
        linear(i_pos+point3d(0.0,0.0,delta)) - linear(i_pos-point3d(0.0,0.0,delta)));

    ret.normalize();
    o_gradient->set(ret[0],ret[1],ret[2]);
}

//----------------------------------------------------------------------------------------------------------------------
float sdf::discretized_field::operator()(const point3d& i_position) const
{
	return linear(i_position);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::discretized_field::serialize(binary_file_out& o_file) const
{
    assert(o_file.is_open());
    m_grid.serialize(o_file);
    o_file(m_min);
    o_file(m_max);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::discretized_field::unserialize(binary_file_in& o_file)
{
    assert(o_file.is_open());
    m_grid.unserialize(o_file);
    o_file(&m_min);
    o_file(&m_max);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::discretized_field::get_positions(std::vector<point3d>* o_positions) const
{
    // Use at least boost::thread!
    const float xscale = 1.f/((float)m_grid.width()-1);
    const float yscale = 1.f/((float)m_grid.height()-1);
    const float zscale = 1.f/((float)m_grid.depth()-1);
    const vector3d extent = m_max-m_min;
    o_positions->resize(m_grid.num_elements());
    unsigned int counter(0);
    for (unsigned int k=0;k<m_grid.depth();k++)
    {
            for (unsigned int j=0;j<m_grid.height();j++)
            {
                    for (unsigned int i=0;i<m_grid.width();i++)
                    {
                            const point3d point(
                                    (float)i*xscale*extent[0],
                                    (float)j*yscale*extent[1],
                                    (float)k*zscale*extent[2]
                                    );
                            (*o_positions)[counter++]=m_min+point;
                    }
            }
    }
}


