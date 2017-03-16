surface mirror()
{
	normal Nf = faceforward(normalize(N),I);
	
	//mirror
	vector In = normalize(I);
	vector reflDir = reflect(In, Nf);
	Ci = trace(P,reflDir);
}

