#include <sdf/core/mesh.hpp>
#include <sdf/core/obj_file.hpp>
#include <sdf/core/tools.hpp>
#include <assert.h>
#include <iostream>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
sdf::mesh::mesh() : m_source() {}

//----------------------------------------------------------------------------------------------------------------------
sdf::mesh::mesh(const std::string& i_filename) : m_source(i_filename) 
{
	obj_file file;
	file.enable_comments(false);
	file.read(i_filename,&m_vertices,&m_indices);
}

//----------------------------------------------------------------------------------------------------------------------
sdf::mesh::mesh(const vertex_array& i_vertices, const index_array& i_indices) : m_source(), m_vertices(i_vertices), m_indices(i_indices)
{
}

//----------------------------------------------------------------------------------------------------------------------
sdf::mesh::mesh(const vertex_type* i_vertex_start, const vertex_type* i_vertex_end,
				const index_type* i_index_start, const index_type* i_index_end) : m_source(), m_vertices(i_vertex_start,i_vertex_end), m_indices(i_index_start, i_index_end) 
{
}

//----------------------------------------------------------------------------------------------------------------------
sdf::mesh::~mesh() {}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::mesh::load_from_obj(const std::string& i_filename, aabb* o_box)
{
	m_indices.resize(0);
	m_vertices.resize(0);

	obj_file file;
	file.enable_comments(false);
	if (file.read(i_filename,&m_vertices,&m_indices,o_box))
	{
		m_source = i_filename;
		return true;
	}
	else
	{
		// CLeanup
		m_indices.resize(0);
		m_vertices.resize(0);
		m_source="";
		return false;
	}
	return false; // How ?
}

bool sdf::mesh::export_to_file(const std::string& i_filename) const
{
	obj_file file;
	return file.write(i_filename, m_vertices,m_indices);
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int sdf::mesh::num_indices() const
{
	return (unsigned int)m_indices.size();
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int sdf::mesh::num_faces() const
{
	assert(num_indices()%3==0);
	return num_indices()/3;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int sdf::mesh::num_vertices() const
{
	return (unsigned int)m_vertices.size();
}

//----------------------------------------------------------------------------------------------------------------------
const std::string& sdf::mesh::source() const
{
	return m_source;
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::mesh::known_source() const
{
	return !m_source.empty();
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::vertex_array& sdf::mesh::vertices() const
{
	return m_vertices;
}

//----------------------------------------------------------------------------------------------------------------------
sdf::vertex_array& sdf::mesh::vertices()
{
	return m_vertices;
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::index_array& sdf::mesh::indices() const
{
	return m_indices;
}

//----------------------------------------------------------------------------------------------------------------------
sdf::index_array& sdf::mesh::indices()
{
	return m_indices;
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::vertex_type& sdf::mesh::vertex_at(unsigned int i_index) const
{
	assert(i_index<num_vertices());
	return m_vertices[i_index];
}

//----------------------------------------------------------------------------------------------------------------------
const sdf::vertex_type& sdf::mesh::vertex(unsigned int i_index) const
{
	assert(i_index<num_indices());
	const unsigned int vindex = m_indices[i_index];
	assert(vindex<num_vertices());
	return m_vertices[vindex];
}

//----------------------------------------------------------------------------------------------------------------------
const  sdf::index_type&  sdf::mesh::index_at(unsigned int i_index) const
{
	assert(i_index<num_indices());
	return m_indices[i_index];
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::mesh::get_face_vertices(unsigned int i_face_id, point3d* o_first, point3d* o_second, point3d* o_third) const
{
	assert( i_face_id*3+2 < num_indices());
	
	const index_type& index_a = index_at(i_face_id*3+0);
	const index_type& index_b = index_at(i_face_id*3+1);
	const index_type& index_c = index_at(i_face_id*3+2);

	*o_first  = m_vertices[index_a];
	*o_second = m_vertices[index_b];
	*o_third  = m_vertices[index_c];
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::mesh::get_face_indices (unsigned int i_face_id, index_type* o_first, index_type* o_second, index_type* o_third) const
{
	assert( i_face_id*3+2 < num_indices());
	
	*o_first  = index_at(i_face_id*3+0);
	*o_second = index_at(i_face_id*3+1);
	*o_third  = index_at(i_face_id*3+2);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::mesh::bounding_box(aabb* o_box) const
{
	if (m_vertices.size()>0)
	{
		o_box->include(m_vertices[0]);
		for (unsigned int i=1;i<(unsigned int)m_vertices.size();i++)
		{
			o_box->include(m_vertices[i]);
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------
bool sdf::mesh::empty() const
{
	return m_indices.size()<2; // At least one face
}

//----------------------------------------------------------------------------------------------------------------------
std::size_t sdf::mesh::memory_usage() const
{
	return dynamic_memory(m_vertices)+dynamic_memory(m_indices);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::mesh::print_details() const
{
	std::cout<<"vertices ("<<num_vertices()<<") indices ("<<num_indices()<<") faces ("<<num_faces()<<")"<<std::endl;
}


//----------------------------------------------------------------------------------------------------------------------
sdf::index_type sdf::mesh::append(const vertex_type& i_vertex)
{
	index_type ret = static_cast<unsigned int>(m_vertices.size());
	m_vertices.push_back(i_vertex);
	return ret;
}

//----------------------------------------------------------------------------------------------------------------------
sdf::index_type sdf::mesh::append(index_type i_indexA, index_type i_indexB, index_type i_indexC)
{
	index_type ret = num_faces();

	m_indices.push_back(i_indexA);
	m_indices.push_back(i_indexB);
	m_indices.push_back(i_indexC);

	return ret;
}