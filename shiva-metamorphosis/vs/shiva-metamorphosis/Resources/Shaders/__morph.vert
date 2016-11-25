uniform mat4 projectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 invView;

uniform vec3 vertsize; // extent of box
uniform vec3 vertposition; // position of box

varying vec3 viewSpacePos;
varying vec3 worldSpacePos;
varying vec3 worldSpaceCam;

void main(void)
{
    vec4 pos = vec4(gl_Vertex.xyz*vertsize+vertposition,1); // Box/local space to World space
    vec4 vsPos4 = transpose(ViewMatrix)*pos;
    viewSpacePos = vsPos4.xyz;
    vec4 cam = vec4(0,0,0,1);

    //mat4 invView = inverse(ViewMatrix);

    worldSpacePos = pos.xyz;
    worldSpaceCam = (transpose(invView)*cam).xyz;

    // Compute homogenuous position
    gl_Position = gl_ProjectionMatrix*vsPos4; // World to Projected space
    
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

}
