surface siggraph10_tv(
	color c = color(1,1,1);
)
{
	float tt = t * 1.3;

	color c1 = color texture("texture1",s,tt);

	color n1 = noise(P*100+vector(time,0,0));

	float trans_t = time*0.0002;
	float s1 = s + sin(t*100)*0.15 * sin(t*500)*t*0.2 + t*0.1 - 0.05;
	float channelstrength = 0.02;
	color cr = color texture("texture1",s1,t+trans_t);
	color cg = color texture("texture1",s1 + channelstrength*sin(time*0.01),t+trans_t+ channelstrength*sin(time*0.01));
	color cb = color texture("texture1",s1 + channelstrength*sin(time*0.02),t+trans_t- channelstrength*sin(time*0.02));

	color c2 = color( xcomp(cr), ycomp(cg), zcomp(cb) ) + n1*0.2;
	
	float time1 = mod( time*0.0005, 4 );

	if( time1 < 1 )
	{
		float time2 = mod( time1, 1 );
		time2*=2;
		time2=clamp(time2,0,1);
		Ci = mix( c1, n1, time2);
	}
	else if( time1 < 2 )
	{
		float time2 = mod( time1, 1 );
		time2*=2;
		time2=clamp(time2,0,1);
		Ci = mix( n1, c2, time2);
	}
	else if( time1 < 3 )
	{
		float time2 = mod( time1, 1 );
		time2*=2;
		time2=clamp(time2,0,1);
		Ci = mix( c2, n1, time2);
	}
	else
	{
		float time2 = mod( time1, 1 );
		time2*=2;
		time2=clamp(time2,0,1);
		Ci = mix( n1, c1, time2);
	}
        
	//Ci = mix( c1, n1, sin(time*0.001)*0.5+0.5);
	//Ci = time1;
}
