#include <sdf/lookup/octree.hpp>
#include <sdf/lookup/octnode.hpp>
#include <assert.h>


//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::octree::build_children(
	const mesh& i_mesh,
	const settings& i_settings,
	const polygon_array i_buckets[],
	const aabb i_boxes[],
	unsigned int i_depth,
	child_index* o_childindex)
{
	const child_index firstindex = static_cast<child_index>(m_nodes.size());
	*o_childindex=firstindex;

	// This resize might remove some memory so that the pointer o_childindex might become invalid, so it needs to set it before hand
	m_nodes.resize(m_nodes.size()+8);

	m_nodes[firstindex+0].build(this,i_mesh,i_boxes[0],i_buckets[0],i_depth,i_settings);
	m_nodes[firstindex+1].build(this,i_mesh,i_boxes[1],i_buckets[1],i_depth,i_settings);
	m_nodes[firstindex+2].build(this,i_mesh,i_boxes[2],i_buckets[2],i_depth,i_settings);
	m_nodes[firstindex+3].build(this,i_mesh,i_boxes[3],i_buckets[3],i_depth,i_settings);
	m_nodes[firstindex+4].build(this,i_mesh,i_boxes[4],i_buckets[4],i_depth,i_settings);
	m_nodes[firstindex+5].build(this,i_mesh,i_boxes[5],i_buckets[5],i_depth,i_settings);
	m_nodes[firstindex+6].build(this,i_mesh,i_boxes[6],i_buckets[6],i_depth,i_settings);
	m_nodes[firstindex+7].build(this,i_mesh,i_boxes[7],i_buckets[7],i_depth,i_settings);
}


//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::octree::append_polygons(
				const polygon_array& i_polygons,
				index_type* o_offset, index_type* o_num_elements)
{
	*o_offset = static_cast<index_type>(m_polylist.size());
	*o_num_elements = static_cast<index_type>(i_polygons.size());

	m_polylist.insert(m_polylist.end(),i_polygons.begin(),i_polygons.end());
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::detail::octnode& sdf::detail::octree::node(unsigned int i) const
{
	assert(i<(unsigned int)m_nodes.size());
	return m_nodes[i];
}

//----------------------------------------------------------------------------------------------------------------------
sdf::detail::octnode& sdf::detail::octree::node(unsigned int i)
{
	assert(i<(unsigned int)m_nodes.size());
	return m_nodes[i];
}


//----------------------------------------------------------------------------------------------------------------------
const sdf::detail::octnode& sdf::detail::octree::root() const
{
	assert(m_nodes.size()>0);
	return m_nodes[0];
}

//----------------------------------------------------------------------------------------------------------------------
sdf::detail::octnode& sdf::detail::octree::root()
{
	assert(m_nodes.size()>0);
	return m_nodes[0];
}


//----------------------------------------------------------------------------------------------------------------------
const sdf::detail::octree::polygon_array& sdf::detail::octree::array() const
{
	return m_polylist;
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::octree::build(const mesh& i_mesh, const settings& i_settings)
{
	assert(!i_mesh.empty());
	m_nodes.push_back(octnode()); // This is the root

	octnode::polygon_array all_polygons(i_mesh.num_faces());
	for (unsigned int i=0;i<i_mesh.num_faces();i++)
		all_polygons[i]=i;

	aabb mainbox;
	i_mesh.bounding_box(&mainbox);

	root().build(this,i_mesh,mainbox,all_polygons,0,i_settings);

	assert(tree_contains(all_polygons));
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::octree::operator()(const mesh& i_mesh, const point3d& i_position, distance_record* o_closest_record, unsigned int* o_face_id) const
{
	assert(ready_to_query());

	root()(this,i_mesh,i_position,o_closest_record,o_face_id);
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::octree::ready_to_query() const
{
	return (m_nodes.size()>0);
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::octree::tree_contains(const octnode::polygon_array& i_polygons)const
{
	std::vector<bool> included(i_polygons.size(),false);
	typedef octnode::polygon_array::size_type size_type;
	for (node_pool::size_type i=0;i<m_nodes.size();i++)
	{
		for (size_type j=m_nodes[i].polygon_start();j<m_nodes[i].polygon_end();j++)
		{
			included[m_polylist[j]]=true;
		}
	}

	for (node_pool::size_type i=0;i<m_nodes.size();i++)
		if (!included[i])
			return false;
	return true;
}