    #include "RTfactShader.hpp"

    PARAMETER Color diffuseColor;
    PARAMETER Color ambientIntensity;
    PARAMETER Color specularColor;
    PARAMETER Color reflectanceColor;
    PARAMETER float shininess;
    PARAMETER int useVertexColor;
    PARAMETER int isSpecular;
    PARAMETER int receiveIllumination;
    PARAMETER int VPLIllumination;

    BEGIN_INIT_PARAM
    INIT_PARAM(diffuseColor, Color(1.f, 0.2f, 0.f))
    INIT_PARAM(ambientIntensity, Color(0.1f, 0.3f, 0.4f))
    INIT_PARAM(specularColor, Color(1.f, 1.f, 1.f))
    INIT_PARAM(reflectanceColor, Color(0.f, 0.f, 0.f))
    INIT_PARAM(shininess, 50.f)
    INIT_PARAM(useVertexColor, 0)
    INIT_PARAM(isSpecular, 1)
    INIT_PARAM(receiveIllumination, 1)
    INIT_PARAM(VPLIllumination, 1)
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

    const Point hit           = origin + hitDistance * dir;
    const Normal fixedNormal  = FaceForward(normal, dir);
    int numLights = Context::getNumLightSources(context);
    const Point reflView      = Reflect(dir, fixedNormal);
    Color diffuse;
    Color ambient;

    bool useVPLIllum = VPLIllumination && (Context::getParticlesGenerated(context));

    //TODO: diffuse texture

    if (useVPLIllum) {
        ambient = Color(.0f, .0f, .0f);
    } else {
        ambient = ambientIntensity;
    }

    bool viewDotNormalMask = Dot(normal, dir) <= 0;

    if (useVertexColor) {
        diffuse = vertColor;
    } else {
        diffuse = diffuseColor;
    }

    Color tmpRes = ambient * diffuse;

    if(rayType == Shader::RayType::ImportonRay)
    {
        Context::insertNewImporton( context, hit,
                                    INV_PI * diffuse,
                                    fixedNormal);
    }
    else if (rayType == Shader::RayType::ParticleRay)
    {
        //generate particle and store it
        Color color = INV_PI * diffuse * cumulativeFactor;

        Context::assessParticle(context, hit, color, fixedNormal);

        //bounce further
        if(depth < maxDepth)
        {
            //generate continuation random sample
            unsigned primeNumberIndex = primeBase;
            const float continuationSample =
              HaltonFaure(randomSeed, primeNumberIndex);
            primeNumberIndex++;

            //russian rulette decides, whether we continue the path or not
            const Color reflectance =
                diffuse + specularColor + reflectanceColor;

            const float albedo = Luminance(reflectance);
            if(continuationSample >= albedo)
            {
                return;
            }

            //ok, how to bounce the particle - diffuse, specular, or reflective?
            const float eventSample = continuationSample / albedo;

            const float diffuseAlbedo = Luminance(diffuse);
            const float glossyAlbedo = Luminance(specularColor);
            const float reflAlbedo = Luminance(reflectanceColor);
            const float totalAlbedo = diffuseAlbedo + glossyAlbedo + reflAlbedo;
            const float scaledSample = eventSample * totalAlbedo;

            Vector bounceDir;
            Vector factor;
            float dirProb;
            float eventProb;

            //let's bounce diffuse
            if(scaledSample < diffuseAlbedo)
            {
                eventProb = diffuseAlbedo / totalAlbedo;
                Vector2D dirSamples = HaltonFaure2D(randomSeed, primeNumberIndex, primeNumberIndex + 1);
                const Vector sampledDir = CosineHemisphere(dirSamples, &dirProb);
                bounceDir = transformToWorld(sampledDir, fixedNormal);
                factor = diffuse * INV_PI;
                primeNumberIndex += 2;
            }
            //let's bounce glossy
            else if(scaledSample >= diffuseAlbedo && scaledSample < (diffuseAlbedo + glossyAlbedo))
            {
                eventProb = glossyAlbedo / totalAlbedo;
                Vector2D dirSamples = HaltonFaure2D(randomSeed, primeNumberIndex, primeNumberIndex + 1);
                const Vector sampledDir = PowerCosineHemisphere(dirSamples, shininess);

                const Vector reflDir = dir -
                        2.f * Dot(dir, fixedNormal) * fixedNormal;

                bounceDir = transformToWorld(sampledDir, reflDir);

                if(Dot(dir, normal) < 0.f)
                {
                    dirProb = 0.f;
                    factor = Vector(1,1,1);
                }
                else
                {
                    dirProb = 1.f;
                    factor = ((shininess + 2.f) / (shininess + 1.f)) * specularColor;
                }
                primeNumberIndex += 2;
            }
            //let's bounce reflected
            else
            {
                eventProb = reflAlbedo / totalAlbedo;
                bounceDir = dir -
                        2.f * Dot(dir, fixedNormal) * fixedNormal;

                if(Dot(dir, normal) < 0.f)
                {
                    dirProb = 0.f;
                    factor = Vector(1.f, 1.f, 1.f);
                }
                else
                {
                    dirProb = 1.f;
                    factor = reflectanceColor;
                }
            }

            if(dirProb != 0.f)
            {
                //evaluate final particle energy
                factor = cumulativeFactor *
                    factor *
                    (fabs(Dot(bounceDir, fixedNormal) / (dirProb * eventProb * albedo)));

                TraceParticle(hit, bounceDir, factor, tmpRes);
            } // of dirProb != 0.f
        }
    }
    else //rayType == Shader::RayType::ShadingRay
    {
        //TODO: weight alpha, diffuse texture

        //TODO: reflect ray

        if(receiveIllumination)
        {
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
        }

        if(useVPLIllum)
        {
            const float invLightPathCount = 1.f / Context::getLightPathCount(context);
            const float clampThreshold = Context::getParticleGeneratorClampThreshold(context);

            unsigned pqHandle;
            Particle particle;
            Point position;
            float intensity;

            Context::initParticleQuery(context, &pqHandle);

            while(Context::getNextParticle(context, pqHandle, particle, position, intensity))
            {

                Vector directionToLight;
                Vector intensity;
                float LdotN;
                bool lightMask;
                if(!Context::getLightIntensity(
                    hit, normal, (Light)particle, context,
                    directionToLight, intensity, LdotN, lightMask))
                {
                  continue;
                }

                if (viewDotNormalMask)
                {

                    const Vector reflView = dir - 2.f * normal * Dot(dir, normal);

                    const float LdotR = Max(Dot(directionToLight, reflView), 0);

                    //LdotN compensates for the fact that radiance is over projected area
                    Color contribution =
                        intensity * LdotN * invLightPathCount * INV_PI *
                        (diffuse + specularColor * (shininess + 2.f) * .5f * pow(LdotR, shininess));

                    //clamp the total contribution
                    float luminance = Luminance(contribution);

                    Vector clampedContribution = luminance > 0 ?
                        contribution * (Min(luminance, clampThreshold) / luminance):
                        Vector(0, 0, 0);

                    tmpRes += clampedContribution;

                    float frameLuminanceContribution = Luminance(clampedContribution);
                    Context::incrementTotalFrameLuminance(context, frameLuminanceContribution);
                }
            }
        }

        //TODO: if(weightAlpha)
        {
        }
        //TODO: else if (taStoreAlpha)
        {
        }
    }

#ifdef RTFACT_VISUALIZE_PARTICLES
    unsigned pqHandle;
    Particle particle;
    Context::initParticleQuery(context, &pqHandle);
    float invLightPathCountC = 1.f / Context::getLightPathCount(context);
    Point position;
    float intensity;

    while(Context::getNextParticle(context, pqHandle, particle, position, intensity))
    {
        Vector posPart = position-hit;
        float dist = sqrt(Dot(posPart, posPart));
        if(dist<0.1f)
            tmpRes = Vector(intensity, 0, 0);

    }
#endif

    result = tmpRes;

    }
