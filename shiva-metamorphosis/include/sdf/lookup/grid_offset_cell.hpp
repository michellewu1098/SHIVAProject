#ifndef SDF_LOOKUP_GRID_OFFSET_CELL_INCLUDED
#define SDF_LOOKUP_GRID_OFFSET_CELL_INCLUDED

#include <sdf/core/types.hpp>

namespace sdf
{
	namespace detail
	{
		//----------------------------------------------------------------------------------------------------------------------
		/// @class grid_offset_cell "include/sdf/lookup/grid_offset_cell.hpp"
		/// @brief The grid offset cell is a lightweight cell containing the offset and the number of elements in a "master" array
		///			It allows the grid to have everything in a single array
		/// @author Mathieu Sanchez
		/// @version 1.0
		/// @date Last Revision 20/07/11 Initial revision
		//----------------------------------------------------------------------------------------------------------------------
		class grid_offset_cell
		{
		public :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Default constructor
			//----------------------------------------------------------------------------------------------------------------------
			grid_offset_cell();
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Constructor
			/// @param[in] i_offset Offset in the master array
			/// @param[in] i_num Number of elements of this cell
			//----------------------------------------------------------------------------------------------------------------------
			grid_offset_cell(index_type i_offset, index_type i_num);
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Set the offset and number 
			/// @param[in] i_offset Offset in the master array
			/// @param[in] i_num Number of elements of this cell
			//----------------------------------------------------------------------------------------------------------------------
			void set(index_type i_offset, index_type i_num);
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Test is the cell is empty
			/// @return size==0 ?
			//----------------------------------------------------------------------------------------------------------------------
			bool is_empty() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the number of elements in the cell
			/// @return The number of elements (size) of the cell
			//----------------------------------------------------------------------------------------------------------------------
			index_type size() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Access an element of the array
			/// @param[in] i The index of the element
			/// @return The offset in the polygon array (master). const index_type polyid = polygons[offsetcell[i]]; 
			//----------------------------------------------------------------------------------------------------------------------
			index_type operator[](index_type i) const;
		private :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Offset in the array of polygons
			//----------------------------------------------------------------------------------------------------------------------
			index_type m_offset;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Number of elements (size attached to this node)
			//----------------------------------------------------------------------------------------------------------------------
			index_type m_size;
		};

	}
}

#endif /* SDF_LOOKUP_GRID_OFFSET_CELL_INCLUDED */