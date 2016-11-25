#include <sdf/distance/distance_to_aabb.hpp>
#include <sdf/core/tools.hpp>
#include <assert.h>


//----------------------------------------------------------------------------------------------------------------------
sdf::distance_to_aabb::distance_to_aabb() : m_position() {}
//----------------------------------------------------------------------------------------------------------------------
sdf::distance_to_aabb::distance_to_aabb(const point3d& i_position) : m_position(i_position) {}

//----------------------------------------------------------------------------------------------------------------------
float sdf::distance_to_aabb::operator()(const aabb& i_box) const
{
	float sqdist(0.f);
	for (unsigned int i=0;i<3;i++)
	{
		const float elem = m_position[i];
		if (elem<i_box.minimum()[i])
		{
			const float difference = i_box.minimum()[i] - elem;
			sqdist+=difference*difference;
		}
		if (elem>i_box.maximum()[i])
		{
			const float difference = elem - i_box.maximum()[i];
			sqdist+=difference*difference;
		}
	}
	return sqdist;
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::distance_to_aabb::operator()(const aabb& i_box, point3d* o_pos) const
{
	point3d& clamped = *o_pos;
	for (unsigned int i=0;i<3;i++)
		clamped[i] = clamp(m_position[i],i_box.minimum()[i],i_box.maximum()[i]);
}
