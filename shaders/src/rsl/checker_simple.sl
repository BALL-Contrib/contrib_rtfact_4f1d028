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
	float repeatCount=5;
	float sTile=floor(s*repeatCount);
	float tTile=floor(t*repeatCount);
	Ci= mod(sTile +tTile,2);
}

	
