

void main(void)
{
	
	float H = gl_TexCoord[0].x;
	float S = 1.0;
	float L = gl_TexCoord[0].y;
	
	/*
	float temp2 = L*(1.0+S);
	if( L >= 0.5f )
		temp2 = L+S - L*S;
		
	float temp1 = 2.0*L - temp2;
	
	float temp3R = H + (1.0/3.0);
	float temp3G = H;
	float temp3B = H - (1.0/3.0);
	
	if( temp3R < 0.0 )
		temp3R += 1.0;
	else if( temp3R > 1.0 )
		temp3R -= 1.0;
		
	if( temp3G < 0.0 )
		temp3G += 1.0;
	else if( temp3G > 1.0 )
		temp3G -= 1.0;
	
	if( temp3B < 0.0 )
		temp3B += 1.0;
	else if( temp3B > 1.0 )
		temp3B-= 1.f;
	
	
	float R,G,B;
	
	if( 6.0*temp3R < 1.0 )
	{
		R = temp1 + (temp2-temp1) * 6.0 * temp3R;
	}
	else if( 2.0*temp3R < 1.0 )
	{
		R = temp1 + (temp2-temp2)*((2.0/3.0)-temp3R)*6.0;
	}
	
	gl_FragColor = gl_Color;
	*/
	
	/// This code is kinda from Bob Powell (http://www.bobpowell.net/RGBHSB.htm)
	/// But he got it from Foley and van Dam
	/// See also http://130.113.54.154/~monger/hsl-rgb.html
	/// (it's the exact same thing, variables same names etc)
	
	float temp2 = ((L<=0.5) ? L*(1.0+S) : L+S-(L*S));
	float temp1 = 2.0*L-temp2;

	float[3] temp3;
	temp3[0] = H+1.0/3.0f;
	temp3[1] = H;
	temp3[2] = H-1.0/3.0f;

	float[3] colour;
	colour[0] = 0.0;
	colour[1] = 0.0;
	colour[2] = 0.0;

	for(int i=0; i<3; i++)
	{

		if( temp3[i] < 0 )
			temp3[i] += 1.0;
		else if( temp3[i] > 1 )
			temp3[i] -= 1.0;

		if( 6.0*temp3[i] < 1.0 )
			colour[i] = temp1+(temp2-temp1)*temp3[i]*6.0;
		else if( 2.0*temp3[i] < 1.0 )
			colour[i] = temp2;
		else if( 3.0*temp3[i] < 2.0 )
			colour[i] = (temp1+(temp2-temp1)*((2.0/3.0)-temp3[i])*6.0);
		else
			colour[i] = temp1;
	}
	
	gl_FragColor = vec4(colour[0], colour[1], colour[2], 1.0f);//gl_Color;
	
}
