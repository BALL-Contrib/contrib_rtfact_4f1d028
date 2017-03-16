#include "RTfactShader.hpp"

PARAMETER float maxDepth;

BEGIN_INIT_PARAM
INIT_PARAM(maxDepth, 2.f)
END_INIT_PARAM

SURFACE_SHADER
{
	Point fixedNormal = FaceForward(normal, dir);
	Point reflDir = Reflect(dir, fixedNormal);
	Point hit = origin + hitDistance * dir;

	if (depth < maxDepth) {
        //TraceRayEps(hit, reflDir, RAY_EPS, result);
        TraceRay(hit, reflDir, result);
	} else {
		result = Color(0.f, 0.f, 0.f);
	}
}
