#ifndef SDF_CORE_TOOLS_INCLUDED
#define SDF_CORE_TOOLS_INCLUDED

#include <sdf/core/config.hpp>
#include <iostream>
#include <vector>

namespace sdf
{

	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Get the minimum of the two vars 
	/// @warning operator< must be implemented or available
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	const T& min(const T& i_first, const T& i_second);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Get the maximum of the two vars 
	/// @warning operator< must be implemented or available
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	const T& max(const T& i_first, const T& i_second);

	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Get the minimum of the three vars 
	/// @warning operator< must be implemented or available
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	const T& min(const T& i_first, const T& i_second, const T& i_third);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Get the maximum of the three vars 
	/// @warning operator< must be implemented or available
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	const T& max(const T& i_first, const T& i_second, const T& i_third);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Get the middle value of the three vars 
	/// @warning operator< must be implemented or available
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	const T& mid(const T& i_first, const T& i_second, const T& i_third);

	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Get the clamp value
	/// @param[in] i_first THe value to clamp
	/// @param[in] i_second The lower bound
	/// @param[in] i_third The upper bound
	/// @warning operator< must be implemented or available
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	const T& clamp(const T& i_first, const T& i_lower, const T& i_upper);

	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Fill the min and max from two "things"
	/// @param[in] i_first One value
	/// @param[in] i_second Another value
	/// @param[out] o_min The minimum value to be filled
	/// @param[out] o_max The maximum value to be filled
	/// @warning operator< must be implemented or available
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void minmax(const T& i_first, const T& i_second, T* o_min, T* o_max);

	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Fill the min and max from two "things"
	/// @param[in] i_first One value
	/// @param[in] i_second Another value
	/// @param[in] i_third And another value
	/// @param[out] o_min The minimum value to be filled
	/// @param[out] o_max The maximum value to be filled
	/// @warning operator< and operator> must be implemented or available
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void minmax(const T& i_first, const T& i_second, const T& i_third, T* o_min, T* o_max);

	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Sort the two numbers 
	/// @param io_min Contains one number, will contain the minimum
	/// @param io_max Contains one number, will contain the maximum
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void minmax(T* io_min, T* io_max);

	template<typename T>
	T lerp(const T& A, const T& B, float t);

	template<typename T>
	std::size_t dynamic_memory(const std::vector<T>& i_array);
}

#include <sdf/core/tools.inl>

#endif // SDF_CORE_TOOLS_INCLUDED
