#include <sdf/core/aabb.hpp>
#include <sdf/core/tools.hpp>
#include <sdf/core/triangle_aabb_overlap.hpp>
#include <iostream>
#include <assert.h>
#include <limits>
#include <math.h>

//----------------------------------------------------------------------------------------------------------------------
sdf::aabb::aabb() 
{
	m_bounds[0].set(
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max());
	m_bounds[1].set(
		-std::numeric_limits<float>::max(),
		-std::numeric_limits<float>::max(),
		-std::numeric_limits<float>::max());
}

//----------------------------------------------------------------------------------------------------------------------
sdf::aabb::aabb(const point3d& i_point)
{
	m_bounds[0]=i_point;
	m_bounds[1]=i_point;
}

//----------------------------------------------------------------------------------------------------------------------
sdf::aabb::aabb(const point3d& A, const point3d& B)
{
	m_bounds[0].set(
		std::min(A[0],B[0]),
		std::min(A[1],B[1]),
		std::min(A[2],B[2]));
	m_bounds[1].set(
		std::max(A[0],B[0]),
		std::max(A[1],B[1]),
		std::max(A[2],B[2]));
}

//----------------------------------------------------------------------------------------------------------------------
sdf::aabb::aabb(const point3d& A, const point3d& B, const point3d& C)
{
	set(A,B,C);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::aabb::set(const point3d& A, const point3d& B)
{
	m_bounds[0].set(
		sdf::min(A[0],B[0]),
		sdf::min(A[1],B[1]),
		sdf::min(A[2],B[2]));
	m_bounds[1].set(
		sdf::max(A[0],B[0]),
		sdf::max(A[1],B[1]),
		sdf::max(A[2],B[2]));
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::aabb::set(const point3d& A, const point3d& B, const point3d& C)
{
	m_bounds[0].set(
		sdf::min(A[0],B[0],C[0]),
		sdf::min(A[1],B[1],C[1]),
		sdf::min(A[2],B[2],C[2]));
	m_bounds[1].set(
		sdf::max(A[0],B[0],C[0]),
		sdf::max(A[1],B[1],C[1]),
		sdf::max(A[2],B[2],C[2]));
}

//----------------------------------------------------------------------------------------------------------------------
sdf::aabb::aabb(const point3d* i_points, unsigned int i_num_elements)
{
	for (unsigned int i=0;i<i_num_elements;i++)
		include(i_points[i]);
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::point3d& sdf::aabb::minimum() const
{
	return m_bounds[0];
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::point3d& sdf::aabb::maximum() const
{
	return m_bounds[1];
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::point3d& sdf::aabb::bound(unsigned int i) const
{
	assert(i<2);
	return m_bounds[i];
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::aabb::include(const point3d& i_element)
{
	m_bounds[0].set(
		std::min(m_bounds[0][0],i_element[0]),
		std::min(m_bounds[0][1],i_element[1]),
		std::min(m_bounds[0][2],i_element[2]));
	m_bounds[1].set(
		std::max(m_bounds[1][0],i_element[0]),
		std::max(m_bounds[1][1],i_element[1]),
		std::max(m_bounds[1][2],i_element[2]));
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::aabb::extent(point3d* o_extent) const
{
	*o_extent=m_bounds[1]-m_bounds[0];
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::aabb::centre(point3d* o_centre) const
{
	*o_centre=(m_bounds[0]+m_bounds[1])*0.5f;
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::aabb::intersects(const aabb& i_other) const
{
	return (
			m_bounds[1][0] >= i_other.m_bounds[0][0] && m_bounds[0][0] <= i_other.m_bounds[1][0] &&
			m_bounds[1][1] >= i_other.m_bounds[0][1] && m_bounds[0][1] <= i_other.m_bounds[1][1] &&
			m_bounds[1][2] >= i_other.m_bounds[0][2] && m_bounds[0][2] <= i_other.m_bounds[1][2] 
			);
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::aabb::intersects(const point3d& i_first,const point3d& i_second,const point3d& i_third) const
{
	const vector3d hextent = (m_bounds[1]-m_bounds[0])*0.5f;
	const point3d centre = (m_bounds[0]+m_bounds[1])*0.5f;
	return triangle_box_overlap(centre,hextent,i_first,i_second,i_third);
}


//----------------------------------------------------------------------------------------------------------------------
bool sdf::aabb::is_inside(const point3d& i_element) const
{
	return m_bounds[0][0]<=i_element[0] && i_element[0]<=m_bounds[1][0] && 
		m_bounds[0][1]<=i_element[1] && i_element[1]<=m_bounds[1][1] && 
		m_bounds[0][2]<=i_element[2] && i_element[2]<=m_bounds[1][2] ;
}