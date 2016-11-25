#ifndef SDF_CORE_POINT3D_INCLUDED
#define SDF_CORE_POINT3D_INCLUDED

#include <sdf/core/config.hpp>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @class point3d "include/sdf/core/point3d.hpp"
	/// @brief A simple 3D point class for basic operations (sum, diff, normalize, dot, cross...)
	/// @author Mathieu Sanchez
	/// @version 1.0
	/// @date Last Revision 28/06/11 Initial revision
	//----------------------------------------------------------------------------------------------------------------------
	class point3d
	{
	public :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Default constructor (0,0,0)
		//----------------------------------------------------------------------------------------------------------------------
		point3d();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief 3 floats constructor 
		/// @param[in] x X component
		/// @param[in] y Y component
		/// @param[in] z Z component
		//----------------------------------------------------------------------------------------------------------------------
		point3d(float x, float y, float z);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Copy constructor
		/// @param[in] i_copy the point to copy
		//----------------------------------------------------------------------------------------------------------------------
		point3d(const point3d& i_copy);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Read an obj file from a filename
		/// @param[in] x new X component
		/// @param[in] y new Y component
		/// @param[in] z new Z component
		//----------------------------------------------------------------------------------------------------------------------
		void set(float x, float y, float z);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Access an element of the point
		/// @param[in] i Index (0 is X, 1 is Y, 2 is Z)
		/// @return A constant reference to the element
		//----------------------------------------------------------------------------------------------------------------------
		const float& operator[](unsigned int i) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Access an element of the point
		/// @param[in] i Index (0 is X, 1 is Y, 2 is Z)
		/// @return A reference to the element
		//----------------------------------------------------------------------------------------------------------------------
		float& operator[](unsigned int i);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Dot product with another point/vector
		/// @param[in] i_other The right hand side of the dot product
		/// @return The dot product
		//----------------------------------------------------------------------------------------------------------------------
		float dot(const point3d& i_other) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Set this vector to the result of the cross product of the parameters
		/// @param[in] i_first Left hand side of the cross product
		/// @param[in] i_second Right hand side of the cross product
		//----------------------------------------------------------------------------------------------------------------------
		void cross(const point3d& i_first, const point3d& i_second);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The length squared of the vector
		/// @return The length squared
		//----------------------------------------------------------------------------------------------------------------------
		float length_squared() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Square distance between this and another point
		/// @param[in] i_other The other point to compare
		/// @return The distance between this and the other point , squared
		//----------------------------------------------------------------------------------------------------------------------
		float distance_squared_to(const point3d& i_other) const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Multiply vector by a scalar
		/// @param[in] scale The scalar value
		/// @return A new point that is the result of the multiplication
		//----------------------------------------------------------------------------------------------------------------------
		point3d operator*(float scale) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Add two points
		/// @param[in] i_other The other point to add
		/// @return A new point containing the sum of this and i_other
		//----------------------------------------------------------------------------------------------------------------------
		point3d operator+(const point3d& i_other) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Subtract two points
		/// @param[in] i_other The other point to add
		/// @return A new point containing (THIS - OTHER)
		//----------------------------------------------------------------------------------------------------------------------
		point3d operator-(const point3d& i_other) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Increment operator
		/// @param[in] i_other The point to increment to the current instance
		/// @return A reference to the current object
		//----------------------------------------------------------------------------------------------------------------------
		point3d& operator+=(const point3d& i_other);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Divide by operator
		/// @param[in] i_divider The divider
		/// @return A reference to the current object
		//----------------------------------------------------------------------------------------------------------------------
		point3d& operator/=(float i_divider);
                point3d& operator/(const point3d& i_div);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Equality operator - test each component, they have to be exactly the same to be valid
		/// @param[in] i_other The other point
		/// @return True if they are identical, false otherwise
		//----------------------------------------------------------------------------------------------------------------------
		bool operator ==(const point3d& i_other) const;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Normalize this vector - make it length 1
		//----------------------------------------------------------------------------------------------------------------------
		void normalize();

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Set the point to a weighted average of 3 points
		/// @param[in] i_first The first vertex
		/// @param[in] alpha The first weight
		/// @param[in] i_second The second vertex
		/// @param[in] beta The second weight
		/// @param[in] i_third The third vertex
		/// @param[in] gamma The third weight
		//----------------------------------------------------------------------------------------------------------------------
		void barycentric(const point3d& i_first, float alpha, const point3d& i_second, float beta, const point3d& i_third, float gamma);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Subtract a point ot another and put the result in this instance (this=left-right)
		/// @param[in] i_left Left side of the subtraction
		/// @param[in] i_right Right side of the subtraction
		//----------------------------------------------------------------------------------------------------------------------
		void subtract(const point3d& i_left, const point3d& i_right);

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get the object as a float array
		/// @return Return the vector as a C style float array
		//----------------------------------------------------------------------------------------------------------------------
		const float* as_float() const { return &x; }
	private :
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief X component
		//----------------------------------------------------------------------------------------------------------------------
		float x;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Y component
		//----------------------------------------------------------------------------------------------------------------------
		float y;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Z component
		//----------------------------------------------------------------------------------------------------------------------
		float z;
#ifdef SDF_USE_PADDING
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Padding to have 4 floats
		//----------------------------------------------------------------------------------------------------------------------
		float m_padding;
#endif 
	};
}

#endif /* SDF_CORE_POINT3D_INCLUDED */
