#version 140
//#version 330
//#extension GL_ARB_explicit_uniform_location : require
//#extension GL_ARB_explicit_attrib_location : require


// GLSL 330
//layout( location = 0 ) in vec3 vPosition;

// GLSL 140
in vec3 vPosition;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ModelViewMatrix;

void main( void )
{
	gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * vec4( vPosition, 1 );
}