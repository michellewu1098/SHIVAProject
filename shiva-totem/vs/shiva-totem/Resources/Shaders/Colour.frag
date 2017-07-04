#version 140
//#version 330
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_explicit_attrib_location : require

uniform vec4 u_Colour;
out vec4 fragColour;

void main( void )
{
	fragColour = u_Colour;
}