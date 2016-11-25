#ifndef SDF_DISTANCE_RECORD_INCLUDED
#define SDF_DISTANCE_RECORD_INCLUDED

#include <sdf/core/types.hpp>
#include <sdf/core/feature_type.hpp>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class distance_record "include/sdf/distance/distance_record.hpp"
	/// @brief A simple class to keep track of the "hit"
	///			The distance record holds the squared distance, the closest point and the type of hit
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	class distance_record
	{
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Default constructor
		//----------------------------------------------------------------------------------------------------------------------
		distance_record();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Set the feature type
		/// @param[in] i_type The feature type
		//----------------------------------------------------------------------------------------------------------------------
		void set_feature(const feature_type& i_type);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Set the distance square - we keep the squared distance for optimization
		/// @param[in] i_dist_sq The squared distance
		//----------------------------------------------------------------------------------------------------------------------
		void set_distance_squared(float i_dist_sq);		
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Set the closest point on the primitive (mesh, triangle ...)
		/// @param[in] i_point Closest point on the primitive
		//----------------------------------------------------------------------------------------------------------------------
		void set_closest_point(const point3d& i_point);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the squared distance
		/// @return The distance square
		//----------------------------------------------------------------------------------------------------------------------
		const float& distance_square() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the feature type of the distance
		/// @return The feature type 
		//----------------------------------------------------------------------------------------------------------------------
		const feature_type& feature()  const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the closest point of this distance
		/// @return The closest point
		//----------------------------------------------------------------------------------------------------------------------
		const point3d& closest_point() const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Less than operator - comaprison is based on distance
		/// @param[in] i_other The other one to compare
		/// @return True if this record is closer than the other
		//----------------------------------------------------------------------------------------------------------------------
		bool operator<(const distance_record& i_other) const;
	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Closest point - point of the distance record
		//----------------------------------------------------------------------------------------------------------------------
		point3d m_closest_point;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The distance squared to the closest point
		//----------------------------------------------------------------------------------------------------------------------
		float m_distance_square;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The feature type (vertex a b c or edges or face)
		//----------------------------------------------------------------------------------------------------------------------
		feature_type m_feature;
	};
}

#endif // SDF_DISTANCE_RECORD_INCLUDED