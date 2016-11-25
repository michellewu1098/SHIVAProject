#version 120

uniform sampler3D volume;
uniform sampler3D volume2;

varying vec3 viewSpacePos;
varying vec3 worldSpacePos;
varying vec3 worldSpaceCam;

//uniform vec3 fragcampos;
uniform vec3 fragboundmin;
uniform vec3 fragboundmax;
uniform vec4 parameters;
uniform vec4 colour;

uniform float a0 = 0.9;
uniform float a1 = 2.85; // 0.3, makes the first object 'bluge'
uniform float a2 = 2.85; // 0.3, makes the second object 'bluge'
uniform float a3 = 60.0;  // 1.0, when set to low value, gives a 'snap'
uniform float alpha;
uniform float clipdist = 0.45; // size in middle where neither object is there
uniform float boundplanedist = 0.55; // length of blending section

vec3 worldToTextureCoords(vec3 pos)
{

        vec3 size = fragboundmax-fragboundmin;
        vec3 mixVal = (pos-fragboundmin)/size;
        return mixVal;
}

float union_c1(float a, float b)
{
	return a+b+sqrt(a*a+b*b);
}

float intersection_c1(float a, float b)
{
	return a+b-sqrt(a*a+b*b);
}

float blended_union(float a, float b, const vec3 param)
{
        float f1influence = a/param.y;
        float f2influence = b/param.z;
        return union_c1(a,b)+(param.x)/(1+f1influence*f1influence+f2influence*f2influence);
}

float blended_intersection(float a, float b, const vec3 param)
{
        float f1influence = a/param.y;
        float f2influence = b/param.z;
        return intersection_c1(a,b)+(param.x)/(1.0+f1influence*f1influence+f2influence*f2influence);
}

float getField(vec3 texcoords)
{

	float time = alpha;//*(2*boundplanedist)-boundplanedist;
    float rtime = time - 0.5;

    float f1 = -texture3D(volume , texcoords).a;
    float f2 = -texture3D(volume2, texcoords).a;

    f1 = intersection_c1(f1, +rtime-clipdist);
    f2 = intersection_c1(f2, -rtime-clipdist);

// unused, it seems
//    float coef1 = (rtime+1)*0.5;
//    float coef2 = 1-coef1;

    float f3 = 0;
    f3 = 1-texcoords.x*texcoords.x-texcoords.y*texcoords.y-rtime*rtime;
    f3 = intersection_c1(
                        intersection_c1(f3, (rtime+boundplanedist)) ,
                        (boundplanedist-rtime)
                        );

    float at1 = a0;

    float invat2 = a1;
    float invat3 = a2;
    float invat4 = a3;

    float f1weighted = f1*invat2;
    float f2weighted = f2*invat3;
    float f3weighted = f3*invat4;

    float r1 = f1weighted*f1weighted+f2weighted*f2weighted;
    float r2=0;
    if (f3>0)
        r2 = f3weighted*f3weighted;

    float rr=0;
    if(r1>0)
        rr = r1/(r1+r2);


    float d=0;
    if (rr<1)
    {
        float reverserr = 1-rr;
        float rrrcube = reverserr*reverserr*reverserr;
        d = at1*rrrcube/(1+rr);
    }

    float fin = f1 + f2 + sqrt(f1*f1+f2*f2)/**(abs(cos(time*3.141592653589793238462643383)))*/ + d;

    return -fin;
}

vec3 bsearch(vec3 previous, vec3 current)
{
        float isoValue = parameters.x;
        float multiplier = -parameters.w;

        vec3 begin = previous;
        vec3 end = current;

        for(int i=0; i<12; i++)
        {
                vec3 midPos = (begin+end)*0.5;
                vec3 samplePos = worldToTextureCoords(midPos);
                float voxel = getField(samplePos);

                if (voxel*multiplier < isoValue*multiplier)
                        begin = midPos;
                else
                        end = midPos;
        }

        return worldToTextureCoords((begin+end)*0.5);
}


vec3 getGradient(vec3 pos)
{
        float delta = parameters.z*2.0;
        vec3 ret;
        ret.x = getField(pos+vec3(delta,0.0,0.0)) - getField(pos-vec3(delta,0.0,0.0));
        ret.y = getField(pos+vec3(0.0,delta,0.0)) - getField(pos-vec3(0.0,delta,0.0));
        ret.z = getField(pos+vec3(0.0,0.0,delta)) - getField(pos-vec3(0.0,0.0,delta));

        return normalize(parameters.w*ret);
}

void main ()
{
        float isoValue = parameters.x;
        int nIterations = 1024;//int(parameters.y);
        float stepSize = parameters.z;
        vec3 diffColour = colour.rgb;
        float multiplier = -parameters.w;


        vec3 cam = worldSpaceCam;
        vec3 pos = worldSpacePos;
        vec3 dir = normalize(worldSpacePos-cam);
        pos+=dir*stepSize*0.5; // Make sure we dont miss straight away
        vec3 norm = vec3(0,1,0);

        //vec3 debugColour = vec3(1,0,0);
        int i=0;
        vec3 texCoord;
        // Begin ray marching, keep i outside to check if we hit or not
        for(i=0;i<nIterations;i++)
        {
                // Get the texture coordinates to sample from the position along the ray
                texCoord = worldToTextureCoords(pos);
                // If the texture coordinates are out of the box
                if (
                        texCoord.x<0.0 || texCoord.x>1.0 ||
                        texCoord.y<0.0 || texCoord.y>1.0 ||
                        texCoord.z<0.0 || texCoord.z>1.0 )
                {
                        // We are out of the box, kill the fragment
                        discard;
                        //debugColour=vec3(0,0,i/nIterations);
                        break;
                }

                float sample = getField(texCoord);
                if(sample*multiplier>isoValue*multiplier)
                {
                        // Binary look up to find a bit more precisely where
                        texCoord = bsearch(pos-stepSize*dir, pos);
                        // Get the normal (by gradient)
                        norm = getGradient(texCoord);
                        // We foudn a solution, stop the loop and proceed to shading
                        break;
                }

                // Ray march!
                pos += stepSize*dir;
        }
        // If we reached the end of the loop "naturally" then we didnt hit anything, discard fragment
        if (i==nIterations)
                discard;

        // proceed to shading
        vec3 I = normalize(cam-pos);
        vec3 N = normalize(norm);
        float LdotN = max(dot(I, N),0.0);
        gl_FragColor = vec4(clamp((diffColour * LdotN)*0.8+0.2*diffColour ,0.0,1.0),1);

}

