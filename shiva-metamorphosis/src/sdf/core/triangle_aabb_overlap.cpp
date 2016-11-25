#include <math.h>
#include <sdf/core/tools.hpp>
#include <sdf/core/triangle_aabb_overlap.hpp>

namespace sdf
{
	namespace detail
	{
		//----------------------------------------------------------------------------------------------------------------------
		bool plane_box(const vector3d& i_normal, float i_dist, const vector3d& i_max);
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::triangle_box_overlap(const point3d& i_centre, const vector3d& i_halfextent, const point3d& i_first, const point3d& i_second, const point3d& i_third)
{
	// The code was edited to remove the defines - and use C++ instead of C
	// The original code and its comments:

	// http://jgt.akpeters.com/papers/AkenineMoller01/tribox.html

	// AABB-triangle overlap test code                     
	// by Tomas Akenine-Möller                             
	// Function: int triBoxOverlap(float boxcenter[3],     
	//          float boxhalfsize[3],float triverts[3][3]);
	// History:                                            
	//   2001-03-05: released the code in its first version
	//   2001-06-18: changed the order of the tests, faster
	//                                                     
	// Acknowledgement: Many thanks to Pierre Terdiman for 
	// suggestions and discussions on how to optimize code.
	// Thanks to David Hunt for finding a ">="-bug!        

	

	// use separating axis theorem to test overlap between triangle and box
	// need to test for overlap in these directions:
	// 1) the {x,y,z}-directions (actually, since we use the AABB of the triangle
	//    we do not even need to test these)
	// 2) normal of the triangle
	// 3) crossproduct(edge from tri, {x,y,z}-directin)
	//    this gives 3x3=9 more tests

	// This is the fastest branch on Sun
	// move everything so that the boxcenter is in (0,0,0)
	vector3d v0; v0.subtract(i_first,i_centre);
	vector3d v1; v1.subtract(i_second,i_centre);
	vector3d v2; v2.subtract(i_third,i_centre);

	// compute triangle edges
	vector3d e0; e0.subtract(v1,v0);
	vector3d e1; e1.subtract(v2,v1);
	vector3d e2; e2.subtract(v0,v2);

	// Bullet 3:
	//  test the 9 tests first (this was faster)
	float fex = fabs(e0[0]);
	float fey = fabs(e0[1]);
	float fez = fabs(e0[2]);
	float minimum(0);
	float maximum(0);
	//AXISTEST_***(a, b, fa, fb) prototype
	//AXISTEST_X01(e0[2], e0[1], fez, fey);
	{
		minmax(
			e0[2]*v0[1] - e0[1]*v0[2],
			e0[2]*v2[1] - e0[1]*v2[2],
			&minimum,&maximum);
		const float rad = fez * i_halfextent[1] + fey * i_halfextent[2];
		if(minimum>rad || maximum<-rad) 
			return false;
	}
	//AXISTEST_Y02(e0[2], e0[0], fez, fex)
	{
		minmax(
			-e0[2]*v0[0] + e0[0]*v0[2],
			-e0[2]*v2[0] + e0[0]*v2[2],
			&minimum,&maximum);
		const float rad = fez * i_halfextent[0] + fex * i_halfextent[2];
		if(minimum>rad || maximum<-rad) 
			return false;
	}
	//AXISTEST_Z12(e0[1], e0[0], fey, fex);
	{
		minmax(
			e0[1]*v1[0] - e0[0]*v1[1],
			e0[1]*v2[0] - e0[0]*v2[1],
			&minimum,&maximum);
		const float rad = fey * i_halfextent[0] + fex * i_halfextent[1];
		if(minimum>rad || maximum<-rad) 
			return false;
	}

	fex = fabs(e1[0]);
	fey = fabs(e1[1]);
	fez = fabs(e1[2]);
	//AXISTEST_X01(e1[2], e1[1], fez, fey);
	{
		minmax(
			e1[2]*v0[1] - e1[1]*v0[2],
			e1[2]*v2[1] - e1[1]*v2[2],
			&minimum,&maximum);
		const float rad = fez * i_halfextent[1] + fey * i_halfextent[2];   \
			if(minimum>rad || maximum<-rad) 
				return false;
	}
	//AXISTEST_Y02(e1[2], e1[0], fez, fex);
	{
		minmax(
			-e1[2]*v0[0] + e1[0]*v0[2],
			-e1[2]*v2[0] + e1[0]*v2[2],
			&minimum,&maximum);
		const float rad = fez * i_halfextent[0] + fex * i_halfextent[2];
		if(minimum>rad || maximum<-rad) 
			return false;
	}
	//AXISTEST_Z0(e1[1], e1[0], fey, fex);
	{
		minmax(
			e1[1]*v0[0] - e1[0]*v0[1],
			e1[1]*v1[0] - e1[0]*v1[1],
			&minimum,&maximum);
		const float rad = fey * i_halfextent[0] + fex * i_halfextent[1];
		if(minimum>rad || maximum<-rad) 
			return false;
	}

	fex = fabs(e2[0]);
	fey = fabs(e2[1]);
	fez = fabs(e2[2]);
	//AXISTEST_X2(e2[2], e2[1], fez, fey);
	{
		minmax(
			e2[2]*v0[1] - e2[1]*v0[2],
			e2[2]*v1[1] - e2[1]*v1[2],
			&minimum,&maximum);
		const float rad = fez * i_halfextent[1] + fey * i_halfextent[2];
		if(minimum>rad || maximum<-rad) 
			return false;
	}
	//AXISTEST_Y1(e2[2], e2[0], fez, fex);
	{
		minmax(
			-e2[2]*v0[0] + e2[0]*v0[2],
			-e2[2]*v1[0] + e2[0]*v1[2],
			&minimum,&maximum);
		const float rad = fez * i_halfextent[0] + fex * i_halfextent[2];
		if(minimum>rad || maximum<-rad) 
			return false;
	}
	//AXISTEST_Z12(e2[1], e2[0], fey, fex);
	{
		minmax(
			e2[1]*v1[0] - e2[0]*v1[1],
			e2[1]*v2[0] - e2[0]*v2[1],
			&minimum,&maximum);
		const float rad = fey * i_halfextent[0] + fex * i_halfextent[1];
		if(minimum>rad || maximum<-rad) 
			return false;
	}

	// Bullet 1:
	//  first test overlap in the {x,y,z}-directions 
	//  find min, max of the triangle each direction, and test for overlap in 
	//  that direction -- this is equivalent to testing a minimal AABB around
	//  the triangle against the AABB

	// test in X-direction
	minmax(v0[0],v1[0],v2[0],&minimum,&maximum);
	if(minimum>i_halfextent[0] || maximum<-i_halfextent[0]) 
		return false;

	// test in Y-direction
	minmax(v0[1],v1[1],v2[1],&minimum,&maximum);
	if(minimum>i_halfextent[1] || maximum<-i_halfextent[1])
		return false;

	// test in Z-direction
	minmax(v0[2],v1[2],v2[2],&minimum,&maximum);
	if(minimum>i_halfextent[2] || maximum<-i_halfextent[2]) 
		return false;

	// Bullet 2:
	//  test if the box intersects the plane of the triangle
	//  compute plane equation of triangle: normal*x+d=0
	vector3d normal; normal.cross(e0,e1);
	const float d=-normal.dot(v0);  // plane eq: normal.x+d=0
	if(!detail::plane_box(normal,d,i_halfextent)) 
		return false;

	// box and triangle overlaps
	return true;   
}



//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::plane_box(const vector3d& i_normal, float i_dist, const vector3d& i_max)
{
	vector3d vmin,vmax;
	for (unsigned int i=0;i<3;i++)
	{
		if (i_normal[i]>0.f)
		{
			vmin[i]=-i_max[i];
			vmax[i]=+i_max[i];
		}
		else
		{
			vmin[i]=+i_max[i];
			vmax[i]=-i_max[i];
		}
	}

	if (i_normal.dot(vmin)+i_dist>0.f)
		return false;
	if (i_normal.dot(vmax)+i_dist>=0.f)
		return true;

	return false;
}
