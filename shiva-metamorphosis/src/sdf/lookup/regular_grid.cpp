#include <sdf/lookup/regular_grid.hpp>
#include <sdf/distance/distance_to_aabb.hpp>
#include <sdf/core/tools.hpp>
#include <assert.h>
#include <iostream>
#include <math.h>

//#define SDF_TIGHTERBOXES

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::regular_grid::build(const mesh& i_mesh, const cell_weights& i_weights)
{	
	vector3d cellsize(0.f,0.f,0.f);
	vector3d primavg(0.f,0.f,0.f);
	vector3d maxsize(0.f,0.f,0.f);
	aabb meshbox;
	get_cell_heuristic(i_mesh,&meshbox,&primavg,&maxsize);
	cellsize = maxsize*i_weights.alpha()+primavg*i_weights.beta();


	vector3d meshextent;
	meshbox.extent(&meshextent);

	assert(meshextent[0]>=cellsize[0]);
	assert(meshextent[1]>=cellsize[1]);
	assert(meshextent[2]>=cellsize[2]);

	// Getting the number of cells (rounded)
	const unsigned int cellx = static_cast<unsigned int>(meshextent[0]/cellsize[0]);
	const unsigned int celly = static_cast<unsigned int>(meshextent[1]/cellsize[1]);
	const unsigned int cellz = static_cast<unsigned int>(meshextent[2]/cellsize[2]);
	// Computing the new cell size (exact cell size this time)
	cellsize[0]=meshextent[0]/static_cast<float>(cellx);
	cellsize[1]=meshextent[1]/static_cast<float>(celly);
	cellsize[2]=meshextent[2]/static_cast<float>(cellz);

	assert(cellx!=0);
	assert(celly!=0);
	assert(cellz!=0);

	m_box = meshbox;
	m_cellsize = cellsize;
	m_invcellsize.set(1.f/cellsize[0],1.f/cellsize[1],1.f/cellsize[2]);
	m_dim[0]=cellx;
	m_dim[1]=celly;
	m_dim[2]=cellz;

	cell_grid grid;
	grid.resize(cellx*celly*cellz);
	unsigned int polylistsize(0);

	// Second round: bucket those primitives!
	for (unsigned int i=0;i<i_mesh.num_faces();i++)
	{
		const unsigned int base = i*3;
		aabb tribox(
			i_mesh.vertex(base+0),
			i_mesh.vertex(base+1),
			i_mesh.vertex(base+2));

		vector3d primextent;
		tribox.extent(&primextent);
		
		index_3d minindex,maxindex;
		const bool minisvalid = get_cell_id(tribox.minimum(),&minindex);
		const bool maxisvalid = get_cell_id(tribox.maximum(),&maxindex);
		assert(minisvalid);
		assert(maxisvalid);
		unsigned int num_buckets(0);
		for (int ix=minindex.x();ix<=maxindex.x();ix++)
			for (int iy=minindex.y();iy<=maxindex.y();iy++)
				for (int iz=minindex.z();iz<=maxindex.z();iz++)
				{
					index_3d current(ix,iy,iz);
#ifdef SDF_TIGHTERBOXES
					aabb cellbox;
					cell_aabb(current,&cellbox);
					if (cellbox.intersects(i_mesh.vertex(base+0),i_mesh.vertex(base+1),i_mesh.vertex(base+2)))
#endif
					{
						grid[get_absolute_id(current)].add(i);
						++num_buckets;
					}
				}
		assert(num_buckets!=0);
		polylistsize+=num_buckets;
	}

	m_polylist.reserve(polylistsize);
	m_grid.resize(cellx*celly*cellz);
	for (unsigned int i=0;i<(unsigned int)m_grid.size();i++)
	{
		m_grid[i].set((unsigned int)m_polylist.size(),grid[i].size());
		m_polylist.insert(m_polylist.end(),grid[i].array().begin(),grid[i].array().end());
	}
}

