#include <sdf/distance/distance_to_triangle.hpp>
#include <math.h>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
sdf::distance_to_triangle::distance_to_triangle() : m_position(0,0,0) {}
//----------------------------------------------------------------------------------------------------------------------
sdf::distance_to_triangle::distance_to_triangle(const point3d& i_position) : m_position(i_position) {}


//----------------------------------------------------------------------------------------------------------------------
float sdf::distance_to_triangle::barycentric_method(const point3d& i_p1, const point3d& i_p2, const point3d& i_p3, distance_record* o_record) const
{
    // This is the implementation from Real-Time Collision Detection, 
	// 5.1.5 Closest Point on Triangle to Point, from page 136 to page 142
    // By Christer Ericson, 2005 - Morgran Kaufmann
    //--------------------------------------------------------------------------------------
    // This solution seems to be a faster solution that the calculus solution from David Eberly
    // Code is a lot simpler and cleaner
	// There are more computations, but more suitable for the SSE instructions
	// And less if/else blocks
	// This solution also does not rely so much on the cross product (it does indirectly)

    // Precondition is:
    // Triangle is not degenerated, that is, the points are not aligned

	const vector3d ab = i_p2-i_p1;
	const vector3d ac = i_p3-i_p1;
	const vector3d ap = m_position-i_p1;

	const float d1 = ab.dot(ap);
	const float d2 = ac.dot(ap);

	if (d1<=0.f && d2<=0.f)
	{
		o_record->set_feature(vertex_a);
		o_record->set_closest_point(i_p1);
		o_record->set_distance_squared(ap.length_squared());
		return o_record->distance_square();
	}

	const vector3d bp = m_position-i_p2;
	const float d3 = ab.dot(bp);
	const float d4 = ac.dot(bp);

	if (d3>=0.f && d4<=d3)
	{
		o_record->set_feature(vertex_b);
		o_record->set_closest_point(i_p2);
		o_record->set_distance_squared(bp.length_squared());
		return o_record->distance_square();
	}

	const float vc = d1*d4 - d3*d2;
	if (vc<=0.f && d1 >=0.f && d3<=0.f)
	{
		const float v = d1/(d1-d3);
		const point3d result = i_p1+ab*v;
		o_record->set_feature(edge_ab);
		o_record->set_closest_point(result);
		o_record->set_distance_squared(result.distance_squared_to(m_position));
		return o_record->distance_square();
	}

	const vector3d cp = m_position - i_p3;
	const float d5 = ab.dot(cp);
	const float d6 = ac.dot(cp);

	if (d6>=0.f && d5<=d6)
	{
		o_record->set_feature(vertex_c);
		o_record->set_closest_point(i_p3);
		o_record->set_distance_squared(cp.length_squared());
		return o_record->distance_square();
	}

	const float vb = d5*d2 - d1*d6;
	if (vb<=0.f && d2>=0.f && d6<=0.f)
	{
		const float w = d2/(d2-d6);
		const point3d result = i_p1+ac*w;
		o_record->set_feature(edge_ca);
		o_record->set_closest_point(result);
		o_record->set_distance_squared(result.distance_squared_to(m_position));
		return o_record->distance_square();
	}

	const float va = d3*d6-d5*d4;
	if (va<=0.f && (d4-d3)>=0.f && (d5-d6)>=0.f)
	{
		const float w = (d4-d3)/((d4-d3)+(d5-d6));
		const point3d result = i_p2+(i_p3-i_p2)*w;
		o_record->set_feature(edge_bc);
		o_record->set_closest_point(result);
		o_record->set_distance_squared(result.distance_squared_to(m_position));
		return o_record->distance_square();
	}

	const float denom = 1.f/(va+vb+vc);
	const float v = vb*denom;
	const float w = vc*denom;

	const point3d result = i_p1+ab*v+ac*w;
	o_record->set_feature(face);
	o_record->set_closest_point(result);
	o_record->set_distance_squared(result.distance_squared_to(m_position));
	return o_record->distance_square();

}

