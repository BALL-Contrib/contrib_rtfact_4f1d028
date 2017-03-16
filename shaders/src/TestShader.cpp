#include "RTfactShader.hpp"

PARAMETER float param1;
PARAMETER float param2;
PARAMETER float param3;
PARAMETER Color param4;
PARAMETER float maxDepth;

BEGIN_INIT_PARAM
INIT_PARAM(param1, 0.f)
INIT_PARAM(param2, 1.f)
INIT_PARAM(param3, 1.f)
INIT_PARAM(param4, Color(param1, param2, param3))
INIT_PARAM(maxDepth, 2)
END_INIT_PARAM

SURFACE_SHADER
{
    // test writing of result (should be red)
    //result = Color(1.f, 0.f, 0.f);

    // test parameters (should be cyan)
    //result = Color(param1, param2, param3);

    // test struct parameters (should be cyan)
    //result = param4;

    // test api call getNumLightSources (should be blue)
    //const unsigned numLights = Context::getNumLightSources(context);
    //result = Color(0.f, 0.f, numLights);
    //return;

    // test api call trace (should be a mirror)
    /*const Point hit = origin + hitDistance * dir;
    const Point fixedNormal = FaceForward(normal, dir);
    const Point reflDir = Reflect(dir, fixedNormal);
    if (depth < MAX_DEPTH) {
		//TraceRayEps(hit, reflDir, RAY_EPS, result);
		TraceRay(hit, reflDir, result);
	} else {
		result = Color(0.f);
	}*/

    // test shadows (should be grey (lighter or darker depending on intensity of light) with black shadows)
    /*const Point P = origin + hitDistance * dir;
	float lightDist;
	Point lightDir, lightPos, hit;
	Color intensity;

	Context::sampleLightSource(context, 0, P, lightDir, lightDist, intensity, lightPos);

	if (Context::getLightContribution(context, lightPos, -lightDir, lightDist, intensity)) {
		result = Color(.5f, .5f, .5f) * intensity;
	} else {
		result = Color(0.f, 0.f, 0.f);
	}*/

    // test illuminance loop (should be violet (lighter for more lightsources) with black shadows)
    // TODO: packetized version only works with floating point comparison!!!!
    const Point P = origin + hitDistance * dir;
    Color res = Color(0.f, 0.f, 0.f);
    {
        const unsigned int numLights = Context::getNumLightSources(context); 
        for (unsigned int L_id = 0u; (float)L_id < (float)numLights; ++L_id)
        {
            //*(int*)1=0;
            Point L_pos;
            Vector L_dir;
            Vector L_dir_norm;
            float L_dist;
            Color Cl;
            Context::sampleLightSource(context, 0, P, L_dir, L_dist, Cl, L_pos);
            L_dir_norm = Normalized(L_dir);
            if (Context::getLightContribution(context, L_pos, -L_dir_norm, L_dist, Cl))
            {
                //body of illuminance macro
                res += Color(0.2f, 0.0f, 0.2f);
            }
            //putchar(res[0] ? '!' : '.');
        }
    }
    //putchar(res[0] ? '+' : '-');
    result = res;

}

