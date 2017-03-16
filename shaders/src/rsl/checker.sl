/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.345 */
/* Listing 16.12  Surface shader providing checkerboard pattern */

/*
 * checker(): surface shader for applying a checkerboard pattern.
 * NOTE: slightly modified version using parameter whitecolor instead of Cs
 */
surface
checker (
	float Kd = .5,
	      Ka = .1,
		  frequency = 5; //10;
	color blackcolor = color (0, 0, 0);
	color whitecolor = color (1, 1, 1); )
{
	float smod = mod (s* frequency, 1),
	      tmod = mod (t* frequency, 1);

	if (smod < 0.5) {
		if (tmod < 0.5)
			Ci = whitecolor; //Cs;
		else
			Ci = blackcolor;
	}
	else {
		if (tmod < 0.5)
			Ci = blackcolor;
		else
			Ci = whitecolor; //Cs;
	}
	
	Oi = Os;
	Ci = Oi * Ci *
	 (Ka * ambient() +
	  Kd * diffuse (faceforward (normalize (N), I) ) );
}

	
