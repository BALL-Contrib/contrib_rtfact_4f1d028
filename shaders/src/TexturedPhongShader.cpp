#include "RTfactShader.hpp"

PARAMETER Texture diffuseTexture;
PARAMETER Color ambientIntensity;
PARAMETER Color specularColor;
PARAMETER float shininess;
PARAMETER int isSpecular;

BEGIN_INIT_PARAM
INIT_PARAM(diffuseTexture, NO_TEXTURE)
INIT_PARAM(ambientIntensity, Color(0.1f, 0.3f, 0.4f))
INIT_PARAM(specularColor, Color(1.f, 1.f, 1.f))
INIT_PARAM(shininess, 50.f)
INIT_PARAM(isSpecular, 1)
END_INIT_PARAM


SURFACE_SHADER
{
    const Point hit          = origin + hitDistance * dir;
    const Point fixedNormal  = FaceForward(normal, dir);
    int numLights = Context::getNumLightSources(context);
    const Point reflView     = Reflect(dir, fixedNormal);
    Color diffuse = SampleTexture(diffuseTexture);

    Color tmpRes = ambientIntensity * diffuse;
    for(int light = 0; light < numLights; ++light)
    {
        float lightDist, dotLightNormal, dotLightRefl;
        Point lightDir, lightPos;
        Color intensity;

        Context::sampleLightSource(context, light, hit, lightDir, lightDist, intensity, lightPos);
        Normalize(lightDir);

        dotLightNormal = Dot(fixedNormal, lightDir);

        // The light ray travels from the hitpoint to the light source. Othwise directional lights don't work
        if (dotLightNormal > 0.f && Context::getLightContribution(context, hit + Epsilon * fixedNormal, -lightDir, 10000.0, intensity))
        {
            dotLightRefl = Dot(lightDir, reflView);

            if (isSpecular) {
                tmpRes += diffuse * intensity * dotLightNormal
                    + (dotLightRefl > EPS ? specularColor * powf(dotLightRefl, shininess) : 0.0f);
            } else {
                tmpRes += diffuse * intensity * dotLightNormal;
            }
        }
    }
    result = tmpRes;
}
