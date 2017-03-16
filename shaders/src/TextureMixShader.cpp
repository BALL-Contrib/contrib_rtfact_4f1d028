#include "RTfactShader.hpp"

PARAMETER Texture texture1;
PARAMETER Texture texture2;
PARAMETER float alpha;

BEGIN_INIT_PARAM
INIT_PARAM(texture1, NO_TEXTURE)
INIT_PARAM(texture2, NO_TEXTURE)
INIT_PARAM(alpha, 0.5f)
END_INIT_PARAM

SURFACE_SHADER
{
	Color c1 = SampleTexture(texture1);
    Color c2 = SampleTexture(texture2);
    //result = Mix(c1, c2, alpha);
	
	const float t = (Mod(Time, 10000)) / 10000.0;
	result = Mix (c1, c2, t);
}
