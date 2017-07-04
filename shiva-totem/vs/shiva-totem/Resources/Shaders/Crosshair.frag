#version 140
//#version 330

uniform vec3 u_Colour;

out vec4 fragColour;

void main()
{
	fragColour = vec4( u_Colour, 1.0 );
}