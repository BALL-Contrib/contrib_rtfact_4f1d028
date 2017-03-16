surface checker_aa(
		float repeatCount = 40;
		float Ka = 1;
		float Kd = .5;
)
{
	float ss= s*repeatCount + t;
	float tt= t*repeatCount - s;
	float filterWidthSS=abs(Du(ss)*du)+abs(Dv(ss)*dv);
	float filterWidthTT=abs(Du(tt)*du)+abs(Dv(tt)*dv);
	float smag,tmag;
	ss=mod(ss,2);
	tt=mod(tt,2);
	smag =smoothstep(0.5-filterWidthSS,0.5 +filterWidthSS,ss);
	smag -=smoothstep(1.5-filterWidthSS,1.5 +filterWidthSS,ss);
	smag = smag*2-1;
	smag *= 1-smoothstep(0.4,1.2,filterWidthSS);

	tmag = smoothstep(0.5 -filterWidthTT,0.5 +filterWidthTT,tt);
	tmag -= smoothstep(1.5 - filterWidthTT,1.5 +filterWidthTT,tt);
	tmag = tmag*2-1;
	tmag *= 1-smoothstep(0.4,1.2,filterWidthTT);

	color Ct =(smag*tmag)/2+0.5;

	normal Nf = normalize(faceforward(N, I));

	Oi = Os;
	Ci = Oi * Ci *
	 (Ka * ambient() +
	  Kd * diffuse (faceforward (normalize (N), I) ) );
}

