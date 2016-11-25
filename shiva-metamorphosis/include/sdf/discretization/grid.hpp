#ifndef SDF_DISCRETIZATION_GRID_INCLUDED
#define SDF_DISCRETIZATION_GRID_INCLUDED

#include <vector>
#include <sdf/core/binary_file.hpp>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class grid "include/sdf/discretization/grid.hpp"
	/// @brief A grid (3D array) for the function samples
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	class grid
	{
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief A scalar field type
		//----------------------------------------------------------------------------------------------------------------------
		typedef std::vector<float> scalar_field;
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Default constructor - unserialize has to be called to fill the data
		//----------------------------------------------------------------------------------------------------------------------
		grid();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Build an empty scalar field
		/// @param[in] i_dimx Dimension in x
		/// @param[in] i_dimy Dimension in y
		/// @param[in] i_dimz Dimension in z
		/// @param[in] i_default Default value to fill the scalar field with
		//----------------------------------------------------------------------------------------------------------------------
		grid(
			unsigned int i_dimx,
			unsigned int i_dimy,
			unsigned int i_dimz,
			float i_default = 0.f);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get a value
		/// @param[in] i_index Absolute index in the array
		/// @return Constant reference to the scalar
		//----------------------------------------------------------------------------------------------------------------------
		const float& operator[](unsigned int i_index) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get a value
		/// @param[in] i_x Index in dimension X
		/// @param[in] i_y Index in dimension Y
		/// @param[in] i_z Index in dimension Z
		/// @return Constant reference to the scalar
		//----------------------------------------------------------------------------------------------------------------------
		const float& operator()(unsigned int i_x, unsigned int i_y, unsigned int i_z) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get a value
		/// @param[in] i_x Index in dimension X
		/// @param[in] i_y Index in dimension Y
		/// @param[in] i_z Index in dimension Z
		/// @return Reference to the scalar
		//----------------------------------------------------------------------------------------------------------------------
		float& operator()(unsigned int i_x, unsigned int i_y, unsigned int i_z);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get raw data
		/// @warning No assumption should be made about the layout
		/// @return A pointer to the data (constant)
		//----------------------------------------------------------------------------------------------------------------------
		const float* raw_data() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the number of elements (total)
		/// @return Number of elements
		//----------------------------------------------------------------------------------------------------------------------
		unsigned int num_elements() const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Resize the grid
		/// @warning Use this at your own risk - it seems very dangerous to use from outside
		//----------------------------------------------------------------------------------------------------------------------
		void resize(unsigned int );

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the width (number of elements in X)
		/// @return Width of the field
		//----------------------------------------------------------------------------------------------------------------------
		unsigned int width() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the height (number of elements in Y)
		/// @return Height of the field
		//----------------------------------------------------------------------------------------------------------------------
		unsigned int height() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the depth (number of elements in X)
		/// @return Depth of the field
		//----------------------------------------------------------------------------------------------------------------------
		unsigned int depth() const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Difference between this grid and another one.
		/// @param[in] i_other The other grid
		/// @return The sum of the absolute difference of each element
		//----------------------------------------------------------------------------------------------------------------------
		float difference(const grid& i_other) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Equality operator
		/// @param[in] i_other The other grid
		/// @return True if difference is null, false otherwise
		//----------------------------------------------------------------------------------------------------------------------
		bool operator==(const grid& i_other) const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the scalar field as a 1D array
		/// @return Constant reference to the scalar field
		//----------------------------------------------------------------------------------------------------------------------
		const scalar_field& data() const { return m_data; }
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the scalar field as a 1D array
		/// @return Reference to the scalar field
		//----------------------------------------------------------------------------------------------------------------------
		scalar_field& data() { return m_data; }

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Serialize the grid to a binary file
		/// @param[out] o_file The file stream to send the data to
		//----------------------------------------------------------------------------------------------------------------------
		void serialize(binary_file_out& o_file) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Unserialize the grid from a binary file
		/// @param[out] o_file The file stream to retrieve the data from
		//----------------------------------------------------------------------------------------------------------------------
		void unserialize(binary_file_in& o_file);
	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the absolute index given a 3D index
		/// @param[in] i_x Index in dimension X
		/// @param[in] i_y Index in dimension Y
		/// @param[in] i_z Index in dimension Z
		/// @return The absolute index 
		//----------------------------------------------------------------------------------------------------------------------
		unsigned int raw_index(unsigned int i_x, unsigned int i_y, unsigned int i_z) const;

	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Number of elements in X
		//----------------------------------------------------------------------------------------------------------------------
		unsigned int m_dimension_x;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Number of elements in Y
		//----------------------------------------------------------------------------------------------------------------------
		unsigned int m_dimension_y;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Number of elements in Z
		//----------------------------------------------------------------------------------------------------------------------
		unsigned int m_dimension_z;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The data as a 1D array
		//----------------------------------------------------------------------------------------------------------------------
		scalar_field m_data;
	};
}

#endif // SDF_DISCRETIZATION_GRID_INCLUDED
