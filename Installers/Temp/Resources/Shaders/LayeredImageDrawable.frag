#version 140
//#version 330

uniform vec4 LayerColour0_0;
uniform vec4 LayerColour0_1;
uniform vec4 LayerColour0_2;
uniform vec4 LayerColour0_3;

uniform vec4 LayerColour1_0;
uniform vec4 LayerColour1_1;
uniform vec4 LayerColour1_2;
uniform vec4 LayerColour1_3;

uniform vec4 LayerColour2_0;
uniform vec4 LayerColour2_1;
uniform vec4 LayerColour2_2;
uniform vec4 LayerColour2_3;

uniform vec4 LayerColour3_0;
uniform vec4 LayerColour3_1;
uniform vec4 LayerColour3_2;
uniform vec4 LayerColour3_3;

uniform sampler2D ImageLayer0;
uniform sampler2D ImageLayer1;
uniform sampler2D ImageLayer2;
uniform sampler2D ImageLayer3;

in vec2 o_Uvs;

out vec4 fragColour;

void main()
{
	vec4 layer0Result = texture( ImageLayer0, o_Uvs );
	layer0Result = ( layer0Result.x * LayerColour0_0 ) + ( layer0Result.y * LayerColour0_1 ) + ( layer0Result.z * LayerColour0_2 ) + ( layer0Result.w * LayerColour0_3 );

	vec4 layer1Result = texture( ImageLayer1, o_Uvs );
	layer1Result = ( layer1Result.x * LayerColour1_0 ) + ( layer1Result.y * LayerColour1_1 ) + ( layer1Result.z * LayerColour1_2 ) + ( layer1Result.w * LayerColour1_3 );
	
	vec4 layer2Result = texture( ImageLayer2, o_Uvs );
	layer2Result = ( layer2Result.x * LayerColour2_0 ) + ( layer2Result.y * LayerColour2_1 ) + ( layer2Result.z * LayerColour2_2 ) + ( layer2Result.w * LayerColour2_3 );
	
	vec4 layer3Result = texture( ImageLayer3, o_Uvs );
	layer3Result = ( layer3Result.x * LayerColour3_0 ) + ( layer3Result.y * LayerColour3_1 ) + ( layer3Result.z * LayerColour3_2 ) + ( layer3Result.w * LayerColour3_3 );

	fragColour = layer0Result + layer1Result + layer2Result + layer3Result; //vec4(1.0, 0.0, 0.0, 1.0);
}
