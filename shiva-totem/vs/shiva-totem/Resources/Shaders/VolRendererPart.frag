
uniform sampler3D CacheTexture0;
uniform sampler3D CacheTexture1;
uniform sampler3D CacheTexture2;
uniform sampler3D CacheTexture3;

varying vec3 viewSpacePos;
varying vec3 worldSpacePos;
varying vec3 worldSpaceCam;

uniform vec3 fragboundmin;
uniform vec3 fragboundmax;
uniform vec4 parameters;
uniform vec3 objectcolour = vec3( 0.88, 0.78, 0.54 );
uniform float stepsize = 0.03;
uniform float gradDelta = 0.001;

//----------------------------------------------------------------------------------

// From Mat's code
vec3 worldToTextureCoords( vec3 pos )
{
    vec3 size = fragboundmax - fragboundmin;
    vec3 mixVal = ( pos - fragboundmin ) / size;
    return mixVal;
}

//----------------------------------------------------------------------------------

float DistPointToUnitAABB( vec3 samplePosition )
{
	// convert coord so cube is two unit cube, centre origin
	if( samplePosition.x <= 0 )
		samplePosition.x -= 1;
	if( samplePosition.y <= 0 )
		samplePosition.y -= 1;
	if( samplePosition.z <= 0 )
		samplePosition.z -= 1;

	// Mirror coords so we only have to deal with one octant
	samplePosition = abs( samplePosition );// + 0.001;
	
	if( samplePosition.x > 1 )
	{
		if( samplePosition.y > 1 )
		{
			if( samplePosition.z > 1 )
			{
				// Length to corner point
				return length( samplePosition - vec3( 1 ) );
			}
			else
			{
				return length( samplePosition.xy - vec2( 1 ) );
			}
		}
		else
		{
			if( samplePosition.z > 1 )
			{
				return length( samplePosition.xz - vec2( 1 ) );
			}
			else
			{
				return samplePosition.x - 1;
			}
		}
	}
	else
	{
		if( samplePosition.y > 1 )
		{
			if( samplePosition.z > 1 )
			{
				return length( samplePosition.yz - vec2( 1 ) );
			}
			else
			{
				return samplePosition.y - 1;
			}
		}
		else
		{
			if( samplePosition.z > 1 )
			{
				return samplePosition.z - 1;
			}
			else
			{
				// Is inside
				//return samplePosition.xyz - 1;
			}
		}
	}
	//return length( samplePosition );
	return -9999.9;
}

//----------------------------------------------------------------------------------

float Sphere( vec3 samplePosition, vec3 radius )
{
	//return radius - sqrt( pow(samplePosition.x-centre.x,2) + pow(samplePosition.y-centre.y,2) + pow(samplePosition.z-centre.z,2) );
	// Oleg's code:
	//vec3 vecDiff = samplePosition - centre;
	//return radius*radius - dot(vecDiff, vecDiff);
	
	vec3 vecDiff = samplePosition / radius;
	return 1.0 - dot( vecDiff, vecDiff );
}

//----------------------------------------------------------------------------------

float CSG_Union( float f1, float f2 )
{
	// AMD graphics cards cannot handle sqrt
	//return max(f1,f2);
	//double a = (f1*f1)+(f2*f2);
	//double b = sqrt( a );
	//return f1 + f2;//f2;
	//return (f1+f2)+sqrt( (f1*f1)+(f2*f2) );
	//return f1+f2+sqrt( pow(f1,2)+pow(f2,2));

	return f1 + f2 + length( vec2( f1, f2 ) );
}

//----------------------------------------------------------------------------------

float CSG_Subtract( float f1, float f2 )
{
	// AMD graphics cards cannot handle sqrt
	return f1 - f2 - length( vec2( f1, f2 ) );
}

//----------------------------------------------------------------------------------

float CSG_Intersect( float f1, float f2 )
{
	//return min(f1,f2);
	return f1 + f2 - length( vec2( f1, f2 ) ); //sqrt( pow(f1,2)+pow(f2,2));
}

//----------------------------------------------------------------------------------

float BlendDisp( float f1, float f2, float a0, float a1, float a2 )
{
	//return -0.1f*(f1+f2);
	//return a0 / (1+pow(f1/a1,2) + pow(f2/a2,2));
	return ( a0 ) / ( 1 + ( ( f1 / a1 ) * ( f1 / a1 ) ) + ( ( f2 / a2 ) * ( f2 / a2 ) ) );
}

//----------------------------------------------------------------------------------

float BlendCSG_Union( float f1, float f2, float a0, float a1, float a2 )
{
	return CSG_Union( f1, f2 ) + BlendDisp( f1, f2, a0, a1, a2 );
}

//----------------------------------------------------------------------------------

