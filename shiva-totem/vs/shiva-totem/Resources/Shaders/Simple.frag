#version 140
//#version 330
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_explicit_attrib_location : require

in vec3 o_Colours;
out vec4 fragColour;

void main( void )
{
	fragColour = vec4(o_Colours, 1);
}