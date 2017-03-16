#version 120

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ModelViewMatrix;

void main( void )
{
	gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * vec4( gl_Vertex.xy, 0, 1 );
}