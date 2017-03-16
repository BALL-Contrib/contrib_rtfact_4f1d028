/*	plastic surface shader
 *
 * 	Pixie is:
 * 	(c) Copyright 1999-2003 Okan Arikan. All rights reserved.
 */

surface plastic (float Ka = 1, Kd = 0.5, Ks = 0.5, roughness = 0.1;
                 color specularcolor = 1;
				 color diffusecolor = color(1.0,0.2,0.2);) {
	normal Nf = faceforward (normalize(N),I);
	//Ci = Cs * (Ka*ambient() + Kd*diffuse(Nf)) + specularcolor * Ks*specular(Nf,-normalize(I),roughness);

	//version which does not use surface color:
	//Ci = col * (Ka*ambient() + Kd*diffuse(Nf));// + specularcolor * Ks*color(0.3,0.3,0.3);
	Ci = diffusecolor * (Ka*ambient() + Kd*diffuse(Nf)) + specularcolor * Ks*specular(Nf,-normalize(I),roughness);

	//Oi = Os;
	//Ci *= Oi;
}

