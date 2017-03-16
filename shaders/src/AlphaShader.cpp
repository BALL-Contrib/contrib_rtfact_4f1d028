#include "RTfactShader.hpp"

PARAMETER Color diffuseColor;
PARAMETER float opacity;

BEGIN_INIT_PARAM
INIT_PARAM(diffuseColor, Color(0.f, 1.f, 0.f))
INIT_PARAM(opacity, 0.5f)
END_INIT_PARAM

SURFACE_SHADER
{
	Point hitPoint = origin + hitDistance * dir;

	Color traced;
	Context::trace(context, hitPoint + 0.01f * dir, dir, depth, traced);

	result = Mix(traced, diffuseColor, opacity);
}
