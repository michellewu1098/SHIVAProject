#include <sdf/lookup/cell_weights.hpp>
#include <assert.h>

//----------------------------------------------------------------------------------------------------------------------
sdf::detail::cell_weights::cell_weights(float i_alpha) : m_alpha(i_alpha) 
{
	assert(m_alpha<1.f);
}

//----------------------------------------------------------------------------------------------------------------------
float sdf::detail::cell_weights::alpha() const
{
	return m_alpha;
}

//----------------------------------------------------------------------------------------------------------------------
float sdf::detail::cell_weights::beta() const
{
	return 1.f-m_alpha;
}