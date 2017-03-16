surface specular_s()
{
	normal Nf = faceforward(normalize(N),I);
	
	//illuminance test (specular using specular-statement)
	vector V = -normalize(I);
	Ci = color(1,1,1) * specular(Nf, V, .1);
	

	//illuminance test (specular inlined)
	/*vector V = -normalize(I);
	color S = color(0,0,0);
	illuminance( P, Nf, PI/2 ) {
		vector H = normalize(L+V);
		S += Cl * pow (max (0, Nf.H), 1/.1);
	}
	Ci = color(1,1,1) * S;
	*/
	
	//illuminance test (specular inlined using specularbrdf)
	/*vector V = -normalize(I);
	Ci = color(0,0,0);
	illuminance( P, Nf, PI/2 ) {
		Ci += color(1,0,0) * Cl * specularbrdf (normalize(L), Nf, V, .1);
	}*/
}