#ifdef SDF_TIGHTERBOXES
#undef SDF_TIGHTERBOXES
#endif // SDF_TIGHTERBOXES

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::regular_grid::operator()(const mesh& i_mesh, const point3d& i_position, distance_record* o_closest_record, unsigned int* o_face_id) const
{
	full_check_lookup(i_mesh,i_position,o_closest_record,o_face_id);
	//propagation_lookup(i_mesh,i_position,o_closest_record,o_face_id);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::regular_grid::get_cell_heuristic(const mesh& i_mesh, aabb* o_mesh, vector3d* o_average, vector3d* o_biggest) const
{
	
	// First round, getting overall bounding box and primitive properties (average extent and max extent)
	for (unsigned int i=0;i<i_mesh.num_faces();i++)
	{
		const unsigned int base = i*3;
		aabb tribox(
			i_mesh.vertex(base+0),
			i_mesh.vertex(base+1),
			i_mesh.vertex(base+2));

		o_mesh->include(i_mesh.vertex(base+0));
		o_mesh->include(i_mesh.vertex(base+1));
		o_mesh->include(i_mesh.vertex(base+2));

		vector3d extent;
		tribox.extent(&extent);
		(*o_biggest)[0]=std::max((*o_biggest)[0],extent[0]);
		(*o_biggest)[1]=std::max((*o_biggest)[1],extent[1]);
		(*o_biggest)[2]=std::max((*o_biggest)[2],extent[2]);
		(*o_average)[0]+=extent[0];
		(*o_average)[1]+=extent[1];
		(*o_average)[2]+=extent[2];
	}
	(*o_average)/=static_cast<float>(i_mesh.num_faces());


	assert((*o_biggest)[0]!=0.f);
	assert((*o_biggest)[1]!=0.f);
	assert((*o_biggest)[2]!=0.f);
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::regular_grid::is_inside(const point3d& i_pos) const
{
	return m_box.is_inside(i_pos);
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::regular_grid::get_cell_id(const point3d& i_pos, index_3d* o_cell_id) const
{
	if (is_inside(i_pos))
	{
		const point3d relativepos = i_pos-m_box.minimum();
		const point3d idpos(relativepos[0]*m_invcellsize[0],relativepos[1]*m_invcellsize[1],relativepos[2]*m_invcellsize[2]);
		o_cell_id->set(min(static_cast<int>(idpos[0]),m_dim[0]-1),
						min(static_cast<int>(idpos[1]),m_dim[1]-1),
						min(static_cast<int>(idpos[2]),m_dim[2]-1));
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int sdf::detail::regular_grid::get_absolute_id(const index_3d& i_index3d) const
{
	assert(i_index3d.x()<m_dim[0]);
	assert(i_index3d.y()<m_dim[1]);
	assert(i_index3d.z()<m_dim[2]);
	return i_index3d.x() + i_index3d.y()*m_dim[0] + i_index3d.z()*(m_dim[0]*m_dim[1]);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int sdf::detail::regular_grid::num_cells() const
{
	return static_cast<unsigned int>(m_grid.size());
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::detail::grid_offset_cell& sdf::detail::regular_grid::operator[](unsigned int i) const
{
	assert(i<num_cells());
	return m_grid[i];
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::vector3d& sdf::detail::regular_grid::cell_size() const
{
        return m_cellsize;
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::vector3d& sdf::detail::regular_grid::inv_cell_size() const
{
        return m_invcellsize;
}

//----------------------------------------------------------------------------------------------------------------------
sdf::point3d sdf::detail::regular_grid::cell_centre(unsigned int i) const
{
	const unsigned int z = i/(m_dim[0]*m_dim[1]);
	i-=z*(m_dim[0]*m_dim[1]);
	const unsigned int y = i/m_dim[0];
	i-=y*m_dim[0];
	const unsigned int x = i;

	const point3d boxorig(
		m_cellsize[0]*static_cast<float>(x),
		m_cellsize[1]*static_cast<float>(y),
		m_cellsize[2]*static_cast<float>(z));
	return m_box.minimum()+boxorig+m_cellsize*0.5f;
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::regular_grid::cell_aabb(const index_3d& id, aabb* o_box) const
{
	const point3d boxorig(
		m_cellsize[0]*static_cast<float>(id.x()),
		m_cellsize[1]*static_cast<float>(id.y()),
		m_cellsize[2]*static_cast<float>(id.z()));
	o_box->set(m_box.minimum()+boxorig,m_box.minimum()+boxorig+m_cellsize);
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::aabb& sdf::detail::regular_grid::grid_box() const
{
	return m_box;
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::regular_grid::full_check_lookup(const mesh& i_mesh, const point3d& i_position, distance_record* o_closest_record, unsigned int* o_face_id) const
{
	distance_to_aabb boxdist(i_position);
	distance_to_triangle tridist(i_position);

	index_3d initialindex;
	get_cell_id(i_position,&initialindex);
	explore_node(i_mesh,tridist,get_absolute_id(initialindex),o_closest_record,o_face_id);

	for (int z=0;z<=m_dim[2]-1;z++)
	{
		for (int y=0;y<=m_dim[1]-1;y++)
		{
			for (int x=0;x<=m_dim[0]-1;x++)
			{
				const index_3d current(x,y,z);
				const unsigned int absid = get_absolute_id(current);
				if(!m_grid[absid].is_empty())
				{
					aabb curbox;
					cell_aabb(current,&curbox);
					if (boxdist(curbox)<o_closest_record->distance_square())
						explore_node(i_mesh,tridist,absid,o_closest_record,o_face_id);
				}
			}
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------
void sdf::detail::regular_grid::propagation_lookup(const mesh& i_mesh, const point3d& i_position, distance_record* o_closest_record, unsigned int* o_face_id) const
{
	///////////////////////////////////////////
	// The idea of the propagation lookup is 
	// to lookup the node it is in, and then grow from there
	// The problem seems to arise when we have a grow position different
	// from the actual sample position
	// One solution is then to to a force full check if the the input
	// is out of the main bounding box, else use the growth method
	///////////////////////////////////////////
	distance_to_aabb boxdist(i_position);
	distance_to_triangle tridist(i_position);

	if (!is_inside(i_position))
	{
		full_check_lookup(i_mesh,i_position,o_closest_record,o_face_id);
		return;
	}

	// Transform if to assert, now start pos MUST be inside
	assert(is_inside(i_position));
	index_3d current;
	get_cell_id(i_position,&current);
	explore_node(i_mesh,tridist,get_absolute_id(current),o_closest_record,o_face_id);
	
	int maxlevel = max(max(
			m_dim[0]-1-current.x(),
			m_dim[1]-1-current.y(),
			m_dim[2]-1-current.z()
			),max(
			current.x(),
			current.y(),
			current.z()			
			));

	for (int level=1;level<=maxlevel;level++)
	{
		for (int z=max(0,current.z()-level);z<=min(m_dim[2]-1,current.z()+level);z++)
		{
			for (int y=max(0,current.y()-level);y<=min(m_dim[1]-1,current.y()+level);y++)
			{
				for (int x=max(0,current.x()-level);x<=min(m_dim[0]-1,current.x()+level);x++)
				{
					if (z==max(0,current.z()-level) ||
						z<=min(m_dim[2]-1,current.z()+level) ||
						y==max(0,current.y()-level) ||
						y<=min(m_dim[1]-1,current.y()+level) ||
						x==max(0,current.x()-level) ||
						x<=min(m_dim[0]-1,current.x()+level))
					{
						const index_3d cellid(x,y,z);
						aabb curbox;
						cell_aabb(current,&curbox);
						if (boxdist(curbox)<o_closest_record->distance_square())
							if (explore_node(i_mesh,tridist,get_absolute_id(cellid),o_closest_record,o_face_id))
							{
								// Reduce maxlevel here
								const float tridistsq = o_closest_record->distance_square();
								const float mincellsize = min(m_cellsize[0],m_cellsize[1],m_cellsize[2]);
								const float mincellsizesq = mincellsize*mincellsize;
								const float fnumcells = tridistsq/mincellsizesq;
								maxlevel = static_cast<int>(fnumcells);
							}
					}
				}
			}
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------
bool sdf::detail::regular_grid::explore_node(const mesh& i_mesh, const distance_to_triangle& i_tridist, unsigned int i_index, distance_record* o_record, unsigned int* o_face_id) const
{

	bool found=false;
	const grid_offset_cell& cell = m_grid[i_index];
	const unsigned int num_polys = cell.size();
	for (unsigned int i=0;i<num_polys;i++)
	{
		const unsigned int face = m_polylist[cell[i]];
		const unsigned int base = face*3;

		distance_record current;
		i_tridist(i_mesh.vertex(base+0),i_mesh.vertex(base+1),i_mesh.vertex(base+2),&current);
		
		if (current<*o_record)
		{
			*o_record = current;
			*o_face_id = face;
			found=true;
		}
	}

	return found;
}
