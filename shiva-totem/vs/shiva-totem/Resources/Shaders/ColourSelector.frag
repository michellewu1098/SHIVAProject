#version 330

in vec2 o_Uvs;
out vec4 fragColour;

void main(void)
{
	float H = o_Uvs.x;
	float S = 1.0;
	float L = o_Uvs.y;
		
	/// This code is kinda from Bob Powell (http://www.bobpowell.net/RGBHSB.htm)
	/// But he got it from Foley and van Dam
	/// See also http://130.113.54.154/~monger/hsl-rgb.html
	/// (it's the exact same thing, variables same names etc)
	
	float temp2 = ( ( L <= 0.5 ) ? L * ( 1.0 + S ) : L + S -( L * S ) );
	float temp1 = 2.0 * L - temp2;

	float[ 3 ] temp3;
	temp3[ 0 ] = H + 1.0 / 3.0f;
	temp3[ 1 ] = H;
	temp3[ 2 ] = H - 1.0 / 3.0f;

	float[ 3 ] colour;
	colour[ 0 ] = 0.0;
	colour[ 1 ] = 0.0;
	colour[ 2 ] = 0.0;

	for( int i = 0; i < 3; i++ )
	{
		if( temp3[ i ] < 0 )
			temp3[ i ] += 1.0;
		else if( temp3[ i ] > 1 )
			temp3[ i ] -= 1.0;

		if( 6.0 * temp3[ i ] < 1.0 )
			colour[ i ] = temp1 + ( temp2 - temp1 ) * temp3[ i ] * 6.0;
		else if( 2.0 * temp3[ i ] < 1.0 )
			colour[ i ] = temp2;
		else if( 3.0 * temp3[ i ] < 2.0 )
			colour[ i ] = ( temp1 + ( temp2 - temp1 ) * ( ( 2.0 / 3.0 ) - temp3[ i ] ) * 6.0 );
		else
			colour[ i ] = temp1;
	}
	
	fragColour = vec4( colour[ 0 ], colour[ 1 ], colour[ 2 ], 1.0f );
}
