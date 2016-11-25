#include <sdf/core/static_stack.hpp>
#include <sdf/lookup/bvh_branch.hpp>
#include <sdf/lookup/bvh.hpp>
#include <sdf/distance/distance_to_aabb.hpp>
#include <sdf/core/triangle_triangle_overlap.hpp>
#include <assert.h>

//----------------------------------------------------------------------------------------------------------------------
const sdf::detail::bvh_branch::child_index not_set_yet = static_cast<sdf::detail::bvh_branch::child_index>(0); // Cannot have root as a child
//----------------------------------------------------------------------------------------------------------------------
const sdf::index_type invalid = static_cast<sdf::index_type>(-1);


//----------------------------------------------------------------------------------------------------------------------
sdf::detail::bvh_branch::bvh_branch() : m_box(), m_children(not_set_yet), m_offset(invalid), m_size(0) {}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::bvh_branch::is_empty() const
{
	return m_size==0;
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::bvh_branch::has_children() const
{
	return m_children!=not_set_yet;
}

//----------------------------------------------------------------------------------------------------------------------
sdf::detail::bvh_branch::child_index sdf::detail::bvh_branch::child(unsigned int i) const
{
	assert(i<2);
	return m_children+i;
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::aabb& sdf::detail::bvh_branch::box() const
{
	return m_box;
}

//----------------------------------------------------------------------------------------------------------------------
sdf::index_type sdf::detail::bvh_branch::num_polygons() const
{
	return m_size;
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::bvh_branch::has_reached_leaf(unsigned int i_depth, const polygon_array& i_polygons, const settings& i_settings) const
{
	return !(
		i_depth<i_settings.max_depth() && 
		static_cast<unsigned int>(i_polygons.size()) > i_settings.minimum_count() &&
		static_cast<unsigned int>(i_polygons.size()) > i_settings.optimum_count() );
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::bvh_branch::build(bvh& io_tree, const mesh& i_mesh, const settings& i_settings, const polygon_array& i_polygons,unsigned  int i_depth)
{
	// First round, getting overall bounding box
	aabb thisbox;
	for (polygon_array::size_type i=0;i<i_polygons.size();i++)
	{
		const unsigned int base = i_polygons[i]*3;
		thisbox.include(i_mesh.vertex(base+0));
		thisbox.include(i_mesh.vertex(base+1));
		thisbox.include(i_mesh.vertex(base+2));
	}
	m_box = thisbox;
	vector3d extent;
	m_box.extent(&extent);

	if (has_reached_leaf(i_depth,i_polygons,i_settings)) 
	{
		// If it s a leaf, finalize
		io_tree.append_polygons(i_polygons,&m_offset,&m_size);
	}
	else
	{
		// Recursive build
		polygon_array buckets[2];
		unsigned int axisindex(0);
		vector3d ext;
		m_box.extent(&ext);
		if (ext[1]>ext[0] && ext[1]>ext[2])
			axisindex=1;
		else if (ext[2]>ext[0] && ext[2]>ext[1])
			axisindex=2;

		float midpos(0.f);
       
		for (unsigned int i=0;i<static_cast<unsigned int>(i_polygons.size());i++)
		{
			const unsigned int base = i_polygons[i]*3;
			midpos+=
				i_mesh.vertex(base+0)[axisindex]+
				i_mesh.vertex(base+1)[axisindex]+
				i_mesh.vertex(base+2)[axisindex];
		}
		midpos/=(static_cast<float>(i_polygons.size())*3.f);

		const unsigned int sizeguess = static_cast<unsigned int>(i_polygons.size())/2;
		buckets[0].reserve(sizeguess);
		buckets[1].reserve(sizeguess);

		const float third = 1.f/3.f;
		for (unsigned int i=0;i<static_cast<unsigned int>(i_polygons.size());i++)
		{
			const unsigned int base = i_polygons[i]*3;
			const float trimid=third*(
				i_mesh.vertex(base+0)[axisindex]+
				i_mesh.vertex(base+1)[axisindex]+
				i_mesh.vertex(base+2)[axisindex]);
			if (midpos>trimid)
				buckets[1].push_back(i_polygons[i]);
			else
				buckets[0].push_back(i_polygons[i]);
		}
		io_tree.build_children(i_mesh,i_settings,buckets,i_depth+1,&m_children);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::bvh_branch::recursive_traversal(const bvh& i_tree, const mesh& i_mesh, const point3d& i_position, distance_record* io_closest_record, unsigned int* o_face_id) const
{
	if (has_children())
	{
		distance_to_aabb boxdist(i_position);
#if 1
		const bvh_branch& child0 = i_tree.branch(m_children+0);
		const bvh_branch& child1 = i_tree.branch(m_children+1);

		const float sqdist0 = boxdist(child0.box());
		const float sqdist1 = boxdist(child1.box());
		if (sqdist0<sqdist1)
		{
			if (sqdist0<io_closest_record->distance_square())
				child0(i_tree,i_mesh,i_position,io_closest_record,o_face_id);
			if (sqdist1<io_closest_record->distance_square())
				child1(i_tree,i_mesh,i_position,io_closest_record,o_face_id);
		}
		else
		{
			if (sqdist1<io_closest_record->distance_square())
				child1(i_tree,i_mesh,i_position,io_closest_record,o_face_id);
			if (sqdist0<io_closest_record->distance_square())
				child0(i_tree,i_mesh,i_position,io_closest_record,o_face_id);
		}
#else
		
		const bvh_branch child[2] = { i_tree.branch(m_children+0),i_tree.branch(m_children+1) };
		const float sqdist[] = { boxdist(child[0].box()),boxdist(child[1].box()) };

		const int closestindex = (sqdist[0]<sqdist[1])?0:1;
		const int furthestindex = (sqdist[0]<sqdist[1])?1:0;
		
		if (sqdist[closestindex]<io_closest_record->distance_square())
			child[closestindex](i_tree,i_mesh,i_position,io_closest_record,o_face_id);
		if (sqdist[furthestindex]<io_closest_record->distance_square())
			child[furthestindex](i_tree,i_mesh,i_position,io_closest_record,o_face_id);
#endif
	}
	else
	{
		const polygon_array& polygons = i_tree.array();
		distance_to_triangle tridist(i_position);
		for (unsigned int i=m_offset;i<m_offset+m_size;i++)
		{
			const unsigned int polyid = polygons[i];
			const unsigned int base = polyid*3;
			distance_record thisrecord;
			tridist(
				i_mesh.vertex(base+0),
				i_mesh.vertex(base+1),
				i_mesh.vertex(base+2),
				&thisrecord
				);
			if (thisrecord<*io_closest_record)
			{
				*io_closest_record = thisrecord;
				*o_face_id = polyid;
			}
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::bvh_branch::operator()(const bvh& i_tree, const mesh& i_mesh,	const point3d i_position[], distance_record o_closest_record[],	unsigned int o_face_id[]) const
{/*
	if (has_children())
	{
		distance_to_aabb boxdist(i_position[0]);

		const bvh_branch& child0 = i_tree.branch(m_children+0);
		const bvh_branch& child1 = i_tree.branch(m_children+1);

		const float sqdist0 = boxdist(child0.box());
		const float sqdist1 = boxdist(child1.box());
		if (sqdist0<sqdist1)
		{
			if (sqdist0<io_closest_record->distance_square())
				child0(i_tree,i_mesh,i_position,io_closest_record,o_face_id);
			if (sqdist1<io_closest_record->distance_square())
				child1(i_tree,i_mesh,i_position,io_closest_record,o_face_id);
		}
		else
		{
			if (sqdist1<io_closest_record->distance_square())
				child1(i_tree,i_mesh,i_position,io_closest_record,o_face_id);
			if (sqdist0<io_closest_record->distance_square())
				child0(i_tree,i_mesh,i_position,io_closest_record,o_face_id);
		}
	}
	else
	{
		const polygon_array& polygons = i_tree.array();
		distance_to_triangle tridist(i_position);
		for (unsigned int i=m_offset;i<m_offset+m_size;i++)
		{
			const unsigned int polyid = polygons[i];
			const unsigned int base = polyid*3;
			distance_record thisrecord;
			tridist(
				i_mesh.vertex(base+0),
				i_mesh.vertex(base+1),
				i_mesh.vertex(base+2),
				&thisrecord
				);
			if (thisrecord<*io_closest_record)
			{
				*io_closest_record = thisrecord;
				*o_face_id = polyid;
			}
		}
	}*/
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::bvh_branch::test_polygons(const bvh& i_tree, const mesh& i_mesh, const point3d& i_position, distance_record* io_closest_record, unsigned int* o_face_id) const
{
	const polygon_array& polygons = i_tree.array();
	distance_to_triangle tridist(i_position);
	for (unsigned int i=m_offset;i<m_offset+m_size;i++)
	{
		const unsigned int polyid = polygons[i];
		const unsigned int base = polyid*3;
		distance_record thisrecord;
		tridist(
			i_mesh.vertex(base+0),
			i_mesh.vertex(base+1),
			i_mesh.vertex(base+2),
			&thisrecord
			);
		if (thisrecord<*io_closest_record)
		{
			*io_closest_record = thisrecord;
			*o_face_id = polyid;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::bvh_branch::stackless_traversal(const bvh& i_tree, const mesh& i_mesh, const point3d& i_position, distance_record* io_closest_record, unsigned int* o_face_id) const
{
	distance_to_aabb boxdist(i_position);

	// We have to avoid dynamic allocation from the std::stack. Running time multiplied by 5 using std::stack :s
	typedef static_stack<child_index,maximum_depth> child_stack;
	typedef static_stack<float,maximum_depth> dist_stack;

	child_stack nodestolook;
	dist_stack boxdistsstack;

	nodestolook.push(0);
	boxdistsstack.push(boxdist(box()));

	while(!nodestolook.empty())
	{
		const child_index current = nodestolook.top();
		const bvh_branch& currentnode = i_tree.branch(current);
		nodestolook.pop();
		
		const float sqdist = boxdistsstack.top();
		boxdistsstack.pop();
		if (sqdist<io_closest_record->distance_square())
		{

			if (currentnode.has_children())
			{
				const child_index child0index = i_tree.branch(current).m_children+0;
				const child_index child1index = i_tree.branch(current).m_children+1;

				const bvh_branch& child0 = i_tree.branch(child0index);
				const bvh_branch& child1 = i_tree.branch(child1index);

				const float sqdist0 = boxdist(child0.box());
				const float sqdist1 = boxdist(child1.box());
				
				// Check whether we need to push them
				if (sqdist0<sqdist1)
				{
					// Order it this way because we want to pick up the child0 first, so we place it on top
					nodestolook.push(child1index,child0index);
					boxdistsstack.push(sqdist1,sqdist0);
				}
				else
				{
					// Order it this way because we want to pick up the child1 first, so we place it on top
					nodestolook.push(child0index,child1index);
					boxdistsstack.push(sqdist0,sqdist1);
				}
			}
			else
			{
				const polygon_array& polygons = i_tree.array();
				distance_to_triangle tridist(i_position);
				for (unsigned int i=currentnode.m_offset;i<currentnode.m_offset+currentnode.m_size;i++)
				{
					const unsigned int polyid = polygons[i];
					const unsigned int base = polyid*3;
					distance_record thisrecord;
					tridist(
						i_mesh.vertex(base+0),
						i_mesh.vertex(base+1),
						i_mesh.vertex(base+2),
						&thisrecord
						);
					if (thisrecord<*io_closest_record)
					{
						*io_closest_record = thisrecord;
						*o_face_id = polyid;
					}
				}
			}
		}
	};
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::bvh_branch::operator()(
				const bvh& i_tree, 
				const mesh& i_mesh,
				const point3d i_triangle[3],
				const aabb& i_box,
				const index_type i_ignore[],
				index_type* o_face) const
{
	if (has_children())
	{
		assert(i_box.intersects(m_box));
		const bvh_branch& child0 = i_tree.branch(m_children+0);
		const bvh_branch& child1 = i_tree.branch(m_children+1);

		if (child0.box().intersects(i_box))
		{
			if (child0(i_tree,i_mesh,i_triangle,i_box,i_ignore,o_face))
			{
				return true;
			}
		}
		if (child1.box().intersects(i_box))
		{
			if (child1(i_tree,i_mesh,i_triangle,i_box,i_ignore,o_face))
			{
				return true;
			}
		}
	}
	else
	{
		const polygon_array& polygons = i_tree.array();
		
		for (unsigned int i=m_offset;i<m_offset+m_size;i++)
		{
			const unsigned int polyid = polygons[i];
			const unsigned int base = polyid*3;
			const index_type& index_a = i_mesh.index_at(base+0);
			const index_type& index_b = i_mesh.index_at(base+1);
			const index_type& index_c = i_mesh.index_at(base+2);
			const point3d& a = i_mesh.vertex_at(index_a);
			const point3d& b = i_mesh.vertex_at(index_b);
			const point3d& c = i_mesh.vertex_at(index_c);
			if (i_box.intersects(a,b,c))
			{
				// CHeck ignore list
				if (index_a!=i_ignore[0] && index_a!=i_ignore[1] && index_a!=i_ignore[2] &&
					index_b!=i_ignore[0] && index_b!=i_ignore[1] && index_b!=i_ignore[2] &&
					index_c!=i_ignore[0] && index_c!=i_ignore[1] && index_c!=i_ignore[2])
				{
					if (triangle_triangle_overlap(i_triangle[0],i_triangle[1],i_triangle[2],a,b,c))
					{
						*o_face = polyid;
						return true;
					}
				}
			}
		}
	}
	return false;
}