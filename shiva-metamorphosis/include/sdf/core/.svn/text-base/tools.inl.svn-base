
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
const T& sdf::min(const T& i_first, const T& i_second)
{
	return std::min(i_first,i_second);
}
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
const T& sdf::max(const T& i_first, const T& i_second)
{
	return std::max(i_first,i_second);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
const T& sdf::min(const T& i_first, const T& i_second, const T& i_third)
{
	return std::min(i_first,std::min(i_second,i_third));
}
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
const T& sdf::max(const T& i_first, const T& i_second, const T& i_third)
{
	return std::max(i_first,std::max(i_second,i_third));
}
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
const T& sdf::mid(const T& i_first, const T& i_second, const T& i_third)
{
	return std::min(i_first,std::max(i_second,i_third));
}
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
const T& sdf::clamp(const T& i_first, const T& i_lower, const T& i_upper)
{
        return std::min(i_upper,std::max(i_lower,i_first));
}


//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void sdf::minmax(const T& i_first, const T& i_second, T* o_min, T* o_max)
{
	if (i_first<i_second)
	{
		*o_min=i_first;
		*o_max=i_second;
	}
	else
	{
		*o_min=i_second;
		*o_max=i_first;
	}
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void sdf::minmax(const T& i_first, const T& i_second, const T& i_third, T* o_min, T* o_max)
{
	*o_min=*o_max=i_first;
	if (i_second<*o_min) *o_min=i_second;
	if (i_second>*o_max) *o_max=i_second;
	if (i_third<*o_min) *o_min=i_third;
	if (i_third>*o_max) *o_max=i_third;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void sdf::minmax(T* io_min, T* io_max)
{
	if (*io_min>*io_max)
		std::swap(*io_min,*io_max);
}


//----------------------------------------------------------------------------------------------------------------------
template<typename T>
T sdf::lerp(const T& A, const T& B, float t)
{
        return A+(B-A)*t;
}


//----------------------------------------------------------------------------------------------------------------------
template<typename T>
std::size_t sdf::dynamic_memory(const std::vector<T>& i_array)
{
	return i_array.size()*sizeof(T);
}