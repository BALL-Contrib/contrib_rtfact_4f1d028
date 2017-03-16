#include "RTfactShader.hpp"

PARAMETER Texture envMap;
PARAMETER float radius;

BEGIN_INIT_PARAM
INIT_PARAM(envMap, NO_TEXTURE)
INIT_PARAM(radius, 1000.0f)
END_INIT_PARAM

SURFACE_SHADER
{
    Vector hitPoint = Normalized(origin + hitDistance * dir);
	float alpha = acosf(-hitPoint.z);
	float temp = sqrtf(hitPoint.x * hitPoint.x + hitPoint.y * hitPoint.y);
	float sinb = hitPoint.y / temp;
	float cosb = hitPoint.x / temp;
	float eu = ((1.0f + (alpha / M_PI) * cosb) / 2.0f);
	float ev = ((1.0f + (alpha / M_PI) * sinb) / 2.0f);
    result = SampleTextureCoord(envMap, eu, ev);
}
