#version 120

uniform sampler3D volume;
uniform sampler3D volume2;
//uniform sampler2D permTexture;

varying vec3 viewSpacePos;
varying vec3 worldSpacePos;
varying vec3 worldSpaceCam;

//uniform vec3 fragcampos;
uniform vec3 fragboundmin;
uniform vec3 fragboundmax;
uniform vec4 parameters;
uniform vec4 colour;

uniform float alpha;



float fieldValue(vec3 texcoords)
{
    float voxel  = texture3D(volume , texcoords).a;
    float voxel2 = texture3D(volume2, texcoords).a;
    
    return alpha*voxel2 + (1-alpha)*voxel;
}

vec3 worldToTextureCoords(vec3 pos)
{

        vec3 size = fragboundmax-fragboundmin;
        vec3 mixVal = (pos-fragboundmin)/size;
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
            texcoord.z<0.0 || texcoord.z>1.0 )
    {
        return 0;
    }
    return 1;
}



void main ()
{
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
