surface diffuse_s()
{
	normal Nf = faceforward(normalize(N),I);
	
	//illuminance test (diffuse using diffuse-statement)
	Ci = color(1,0,0) * diffuse(Nf);
	
	//TESTS
	
	//time test
	//Ci = color (time/10000, 0, 0);
	//Ci = color (1, 1, 0);


	//illuminance test (b/w)
	/*Ci = color(0,0,0);
	illuminance(P, Nf, PI/2) {
		Ci = color(1,1,1);
	}*/
	
	//illuminance test (lambertian/diffuse inlined)
	/*color D = color(0,0,0);
	illuminance( P, Nf, PI/2 ) {
		D += Cl * normalize(L).Nf;
	}
	Ci = color(1,0,0) * D;
	*/
	
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

	//illuminance test (specular using specular-statement)
	//vector V = -normalize(I);
	//Ci = color(1,1,1) * specular(Nf, V, .1);
	

	//illuminance test (phong inlined)
	/*vector V = -normalize(I);
	vector R = reflect( -normalize(V), normalize(N) );
	Ci = color(0,0,0);
	illuminance( P, Nf, PI/2 ) {
		vector Ln = normalize(L);
		Ci += color(1,0,0) * Cl * pow(max(0.0,R.Ln), 3);
	}*/

	//illuminance test (phong using phong-statement)
	//vector V = -normalize(I);
	//Ci = color(1,0,0) * phong(Nf, V, 3);
}

