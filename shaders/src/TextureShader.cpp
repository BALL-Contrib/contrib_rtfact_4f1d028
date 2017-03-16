#include "RTfactShader.hpp"

PARAMETER Texture texture1;

BEGIN_INIT_PARAM
INIT_PARAM(texture1, NO_TEXTURE)
END_INIT_PARAM

SURFACE_SHADER
{
	//3 equivalent ways to sample the same texture point
	//result = SampleTextureOffset(texture1, texCoord1, texCoord2, 0.f, 0.f);
	//result = SampleTextureCoord(texture1, texCoord1, texCoord2);
	result = SampleTexture(texture1);
}
