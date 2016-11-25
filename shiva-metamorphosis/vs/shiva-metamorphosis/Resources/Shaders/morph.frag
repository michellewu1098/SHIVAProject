#version 120

uniform sampler3D volume1;
uniform sampler3D volume2;

varying vec3 viewSpacePos;
varying vec3 worldSpacePos;
varying vec3 worldSpaceCam;

uniform vec3 fragboundmin;
uniform vec3 fragboundmax;

uniform vec4 inputColour;

uniform float a0;
uniform float a1;
uniform float a2;
uniform float a3;
uniform float time;
uniform float clipdist;
uniform float timegap;

vec3 rgb_to_hsv(vec3 rgb)
{
    float maxval = max(rgb.r,max(rgb.g,rgb.b));
    float minval = min(rgb.r,min(rgb.g,rgb.b));

    float v = maxval;
    float delta = maxval-minval;

    float s = 0;
    if (maxval!=0)
        s = delta/maxval;
    else
        return vec3(-1,0,0);

    float h = 0;
    if (rgb.r==maxval)
        h = (rgb.g-rgb.b)/delta;
    else if (rgb.g==maxval)
        h = 2 + (rgb.b-rgb.r)/delta;
    else if (rgb.b==maxval)
        h = 4 + (rgb.r-rgb.g)/delta;
    h*=60;
    if(h<0)
        h+=360;

    return vec3(h,s,v);
}

vec3 hsv_to_rgb(vec3 hsv)
{
    vec3 rgb;
    if(hsv.g==0)
    {
        rgb.r=rgb.g=rgb.b=hsv.b;
        return rgb;
    }
    hsv.r /= 60;			// sector 0 to 5
    float i = floor(hsv.r);
    float f = hsv.r - i;			// factorial part of h
    float p = hsv.b * ( 1 - hsv.g );
    float q = hsv.b * ( 1 - hsv.g * f );
    float t = hsv.b * ( 1 - hsv.g * ( 1 - f ) );
    int ii = int(i);
    switch(ii)
    {
    case 0:
        rgb.r = hsv.b;
        rgb.g = t;
        rgb.b = p;
        break;
    case 1:
        rgb.r = q;
        rgb.g = hsv.b;
        rgb.b = p;
        break;
    case 2:
        rgb.r = p;
        rgb.g = hsv.b;
        rgb.b = t;
        break;
    case 3:
        rgb.r = p;
        rgb.g = q;
        rgb.b = hsv.b;
        break;
    case 4:
        rgb.r = t;
        rgb.g = p;
        rgb.b = hsv.b;
        break;
    case 5:
        rgb.r = hsv.b;
        rgb.g = p;
        rgb.b = q;
        break;
    }
    return rgb;
}

vec3 rgb_to_ryb(vec3 rgb)
{
    float w = min(rgb.r,min(rgb.g,rgb.b));
    float r = rgb.r - w;
    float g = rgb.g - w;
    float b = rgb.b - w;

    float mg = max(max(r,g),b);

    float y = min(r,g);
    r-=y;
    g-=y;

    if (b!=0.0 && g!=0.0)
    {
        b*=0.5;
        g*=0.5;
    }
    y+=g;
    b+=g;
    float my = max(r,max(y,b));
    if (my!=0.0)
    {
        float n = mg/my;
        r*=n;
        y*=n;
        b*=n;
    }

    r+=w;
    y+=w;
    b+=w;

    return vec3(r,y,b);
}

vec3 ryb_to_rgb(vec3 ryb)
{
    float w = min(ryb.r,min(ryb.g,ryb.b));
    float r = ryb.r - w;
    float y = ryb.g - w;
    float b = ryb.b - w;

    float my = max(r,max(y,b));

    float g = min(y,b);
    y -= g;
    b -= g;

    if (b!=0.0 && g!=0.0)
    {
        b*=2.0;
        g*=2.0;
    }
    r+=y;
    g+=y;
    float mg = max(r,max(g,b));
    if(mg!=0.0)
    {
        float n = my / mg;
        r*=n;
        g*=n;
        b*=n;
    }
    r+=w;
    g+=w;
    b+=w;
    return vec3(r,g,b);
}


vec3 rgb_blend_colours(vec3 C1, vec3 C2, float w1, float w2)
{
    return C1*w1 + C2*w2;
}

vec3 hsv_blend_colours(vec3 C1, vec3 C2, float w1, float w2)
{
    vec3 hsv1 = rgb_to_hsv(C1);
    vec3 hsv2 = rgb_to_hsv(C2);
    vec3 hsvout = hsv1*w1 + hsv2*w2;
    return hsv_to_rgb(hsvout);
}

