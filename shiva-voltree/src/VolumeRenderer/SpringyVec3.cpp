#include "VolumeRenderer/SpringyVec3.h"

//----------------------------------------------------------------------------------

SpringyVec3::SpringyVec3()
{
	m_speed = 1.0f;
	m_interp = 1.0f;
}

//----------------------------------------------------------------------------------

SpringyVec3::SpringyVec3( const cml::vector3f &_currentPos )
{
	m_speed = 1.0f;
	m_interp = 1.0f;
	m_original = m_current = m_aim = _currentPos;
}

//----------------------------------------------------------------------------------

SpringyVec3::SpringyVec3( const cml::vector3f &_currentPos, const cml::vector3f &_newPos )
{
	m_speed = 1.0f;
	m_interp = 0.0f;
	m_original = m_current = _currentPos;
	m_aim = _newPos;
}

//----------------------------------------------------------------------------------

SpringyVec3::SpringyVec3( const cml::vector3f &_currentPos, const cml::vector3f &_newPos, const float &_speed )
{
	m_interp = 0.0f;
	m_original = m_current = _currentPos;
	m_aim = _newPos;
	m_speed = _speed;
}

//----------------------------------------------------------------------------------

void SpringyVec3::SetAim( const cml::vector3f &_aim )
{
	m_interp = 0.0f;
	m_original = m_current;
	m_aim = _aim;
}

//----------------------------------------------------------------------------------

void SpringyVec3::SetPos( const cml::vector3f &_value )
{
	m_original = m_current = m_aim = _value;
	m_interp = 1.0f;
}

//----------------------------------------------------------------------------------

void SpringyVec3::Update( float _deltaTs )
{
	// Just lerp for testing
	if( m_interp < 1.0f )
	{
		m_interp += _deltaTs / m_speed;
		if( m_interp > 1.0f )
		{
			m_interp = 1.0f;
		}
		m_current = m_original * ( 1.0f - m_interp ) + m_aim * m_interp;
	}
	else
	{
		m_current = m_aim;
	}
}

//----------------------------------------------------------------------------------

std::string SpringyVec3::Print()
{
	std::stringstream stream;
	stream << m_current.data()[ 0 ] << " " << m_current.data()[ 1 ] << " " << m_current.data()[ 2 ];
	return stream.str();
}

//----------------------------------------------------------------------------------