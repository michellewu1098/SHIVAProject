#version 330
#extension GL_ARB_explicit_uniform_location : require

layout( location = 0 ) in vec2 vPosition;
layout( location = 1 ) in vec2 vTexCoords;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ModelViewMatrix;

out vec2 o_Uvs;

void main()
{
	gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * vec4( vPosition, 0, 1  );
	o_Uvs = vTexCoords;
}

