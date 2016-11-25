
varying vec2 texcoords;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
	texcoords  = gl_MultiTexCoord0.xy;
	
//	#ifdef __GLSL_CG_DATA_TYPES
		gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
//	#endif
}
