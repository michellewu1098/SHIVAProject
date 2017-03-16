#version 330
#extension GL_ARB_explicit_uniform_location : require

layout( location = 0 ) in vec3 vPosition;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ModelViewMatrix;
uniform mat4 u_InverseModelViewMatrix;

uniform vec3 boxSize; // extent of box
uniform vec3 boxCentre; // position of box

out vec3 o_WorldSpacePos;
out vec3 o_WorldSpaceCam;

void main( void )
{
	// Box/local space to World space
	vec4 pos = vec4( vPosition * boxSize + boxCentre, 1 ); 
	o_WorldSpacePos = pos.xyz;
	
	vec4 cam = vec4( 0, 0, 0, 1 );
	o_WorldSpaceCam = ( u_InverseModelViewMatrix * cam ).xyz;
	
    // Compute homogenuous position - World to Projected space
    gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * pos; 

}

