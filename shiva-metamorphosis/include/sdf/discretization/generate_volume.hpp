#ifndef SDF_DISCRETIZATION_GENERATE_VOLUME_INCLUDED
#define SDF_DISCRETIZATION_GENERATE_VOLUME_INCLUDED

#include <string>

namespace sdf
{
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Discreatize a field to a file. It is a useful function
	///			It will open the file at i_filename, read the mesh from there (.obj)
	///			Prepare the signed distance function (using the template provided)
	///			Discretize it to a uniform grid using the accuracy provided
	///			And save the field to the file changing the extension .obj to .bdf (binary discretized field)
	/// @param[in] i_filename THe path to the object file, will be used to create the bdf
	/// @param[in] i_accuracy Field accuracy
	//----------------------------------------------------------------------------------------------------------------------
	template<typename DistanceFunction>
	void discretize_field_to(const std::string& i_filename, unsigned int i_accuracy);

	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Discreatize a field to a file. It is a useful function
	///			It will open the file at i_filename, read the mesh from there (.obj)
	///			Prepare the signed distance function (using the template provided)
	///			Discretize it to a uniform grid using the accuracy provided and using the box provided
	///			And save the field to the file changing the extension .obj to .bdf (binary discretized field)
	/// @param[in] i_filename THe path to the object file, will be used to create the bdf
	/// @param[in] i_accuracy Field accuracy
	/// @param[in] i_box THe bounding box to discretize over
	//----------------------------------------------------------------------------------------------------------------------
	template<typename DistanceFunction>
	void discretize_field_to(const std::string& i_filename, unsigned int i_accuracy, const aabb& i_box);
}

#include <sdf/discretization/generate_volume.inl>

#endif // SDF_DISCRETIZATION_GENERATE_VOLUME_INCLUDED