//----------------------------------------------------------------------------------------------------------------------
float sdf::distance_to_triangle::calculus_method(const point3d& i_p1, const point3d& i_p2, const point3d& i_p3, distance_record* o_record) const
{

    // This is a direct implementation of the report
    // "Distance Between Point and Tirangle in 3D" by David Eberly, 1999 (modified in 2008)
    // Geometric Tools, LLC
    //--------------------------------------------------------------------------------------
    // The triangle is defined with a base (B) and two edges (E0, E1)
    // The new coordinates in this new reference frame are (s,t)

    // Precondition is:
    // Triangle is not degenerated, that is, the points are not aligned

    const point3d B = i_p1;
    const vector3d E0 = i_p2-i_p1;
    const vector3d E1 = i_p3-i_p1;
    const vector3d pointToBase = B-m_position;

    // From section 1, Q(s,t) = as2 + 2bst + ct2 + 2ds + 2et + f
    // where
    // a = E0.E0
    // b = E0.E1
    // c = E1.E1
    // d = E0.(B-P)
    // e = E1.(B-P)
    // f = (B-P).(B-P)

    // Those are independent from the point
    const float a = E0.dot(E0);
    const float b = E0.dot(E1);
    const float c = E1.dot(E1);
    // While those are
    const float d = E0.dot(pointToBase);
    const float e = E1.dot(pointToBase);
    //const float f = pointToBase.dot(pointToBase);

    // Now get the coordinates (s,t) in the scaled domain [o,delta] to differ division
    float s = b*e-c*d;
    float t = b*d-a*e;
    const float delta = fabs(a*c-b*b); // Absolute value for test

    /*
     *     ^ t
     * \ R2|
     *  \  |
     *   \ |
     *    \|
     *     *
     *     |\
     *     | \
     *     |  \
     * R3  |   \ R1
     *  E1 |    \
     *     | R0  \
     *     |      \           s
     *-----|-------*----------->
     *   B |   E0   \
     *  R4 |   R5    \  R6
     *     |          \
     *
     *
     *
     *
     *
     */

	
	static const feature_type region[max_close_type] =
	{
		face,
		edge_bc,
		vertex_c,
		edge_ca,
		vertex_a,
		edge_ab,
		vertex_b
	};

    if (s+t<=delta)
    {
        if(s<0)
        {
            if(t<0)
            {
                // Region 4
				o_record->set_feature(region[4]);
                if (d<0)
                {
                    t=0;
                    if (-d>=a)
                    {
                        s=1;
                    }
                    else
                    {
                        s=-d/a;
                    }
                }
                else
                {
                    s=0;
                    if (e>=0)
                    {
                        t=0;
                    }
                    else if (-e>=c)
                    {
                        t=1;
                    }
                    else
                    {
                        t=-e/c;
                    }
                }
            }
            else
            {
                // Region 3
                // F(t) = Q(0,t) = ct2 + 2et + f
                // F'(t)/2 = ct+e
                // F'(T) = 0 when T = -e/c
				o_record->set_feature(region[3]);
                s=0;
                t = ( e>=0 ? 0 : (-e>=c?1:(-e/c)) );
            }
        }
        else if (t<0)
        {
            // Region 5
			o_record->set_feature(region[5]);
            t=0;
            s = ( d>=0 ? 0 : (-d>=a?1:(-d/a)) );
        }
        else
        {
            // Region 0
			o_record->set_feature(region[0]);
            const float deltaScale = 1/delta;
            s*=deltaScale;
            t*=deltaScale;
        }
    }
    else
    {

        if(s<0)
        {
            // Region 2
			o_record->set_feature(region[2]);
            const float t0 = b+d;
            const float t1 = c+e;
            if (t1>t0)
            {
                const float numer = t1-t0;
                const float denom = a-2*b+c;
                s = (numer>=denom?1:(numer/denom));
                t = 1-s;
            }
            else
            {
                s=0;
                t=(t1<=0?1:(e>=0?0:(-e/c)));
            }
        }
        else if (t<0)
        {
            // Region 6
			o_record->set_feature(region[6]);
            const float t0 = b + e;
            const float t1 = a + d;
            if (t1 > t0)
            {
                const float numer = t1 - t0;
                const float denom = a - 2*b + c;
                if (numer >= denom)
                {
                    t = 1;
                    s = 0;
                }
                else
                {
                    t = numer/denom;
                    s = 1 - t;
                }
            }
            else
            {
                t = 0;
                if (t1 <= 0)
                {
                    s = 1;
                }
                else if (d>=0)
                {
                    s = 0;
                }
                else
                {
                    s = -d/a;
                }
            }
        }
        else
        {
			o_record->set_feature(region[1]);
            // Region 1
            // F(s) = Q(s,1-s) = (a-2b+c)s2 + 2(b-c+d-e)s + (c+2e+f)
            // F'(s) = 2(a-2b+c)s + 2(b-c+d-e)
            // F'(s) = 0 when S=(c+e-b-d)/(a-2b+c)
            // a-2b+c = |E0-E1|2>0 so only sign of c+e-b-d need be considered
            const float numer = c+e-b-d;
            const float denom = a-2*b+c; // positive quantity
			s = (numer<=0)? 0 : (numer >= denom ? 1:(numer/denom));
            t = 1-s;
        }
    }
    point3d closest = B+E0*s+E1*t;
	o_record->set_closest_point(closest);
    o_record->set_distance_squared(m_position.distance_squared_to(closest));

	return o_record->distance_square();
}