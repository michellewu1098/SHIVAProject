#include <sdf/lookup/bvh.hpp>
#include <sdf/core/binary_file.hpp>
#include <sdf/core/tools.hpp>
#include <assert.h>

//----------------------------------------------------------------------------------------------------------------------
const sdf::detail::bvh_branch& sdf::detail::bvh::root() const
{
	assert(!m_branches.empty());
	return m_branches[0];
}

//----------------------------------------------------------------------------------------------------------------------
sdf::detail::bvh_branch& sdf::detail::bvh::root()
{
	assert(!m_branches.empty());
	return m_branches[0];
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::detail::bvh::polygon_array& sdf::detail::bvh::array() const
{
	return m_polylist;
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::detail::bvh_branch& sdf::detail::bvh::branch(unsigned int i) const
{
	assert(i<static_cast<unsigned int>(m_branches.size()));
	return m_branches[i];
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::bvh::build(const mesh& i_mesh, const settings& i_settings)
{
	if (i_mesh.known_source())
	{
		if (i_mesh.source().find_last_of(".")!=std::string::npos)
		{
			const std::string bvhfilename = i_mesh.source().substr(0,i_mesh.source().find_last_of(".")) + ".bvh";
			binary_file_in bvhfilein(bvhfilename);
			if (bvhfilein.is_open())
			{
				// Read in
				bvhfilein(&m_polylist);
				bvhfilein(&m_branches);
				bvhfilein.close();
			}
			else
			{
				// Initialize
				force_build(i_mesh,i_settings);
				// Write out
				binary_file_out bvhfileout(bvhfilename);
				bvhfileout(m_polylist);
				bvhfileout(m_branches);
				bvhfileout.close();
			}
		}
	}
	else
	{
		force_build(i_mesh,i_settings);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::bvh::force_build(const mesh& i_mesh, const settings& i_settings)
{
	assert(i_settings.max_depth()<bvh_branch::maximum_depth);
	assert(!i_mesh.empty());
	// Create the root
	m_branches.push_back(bvh_branch());

	polygon_array all_polygons(i_mesh.num_faces());
	for (unsigned int i=0;i<i_mesh.num_faces();i++)
		all_polygons[i]=i;

	root().build(*this,i_mesh,i_settings,all_polygons);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::bvh::append_polygons(
				const polygon_array& i_polygons,
				index_type* o_offset, index_type* o_num_elements)
{
	*o_offset = static_cast<index_type>(m_polylist.size());
	*o_num_elements = static_cast<index_type>(i_polygons.size());

	m_polylist.insert(m_polylist.end(),i_polygons.begin(),i_polygons.end());
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::bvh::build_children(
	const mesh& i_mesh,
	const settings& i_settings,
	const polygon_array i_buckets[],
	unsigned int i_depth,
	bvh_branch::child_index* o_childindex)
{
	const bvh_branch::child_index firstindex = static_cast<bvh_branch::child_index>(m_branches.size());
	*o_childindex=firstindex;

	m_branches.resize(m_branches.size()+2);

	m_branches[firstindex+0].build(*this,i_mesh,i_settings,i_buckets[0],i_depth);
	m_branches[firstindex+1].build(*this,i_mesh,i_settings,i_buckets[1],i_depth);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::bvh::operator()(
				const mesh& i_mesh,
				const point3d& i_position,
				distance_record* o_closest_record,
				unsigned int* o_face_id) const
{
	root()(*this,i_mesh,i_position,o_closest_record,o_face_id);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::bvh::operator()(
				const mesh& i_mesh,
				const point3d i_position[],
				distance_record o_closest_record[],
				unsigned int o_face_id[]) const
{
	root()(*this,i_mesh,i_position,o_closest_record,o_face_id);
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::bvh::operator()(
				const mesh& i_mesh,
				const point3d i_triangle[3],
				const index_type i_ignore[],
				index_type* o_face) const
{
	const aabb tribox(i_triangle[0],i_triangle[1],i_triangle[2]);
	return root()(*this,i_mesh,i_triangle,tribox,i_ignore,o_face);
}

//----------------------------------------------------------------------------------------------------------------------
std::size_t sdf::detail::bvh::memory_usage() const
{
	return dynamic_memory(m_polylist)+dynamic_memory(m_branches);
}