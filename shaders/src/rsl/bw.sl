surface bw()
{
	normal Nf = faceforward(normalize(N),I);
	
	//illuminance test (b/w)
	Ci = color(0,0,0);
	illuminance(P, Nf, PI/2) {
		Ci = color(1,1,1);
	}
}

