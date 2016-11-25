#include <sdf/sign/angle_weighted_average.hpp>
#include <sdf/core/binary_file.hpp>
#include <sdf/core/tools.hpp>
#include <map>
#include <assert.h>
#include <math.h>


//----------------------------------------------------------------------------------------------------------------------
sdf::angle_weighted_average::angle_weighted_average() : m_mesh(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::angle_weighted_average::is_initialized() const
{
	if (m_mesh==0)
		return false;
	return 
		m_face_normals.size()==m_mesh->num_faces()   &&
		m_edge_normals.size()==m_mesh->num_indices() &&
		m_vertex_normals.size()==m_mesh->num_vertices();
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::angle_weighted_average::initialize(const mesh& i_mesh)
{
	m_mesh = &i_mesh;
	if (m_mesh->known_source())
	{
		if (m_mesh->source().find_last_of(".")!=std::string::npos)
		{
			const std::string awnfilename = m_mesh->source().substr(0,m_mesh->source().find_last_of(".")) + ".awn";
			binary_file_in normfile(awnfilename);
			if (normfile.is_open())
			{
				// Read in
				normfile(&m_face_normals);
				normfile(&m_vertex_normals);
				normfile(&m_edge_normals);
				normfile.close();

				// There might be a mismatch if the mesh changed. 
				// We detect it and update the files
				if (!is_initialized())
				{
					// Initialize
					force_init(i_mesh);
					// Write out
					binary_file_out normfileout(awnfilename);
					normfileout(m_face_normals);
					normfileout(m_vertex_normals);
					normfileout(m_edge_normals);
					normfileout.close();
				}
			}
			else
			{
				// Initialize
				force_init(i_mesh);
				// Write out
				binary_file_out normfileout(awnfilename);
				normfileout(m_face_normals);
				normfileout(m_vertex_normals);
				normfileout(m_edge_normals);
				normfileout.close();
			}
		}
	}
	else
	{
		force_init(i_mesh);
	}
	assert(is_initialized());
}
//----------------------------------------------------------------------------------------------------------------------
void sdf::angle_weighted_average::force_init(const mesh& i_mesh)
{
	m_mesh = &i_mesh;

	// Initialize
	build_face_normals();
	build_vertex_normals();
	build_edge_normals();

	assert(is_initialized());
}

//----------------------------------------------------------------------------------------------------------------------
float sdf::angle_weighted_average::operator ()(const point3d& i_position, const distance_record& closest_record, unsigned int face_id) const
{
	assert(is_initialized());

	float sign(0.f);
	{	
		vector3d normal;
		switch (closest_record.feature())
		{
		case vertex_a : 
			{
				normal = m_vertex_normals[m_mesh->index_at(face_id*3+0)];
			}
			break;
		case vertex_b : 
			{
				normal = m_vertex_normals[m_mesh->index_at(face_id*3+1)];
			}
			break;
		case vertex_c : 
			{
				normal = m_vertex_normals[m_mesh->index_at(face_id*3+2)];
			}
			break;
		case edge_ab : 
			{
				normal = m_edge_normals[face_id*3+0];
			}
			break;
		case edge_bc : 
			{
				normal = m_edge_normals[face_id*3+1];
			}
			break;
		case edge_ca : 
			{
				normal = m_edge_normals[face_id*3+2];
			}
			break;
		case face : 
			{
				normal = m_face_normals[face_id];
			}
			break;
		default :
			assert(false); // This should never happen
			break;
		}


		const vector3d ptb = i_position-closest_record.closest_point();

		sign = 1.f;
		if (ptb.dot(normal)<=0)
		{
			sign=-1.f;
		}
	}
	// Because it is a multiplier, we should only return one or the other
	assert(sign==-1.f || sign==1.f); 
	return sign;
}



//----------------------------------------------------------------------------------------------------------------------
void sdf::angle_weighted_average::build_face_normals()
{
	m_face_normals.resize(m_mesh->num_faces());
	for (unsigned int i=0;i<m_mesh->num_faces();i++)
	{
		const index_type offset = i*3;
		const index_type& index_a = m_mesh->index_at(offset+0);
		const index_type& index_b = m_mesh->index_at(offset+1);
		const index_type& index_c = m_mesh->index_at(offset+2);
		const point3d& a = m_mesh->vertex_at(index_a);
		const point3d& b = m_mesh->vertex_at(index_b);
		const point3d& c = m_mesh->vertex_at(index_c);		
		const vector3d ab = b-a;
		const vector3d ac = c-a;
		vector3d normal;
		normal.cross(ab,ac);
		m_face_normals[i]=normal;
		// This assert makes sure the triangle is not degenerated
		// The length of a cross product is 
		// the area of the parallelogram craeted by the two vectors
		//assert(m_face_normals[i].length_squared()!=0);
		m_face_normals[i].normalize();
	}
}


//----------------------------------------------------------------------------------------------------------------------
void sdf::angle_weighted_average::build_vertex_normals()
{
	m_vertex_normals.resize(m_mesh->num_vertices());
	for (unsigned int i=0;i<m_mesh->num_faces();i++)
	{
		const index_type offset = i*3;
		const index_type& index_a = m_mesh->index_at(offset+0);
		const index_type& index_b = m_mesh->index_at(offset+1);
		const index_type& index_c = m_mesh->index_at(offset+2);
		const point3d& a = m_mesh->vertex_at(index_a);
		const point3d& b = m_mesh->vertex_at(index_b);
		const point3d& c = m_mesh->vertex_at(index_c);
		const vector3d& normal = m_face_normals[i];

		vector3d ab = b-a; 
		ab.normalize();
		vector3d bc = c-b;
		bc.normalize();
		vector3d ca = a-c;
		ca.normalize();
		const vector3d ba(-ab[0],-ab[1],-ab[2]);
		const vector3d cb(-bc[0],-bc[1],-bc[2]);
		const vector3d ac(-ca[0],-ca[1],-ca[2]);


		const float weight_a = acos(ac.dot(ab));
		const float weight_b = acos(ba.dot(bc));
		const float weight_c = acos(cb.dot(ca));

		m_vertex_normals[index_a]+=normal*weight_a;
		m_vertex_normals[index_b]+=normal*weight_b;
		m_vertex_normals[index_c]+=normal*weight_c;
	}
	// Normalization is probably unnecessary
	for (unsigned int i=0;i<m_mesh->num_vertices();i++)
		m_vertex_normals[i].normalize();	
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::angle_weighted_average::build_edge_normals()
{

	m_edge_normals.resize(m_mesh->num_indices());

	typedef std::pair<index_type,index_type> edge_key;
	typedef std::pair<unsigned int, index_type> face_edge;
	typedef std::map<edge_key,face_edge> edge_map;

	// Edge key references both vertex indices
	// Face edge reference the face index (to get the normal) and the index of the index (offset in edge normal array)
	edge_map edges;

	for (unsigned int i=0;i<m_mesh->num_faces();i++)
	{
		const index_type offset = i*3;

		for (unsigned int j=0;j<3;j++)
		{
			const index_type& index_a = m_mesh->index_at(offset+j);
			const index_type& index_b = m_mesh->index_at(offset+((j+1)%3));
			edge_key e1 = std::make_pair(index_a,index_b);
			edge_map::iterator e1adjacent = edges.find(e1);

			if (e1adjacent!=edges.end())
			{
				vector3d edge_normal = m_face_normals[i]+m_face_normals[e1adjacent->second.first];
				m_edge_normals[offset+j]=edge_normal;
				m_edge_normals[e1adjacent->second.second]=edge_normal;
				edges.erase(e1adjacent);
			}
			else
			{
				const vector3d edge_normal = m_face_normals[i];
				m_edge_normals[offset+j]=edge_normal;
				edge_key flippede1 = std::make_pair(index_b,index_a);
				if (edges.find(flippede1)!=edges.end())
				{
					// Flipped face!
					// Flipped faces are now allowed
					//assert(false);
				}
				edges[std::make_pair(index_b,index_a)]=std::make_pair(i,offset+j);
			}
		}
	}
	// This assert is for manifold things
	//assert(edges.size()==0);
}

std::size_t sdf::angle_weighted_average::memory_usage() const
{
	return dynamic_memory(m_face_normals)+dynamic_memory(m_vertex_normals)+dynamic_memory(m_edge_normals);
}