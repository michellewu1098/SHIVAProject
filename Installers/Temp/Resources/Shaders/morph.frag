#version 120

uniform sampler3D volume;
uniform sampler3D volume2;
uniform sampler3D volume3;
uniform sampler3D volume4;
//uniform sampler2D permTexture;

varying vec3 viewSpacePos;
varying vec3 worldSpacePos;
varying vec3 worldSpaceCam;

//uniform vec3 fragcampos;
uniform vec3 fragboundmin;
uniform vec3 fragboundmax;
uniform vec4 parameters;
uniform vec4 colour;

uniform int topObject;

uniform float alpha;

/// The following section is from :-
/// Stefan Gustavson (stegu@itn.liu.se) (2005). GLSL-noise [online]. [Accessed March 2011].
/// Available from: <http://webstaff.itn.liu.se/~stegu/simplexnoise/GLSL-noise.zip>.
/// --------------------------------------------------------------------------------------------------------
/// --------------------------------------------------------------------------------------------------------
/// --------------------------------------------------------------------------------------------------------
/// --------------------------------------------------------------------------------------------------------
/// --------------------------------------------------------------------------------------------------------
/// --------------------------------------------------------------------------------------------------------
/// end of Citation
/*
float noiseNormalized(vec3 p)
{
    return 0.5*noise(p)+0.5;
}

// Turbulance returns in [0,1] !
float turbulence(vec3 pos, vec3 offset, float initialFrequency, float amplitudeScale, float freqScale, int numOctaves)
{
    float sum = 0.0;
    float scale = 1.0;

    vec3 p = pos*initialFrequency+offset;

    float finalResultScale = 0.0;

    for (int i=0;i<numOctaves;i++)
    {
        sum += noiseNormalized(p)*scale; // This is not always used (absolute), 2 functions ?
        finalResultScale+=scale;

        scale *= amplitudeScale;
        p *= freqScale;
    }
    return sum/finalResultScale;
}

// Turbulance returns in [0,1] !
float turbulenceAbs(vec3 pos, vec3 offset, float initialFrequency, float amplitudeScale, float freqScale, int numOctaves)
{
    float sum = 0.0;
    float scale = 1.0;

    vec3 p = pos*initialFrequency+offset;

    float finalResultScale = 0.0;

    for (int i=0;i<numOctaves;i++)
    {
        sum += abs(noise(p)-0.5)*scale; // -0.5 to keep the chances of having low values
        finalResultScale+=scale;

        scale *= amplitudeScale;
        p *= freqScale;
    }
    return sum/finalResultScale;
}

float marbleCore(vec3 pos)
{

    float mixVal = turbulenceAbs(pos,vec3(0,0,0),3.0,0.5,2.0,6);

    mixVal = 0.5 * sin((pos.x+pos.y)*0.05 + mixVal) +0.5;
    mixVal*=mixVal*mixVal;

    return mixVal;
}

vec4 marble(vec3 pos)
{

    vec4 colour = vec4 (0.545,0.467,0.396,1.0);
    vec4 bgcolour = vec4(1.0,0.855,0.725,1.0);

    return mix(colour,bgcolour,marbleCore(pos));
}*/

float cylinderValue(vec3 coords)
{
	float radius = 0.1;
	float centreX = 0.5;
	float centreY = 0.5;
	float lowerZ = 0;
	float upperZ = 4;
	
	float value = sqrt( pow(coords.x-centreX,2) + pow(coords.y-centreY,2) ) - radius;

	value = max(value,lowerZ - coords.z);
	value = max(value,coords.z - upperZ);
	return value;
}

float fieldValue(vec3 texcoords)
{
	/*
    float voxel  = texture3D(volume , texcoords).a;
    //return voxel;
    
    texcoords.z-=1;
    float voxel2 = texture3D(volume2, texcoords).a;
    texcoords.z-=1;
    float voxel3 = texture3D(volume3, texcoords).a;
    texcoords.z-=1;
    float voxel4 = texture3D(volume4, texcoords).a;
    */
    
	float voxel, voxel2, voxel3, voxel4;
	voxel = voxel2 = voxel3 = voxel4 = 1.0f;
	
	float cylinder = cylinderValue(texcoords);

//    texcoords.z *= 4.0;
	if( topObject > 0 )
	{
    	voxel  = texture3D(volume , texcoords).a;
    	if( topObject > 1 )
    	{
			texcoords.z-=1;
			voxel2 = texture3D(volume2, texcoords).a;
			if( topObject > 2 )
			{
				texcoords.z-=1;
				voxel3 = texture3D(volume3, texcoords).a;
				if( topObject > 3 )
				{
					texcoords.z-=1;
					voxel4 = texture3D(volume4, texcoords).a;
				}
			}
		}
	}
	
    
    //return alpha*voxel2 + (1-alpha)*voxel;
    float resultA = min(voxel,voxel2);
    float resultB = min(voxel3,voxel4);
    return min(cylinder,min(resultA,resultB));
}

