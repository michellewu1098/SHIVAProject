#ifndef SDF_DISCRETIZED_FIELD_INCLUDED
#define SDF_DISCRETIZED_FIELD_INCLUDED

#include <sdf/discretization/grid.hpp>
#include <sdf/core/types.hpp>
#include <sdf/core/thread_work.hpp>
#include <sdf/core/binary_file.hpp>
#include <sdf/distance/distance_record.hpp>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class discretized_field "include/sdf/discretization/discretized_field.hpp"
	/// @brief A dicretized field is sampling the function on the grid points
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	class discretized_field
	{
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Default constructor - you really need to set it up before filling it
		//----------------------------------------------------------------------------------------------------------------------
		discretized_field();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Build the discretized field object
		/// @param[in] i_min Minimum field point
		/// @param[in] i_max Maximum field point
		/// @param[in] i_nx Number of samples in X direction
		/// @param[in] i_ny Number of samples in Y direction
		/// @param[in] i_nz Number of samples in Z direction
		//----------------------------------------------------------------------------------------------------------------------
		discretized_field(
			const point3d& i_min,
			const point3d& i_max,
			unsigned int i_nx,
			unsigned int i_ny,
			unsigned int i_nz);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the cell size
		/// @param[out] o_size Size of a field cell (distance between two samples in each direction)
		//----------------------------------------------------------------------------------------------------------------------
		void cell_size(vector3d* o_size) const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the number of cells in the field
		/// @return Number of cells 
		//----------------------------------------------------------------------------------------------------------------------
		unsigned int num_cells() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the number of cells in X direction
		/// @return Number of cells in X direction
		//----------------------------------------------------------------------------------------------------------------------
		unsigned int num_cell_x() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the number of cells in Y direction
		/// @return Number of cells in Y direction
		//----------------------------------------------------------------------------------------------------------------------
		unsigned int num_cell_y() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the number of cells in Z direction
		/// @return Number of cells in Z direction
		//----------------------------------------------------------------------------------------------------------------------
		unsigned int num_cell_z() const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Absolute difference between two fields (sum of the absolute differences)
		/// @param[in] i_other The other field 
		/// @return The sum of the absolute differences per cell
		//----------------------------------------------------------------------------------------------------------------------
		float difference(const discretized_field& i_other) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Equality operator
		/// @param[in] i_other The other field 
		/// @return True if the difference is 0, false otherwise
		//----------------------------------------------------------------------------------------------------------------------
		bool operator==(const discretized_field& i_other) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Inequality operator
		/// @param[in] i_other The other field 
		/// @return True if the difference is different than 0, false if it is 0
		//----------------------------------------------------------------------------------------------------------------------
		bool operator!=(const discretized_field& i_other) const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Fill the field using a tri-variate function ( F(x,y,z)=... )
		/// @param[in] i_function The function to evaluate
		//----------------------------------------------------------------------------------------------------------------------
		template<typename TriVariateFn>
		void fill(const TriVariateFn& i_function);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Fill a slice of the field using a tri-variate function ( F(x,y,z)=... )
		/// @param[in] i_function The function to evaluate
		/// @param[in] i_slice The slice id
		//----------------------------------------------------------------------------------------------------------------------
		template<typename TriVariateFn>
		void fill_slice(const TriVariateFn& i_function, unsigned int i_slice);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Fill the field using a tri-variate function ( F(x,y,z)=... ) Multithreaded version
		/// @param[in] i_function The function to evaluate
		//----------------------------------------------------------------------------------------------------------------------
		template<typename TriVariateFn>
		void fill_mt(const TriVariateFn& i_function);

		template<typename TriVariateFn>
		void fill_packets(const TriVariateFn& i_function);


		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the value at the sample i,j,k
		/// @param[in] i Index in X
		/// @param[in] j Index in Y
		/// @param[in] k Index in Z
		/// @return The value at i,j,k
		//----------------------------------------------------------------------------------------------------------------------
		float element_at(unsigned int i, unsigned int j, unsigned int k) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Nearest filtering given a position - position should be within the field
		/// @param[in] i_position Position to sample
		/// @return The nearest value at that position
		//----------------------------------------------------------------------------------------------------------------------
		float nearest(const point3d& i_position) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Tri-linear filtering - will make cascade linear interpolation to find the smooth value
		/// @param[in] i_position Position to sample
		/// @return The interpolated value at that position
		//----------------------------------------------------------------------------------------------------------------------
		float linear(const point3d& i_position) const;
                void gradient(const point3d& i_position, sdf::point3d* o_gradient) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Tri-linear filtering - will make cascade linear interpolation to find the smooth value
		/// @param[in] i_position Position to sample
		/// @return The interpolated value at that position
		//----------------------------------------------------------------------------------------------------------------------
		float operator()(const point3d& i_position) const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Minimum of the box
		/// @return Constant reference to the minimum of the field
		//----------------------------------------------------------------------------------------------------------------------
		const point3d& minimum() const { return m_min; }
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Maximum of the box
		/// @return Constant reference to the maximum of the field
		//----------------------------------------------------------------------------------------------------------------------
		const point3d& maximum() const { return m_max; }
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the grid data
		/// @return A constant reference to the grid
		//----------------------------------------------------------------------------------------------------------------------
		const sdf::grid& grid() const { return m_grid; }
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the positions sampled
		/// @param[out] o_positions An array containing all the positions
		//----------------------------------------------------------------------------------------------------------------------
		void get_positions(std::vector<point3d>* o_positions) const;

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
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the grid as raw data
		/// @return A constant reference to the raw data 
		//----------------------------------------------------------------------------------------------------------------------
		const grid::scalar_field& raw_data() const { return m_grid.data(); }
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the grid as raw data
		/// @return A reference to the raw data 
		//----------------------------------------------------------------------------------------------------------------------
		grid::scalar_field& raw_data() { return m_grid.data(); }
	private :

#ifndef SDF_NO_BOOST_THREAD
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The main thread loop to fill the field
		/// @param[in] i_function The function to evaluate
		/// @param[in] i_wq The work queue that deals the tasks
		//----------------------------------------------------------------------------------------------------------------------
		template<typename TriVariateFn>
		void fill_mt_single(const TriVariateFn& i_function, thread::work_queue* i_wq);
#endif

	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The grid samples
		//----------------------------------------------------------------------------------------------------------------------
		sdf::grid m_grid;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The minimum of the bounding box
		//----------------------------------------------------------------------------------------------------------------------
		point3d m_min;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The maximum of the bounding box
		//----------------------------------------------------------------------------------------------------------------------
		point3d m_max;
	};
}

#include <sdf/discretization/discretized_field.inl>

#endif // SDF_DISCRETIZED_FIELD_INCLUDED
