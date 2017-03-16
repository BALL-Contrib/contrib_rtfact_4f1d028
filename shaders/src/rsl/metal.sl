surface metal(
	float Ka = 1;
	float Ks = 1;
	float roughness = .1;
	color Cc = color(0.2,0.8,0.8); //replacement for Cs (surface color)
)
{
	normal Nf = faceforward(normalize(N), I);
	vector V = -normalize(I);
	Oi = Os;
	Ci = Os * Cc * (Ka*ambient()+Ks*specular(Nf, V, roughness));
}
