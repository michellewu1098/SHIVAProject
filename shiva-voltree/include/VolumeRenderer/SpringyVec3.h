/*
 * SpringyVec3.h
 *
 *  Created on: Jan 10, 2013
 *      Author: leigh
 */

#ifndef SPRINGYVEC3_H_
#define SPRINGYVEC3_H_

#include <cml/cml.h>
#include <string>
#include <sstream>

#include "VolumeTree/VolumeTree.h"

// TODO: This should be templated, but I only need it for a vector3f at the moment
class SpringyVec3
{
public:
	SpringyVec3();
	SpringyVec3(cml::vector3f currentPos);
	SpringyVec3(cml::vector3f currentPos, cml::vector3f newPos);
	SpringyVec3(cml::vector3f currentPos, cml::vector3f newPos, float speed);

	cml::vector3f GetCurrent() {return _current;}

	void SetAim( cml::vector3f newPos );
	cml::vector3f GetAim() {return _aim;}
	void Set( cml::vector3f value );
	void Set( float x, float y, float z ) {Set( cml::vector3f(x,y,z) );}

	void Update(float deltaTs);

	std::string Print();

protected:

	cml::vector3f _original, _aim, _current;

	float _speed, _interp;
};

#endif
