#version 140
//#version 330

uniform vec4 u_Colour;
out vec4 fragColour;

void main( void )
{
	fragColour = u_Colour;
}