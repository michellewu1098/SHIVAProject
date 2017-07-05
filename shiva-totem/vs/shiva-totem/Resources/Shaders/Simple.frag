#version 140
//#version 330

in vec3 o_Colours;
out vec4 fragColour;

void main( void )
{
	fragColour = vec4(o_Colours, 1);
}