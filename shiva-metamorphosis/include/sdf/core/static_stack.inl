
//----------------------------------------------------------------------------------------------------------------------
template<typename T, unsigned int MaxSize>
sdf::static_stack<T,MaxSize>::static_stack() : m_index(0) {}


//----------------------------------------------------------------------------------------------------------------------
template<typename T, unsigned int MaxSize>
bool sdf::static_stack<T,MaxSize>::empty() const
{
	return m_index==0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T, unsigned int MaxSize>
unsigned int sdf::static_stack<T,MaxSize>::size() const
{
	return m_index;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T, unsigned int MaxSize>
unsigned int sdf::static_stack<T,MaxSize>::capacity() const
{
	return MaxSize;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T, unsigned int MaxSize>
void sdf::static_stack<T,MaxSize>::push(const T& i_elem)
{
	assert(m_index<MaxSize);
	m_array[m_index++]=i_elem;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T, unsigned int MaxSize>
void sdf::static_stack<T,MaxSize>::push(const T& i_elem1, const T& i_elem2)
{
	assert(m_index<MaxSize);
	m_array[m_index++]=i_elem1;
	assert(m_index<MaxSize);
	m_array[m_index++]=i_elem2;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T, unsigned int MaxSize>
void sdf::static_stack<T,MaxSize>::pop()
{
	assert(!empty());
	--m_index;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T, unsigned int MaxSize>
const T& sdf::static_stack<T,MaxSize>::top() const
{
	assert(!empty());
	return m_array[m_index-1];
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T, unsigned int MaxSize>
T& sdf::static_stack<T,MaxSize>::top()
{
	assert(!empty());
	return m_array[m_index-1];
}