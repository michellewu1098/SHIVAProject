#include <assert.h>


//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void sdf::binary_file_out::operator()(const T& i_element)
{
    assert(is_open());
    m_stream.write((char*)&i_element,sizeof(T));
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T, typename SizeElement>
void sdf::binary_file_out::operator()(const T* const i_array, const SizeElement& i_num_elements)
{
    assert(i_num_elements>0);
    assert(is_open());
	m_stream.write((const bytestream*)i_array,static_cast<std::streamsize>(sizeof(T)*i_num_elements));
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void sdf::binary_file_out::operator()(const std::vector<T>& i_vector)
{
    assert(is_open());
    if (i_vector.size()==0)
    {
        (*this)((unsigned int)0);
    }
    else
    {
        (*this)((unsigned int)i_vector.size());
        (*this)(&i_vector[0],i_vector.size());
    }
}


//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void sdf::binary_file_in::operator()(T* i_element)
{
    assert(is_open());
    m_stream.read((bytestream*)i_element,sizeof(T));
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T, typename SizeElement>
void sdf::binary_file_in::operator()(T* const i_array, const SizeElement& i_num_elements)
{
    assert(is_open());
    m_stream.read((bytestream*)i_array,static_cast<std::streamsize>(sizeof(T)*i_num_elements));
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void sdf::binary_file_in::operator()(std::vector<T>* o_vector)
{
    assert(is_open());
    unsigned int size(0);
    (*this)(&size);
    if (size!=0)
    {
        std::vector<T>& out = *o_vector;
        out.resize(size);
        (*this)(&out[0],size);
    }
}
