#include <sdf/core/mesh.hpp>
#include <sdf/core/obj_file.hpp>
#include <sdf/signed_distance_field_from_mesh.hpp>
#include <sdf/discretization/discretized_field.hpp>

//----------------------------------------------------------------------------------------------------------------------
template<typename DistanceFunction>
void sdf::discretize_field_to<DistanceFunction>(const std::string& i_filename, unsigned int i_accuracy)
{
	const std::string out = i_filename.substr(0,i_filename.size()-4)+".bdf";

	sdf::aabb box;
	sdf::mesh mesh;
	sdf::obj_file mr;
	DistanceFunction eval(mesh);
	mr.read(i_filename,&(mesh.vertices()), &(mesh.indices()),&box);
	eval.initialize();
	sdf::vector3d extent;
	box.extent(&extent);
	sdf::discretized_field df(box.minimum()-extent*0.1f,box.maximum()+extent*0.1f,i_accuracy,i_accuracy,i_accuracy);
	df.fill_mt(eval);
	sdf::binary_file_out fileout(out);
	df.serialize(fileout);
}


//----------------------------------------------------------------------------------------------------------------------
template<typename DistanceFunction>
void sdf::discretize_field_to<DistanceFunction>(const std::string& i_filename, unsigned int i_accuracy, const aabb& i_box)
{
	const std::string out = i_filename.substr(0,i_filename.size()-4)+".bdf";
	sdf::mesh mesh;
	sdf::obj_file mr;
	DistanceFunction eval(mesh,0.002f);
	mr.read(i_filename,&(mesh.vertices()), &(mesh.indices()));
	eval.initialize();

	sdf::discretized_field df(i_box.minimum(),i_box.maximum(),i_accuracy,i_accuracy,i_accuracy);
	df.fill_mt(eval);
	sdf::binary_file_out fileout(out);
	df.serialize(fileout);
}