vec3 rgbsv_blend_colours(vec3 C1, vec3 C2, float w1, float w2)
{
    vec3 hsv1 = rgb_to_hsv(C1);
    vec3 hsv2 = rgb_to_hsv(C2);
    vec3 hsvi = hsv1*w1 + hsv2*w2;
    vec3 rgbi = rgb_blend_colours(C1,C2,w1,w2);
    vec3 rgbi2 = rgb_to_hsv(rgbi);
    vec3 hsvfinal = vec3(rgbi2.r,hsvi.g,hsvi.b);
    return hsv_to_rgb(hsvfinal);
}

vec3 blend_colours(vec3 C1, vec3 C2, float w1, float w2)
{
    return vec3(1.0);//rgbsv_blend_colours(C1,C2,w1,w2);
}

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

float getField(vec3 texcoords)
{
    float t = time;
    float va = texture3D(volume1, texcoords).a;
    float vb = texture3D(volume2, texcoords).a;

	return (t*vb + (1-t)*va);

/*
    float midrange = 0.5f; // Middle of time range

    float bounddist = 0.5f; // The bounding blend distance (will go both sides of the mid range)

    float plane1 = +t - midrange - clipdist; // Cutting plane for source object
    float plane2 = -t + midrange - clipdist; // Cutting plane for target object

    // Could use blended intersection here
    float f1 = intersection_c1(va,plane1); // Source object half cylinder
    float f2 = intersection_c1(vb,plane2); // Target object half cylinder

    float boundplane1 = +t - midrange - bounddist;
    float boundplane2 = -t + midrange - bounddist;

    float f3 = -union_c1(boundplane1,boundplane2); // This is the time slab in which bounded blending can happen

    float r1sq = (f1/a1)*(f1/a1)+(f2/a2)*(f2/a2);
    float r2sq = 0.f;

    if (f3>0.f)
            r2sq = (f3/a3)*(f3/a3);

    float rsq = r1sq/(r1sq+r2sq);

    float d = 0.f; // Normalized displacement (will be scaled by a0)

    if (rsq<1.f)
    {
            float midterm = 1.f-rsq;
            d = midterm*midterm*midterm/(1.f+rsq);
    }

    float fin = union_c1(f1,f2)+a0*d; // Blended union, blend is scaled by a0

    return -fin;
*/
}

vec3 bsearch(vec3 previous, vec3 current)
{
        float isoValue = 0.0;
        float multiplier = -1.0;

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
        float delta = 0.01*2.0;
        vec3 ret;
        ret.x = getField(pos+vec3(delta,0.0,0.0)) - getField(pos-vec3(delta,0.0,0.0));
        ret.y = getField(pos+vec3(0.0,delta,0.0)) - getField(pos-vec3(0.0,delta,0.0));
        ret.z = getField(pos+vec3(0.0,0.0,delta)) - getField(pos-vec3(0.0,0.0,delta));

        return normalize(ret);
}

vec3 first_colour(vec3 pos)
{
    vec3 col = vec3(1,0,0);
    return col;
}

vec3 second_colour(vec3 pos)
{
    vec3 col = vec3(0,0,1);
    return col;
}

vec3 fieldColour(vec3 pos, float m1, float m2)
{
    float alpha = time;
    vec3 C1 = first_colour(pos);
    vec3 C2 = second_colour(pos);

    float w1 = 0;
    float w2 = 0;

    float f1 = intersection_c1(m1,(-alpha)*timegap);
    float f2 = intersection_c1(m2,(alpha-1.0)*timegap);

    float fsum = f1+f2;
    w1 = f2/fsum;
    w2 = f1/fsum;
    
    vec3 blendedColours = blend_colours(C1,C2,w1,w2);

    return (1.0-inputColour.w)*blendedColours + (inputColour.w)*vec3(inputColour);
}

void main ()
{
        float isoValue = 0.0;
        int nIterations = 2048;
        float stepSize = 0.01;
        float multiplier = -1.0;


        vec3 cam = worldSpaceCam;
        vec3 pos = worldSpacePos;
        vec3 dir = normalize(worldSpacePos-cam);
        pos+=dir*stepSize; // Make sure we dont miss straight away
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

        vec3 diffColour=fieldColour(pos,-texture3D(volume1, texCoord).r,-texture3D(volume2, texCoord).r);
        // proceed to shading
        vec3 I = normalize(cam-pos);
        vec3 N = normalize(norm);
        float LdotN = max(dot(I, N),0.0);
        gl_FragColor = vec4(clamp((diffColour * LdotN)*0.8+0.2*diffColour ,0.0,1.0),1);

}