float BlendCSG_Subtract( float f1, float f2, float a0, float a1, float a2 )
{
	return CSG_Subtract( f1, f2 ) + BlendDisp( f1, f2, a0, a1, a2 );
}

//----------------------------------------------------------------------------------

float BlendCSG_Intersect( float f1, float f2, float a0, float a1, float a2 )
{
	return CSG_Intersect( f1, f2 ) + BlendDisp( f1, f2, a0, a1, a2 );
}

//----------------------------------------------------------------------------------

float Cylinder( vec3 samplePosition, float length, float radiusX, float radiusY )
{

    float lowerZ = -length * 0.5;
    float upperZ = length * 0.5;
    vec2 diff = vec2( samplePosition.x / radiusX, samplePosition.y / radiusY );
    float value = 1.0 - dot( diff, diff );
    //float value = 1.0 - pow(samplePosition.x/radiusX,2) - pow(samplePosition.y/radiusY,2);//dot(samplePosition.xy/radiusX, samplePosition.xy/radiusY);
    value = CSG_Intersect( value, samplePosition.z - lowerZ );
    value = CSG_Intersect( value, upperZ - samplePosition.z );
    return value;
    
/*
	float lowerZ = -length*0.5;
	float upperZ = length*0.5;
	samplePosition.xy = samplePosition.xy / radius;
	float value = 1.0 - dot(samplePosition.xy, samplePosition.xy);
//	float value = radius*radius - dot(samplePosition.xy, samplePosition.xy);

	value = CSG_Intersect( value, samplePosition.z - lowerZ );
	value = CSG_Intersect( value, upperZ - samplePosition.z );

	return value;
*/
}

//----------------------------------------------------------------------------------

float Cone( vec3 samplePosition, float length, float radius )
{
	float lowerZ = -length * 0.5;
	float upperZ = length * 0.5;
//	radius = radius * (1.0/length);
	float value = ( samplePosition.z - upperZ ) * ( samplePosition.z - upperZ ) - dot( samplePosition.xy * length / radius, samplePosition.xy * length / radius );
	
	value = CSG_Intersect( value, upperZ - samplePosition.z );
	value = CSG_Intersect( value, samplePosition.z - lowerZ );

	return value;
}

//----------------------------------------------------------------------------------

float Cube( vec3 samplePosition, vec3 length )
{
	vec3 lowerBounds = -length * 0.5;
	vec3 upperBounds = length * 0.5;
	
	float value = CSG_Intersect( upperBounds.z - samplePosition.z, samplePosition.z - lowerBounds.z );
	value = CSG_Intersect( value, upperBounds.y - samplePosition.y );
	value = CSG_Intersect( value, samplePosition.y - lowerBounds.y );
	value = CSG_Intersect( value, upperBounds.x - samplePosition.x );
	value = CSG_Intersect( value, samplePosition.x - lowerBounds.x );
	
	return value;
}

//----------------------------------------------------------------------------------

float Torus( vec3 samplePosition, float circleRadius, float sweepRadius )
{
	return ( circleRadius * circleRadius ) - dot( samplePosition, samplePosition ) - ( sweepRadius * sweepRadius ) + 2.0 * sweepRadius * sqrt( dot( samplePosition.xy, samplePosition.xy ) );
	
	// 'normalised' version
	//return circleRadius - sqrt(dot(samplePosition, samplePosition)+(sweepRadius*sweepRadius)-2.0*sweepRadius*sqrt( dot(samplePosition.xz,samplePosition.xz) ));
}

//----------------------------------------------------------------------------------

float Cache( vec3 samplePosition, sampler3D tex, vec3 posOffset, vec3 scaleOffset )
{
	vec3 sampleCoords = ( samplePosition + posOffset ) * scaleOffset + vec3( 0.5 );
	if( all( greaterThan( sampleCoords, vec3( -0.0001 ) ) ) && all( lessThan( sampleCoords, vec3( 1.0001 ) ) ) )
	{
		float value = texture3D( tex, sampleCoords ).a;
		if( value < -9999 )
			return value * 0.001;
		return value;
	}
	else
	{
		//return -999.0;
		
		float dist = DistPointToUnitAABB( sampleCoords );//length(sampleCoords);
		float value = texture3D( tex, sampleCoords ).a;
		value = -abs( value );
		return value - abs( dist );
	}
}

//----------------------------------------------------------------------------------

vec3 Translate( vec3 samplePosition, vec3 offset )
{
	return samplePosition + offset;
}

//----------------------------------------------------------------------------------

vec3 Transform( vec3 samplePosition, mat4 transMatrix )
{
	return ( transMatrix * vec4( samplePosition, 1 ) ).xyz;
}

//----------------------------------------------------------------------------------

