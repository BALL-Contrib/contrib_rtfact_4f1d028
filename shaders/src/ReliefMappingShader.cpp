#include "RTfactShader.hpp"
#include <stdio.h>
#define MIN_LINEAR_STEPS	10
#define MAX_LINEAR_STEPS	40
#define BINARY_STEPS		8
#define HEIGHT_SCALE        0.08

PARAMETER Texture colorMap;
PARAMETER Texture normalMap;
PARAMETER Texture heightMap;

BEGIN_INIT_PARAM
INIT_PARAM(colorMap, NO_TEXTURE)
INIT_PARAM(normalMap, NO_TEXTURE)
INIT_PARAM(heightMap, NO_TEXTURE)
END_INIT_PARAM

SURFACE_SHADER
{

    const Point P = origin + hitDistance * dir;
    const Vector & IN = dir;
    const Normal & N = normal;

    Normal Nn = FaceForward(N, IN);
    Vector eyeDir = Normalized(worldToTangent(-IN, tangent, binormal, Nn));

    // Run variables
    float edotn = Dot(IN,N);
    int numSteps = edotn * MIN_LINEAR_STEPS + (1.0f - edotn) * MAX_LINEAR_STEPS;
	float height = 1.0f;
	float step = 1.0f / numSteps;
	float currentHeight = SampleTextureCoord(heightMap, texCoord1, texCoord2).x;
	float signDir = Dot(N, IN) < 0.0f ? -1.0f : 1.0f;
    float deltax = signDir * eyeDir.x * HEIGHT_SCALE / eyeDir.z;
    float deltay = signDir * eyeDir.y * HEIGHT_SCALE / eyeDir.z;

	// Linear search
	for(int i=1; i <= numSteps; ++i)
	{
		if(currentHeight <= height)
		{
			height -= step;
            float tempu = texCoord1 - deltax * height;
            float tempv = texCoord2 - deltay * height;
            currentHeight = SampleTextureCoord(heightMap, tempu, tempv).x;
		}
		else
			break;
	}

	// Binary search
	height += step;
	for(int i=0; i < BINARY_STEPS; ++i)
	{
		step *= 0.5f;
		height -=step;
        float tempu = texCoord1 - deltax * height;
        float tempv = texCoord2 - deltay * height;
        currentHeight = SampleTextureCoord(heightMap, tempu, tempv).x;

		if(currentHeight >= height)
			height += step;
	}

	// Compute final texture coordinate
    float tempu = texCoord1 - deltax * height;
    float tempv = texCoord2 - deltay * height;

    // Compute final color
    Color Kd = SampleTextureCoord(colorMap, tempu, tempv);
    Nn = Normalized(tangentToWorld(SampleTextureCoord(normalMap, tempu, tempv) * 2.0 - 1.0, tangent, binormal));
#if 0
    result = Kd;
#else
    const Point hit          = origin + hitDistance * dir;
	const Point fixedNormal  = Nn;
	int numLights = Context::getNumLightSources(context);
	const Point reflView     = Reflect(dir, fixedNormal);
	Color diffuse = Kd;

	Color tmpRes = Color(0,0,0);
	for(int light = 0; light < numLights; ++light)
	{
		float lightDist, dotLightNormal, dotLightRefl;
		Point lightDir, lightPos;
		Color intensity;

		Context::sampleLightSource(context, light, hit, lightDir, lightDist, intensity, lightPos);
		Normalize(lightDir);

		dotLightNormal = Dot(fixedNormal, lightDir);

		if (dotLightNormal > 0.f)// && Context::getLightContribution(context, lightPos, lightDir, lightDist, intensity))
		{

			dotLightRefl = Dot(lightDir, reflView);
			tmpRes += diffuse * intensity * dotLightNormal
				      + (dotLightRefl > EPS ? Color(1,1,1) * powf(dotLightRefl, 100.0f) : 0.0f);
		}
	}

	result = tmpRes;
#endif
}
