    #include "RTfactShader.hpp"

    PARAMETER Color diffuseColor;
    PARAMETER Color ambientIntensity;
    PARAMETER Color specularColor;
    PARAMETER float shininess;
    PARAMETER int useVertexColor;
    PARAMETER int isSpecular;

    BEGIN_INIT_PARAM
    INIT_PARAM(diffuseColor, Color(1.f, 0.2f, 0.f))
    INIT_PARAM(ambientIntensity, Color(0.1f, 0.3f, 0.4f))
    INIT_PARAM(specularColor, Color(1.f, 1.f, 1.f))
    INIT_PARAM(shininess, 50.f)
    INIT_PARAM(useVertexColor, 0)
    INIT_PARAM(isSpecular, 1)
    END_INIT_PARAM

    //RenderMan like:
    //PARAMETER float Ka = 1.f;
    //PARAMETER float Kd = 1.f;
    //PARAMETER float Ks = 0.5f;
    //PARAMETER float roughness = 0.1f;

    SURFACE_SHADER
    {
    /*
    //RenderMan like phong
    const Point P = origin + hitDistance * dir;
    const Normal Nf = FaceForward(normal, dir);
    const Vector RD = Reflect(dir, Nf);

    Color C_diffuse = Color(0.f, 0.f, 0.f);
    Color C_specular = Color(0.f, 0.f, 0.f);
    BEGIN_ILLUMINANCE_LOOP(P) {
        C_diffuse += Dot(Nf, L_dir_norm);
        C_specular += powf(Dot(L_dir_norm, RD), shininess);
    } END_ILLUMINANCE_LOOP;

    result = Ka * ambientIntensity + diffuseColor * Kd * C_diffuse + specularColor * Ks * C_specular;
    */

    //RTfact-like phong

    const Point hit          = origin + hitDistance * dir;
    const Point fixedNormal  = FaceForward(normal, dir);
    int numLights = Context::getNumLightSources(context);
    const Point reflView     = Reflect(dir, fixedNormal);
    Color diffuse;

    if (useVertexColor) {
        diffuse = vertColor;
    } else {
        diffuse = diffuseColor;
    }

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
        if (dotLightNormal > 0.f && Context::getLightContribution(context, hit + Epsilon * fixedNormal, -lightDir, lightDist, intensity))
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
