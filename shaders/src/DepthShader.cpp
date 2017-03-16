#include "RTfactShader.hpp"

SURFACE_SHADER
{
	float c = expf(hitDistance * -0.04f);
	result = Color(c);
}
