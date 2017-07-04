#version 140
//#version 330
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_explicit_attrib_location : require

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec3 vColours;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ModelViewMatrix;

out vec3 o_Colours;

void main( void )
{
	o_Colours = vColours;
	gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * vec4( vPosition, 1 );
}