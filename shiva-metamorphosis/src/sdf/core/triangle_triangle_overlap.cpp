#include <sdf/core/triangle_triangle_overlap.hpp>
#include <sdf/core/tools.hpp>
#include <sdf/core/types.hpp>
#include <math.h>

namespace detail
{
	//----------------------------------------------------------------------------------------------------------------------
	struct intervals
	{
		float a,b,c;
		float x0,x1;
	};

	bool edge_edge_test(
		const sdf::point3d& V0,
		const sdf::point3d& U0,
		const sdf::point3d& U1,
		float Ax, float Ay,
		short i0, short i1);

	bool edge_against_triangle_edges(
		const sdf::point3d& V0,
		const sdf::point3d& V1,
		const sdf::point3d& U0,
		const sdf::point3d& U1,
		const sdf::point3d& U2,
		short i0, short i1);

	bool point_in_triangle(
		const sdf::point3d& V0,
		const sdf::point3d& U0,
		const sdf::point3d& U1,
		const sdf::point3d& U2,
		short i0, short i1);

	bool coplanar_tri_tri(
		const sdf::point3d& N,
		const sdf::point3d& V0,const sdf::point3d& V1,const sdf::point3d& V2,
		const sdf::point3d& U0,const sdf::point3d& U1,const sdf::point3d& U2);

	bool newcompute_intervals(
		const sdf::point3d& vv,
		const sdf::point3d& d,
		float d0d1, float d0d2,
		detail::intervals* o_int);

	bool no_division_triangle_intersection(const sdf::point3d& V0,const sdf::point3d& V1,const sdf::point3d& V2,
		const sdf::point3d& U0,const sdf::point3d& U1,const sdf::point3d& U2);
}


//----------------------------------------------------------------------------------------------------------------------
bool sdf::triangle_triangle_overlap(
									const point3d& v0, const point3d& v1, const point3d& v2,
									const point3d& u0, const point3d& u1, const point3d& u2)
{
	return detail::no_division_triangle_intersection(v0,v1,v2,u0,u1,u2);
}


//Triangle/triangle intersection test routine,
//by Tomas Moller, 1997.
//See article "A Fast Triangle-Triangle Intersection Test",
//Journal of Graphics Tools, 2(2), 1997



