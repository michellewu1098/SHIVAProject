#include <sdf/core/obj_file.hpp>
#include <fstream>
#include <assert.h>

//----------------------------------------------------------------------------------------------------------------------
void sdf::obj_file::enable_comments(bool i_enable)
{
	m_enable_comments=i_enable;
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::obj_file::read_vertex(std::stringstream& io_stream, float* o_vertex) const
{
	io_stream>>(o_vertex[0])>>(o_vertex[1])>>(o_vertex[2]);
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::obj_file::read_face(std::stringstream& io_stream, index_type o_indices[], unsigned char& i_nAttrib) const
{
	io_stream>>o_indices[i_nAttrib++];
	while(o_indices[i_nAttrib-1]!=(unsigned int)-1)
	{
		char sep(0);
		unsigned int dummy;
		io_stream.read(&sep,1);

		int counter(0);
		while(sep=='/' && counter++<3)
		{
			io_stream>>dummy;
			io_stream.read(&sep,1);
		};
		io_stream>>o_indices[i_nAttrib++];
	};
	i_nAttrib--;
	assert(i_nAttrib==3); // We only want triangles
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::obj_file::read(const std::string& i_filename, vertex_array* o_vb, index_array* o_ib, aabb* o_box) const
{
	std::ifstream file_handler(i_filename.c_str(),std::ios::in);
	if (!file_handler.is_open())
		return false;

	std::stringstream stream;
	stream << file_handler.rdbuf();
	const bool result = read(stream,o_vb,o_ib, o_box);

        file_handler.close();

	return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::obj_file::read(std::stringstream& i_sstream, vertex_array* o_vb, index_array* o_ib, aabb* o_box) const
{

	static const std::string vertex("v");
	static const std::string face("f");
	static const std::string comment ("#");

	vertex_array& vertices  = *o_vb;
	index_array& indices   = *o_ib;
	aabb box;

	while (!i_sstream.eof())
	{
		std::stringstream line;
		std::string sline;
		std::getline(i_sstream,sline);
		line<<sline;

		std::string identifier;
		line>>identifier;

		// Exclude vt and vn
		if (identifier.size()==1)
		{
			if (identifier==vertex)
			{
				point3d vertex;
				read_vertex(line,&vertex[0]);
				box.include(vertex);
				vertices.push_back(vertex);
			}
			else if (identifier==face)
			{
				index_type invalid = static_cast<index_type>(-1);
				index_type face[4] = { invalid,invalid,invalid,invalid};
				unsigned char nAttrib = 0;
				read_face(line,face,nAttrib);
				assert(nAttrib==3);
				for (unsigned char i=0;i<3;i++)
					indices.push_back(face[i]-1);
			}
		}
		else if (identifier[0]==comment[0] && m_enable_comments)
		{
			std::cout<<sline<<std::endl;
		}
	}

	if (o_box)
	{
		*o_box = box;
	}

	return true;
}


//----------------------------------------------------------------------------------------------------------------------
bool sdf::obj_file::write(const std::string& i_filename, const vertex_array& i_vb, const index_array& i_ib) const
{


	std::ofstream file_handler(i_filename.c_str(),std::ios::out);
	if (!file_handler.is_open())
		return false;

	
	for (vertex_array::size_type i=0;i<i_vb.size();i++)
	{
		file_handler<<"v "<<i_vb[i][0]<<" "<<i_vb[i][1]<<" "<<i_vb[i][2]<<std::endl;
	}
	for (index_array::size_type i=0;i<i_ib.size();i+=3)
	{
		file_handler<<"f "<<i_ib[i+0]+1<<" "<<i_ib[i+1]+1<<" "<<i_ib[i+2]+1<<std::endl;
	}

	file_handler.close();

	return true;
}