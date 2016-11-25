
#include "VolumeRenderer/SpringyVec3.h"

SpringyVec3::SpringyVec3()
{
	_speed = 1.0f;
	_interp = 1.0f;
}

SpringyVec3::SpringyVec3(cml::vector3f currentPos)
{
	_speed = 1.0f;
	_interp = 1.0f;
	_original = _current = _aim = currentPos;
}

SpringyVec3::SpringyVec3(cml::vector3f currentPos, cml::vector3f newPos)
{
	_speed = 1.0f;
	_interp = 0.0f;
	_original = _current = currentPos;
	_aim = newPos;
}

SpringyVec3::SpringyVec3(cml::vector3f currentPos, cml::vector3f newPos, float speed)
{
	_interp = 0.0f;
	_original = _current = currentPos;
	_aim = newPos;
	_speed = speed;
}

void SpringyVec3::SetAim( cml::vector3f newPos )
{
	_interp = 0.0f;
	_original = _current;
	_aim = newPos;
}

void SpringyVec3::Set( cml::vector3f value )
{
	_original = _current = _aim = value;
	_interp = 1.0f;
}

void SpringyVec3::Update(float deltaTs)
{
	// Just lerp for testing
	if( _interp < 1.0f )
	{
		_interp += deltaTs / _speed;
		if( _interp > 1.0f )
		{
			_interp = 1.0f;
		}
		_current = _original*(1.0f-_interp) + _aim*_interp;
	}
	else
	{
		_current = _aim;
	}
}

std::string SpringyVec3::Print()
{
	std::stringstream stream;
	stream<<_current.data()[0]<<" "<<_current.data()[1]<<" "<<_current.data()[2];
	return stream.str();
}