float GetField( vec3 samplePosition );
//float GetField(vec3 samplePosition) { return CSG_Union(Cylinder(Translate(samplePosition,vec3(0,0,0.5)),1,0.05),Cylinder(Translate(samplePosition,vec3(0,0,-0.05)),0.1,0.5));}
//float GetField(vec3 samplePosition) { return Cylinder(Translate(samplePosition,vec3(0,0,-0.05)),0.1,0.5); }

//----------------------------------------------------------------------------------

vec3 bsearch( vec3 previous, vec3 current )
{
        float isoValue = 0.0;
        float multiplier = 1.0;

        vec3 begin = previous;
        vec3 end = current;

        for( int i = 0; i < 12; i++ )
        {
                vec3 midPos = ( begin + end ) * 0.5;
                //vec3 samplePos = worldToTextureCoords(midPos);
                float voxel = GetField( midPos );

                if ( voxel * multiplier < isoValue * multiplier )
                        begin = midPos;
                else
                        end = midPos;
        }

        return ( begin + end ) * 0.5; //worldToTextureCoords((begin+end)*0.5);
}

//----------------------------------------------------------------------------------

vec3 getGradient( vec3 pos )
{
    float delta = gradDelta;//0.001f;
    vec3 ret;
    ret.x = GetField( pos + vec3( delta, 0.0, 0.0 ) ) - GetField( pos - vec3( delta, 0.0, 0.0 ) );
    ret.y = GetField( pos + vec3( 0.0, delta, 0.0 ) ) - GetField( pos - vec3( 0.0, delta, 0.0 ) );
    ret.z = GetField( pos + vec3( 0.0, 0.0, delta ) ) - GetField( pos - vec3( 0.0, 0.0, delta ) );

    return normalize( -ret );
}

//----------------------------------------------------------------------------------

bool CheckInBounds( vec3 pos )
{
	return ( pos.x < fragboundmax.x ) && ( pos.y < fragboundmax.y ) && ( pos.z < fragboundmax.z ) && ( pos.x > fragboundmin.x ) && ( pos.y > fragboundmin.y ) && ( pos.z > fragboundmin.z );
}

//----------------------------------------------------------------------------------

vec3 fieldColor( vec3 G )
{
	return G * 0.5 + 0.5;
}

//----------------------------------------------------------------------------------

void main ()
{

	// Mat's code (reduced)

    vec3 cam = worldSpaceCam;
    vec3 pos = worldSpacePos;
    vec3 dir = normalize( worldSpacePos - cam );
	// pos += dir * stepsize * 0.5; // Make sure we dont miss straight away
    vec3 norm = vec3( 0, 0, 1 );
    
	int nIterations = 384; //256;//128;
    int i = 0;
    bool inBounds = true;
	
    // Begin ray marching, keep i outside to check if we hit or not
    for( i = 0; i < nIterations; i++ )
    {
 		float functionValue = GetField( pos );
 		
 		if( functionValue > 0.0f )
 		{
 			pos = bsearch( pos - stepsize * dir, pos );
 			//norm = getGradient(pos);
 			//gl_FragColor = vec4(1,0,0,0);
 			//return;
 			break;
 		}
 		else
 		{
			// Ray march!
			pos += stepsize * dir;
			inBounds = CheckInBounds( pos );
			if( !inBounds )
			{
    			break;
			}
		}
    }
    // If we reached the end of the loop "naturally" then we didnt hit anything, discard fragment
    if( i == nIterations || !inBounds )
            discard;


		
    // Proceed to shading
    norm = getGradient( pos ); // Need to get the normal here, it goes wrong otherwise
    vec3 I = normalize( cam - pos );
    vec3 N = normalize( norm );
    float LdotN = max( dot( I, N ), 0.0 );
    gl_FragColor = vec4( clamp( ( objectcolour * LdotN ) * 0.9 + 0.1, 0.0, 1.0 ), 1 );
    
	
	//gl_FragColor = vec4(clamp((vec3(0.88,0.78,0.54) * LdotN)*0.9+0.1,0.0,1.0),1);
    //gl_FragColor = vec4(fieldColor(N),1);
	//gl_FragColor = vec4(1,0,0,1);
	
	// Depth calculation code from: 
	// http://stackoverflow.com/questions/10264949/glsl-gl-fragcoord-z-calculation-and-setting-gl-fragdepth
	
	float far = gl_DepthRange.far; 
	float near = gl_DepthRange.near;

	vec4 eye_space_pos = gl_ModelViewMatrix * vec4( pos, 1 );
	vec4 clip_space_pos = gl_ProjectionMatrix * eye_space_pos;

	float ndc_depth = clip_space_pos.z / clip_space_pos.w;

	float depth = ( ( ( far - near ) * ndc_depth ) + near + far) / 2.0;
	gl_FragDepth = depth;
		
}