vec3 worldToTextureCoords(vec3 pos)
{
        vec3 size = fragboundmax-fragboundmin;
        vec3 mixVal = (pos-fragboundmin)/size;
        mixVal.z *= 4.0f;    // <- totem hack
        return mixVal;
}

vec3 bsearch(vec3 previous, vec3 current)
{
        float isoValue = parameters.x;
        float multiplier = -parameters.w;

        vec3 begin = previous;
        vec3 end = current;
        vec3 midPos;

        for(int i=0; i<6; i++)
        {
                midPos = (begin+end)*0.5;
                vec3 samplePos = worldToTextureCoords(midPos);
                float voxel = fieldValue(samplePos);

                if (voxel < 0.0)
                    end = midPos;
                else
                    begin = midPos;
        }

        return midPos;
}

vec3 getGradient(vec3 pos)
{
        float delta = parameters.z*2.0;
        vec3 ret;
        ret.x = fieldValue(pos+vec3(delta,0.0,0.0)) - fieldValue(pos-vec3(delta,0.0,0.0));
        ret.y = fieldValue(pos+vec3(0.0,delta,0.0)) - fieldValue(pos-vec3(0.0,delta,0.0));
        ret.z = fieldValue(pos+vec3(0.0,0.0,delta)) - fieldValue(pos-vec3(0.0,0.0,delta));

        return parameters.w*ret;
}

int validTexCoord(vec3 texcoord)
{
    if (
            texcoord.x<0.0 || texcoord.x>1.0 ||
            texcoord.y<0.0 || texcoord.y>1.0 ||
            texcoord.z<0.0 || texcoord.z>4.0 )
    {
        return 0;
    }
    return 1;
}

float ambientOcclusion(vec3 pos, vec3 norm)
{
    float delta = parameters.z*0.4;
    float accumulation = 0.0;
    float max_possible = 0.0;
    for (float i=1.0;i<=5.0;i++)
    {
        float expo = 1.0/pow(2.0,i);
        vec3 texcoord = worldToTextureCoords(pos+norm*i*delta);
        float actual = 0.0;
        float expected = i*delta;
        if (validTexCoord(texcoord)==0)
            actual=expected;
        else
            actual = max(fieldValue(texcoord),0.0);

        accumulation+=expo*(expected-actual);
        max_possible+=expo*(expected);
    }
    return 1.0-accumulation/max_possible;
}



void main ()
{
//       gl_FragColor = vec4(1);
//        return;
        
        float isoValue = parameters.x;
        int nIterations = int(parameters.y);
        float stepSize = parameters.z;
        vec3 diffColour = colour.rgb;
        float multiplier = -parameters.w;
        

        vec3 cam = worldSpaceCam;
        vec3 pos = worldSpacePos;
        vec3 dir = normalize(worldSpacePos-cam);
        pos+=dir*stepSize*0.5f; // Make sure we dont miss straight away
        vec3 norm = vec3(0,1,0);

        int i=0;


        vec3 texCoord;
        vec3 lastPos = pos;
        float sample = 0;
        for (i=0;i<nIterations;i++)
        {
            // Get the texture coordinates to sample from the position along the ray

            texCoord = worldToTextureCoords(pos);
            // If the texture coordinates are out of the box
            if (validTexCoord(texCoord)==0)
                discard;
            float lastSample = sample;
            lastPos = pos;
            sample = fieldValue(texCoord);
            if (sample<0.0)
            {
                pos = bsearch(lastPos,pos);
                texCoord = worldToTextureCoords(pos);
                break;
            }
            pos += sample*dir;
        }


        norm = getGradient(texCoord);
        // proceed to shading
        float ambientimportant = 0.1;
        float diffuseimportant = 1.0-ambientimportant;
        vec3 I = normalize(cam-pos);
        vec3 N = normalize(norm);
        float LdotN = max(dot(I, N),0.0);



        float factor = 1.0;
        float frequency = 20.0;
        vec4 colour2 = colour;//marble(pos*frequency);



        vec3 viewcolour = (colour2.rgb) * LdotN;
        vec3 diffusecolour = clamp(viewcolour*diffuseimportant+diffColour*ambientimportant,0.0,1.0);


        gl_FragColor = vec4(diffusecolour,1);
        
}
