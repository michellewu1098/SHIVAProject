#ifndef SDF_LOOKUP_GRID_CELL_INCLUDED
#define SDF_LOOKUP_GRID_CELL_INCLUDED

#include <sdf/core/types.hpp>
#include <vector>

namespace sdf
{
	namespace detail
	{
		//----------------------------------------------------------------------------------------------------------------------
		/// @class grid_cell "include/sdf/lookup/grid_cell.hpp"
		/// @brief A grid cell from a regular grid - It is only a "bucket" for now, but might be something else later
		/// @author Mathieu Sanchez
		/// @version 1.0
		/// @date Last Revision 28/06/11 Initial revision
		//----------------------------------------------------------------------------------------------------------------------
		class grid_cell
		{
		public :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Typedef of the polygon array (bucket)
			//----------------------------------------------------------------------------------------------------------------------
			typedef std::vector<index_type> polygon_array;
		public :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Add a polygon to the bucket
			/// @param[in] i_polygon The polygon index
			//----------------------------------------------------------------------------------------------------------------------
			void add(index_type i_polygon);
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief An empty cell is a cell with no polygons intersecting the cell
			/// @return True if there are no polygons
			//----------------------------------------------------------------------------------------------------------------------
			bool is_empty() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Number of polygons in the cell
			/// @return The number of polygons in the bucket
			//----------------------------------------------------------------------------------------------------------------------
			unsigned int size() const;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Access an element of the bucket
			/// @param[in] i Index in the bucket
			/// @return The polygon index in the bucket at index 'i'
			//----------------------------------------------------------------------------------------------------------------------
			const index_type& operator[](unsigned int i) const;

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the array of polygons
			/// @return A reference tothe polygon array
			//----------------------------------------------------------------------------------------------------------------------
			polygon_array& array() { return m_polygons; }
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the array of polygons
			/// @return A constant reference tothe polygon array
			//----------------------------------------------------------------------------------------------------------------------
			const polygon_array& array() const { return m_polygons; }
		private :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The polygons in the bucket
			//----------------------------------------------------------------------------------------------------------------------
			polygon_array m_polygons;
		};
	}
}

#endif /* SDF_LOOKUP_GRID_CELL_INCLUDED */