//----------------------------------------------------------------------------------------------------------------------
bool detail::edge_edge_test(
							const sdf::point3d& V0,
							const sdf::point3d& U0,
							const sdf::point3d& U1,
							float Ax, float Ay,
							short i0, short i1)
{
	// This edge to edge test is based on Franlin Antonio's gem:
	// "Faster Line Segment Intersection", in Graphics Gems III, pp. 199-202
	const float Bx=U0[i0]-U1[i0];
	const float By=U0[i1]-U1[i1];
	const float Cx=V0[i0]-U0[i0];
	const float Cy=V0[i1]-U0[i1];
	const float f=Ay*Bx-Ax*By;
	const float d=By*Cx-Bx*Cy;
	if((f>0 && d>=0 && d<=f) || (f<0 && d<=0 && d>=f))
	{
		const float e=Ax*Cy-Ay*Cx;
		if(f>0)
		{
			if(e>=0 && e<=f) 
				return true;
		}
		else
		{
			if(e<=0 && e>=f) 
				return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool detail::edge_against_triangle_edges(
	const sdf::point3d& V0,
	const sdf::point3d& V1,
	const sdf::point3d& U0,
	const sdf::point3d& U1,
	const sdf::point3d& U2,
	short i0, short i1)
{
	const float Ax=V1[i0]-V0[i0];
	const float Ay=V1[i1]-V0[i1];

	if (edge_edge_test(V0,U0,U1,Ax,Ay,i0,i1))
		return true;
	if (edge_edge_test(V0,U1,U2,Ax,Ay,i0,i1))
		return true;
	if (edge_edge_test(V0,U2,U0,Ax,Ay,i0,i1))
		return true;

	return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool detail::point_in_triangle(
							   const sdf::point3d& V0,
							   const sdf::point3d& U0,
							   const sdf::point3d& U1,
							   const sdf::point3d& U2,
							   short i0, short i1)
{
	// is T1 completly inside T2?
	// check if V0 is inside tri(U0,U1,U2)
	float d0(0);
	{
		const float a=U1[i1]-U0[i1];
		const float b=-(U1[i0]-U0[i0]);
		const float c=-a*U0[i0]-b*U0[i1];
		d0=a*V0[i0]+b*V0[i1]+c;
	}


	float d1(0);
	{
		const float a=U2[i1]-U1[i1];
		const float b=-(U2[i0]-U1[i0]);
		const float c=-a*U1[i0]-b*U1[i1];
		d1=a*V0[i0]+b*V0[i1]+c;
	}

	float d2(0);
	{
		const float a=U0[i1]-U2[i1];
		const float b=-(U0[i0]-U2[i0]);
		const float c=-a*U2[i0]-b*U2[i1];
		d2=a*V0[i0]+b*V0[i1]+c;
	}
	if(d0*d1>0.f)
		if(d0*d2>0.f) 
			return true;

	return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool detail::coplanar_tri_tri(const sdf::point3d& N,const sdf::point3d& V0,const sdf::point3d& V1,const sdf::point3d& V2,
							  const sdf::point3d& U0,const sdf::point3d& U1,const sdf::point3d& U2)
{
	float A[3];
	short i0,i1;
	// first project onto an axis-aligned plane, that maximizes the area 
	// of the triangles, compute indices: i0,i1. 
	A[0]=fabs(N[0]);
	A[1]=fabs(N[1]);
	A[2]=fabs(N[2]);
	if(A[0]>A[1])
	{
		if(A[0]>A[2])
		{
			// A[0] is greatest
			i0=1;
			i1=2;
		}
		else
		{
			// A[2] is greatest
			i0=0;
			i1=1;
		}
	}
	// A[0]<=A[1]
	else
	{
		if(A[2]>A[1])
		{
			// A[2] is greatest
			i0=0;
			i1=1;
		}
		else
		{
			// A[1] is greatest
			i0=0;
			i1=2;
		}
	}

	// test all edges of triangle 1 against the edges of triangle 2
	if (edge_against_triangle_edges(V0,V1,U0,U1,U2,i0,i1))
		return true;
	if (edge_against_triangle_edges(V1,V2,U0,U1,U2,i0,i1))
		return true;
	if (edge_against_triangle_edges(V2,V0,U0,U1,U2,i0,i1))
		return true;

	// finally, test if tri1 is totally contained in tri2 or vice versa
	if (point_in_triangle(V0,U0,U1,U2,i0,i1))
		return true;
	if (point_in_triangle(U0,V0,V1,V2,i0,i1))
		return true;

	return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool detail::newcompute_intervals(
								  const sdf::point3d& vv,
								  const sdf::point3d& d,
								  float d0d1, float d0d2,
								  detail::intervals* o_int)
{
	if(d0d1>0.0f)
	{
		// here we know that D0D2<=0.0
		// that is D0, D1 are on the same side, D2 on the other or on the plane
		o_int->a=vv[2]; 
		o_int->b=(vv[0]-vv[2])*d[2];
		o_int->c=(vv[1]-vv[2])*d[2];
		o_int->x0=d[2]-d[0];
		o_int->x1=d[2]-d[1];
	}
	else if(d0d2>0.0f)
	{
		// here we know that d0d1<=0.0
		o_int->a=vv[1]; 
		o_int->b=(vv[0]-vv[1])*d[1]; 
		o_int->c=(vv[2]-vv[1])*d[1]; 
		o_int->x0=d[1]-d[0]; 
		o_int->x1=d[1]-d[2];
	}
	else if(d[1]*d[2]>0.0f || d[0]!=0.0f)
	{
		// here we know that d0d1<=0.0 or that D0!=0.0
		o_int->a=vv[0];
		o_int->b=(vv[1]-vv[0])*d[0];
		o_int->c=(vv[2]-vv[0])*d[0];
		o_int->x0=d[0]-d[1];
		o_int->x1=d[0]-d[2];
	}
	else if(d[1]!=0.0f)
	{
		o_int->a=vv[1];
		o_int->b=(vv[0]-vv[1])*d[1];
		o_int->c=(vv[2]-vv[1])*d[1];
		o_int->x0=d[1]-d[0];
		o_int->x1=d[1]-d[2];
	}
	else if(d[2]!=0.0f)
	{
		o_int->a=vv[2];
		o_int->b=(vv[0]-vv[2])*d[2];
		o_int->c=(vv[1]-vv[2])*d[2];
		o_int->x0=d[2]-d[0];
		o_int->x1=d[2]-d[1];
	}
	else
	{
		// triangles are coplanar
		return true;
	}
	return false; // If not, carry on
}

//----------------------------------------------------------------------------------------------------------------------
bool detail::no_division_triangle_intersection(const sdf::point3d& V0,const sdf::point3d& V1,const sdf::point3d& V2,
											   const sdf::point3d& U0,const sdf::point3d& U1,const sdf::point3d& U2)
{
	// compute plane equation of triangle(V0,V1,V2)
	sdf::point3d E1 = V1-V0;
	sdf::point3d E2 = V2-V0;

	sdf::point3d N1;
	N1.cross(E1,E2);
	const float d1=-N1.dot(V0);
	// plane equation 1: N1.X+d1=0

	// put U0,U1,U2 into plane equation 1 to compute signed distances to the plane
	sdf::point3d du(N1.dot(U0)+d1,N1.dot(U1)+d1,N1.dot(U2)+d1);

	// coplanarity robustness check
	const bool use_epsilon_test = false;
	const float epsilon = 0.000001f;
	if (use_epsilon_test)
	{
		if(fabs(du[0])<epsilon) du[0]=0.f;
		if(fabs(du[1])<epsilon) du[1]=0.f;
		if(fabs(du[2])<epsilon) du[2]=0.f;
	}

	const float du0du1=du[0]*du[1];
	const float du0du2=du[0]*du[2];

	// same sign on all of them + not equal 0 ?
	// no intersection occurs
	if(du0du1>0.0f && du0du2>0.0f) 
		return false; 

	// compute plane of triangle (U0,U1,U2)
	E1 = U1-U0;
	E2 = U2-U0;
	sdf::point3d N2;
	N2.cross(E1,E2);
	const float d2=-N2.dot(U0);
	// plane equation 2: N2.X+d2=0

	// put V0,V1,V2 into plane equation 2
	sdf::point3d dv(N2.dot(V0)+d2,N2.dot(V1)+d2,N2.dot(V2)+d2);

	if (use_epsilon_test)
	{
		if(fabs(dv[0])<epsilon) dv[0]=0.f;
		if(fabs(dv[1])<epsilon) dv[1]=0.f;
		if(fabs(dv[2])<epsilon) dv[2]=0.f;
	}

	const float dv0dv1=dv[0]*dv[1];
	const float dv0dv2=dv[0]*dv[2];

	// same sign on all of them + not equal 0 ?
	// no intersection occurs
	if(dv0dv1>0.0f && dv0dv2>0.0f) 
		return false;

	// compute direction of intersection line
	sdf::point3d D;
	D.cross(N1,N2);

	// compute and index to the largest component of D
	float maximum=fabs(D[0]);
	short index(0);
	const float bb=fabs(D[1]);
	const float cc=fabs(D[2]);
	if(bb>maximum)
	{
		maximum=bb;
		index=1;
	}
	if(cc>maximum)
	{
		maximum=cc;
		index=2;
	}

	// this is the simplified projection onto L
	sdf::point3d vp(V0[index],V1[index],V2[index]);
	sdf::point3d up(U0[index],U1[index],U2[index]);

	// ::WARNING:: Both us N1, is this normal ?
	// compute interval for triangle 1
	detail::intervals tri1,tri2;
	if (detail::newcompute_intervals(vp,dv,dv0dv1,dv0dv2,&tri1))
		return detail::coplanar_tri_tri(N1,V0,V1,V2,U0,U1,U2);
	// compute interval for triangle 2
	if (detail::newcompute_intervals(up,du,du0du1,du0du2,&tri2))
		return detail::coplanar_tri_tri(N2,V0,V1,V2,U0,U1,U2);

	const float xx=tri1.x0*tri1.x1;
	const float yy=tri2.x0*tri2.x1;
	const float xxyy=xx*yy;

	const float tmp1=tri1.a*xxyy;
	float isect1[] = 
	{
		tmp1+tri1.b*tri1.x1*yy,
		tmp1+tri1.c*tri1.x0*yy
	};

	const float tmp2=tri2.a*xxyy;
	float isect2[] =
	{
		tmp2+tri2.b*tri2.x1*xx,
		tmp2+tri2.c*tri2.x0*xx
	};

	sdf::minmax(&isect1[0],&isect1[1]);
	sdf::minmax(&isect2[0],&isect2[1]);

	if(isect1[1]<isect2[0] || isect2[1]<isect1[0]) 
		return false;

	return true;
}