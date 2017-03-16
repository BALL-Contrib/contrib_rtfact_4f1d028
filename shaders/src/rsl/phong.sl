surface phong(
//	float Ka = 1, Kd =1, Ks = 0.5;
//	float roughness = 0.1;
//	color specularcolor = 1;
//	color diffusecolor = color(1, 0.2, 0);
//	color ambientIntensity = color(0.1, 0.3, 0.4);
//	float shininess = 50.f;
)
{
//	normal Nf = faceforward( normalize(N), I );
//	vector V = -normalize(I);
//	color C = 0;
//	illuminance( P, Nf, PI/2 ) {
//		vector R = 2*normalize(N)*
//			(normalize(N) . normalize( L )) -
//			normalize( L );
//		C += Ka*Cc + 
//			Kd*Cc*( normalize(N) . normalize(L) ) +
//			Ks*specularcolor* pow(( R . V ), 10);
//	}
//	Ci = C*Cc;

	normal Nf = faceforward (normalize(N),I);
    vector reflView = reflect(I, Nf);

    color tmp = color(0.1, 0.06, 0.0);

    illuminance(P, Nf, PI/2) {
        color c = color (0, 0, 0);
        float dotLightRefl = L.reflView;
        if (dotLightRefl > 0.01) c = pow(dotLightRefl, 50.0);
        float f = Nf.L;
        tmp += color(1.0, 0.2, 0.0) * Cl * f + c;
    }

    Ci = tmp;
}
