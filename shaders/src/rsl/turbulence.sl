surface turbulence (
	float Kd=.8, Ka=.2;
)
{
	float a, scale, sum ;
	float IdotN;
	point M; /* convert to texture coordinate system */
	M = P; //M = transform( "marble", P );
	scale = 1;
	sum = 0;
	a = sqrt(area(M));

	while( a < scale ) {
		sum += scale * float noise(M/scale);
		scale *= 0.5;
	}

	Oi = sum;
	Ci = Cs * Oi * (Ka + Kd * I.N * I.N / (I.I * N.N) );
}
