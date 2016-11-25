#ifndef SDF_CORE_AABB_INCLUDED
#define SDF_CORE_AABB_INCLUDED

#include <sdf/core/config.hpp>
#include <sdf/core/math.hpp>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class aabb "include/sdf/core/aabb.hpp"
	/// @brief Axis-aligned bounding box - stored as a minimum/maximum point
	///			Base class for most trees
	///			The minimum point of a box is the point with the minimum x , minimum y and minimum z
	///			The maximum is similar
	///			The extent is the vector from the minimum to the maximum
	///			The centre is the centre of the box
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	class aabb
	{
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Default constructor - Box will be set to an invalid box
		//----------------------------------------------------------------------------------------------------------------------
		aabb();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Construct from one point - the bounding box will have no volume
		/// @param[in] i_point A point the bounding box should enclose
		//----------------------------------------------------------------------------------------------------------------------
		aabb(const point3d& i_point);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructor of a box from 2 points - it will build the box from the two points, they dont need to be minimum/maximum
		/// @param[in] A First point
		/// @param[in] B Second point
		//----------------------------------------------------------------------------------------------------------------------
		aabb(const point3d& A, const point3d& B);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructor of a box from 3 points - it will build the box from the three points
		/// @param[in] A First point
		/// @param[in] B Second point
		/// @param[in] C Third point
		//----------------------------------------------------------------------------------------------------------------------
		aabb(const point3d& A, const point3d& B, const point3d& C);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Build the bounding box from an array of points
		/// @param[in] i_points Pointer to the start of the point array 
		/// @param[in] i_num_elements Number of elements in the array
		//----------------------------------------------------------------------------------------------------------------------
		aabb(const point3d* i_points, unsigned int i_num_elements);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the minimum of the box (min x, min y, min z)
		/// @return Constant reference to the minimum point
		//----------------------------------------------------------------------------------------------------------------------
		const point3d& minimum() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the maximum of the box 
		/// @return Constant reference to the maximum point
		//----------------------------------------------------------------------------------------------------------------------
		const point3d& maximum() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get one of the two bounds using an index
		/// @param[in] i Index of the bound, 0 is minimum, 1 is maximum, anything else is invalid
		/// @return Constant reference to the bound requested
		//----------------------------------------------------------------------------------------------------------------------
		const point3d& bound(unsigned int i) const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Include the point to the current volume, it will extend it if necessary
		/// @param[in] i_element The new point/element to include in the volume
		//----------------------------------------------------------------------------------------------------------------------
		void include(const point3d& i_element);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Directly set the bounding box by including the two points (order doesnt matter, it will build the tightest box including A and B)
		/// @param[in] A First point
		/// @param[in] B Second point
		//----------------------------------------------------------------------------------------------------------------------
		void set(const point3d& A, const point3d& B);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Directly set the bounding box by including the three points (order doesnt matter, it will build the tightest box including A, B and C)
		/// @param[in] A First point
		/// @param[in] B Second point
		/// @param[in] C Third point
		//----------------------------------------------------------------------------------------------------------------------
		void set(const point3d& A, const point3d& B, const point3d& C);
		
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the extent of the box
		/// @param[out] o_extent Fill the extent of the volume in there
		//----------------------------------------------------------------------------------------------------------------------
		void extent(point3d* o_extent) const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Centre of the box
		/// @param[out] o_centre The box centre
		//----------------------------------------------------------------------------------------------------------------------
		void centre(point3d* o_centre) const;
		
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Detect a collision between two axis aligned bounding boxes
		/// @param[in] i_other The other bounding box (to test against)
		/// @return True if it collides, false otherwise. Note that if one is fully included in the other it is considered a collision
		//----------------------------------------------------------------------------------------------------------------------
		bool intersects(const aabb& i_other) const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Intersection between a triangle and this box
		/// @param[in] i_first First vertex of the triangle
		/// @param[in] i_second Second vertex of the triangle
		/// @param[in] i_third Third vertex of the triangle
		/// @warning ::WARNING:: This function is incomplete yet
		//----------------------------------------------------------------------------------------------------------------------
		bool intersects(const point3d& i_first,const point3d& i_second,const point3d& i_third) const;
		
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Test if the point is in the box
		/// @param[in] i_element The point to test
		/// @return True if the point is in the box, false otherwise
		//----------------------------------------------------------------------------------------------------------------------
		bool is_inside(const point3d& i_element) const;
	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Bounds of the volume, minimum and maximum
		//----------------------------------------------------------------------------------------------------------------------
		point3d m_bounds[2];
	};
}

#endif /* SDF_CORE_AABB_INCLUDED */