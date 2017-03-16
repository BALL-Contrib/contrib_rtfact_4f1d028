surface texture_mix(
	float alpha = 0.5
)
{
	//mixed texture
	color c1 = color texture("texture1");
	color c2 = color texture("texture2");
	//Ci = mix(c1, c2, alpha);

	//use time
	float t = (mod(time, 10000)) / 10000.0;
	Ci = mix (c1, c2, t);
}
