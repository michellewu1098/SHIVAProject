#include <sdf/core/point3d.hpp>
#include <assert.h>
#include <math.h>

//----------------------------------------------------------------------------------------------------------------------
sdf::point3d::point3d() : x(0.f),y(0.f),z(0.f) {}
//----------------------------------------------------------------------------------------------------------------------
sdf::point3d::point3d(float i_x, float i_y, float i_z) : x(i_x), y(i_y), z(i_z) {}
//----------------------------------------------------------------------------------------------------------------------
sdf::point3d::point3d(const point3d& i_copy) : x(i_copy.x),y(i_copy.y),z(i_copy.z) {}

//----------------------------------------------------------------------------------------------------------------------
void sdf::point3d::set(float i_x, float i_y, float i_z)
{
	x=i_x;
	y=i_y;
	z=i_z;
}

//----------------------------------------------------------------------------------------------------------------------
const float& sdf::point3d::operator[](unsigned int i) const
{
	return (&x)[i];
}

//----------------------------------------------------------------------------------------------------------------------
float& sdf::point3d::operator[](unsigned int i)
{
	return (&x)[i];
}

//----------------------------------------------------------------------------------------------------------------------
float sdf::point3d::dot(const point3d& i_other) const
{
	return x*i_other.x+y*i_other.y+z*i_other.z;
}
//----------------------------------------------------------------------------------------------------------------------
void sdf::point3d::cross(const point3d& i_first, const point3d& i_second)
{
	x=i_first.y*i_second.z - i_first.z*i_second.y;
	y=i_first.z*i_second.x - i_first.x*i_second.z;
	z=i_first.x*i_second.y - i_first.y*i_second.x;
}

//----------------------------------------------------------------------------------------------------------------------
float sdf::point3d::length_squared() const
{
	return x*x+y*y+z*z;
}

//----------------------------------------------------------------------------------------------------------------------
float sdf::point3d::distance_squared_to(const point3d& i_other) const
{
	float xdiff = i_other.x-x;
	float ydiff = i_other.y-y;
	float zdiff = i_other.z-z;
	return xdiff*xdiff+ydiff*ydiff+zdiff*zdiff;
}

//----------------------------------------------------------------------------------------------------------------------
sdf::point3d sdf::point3d::operator*(float scale) const
{
	return point3d(
		x*scale,
		y*scale,
		z*scale
		);
}

//----------------------------------------------------------------------------------------------------------------------
sdf::point3d sdf::point3d::operator+(const point3d& i_other) const
{
	return point3d(
		x+i_other.x,
		y+i_other.y,
		z+i_other.z
		);
}

//----------------------------------------------------------------------------------------------------------------------
sdf::point3d sdf::point3d::operator-(const point3d& i_other) const
{
	return point3d(
		x-i_other.x,
		y-i_other.y,
		z-i_other.z
		);
}

//----------------------------------------------------------------------------------------------------------------------
sdf::point3d& sdf::point3d::operator+=(const point3d& i_other)
{
	x+=i_other.x;
	y+=i_other.y;
	z+=i_other.z;
	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
sdf::point3d& sdf::point3d::operator/=(float i_divider)
{
        const float multiplier = 1.f/i_divider;
        x*=multiplier;
        y*=multiplier;
        z*=multiplier;
        return *this;
}
//----------------------------------------------------------------------------------------------------------------------
sdf::point3d& sdf::point3d::operator/(const point3d& i_div)
{
        x/=i_div.x;
        y/=i_div.y;
        z/=i_div.z;
        return *this;
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::point3d::operator ==(const point3d& i_other) const
{
	return x==i_other.x && y==i_other.y && z==i_other.z;
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::point3d::normalize()
{
	const float ltsq = length_squared();
        //assert(ltsq!=0.f);

	const float scale = 1.f/sqrt(ltsq);
	x*=scale;
	y*=scale;
	z*=scale;
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::point3d::barycentric(const point3d& i_first, float alpha, const point3d& i_second, float beta, const point3d& i_third, float gamma)
{
	x=i_first.x*alpha+i_second.x*beta+i_third.x*gamma;
	y=i_first.y*alpha+i_second.y*beta+i_third.y*gamma;
	z=i_first.z*alpha+i_second.z*beta+i_third.z*gamma;
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::point3d::subtract(const point3d& i_left, const point3d& i_right)
{
	x=i_left.x-i_right.x;
	y=i_left.y-i_right.y;
	z=i_left.z-i_right.z;
}
