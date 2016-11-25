#ifndef SDF_CORE_BINARY_FILE_INCLUDED
#define SDF_CORE_BINARY_FILE_INCLUDED

#include <sdf/core/config.hpp>
#include <fstream>
#include <ostream>
#include <istream>
#include <vector>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class binary_file_out "include/sdf/core/binary_file.hpp"
	/// @brief Binary file out - writing to a binary file
	///			It can only write to the file, and handles some basic types so that it is easier to write files
	///			Remember to close the file. The Destructor should take care of it, but doing it manually is safer
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
    class binary_file_out
    {
    public:
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructor
		/// @param[in] The filename (path).
		//----------------------------------------------------------------------------------------------------------------------
        binary_file_out(const std::string& i_filename);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Test whether the stream is opened. If it is not open, maybe the file cant be created. 
		/// @warning If the file is not opened, nothing will be writen
		/// @return True if the file is opened, false otherwise
		//----------------------------------------------------------------------------------------------------------------------
        bool is_open() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Close the stream. Once you are done writing, you should close the file to save
		//----------------------------------------------------------------------------------------------------------------------
		void close();

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Write to the file a single variable
		/// @param[in] i_element A variable to save
		/// @warning The element needs to be serializable - that means the data is copied. A structure with a pointer would not be serializable
		//----------------------------------------------------------------------------------------------------------------------
        template<typename T>
        void operator()(const T& i_element);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Write to the file an array of variables
		/// @param[in] i_array The array
		/// @param[in] i_num_elements The number of elements in the array
		/// @warning If the array can have a variable size, you should first write the size of the array. 
		//----------------------------------------------------------------------------------------------------------------------
        template<typename T, typename SizeElement>
        void operator()(const T* const i_array, const SizeElement& i_num_elements);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Write a std::vector to a file
		/// @param[in] i_vector The vector/array to write - it will write the size (as a 4 bytes unsigned integer) and the raw data
		//----------------------------------------------------------------------------------------------------------------------
        template<typename T>
        void operator()(const std::vector<T>& i_vector);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Dtor - will close the stream
		//----------------------------------------------------------------------------------------------------------------------
        ~binary_file_out();
    private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief A typedef for the byte streams
		//----------------------------------------------------------------------------------------------------------------------
        typedef std::ofstream::char_type bytestream;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The file stream (out)
		//----------------------------------------------------------------------------------------------------------------------
        std::ofstream m_stream;
    };

	//----------------------------------------------------------------------------------------------------------------------
	/// @class binary_file_in "include/sdf/core/binary_file.hpp"
	/// @brief Binary file in - reading to a binary file
	///			It can only read the file, and handles some basic types so that it is easier to read bin files
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
    class binary_file_in
    {
    public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructor
		/// @param[in] The filename (path).
		//----------------------------------------------------------------------------------------------------------------------
        binary_file_in(const std::string& i_filename);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Test whether the stream is opened. If it is not open, maybe the file cant be created. 
		/// @warning If the file is not opened, nothing will be read. Many reasons could prevent a file from being read: wrong path, read rights etc...
		/// @return True if the file is opened, false otherwise
		//----------------------------------------------------------------------------------------------------------------------
        bool is_open() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Close the stream. 
		//----------------------------------------------------------------------------------------------------------------------
		void close();

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Read a single variable
		/// @param[out] o_element A variable to load
		//----------------------------------------------------------------------------------------------------------------------
        template<typename T>
        void operator()(T* o_element);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Read an array of variables
		/// @param[out] o_array The array to read
		/// @param[in] i_num_elements The number of elements in the array
		//----------------------------------------------------------------------------------------------------------------------
        template<typename T, typename SizeElement>
        void operator()(T* const o_array, const SizeElement& i_num_elements);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Read a std::vector 
		/// @param[in] o_vector The vector/array to read
		//----------------------------------------------------------------------------------------------------------------------
        template<typename T>
        void operator()(std::vector<T>* o_vector);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Dtor - will close the stream
		//----------------------------------------------------------------------------------------------------------------------
        ~binary_file_in();
    private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief A typedef for the byte streams
		//----------------------------------------------------------------------------------------------------------------------
        typedef std::ifstream::char_type bytestream;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The file stream (in)
		//----------------------------------------------------------------------------------------------------------------------
        std::ifstream m_stream;
    };
}

#include <sdf/core/binary_file.inl>

#endif // SDF_CORE_BINARY_FILE_INCLUDED
