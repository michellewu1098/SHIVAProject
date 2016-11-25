#include <sdf/distance/distance_record.hpp>
#include <sdf/core/types.hpp>
#include <sdf/core/feature_type.hpp>
#include <limits>
#include <iostream>

	
//----------------------------------------------------------------------------------------------------------------------
sdf::distance_record::distance_record() 
		: m_distance_square( std::numeric_limits<float>::max() ),
		  m_feature(face) 
{
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::distance_record::set_feature(const feature_type& i_type)
{
	m_feature=i_type;
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::distance_record::set_distance_squared(float i_dist_sq)
{
	m_distance_square=i_dist_sq;
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::distance_record::set_closest_point(const point3d& i_point)
{
	m_closest_point=i_point;
}

//----------------------------------------------------------------------------------------------------------------------
const float& sdf::distance_record::distance_square() const 
{
	return m_distance_square; 
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::feature_type& sdf::distance_record::feature()  const 
{ 
	return m_feature; 
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::point3d& sdf::distance_record::closest_point() const 
{ 
	return m_closest_point; 
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::distance_record::operator<(const distance_record& i_other) const
{
	return m_distance_square<i_other.m_distance_square;
}
