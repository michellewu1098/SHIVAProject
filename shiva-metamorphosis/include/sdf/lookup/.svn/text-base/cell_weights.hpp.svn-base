#ifndef SDF_LOOKUP_CELL_WEIGHTS_INCLUDED
#define SDF_LOOKUP_CELL_WEIGHTS_INCLUDED

namespace sdf
{
	namespace detail
	{
		//----------------------------------------------------------------------------------------------------------------------
		/// @class cell_weights "include/sdf/lookup/cell_weights.hpp"
		/// @brief Linear weighting used for cells between the average size and maximum size
		/// @author Mathieu Sanchez
		/// @version 1.0
		/// @date Last Revision 28/06/11 Initial revision
		//----------------------------------------------------------------------------------------------------------------------
		class cell_weights
		{
		public :
			cell_weights(float i_alpha);

			float alpha() const;
			float beta() const;
		private:
			float m_alpha;
		};
	}
}

#endif /* SDF_LOOKUP_CELL_WEIGHTS_INCLUDED */