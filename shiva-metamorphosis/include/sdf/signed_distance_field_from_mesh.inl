
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
sdf::signed_distance_field_from_mesh<T>::signed_distance_field_from_mesh(const std::string& i_filename) : m_mesh(i_filename) 
{
	m_mesh.bounding_box(&m_box);
#ifdef _DEBUG
	m_initialized=false;
#endif 
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void sdf::signed_distance_field_from_mesh<T>::initialize()
{
	sign.initialize(m_mesh);
	lookup.initialize(m_mesh);
#ifdef _DEBUG
	m_initialized=true;
#endif 
}


//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void sdf::signed_distance_field_from_mesh<T>::initialize(const parameters& i_params)
{
	sign.initialize(m_mesh);
	lookup.initialize(m_mesh,i_params);
#ifdef _DEBUG
	m_initialized=true;
#endif 
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
float sdf::signed_distance_field_from_mesh<T>::operator()(const point3d& i_position) const
{
	unsigned int dummyint(0);
	distance_record dummyrecord;
	return (*this)(i_position,&dummyint,&dummyrecord);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool sdf::signed_distance_field_from_mesh<T>::operator()(const point3d i_positions[], float o_distances[]) const
{
#ifdef _DEBUG
	assert(m_initialized);
#endif 

	static const unsigned int numsamples = 8;
	unsigned int face_id[numsamples] = { 0,0,0,0,0,0,0,0 };
	distance_record closest_record[numsamples];

	lookup(i_positions,closest_record,face_id);

	for (int i = 0; i<numsamples; i++)
		o_distances[i]=sign(i_positions[i],closest_record[i],face_id[i])*sqrt(closest_record[i].distance_square());
	

	return 
		face_id[0]==face_id[1] && 
		face_id[0]==face_id[2] && 
		face_id[0]==face_id[4] ;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
float sdf::signed_distance_field_from_mesh<T>::operator()(float x, float y, float z) const
{
	unsigned int dummyint(0);
	distance_record dummyrecord;
	return (*this)(point3d(x,y,z),&dummyint,&dummyrecord);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
float sdf::signed_distance_field_from_mesh<T>::operator()(const point3d& i_position, unsigned int* o_face_id, distance_record* o_hit) const
{
#ifdef _DEBUG
	assert(m_initialized);
#endif 

	unsigned int face_id = 0;
	distance_record closest_record;

	lookup(i_position,&closest_record,&face_id);
	float dist = sqrt(closest_record.distance_square());

	dist*=sign(i_position,closest_record,face_id);

	*o_face_id = face_id;
	*o_hit=closest_record;

	return dist;
}


//----------------------------------------------------------------------------------------------------------------------
template<typename T>
std::size_t sdf::signed_distance_field_from_mesh<T>::memory_usage() const 
{ 
	return m_mesh.memory_usage()+sign.memory_usage()+lookup.memory_usage();
}