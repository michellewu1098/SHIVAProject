#include <sdf/lookup/octnode.hpp>
#include <sdf/lookup/octree.hpp>
#include <sdf/distance/distance_to_aabb.hpp>
#include <sdf/distance/distance_to_triangle.hpp>
#include <assert.h>
#include <algorithm>

//----------------------------------------------------------------------------------------------------------------------
const unsigned int leaf_marker = (unsigned int)-1;

//----------------------------------------------------------------------------------------------------------------------
sdf::detail::octnode::octnode() : m_box(), m_children(leaf_marker)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::octnode::is_leaf() const
{
	return m_children==leaf_marker;
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::octnode::is_node() const
{
	return !is_leaf();
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::octnode::set_children(octree::child_index startindex)
{
	m_children=startindex;
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::octnode::has_children() const
{
	return is_node();
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::aabb& sdf::detail::octnode::get_bounding_box() const
{
	return m_box;
}

//----------------------------------------------------------------------------------------------------------------------
sdf::detail::octnode::child_index sdf::detail::octnode::operator[](unsigned int i) const
{
	assert(i<8);
	return m_children+i;
}

//----------------------------------------------------------------------------------------------------------------------
sdf::detail::octnode::child_index sdf::detail::octnode::child(unsigned int i) const
{
	assert(i<8);
	return m_children+i;
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::octnode::build_box(const mesh& i_mesh, const polygon_array& i_polygon, aabb* o_box) const
{
	aabb& box = *o_box;
	const unsigned int num_polys = (unsigned int) i_polygon.size();
	for (unsigned int i=0;i<num_polys;i++)
	{
		const unsigned int base = i_polygon[i]*3;
		box.include(i_mesh.vertex(base+0));
		box.include(i_mesh.vertex(base+1));
		box.include(i_mesh.vertex(base+2));
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::octnode::has_reached_leaf(unsigned int i_depth, const polygon_array& i_polygon, const settings& i_settings) const
{
	return 
		i_depth >= i_settings.max_depth() || 
		/*(unsigned int)i_polygon.size() <= i_settings.minimum_count() || // Useless ? */
		(unsigned int)i_polygon.size() <= i_settings.optimum_count();
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::octnode::set_leaf(octree* io_tree, const polygon_array& i_polygon)
{
	io_tree->append_polygons(i_polygon,&m_offset,&m_size);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::octnode::set_node(
							octree* const io_tree,
							const mesh& i_mesh,
							const polygon_array& i_polygons,
							unsigned int i_depth,
							const settings& i_settings)
{

	point3d centre;
	m_box.centre(&centre);
	const unsigned int poly_count = (unsigned int)i_polygons.size();


	polygon_array buckets[8];
	const unsigned int expected_count = poly_count/8;
	for (unsigned int i=0;i<8;i++)
		buckets[i].reserve(expected_count);

	const point3d& mi = m_box.minimum();
	const point3d& ma = m_box.maximum();
	const aabb boxes[] =
	{
		aabb(point3d(mi[0],mi[1],mi[2]),centre),
		aabb(point3d(mi[0],mi[1],ma[2]),centre),
		aabb(point3d(mi[0],ma[1],mi[2]),centre),
		aabb(point3d(mi[0],ma[1],ma[2]),centre),
		aabb(point3d(ma[0],mi[1],mi[2]),centre),
		aabb(point3d(ma[0],mi[1],ma[2]),centre),
		aabb(point3d(ma[0],ma[1],mi[2]),centre),
		aabb(point3d(ma[0],ma[1],ma[2]),centre)
	};

	polygon_array toobigtofitinchildren;
	for (unsigned int i=0;i<poly_count;i++)
	{
		const unsigned int face = i_polygons[i];
		const unsigned int base = face*3;
		aabb tribox(
			i_mesh.vertex(base+0),
			i_mesh.vertex(base+1),
			i_mesh.vertex(base+2));
		assert(tribox.intersects(m_box));
		int included(0);
		int bucketid(0);
		for (int j=0;j<8;j++)
		{
			if (tribox.intersects(boxes[j]))
			{
				included++;
				bucketid=j;
			}
		}
		assert(included!=0);
		if (included>1)
		{
			
			toobigtofitinchildren.push_back(face);
		}
		else
			buckets[bucketid].push_back(face);

	}
	set_leaf(io_tree,toobigtofitinchildren);
	io_tree->build_children(i_mesh,i_settings,buckets,boxes,i_depth+1,&m_children);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::octnode::build(
						 octree* io_tree,
						 const mesh& i_mesh,
						 const aabb& i_box,
						 const polygon_array& i_polygons,
						 unsigned int i_depth,
						 const settings& i_settings)
{
	m_box=i_box;

	if (has_reached_leaf(i_depth,i_polygons,i_settings))
		set_leaf(io_tree,i_polygons);
	else
		set_node(io_tree,i_mesh,i_polygons,i_depth,i_settings);
}


//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::octnode::brute_force(
							  const octree* const io_octree,
							  const mesh& i_mesh,
							  const point3d& i_position,
							  distance_record* io_closest_record,
							  unsigned int* o_face_id) const
{
	distance_to_triangle tridist(i_position);

	const unsigned int num_polys = m_size;
	const polygon_array& polygons = io_octree->array();
	for (unsigned int i=0;i<num_polys;i++)
	{
		const unsigned int face = polygons[m_offset+i];
		const unsigned int base = face*3;

		distance_record current;
		tridist(i_mesh.vertex(base+0),i_mesh.vertex(base+1),i_mesh.vertex(base+2),&current);
		
		if (current<*io_closest_record)
		{
			*io_closest_record = current;
			*o_face_id = face;
		}
	}


	if (is_node())
	{
		distance_to_aabb boxdist(i_position);

		// This is the most naive way!
		for (unsigned int i=0;i<8;i++)
		{
			const octnode& child = io_octree->node(m_children+i);
			const float sqdist = boxdist(child.get_bounding_box());
			if (sqdist<io_closest_record->distance_square())
			{
				child(io_octree,i_mesh,i_position,io_closest_record,o_face_id);
			}
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::octnode::sorted_look(
							  const octree* const io_octree,
							  const mesh& i_mesh,
							  const point3d& i_position,
							  distance_record* io_closest_record,
							  unsigned int* o_face_id) const
{
	distance_to_triangle tridist(i_position);

	const unsigned int num_polys = m_size;
	const polygon_array& polygons = io_octree->array();
	for (unsigned int i=0;i<num_polys;i++)
	{
		const unsigned int face = polygons[m_offset+i];
		const unsigned int base = face*3;

		distance_record current;
		tridist(i_mesh.vertex(base+0),i_mesh.vertex(base+1),i_mesh.vertex(base+2),&current);
		
		if (current<*io_closest_record)
		{
			*io_closest_record = current;
			*o_face_id = face;
		}
	}


	if (is_node())
	{
		distance_to_aabb boxdist(i_position);
		struct node_dist
		{
			float distsq;
			unsigned int id;
                        static bool less_than(const node_dist& i_first, const node_dist& i_other)
			{
                                return i_first.distsq<i_other.distsq;
			}
		};
		node_dist boxsqdist[8];

		for (unsigned int i=0;i<8;i++)
		{
			const octnode& child = io_octree->node(m_children+i);
			boxsqdist[i].distsq = boxdist(child.get_bounding_box());
			boxsqdist[i].id = i;
		}

                /*std::sort(&boxsqdist[0],&boxsqdist[8],node_dist::less_than);*/

		for (unsigned int i=0;i<8;i++)
		{
			const octnode& child = io_octree->node(m_children+boxsqdist[i].id);
			
			// ::TODO:: Should recompute the distance here to do more rejections
			if (boxsqdist[i].distsq<io_closest_record->distance_square())
			{
				child(io_octree,i_mesh,i_position,io_closest_record,o_face_id);
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
sdf::index_type sdf::detail::octnode::polygon_start() const
{
	return m_offset;
}


//----------------------------------------------------------------------------------------------------------------------
sdf::index_type sdf::detail::octnode::polygon_end() const
{
	return m_offset+m_size;
}
