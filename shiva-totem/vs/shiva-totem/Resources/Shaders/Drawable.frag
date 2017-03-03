#version 330

uniform sampler2D tex;

in vec2 o_Uvs;
out vec4 fragColor;

void main()
{
	fragColor = texture( tex, o_Uvs );
}
