/*
 *  XML3DPhongShaderGI.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2010  Saarland University
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Created on: 2011-06-17 10:17:48 +0200
 *  Author(s): Iliyan Georgiev, Petr Kmoch, Lukas Marsalek, Vaclav Krajicek
 */

#ifndef RTFACT__XML3DPHONGSHADERGI_HPP
#define RTFACT__XML3DPHONGSHADERGI_HPP

#include <RTfact/Config/Common.hpp>

#include <utility>

#include <RTfact/Concept/Appearance/SurfaceShader.hpp>
#include <RTfact/Utils/ShadingUtils.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tContext,
    class tDiffuseTexture>
class XML3DPhongShaderGI :
    public SurfaceShader<tContext>
{
    tDiffuseTexture mDiffuseTexture;
    Vec3f1 mDiffuseColor;
    Vec3f1 mSpecularColor;
    Vec3f1 mEmissiveColor;
    Vec3f1 mReflectanceColor;
    Vec3f1 mAmbientIntensity;
    float mShininess;
    float mTransparency;
    float mTransparencyTolerance;
    bool mUseVertexColor;
    bool mReceiveIllumination;
    bool mVPLIllumination;

    bool mIsReflective;
    uint mNumShadowSamples;

public:

    XML3DPhongShaderGI() :
        mDiffuseColor(0.5f, 0.5f, 0.5f),
        mSpecularColor(0.f, 0.f, 0.f),
        mEmissiveColor(0.f, 0.f, 0.f),
        mReflectanceColor(0.f, 0.f, 0.f),
        mAmbientIntensity(0.7f, 0.7f, 0.7f),
        mShininess(0.2f),
        mTransparency(0.f),
        mTransparencyTolerance(0.05f),
        mUseVertexColor(false),
        mReceiveIllumination(true),
        mIsReflective(false),
        mVPLIllumination(true),
        mNumShadowSamples(1)
    {}

    RTFACT_DEFINE_PROPERTY(tDiffuseTexture&, DiffuseTexture);
    RTFACT_DEFINE_PROPERTY(Vec3f1&,          DiffuseColor);
    RTFACT_DEFINE_PROPERTY(Vec3f1&,          SpecularColor);
    RTFACT_DEFINE_PROPERTY(Vec3f1&,          EmissiveColor);
    RTFACT_DEFINE_PROPERTY(Vec3f1&,          AmbientIntensity);
    RTFACT_DEFINE_PROPERTY(float,            Shininess);
    RTFACT_DEFINE_PROPERTY(float,            Transparency);
    RTFACT_DEFINE_PROPERTY(bool,             UseVertexColor);
    RTFACT_DEFINE_PROPERTY(bool,             ReceiveIllumination);
    RTFACT_DEFINE_PROPERTY(bool,             VPLIllumination);
    RTFACT_DEFINE_PROPERTY(uint,             NumShadowSamples);

    RTFACT_DEFINE_GET_PROPERTY(Vec3f1&,      ReflectanceColor);

    tDiffuseTexture& getDiffuseTexture()
    {
        return mDiffuseTexture;
    }

    void setReflectanceColor(
        const Vec3f1& aReflectanceColor)
    {
        mReflectanceColor = aReflectanceColor;

        mIsReflective = (mReflectanceColor != Vec3f1::C_0_0_0()).isTrue();
    }

    template<
        bool taCommonOrg,
        bool taStoreAlpha,
        uint taSize>
    RTFACT_APPEARANCE_INLINE void shade(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        SurfaceIntersection<taSize>& aIntersection,
        tContext& aContext,
        Vec3f<taSize>& oResult,
        Packet<taSize, float>* oResultAlpha = RTFACT_NULL) const
    {
        RTFACT_APPEARANCE_ASSERT(!taStoreAlpha || oResultAlpha != RTFACT_NULL);

        typedef Vec3f<taSize>                 t_Vec3f;
        typedef Packet<taSize, float>         t_FPacket;
        typedef typename t_FPacket::Container t_FPacketC;
        typedef Packet<taSize, int>           t_IPacket;
        typedef typename t_Vec3f::Container   t_Vec3fC;
        typedef Mask<taSize>                  t_Mask;
        typedef typename t_Mask::Container    t_MaskC;
        typedef ActiveMask<taSize>            t_ActiveMask;

        bool useVPLIllum =
            mVPLIllumination && (aContext.particleGenerator.getParticlesGenerated());

        t_Vec3f diffuseColor;
        t_FPacket opacity;

        if(mDiffuseTexture.hasTransparency())
        {
            mDiffuseTexture.template sample<true, true>(
                aIntersection, aRayMask, &diffuseColor, &opacity);
        }
        else
        {
            mDiffuseTexture.template sample<true, false>(
                aIntersection, aRayMask, &diffuseColor);
        }

        const t_Vec3fC diffColC = t_Vec3fC::replicate(mDiffuseColor);
        const t_Vec3fC emissiveColC = t_Vec3fC::replicate(mEmissiveColor);

        //When we are using VPL illumination, no ambient term should be used
        t_Vec3fC ambientIntC;
        if(useVPLIllum)
        {
            ambientIntC = t_Vec3fC::C_0_0_0();
        }
        else
        {
            ambientIntC = t_Vec3fC::replicate(mAmbientIntensity);
        }

        t_Vec3f hitPoint;
        ActiveMask<taSize> viewDotNormalMask;
        viewDotNormalMask.setTrue();

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        {
            if(RTFACT_CHECK_UMASK(tContext::Scene::PROPERTIES, SceneProperties::INVERT_NORMAL))
            {
                aIntersection.normal(i) =
                    (Math::dot(aIntersection.normal(i), aRayPacket.dir(i)) >
                        t_FPacketC::C_0()).iif(
                            -aIntersection.normal(i),
                            aIntersection.normal(i));
            }

            //set to true if shading should happen, e.g. N dot Dir <= 0
            viewDotNormalMask(i) = Math::dot(aIntersection.normal(i), aRayPacket.dir(i)) <= t_FPacketC::C_0();

            if(mUseVertexColor)
            {
                diffuseColor(i) *= aIntersection.cap(i).iif(aIntersection.capColor(i), aIntersection.vertexColor(i));
            }
            else
            {
                diffuseColor(i) *= aIntersection.cap(i).iif(aIntersection.capColor(i), diffColC);
            }

            

            oResult(i) = aRayMask(i).iif(
                emissiveColC + ambientIntC * diffuseColor(i),
                oResult(i));

            if(taCommonOrg)
            {
                hitPoint(i) =
                    aRayPacket.org(0) + aIntersection.dist(i) * aRayPacket.dir(i);
            }
            else
            {
                hitPoint(i) =
                    aRayPacket.org(i) + aIntersection.dist(i) * aRayPacket.dir(i);
            }
        }

        //For evaluating importons or particles, we need all the sample event coefficients (diffuse, specular, refl.)
        //which has been computed above. Thus we generate importon/particle and exit
        //Grabs only first ray in the packet, for now (patience!)
        Vec3f1 ffNormal = faceForward(aIntersection.normal[0], aRayPacket.dir[0]);
        if(aRayPacket.rayType == RayType::ImportonRay)
        {
            typename tContext::tImporton newImporton(
                            hitPoint[0],
                            RTFACT_INV_PI * diffuseColor[0],
                            ffNormal);

            aContext.particleFilter.importonStorage.insertParticle(newImporton);
        }
        else if (aRayPacket.rayType == RayType::ParticleRay)
        {
            //generate continuation random sample
            uint primeNumberIndex = aRayPacket.primeBase[0];
            const float continuationSample =
                Math::haltonFaure(aRayPacket.randomSeed[0], primeNumberIndex);
            primeNumberIndex++;

            //generate particle and store it
            QuadraticLightAttenuation particleAttn;
            //particleAttn.setMinimalDistance(aContext.particleGenerator.statistics.clampThreshold);
            //the inverse pi is to convert from radiance to irradiance, roughly, ehm., NOT RIGHT
            typename tContext::tParticle newParticle(
                hitPoint[0],
                diffuseColor[0] * aRayPacket.cumulativeFactor[0],
                ffNormal,
                particleAttn);
            aContext.assessParticle(newParticle);

            //bounce further
            if(aRayPacket.depth < aRayPacket.maxDepth)
            {
                //russian rulette decides, whether we continue the path or not
                const Vec3f1 reflectance = mDiffuseColor[0];
                const float albedo = reflectance.luminance();
                if(continuationSample >= albedo)
                {
                    return;
                }

                //ok, how to bounce the particle - diffuse, specular, or reflective?
                const float eventSample = continuationSample / albedo;

                const float diffuseAlbedo = mDiffuseColor[0].luminance();
                const float totalAlbedo = diffuseAlbedo;
                const float scaledSample = eventSample * totalAlbedo;

                Vec3f1 bounceDir;
                Vec3f1 factor;
                float dirProb = 0.f;
                float eventProb;
                //let's bounce diffuse
                if(scaledSample < diffuseAlbedo)
                {
                    eventProb = diffuseAlbedo / totalAlbedo;
                    Vec2f1 dirSamples = Math::haltonFaure2D(aRayPacket.randomSeed[0], primeNumberIndex, primeNumberIndex + 1);
                    const Vec3f1 sampledDir = Math::cosineHemisphere(dirSamples, &dirProb);
                    bounceDir = Math::transformToWorld(sampledDir, ffNormal);
                    factor = mDiffuseColor[0] * RTFACT_INV_PI;
                    primeNumberIndex += 2;
                }

                if(dirProb != 0.f)
                {
                    //evaluate final particle energy
                    factor = aRayPacket.cumulativeFactor[0] *
                        factor *
                        (Math::absDot(bounceDir, ffNormal) / (dirProb * eventProb * albedo));
                    //albedo here = probability of continuing the path approximation

                    //fill in the data and trace the particle further on
                    //This bounced particle creation is correctly packetized
                    //and will be useful once the
                    //above computations are actually packetized.
                    RayPacket<taSize> bouncedParticle;
                    t_Vec3f bounceDirP = t_Vec3f::replicate(bounceDir);
                    t_Vec3f factorP = t_Vec3f::replicate(factor);
                    t_IPacket primeBaseP = t_IPacket::replicate(primeNumberIndex);
                    RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
                    {
                        bouncedParticle.dir(i) = bounceDirP(i);
                        bouncedParticle.dir(i).setNormalized();
                        bouncedParticle.invDir(i) = bouncedParticle.dir(i).reciprocal();
                        bouncedParticle.org(i) = hitPoint(i) + ffNormal(i) * t_FPacketC::C_RAY_EPS;
                        bouncedParticle.tMin(i) = t_FPacketC::C_RAY_EPS;
                        bouncedParticle.tMax(i) = t_FPacketC::C_INFINITY;
                        bouncedParticle.cumulativeFactor(i) = factorP(i);
                        bouncedParticle.primeBase(i) = primeBaseP(i);
                    }

                    bouncedParticle.maxDepth = aRayPacket.maxDepth;
                    bouncedParticle.depth = aRayPacket.depth + 1;
                    bouncedParticle.randomSeed = aRayPacket.randomSeed;
                    bouncedParticle.rayType = RayType::ParticleRay;
                    bouncedParticle.inside = aRayPacket.inside;

                    aContext.template trace<false, false>(bouncedParticle, aRayMask, oResult);
                } // of dirProb != 0.f
            } //of ray.depth < ray.maxDepth
        } // of particle branch
        else
        {
            bool weightAlpha = false;
            t_Vec3f transparentColor;
            t_FPacket transparentAlpha;

            if(aRayPacket.depth < aRayPacket.maxDepth &&
               (mDiffuseTexture.hasTransparency() || mTransparency > mTransparencyTolerance))
            {
                RayPacket<taSize> newRays = aRayPacket;
                ++newRays.depth;

                newRays.tMin = aIntersection.dist + t_FPacket::C_RAY_EPS;

                if(mDiffuseTexture.hasTransparency())
                {
                    t_ActiveMask mask;
                    const t_FPacketC opacityTolerance =
                        t_FPacketC::replicate(1.0f - mTransparencyTolerance);

                    RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
                    {
                        mask(i) = aRayMask(i) & (opacity(i) < opacityTolerance);
                        mask.updateActiveContainerEndIndex(i);
                    }

                    if(!mask.isFalse())
                    {
                        weightAlpha = true;

                        transparentColor = t_Vec3f::C_0_0_0();
                        if (
                            taStoreAlpha)
                        {
                            transparentAlpha = t_FPacket::C_0();

                            aContext.template trace<taCommonOrg, true>(
                                newRays, mask, transparentColor, &transparentAlpha);
                        }
                        else
                        {
                            aContext.template trace<taCommonOrg, false>(
                                newRays, mask, transparentColor);
                        }
                    }
                }
                else
                {
                    weightAlpha = true;

                    transparentColor = t_Vec3f::C_0_0_0();
                    if (
                        taStoreAlpha)
                    {
                        transparentAlpha = t_FPacket::C_0();

                        aContext.template trace<taCommonOrg, true>(
                            newRays, aRayMask, transparentColor, &transparentAlpha);
                    }
                    else
                    {
                        aContext.template trace<taCommonOrg, false>(
                            newRays, aRayMask, transparentColor);
                    }
                }
            }

            if(aRayPacket.depth < aRayPacket.maxDepth && mIsReflective)
            {
                RayPacket<taSize> reflRay;

                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
                {
                    reflRay.dir(i) = aRayPacket.dir(i) - 2.f * Math::dot(
                        aRayPacket.dir(i), aIntersection.normal(i)) * aIntersection.normal(i);
                    reflRay.dir(i).setNormalized();
                    reflRay.invDir(i) = reflRay.dir(i).reciprocal();
                    reflRay.org(i) = hitPoint(i);
                    reflRay.tMin(i) = t_FPacketC::C_RAY_EPS;
                    reflRay.tMax(i) = t_FPacketC::C_INFINITY;
                }

                reflRay.maxDepth = aRayPacket.maxDepth;
                reflRay.depth = aRayPacket.depth + 1;

                t_Vec3f reflectedColor = t_Vec3f::C_0_0_0();

                aContext.template trace<false, false>(reflRay, aRayMask, reflectedColor);

                const t_Vec3fC reflectanceColorC = t_Vec3fC::replicate(mReflectanceColor);

                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
                {
                    oResult(i) = aRayMask(i).iif(
                        oResult(i) + reflectedColor(i) * reflectanceColorC,
                        oResult(i));
                }
            }

            if(mReceiveIllumination)
            {
                for(uint l = 0; l < aContext.scene.lights.size(); ++l)
                {
                    t_Vec3f directionToLight;
                    t_Vec3f intensity;
                    t_FPacket dotLightNormal;
                    t_ActiveMask lightContributionMask;
                    t_FPacketC numSamplesInvC;

                    Light& currentLight = *aContext.scene.lights[l];
                    uint numSamples = currentLight.getHasCommonSamplePosition() ? 1 : mNumShadowSamples;
                    numSamplesInvC = t_FPacketC::replicate(1.f / numSamples);
                    for(uint s = 0; s < numSamples; s++)
                    {
                        if(!getLightIntensity(
                            hitPoint, aIntersection.normal, aRayMask,
                            currentLight, aContext, directionToLight, intensity,
                            dotLightNormal, lightContributionMask))
                        {
                            continue;
                        }

                        const t_Vec3fC specColorC = t_Vec3fC::replicate(mSpecularColor);

                        //use the external illumination only if front facing
                        lightContributionMask &= viewDotNormalMask;

                        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, lightContributionMask)
                        {
                            const t_Vec3fC reflView =
                                aRayPacket.dir(i) - 2.f * aIntersection.normal(i) * Math::dot(
                                    aRayPacket.dir(i), aIntersection.normal(i));

                            const t_FPacketC dotReflNormal = Math::max(
                                Math::dot(directionToLight(i), reflView), t_FPacketC::C_0());

                            oResult(i) = lightContributionMask(i).iif(
                                oResult(i) + intensity(i) * numSamplesInvC * (diffuseColor(i) * dotLightNormal(i)
                                           + specColorC * Math::pow(
                                                dotReflNormal, t_FPacketC::replicate(mShininess*128.f))),
                                oResult(i));
                        }
                    }
                }
            }

            if(useVPLIllum)
            {
                typename tContext::tParticle particle;
                typename tContext::tParticleQuery pqHandle;
                aContext.particleGenerator.particleStorage.initParticleQuery(pqHandle);
                t_FPacketC invLightPathCountC = t_FPacketC::replicate(1.f / aContext.particleGenerator.getLightPathCount());
                while(aContext.particleGenerator.particleStorage.getNextParticle(pqHandle, particle))
                {
                    t_Vec3f directionToLight;
                    t_Vec3f intensity;
                    t_FPacket LdotN;
                    t_ActiveMask lightContributionMask;

                    if(!getLightIntensity(
                        hitPoint, aIntersection.normal, aRayMask,
                        particle, aContext, directionToLight, intensity,
                        LdotN, lightContributionMask))
                    {
                        continue;
                    }

                    //use the external illumination only if front facing
                    lightContributionMask &= viewDotNormalMask;

                    t_FPacketC specExponentC = t_FPacketC::replicate(mShininess);
                    const t_Vec3fC specColorC = t_Vec3fC::replicate(mSpecularColor);
                    t_FPacketC clampThresholdC = t_FPacketC::replicate(aContext.particleGenerator.statistics.clampThreshold);
                    t_FPacketC inversePIC = t_FPacketC::replicate(RTFACT_INV_PI);
                    float frameLuminanceContribution = 0.f;
                    RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, lightContributionMask)
                    {
                        const t_Vec3fC reflView =
                            aRayPacket.dir(i) - 2.f * aIntersection.normal(i) * Math::dot(
                                aRayPacket.dir(i), aIntersection.normal(i));

                        const t_FPacketC LdotRC = Math::max(
                            Math::dot(directionToLight(i), reflView), t_FPacketC::C_0());
                        //LdotN compensates for the fact that radiance is over projected area
                        t_Vec3fC contributionC =
                            intensity(i) * invLightPathCountC * inversePIC *
                            (LdotN(i) * diffuseColor(i) +
                            specColorC * Math::pow(LdotRC, specExponentC*128.f));

                        //clamp the total contribution
                        t_FPacketC luminanceC = contributionC.luminance();
                        t_MaskC lumNonNegativeMask = luminanceC > t_FPacketC::C_0();
                        t_Vec3fC clampedContributionC = lumNonNegativeMask.iif(
                            contributionC * (Math::min(luminanceC, clampThresholdC) / luminanceC),
                            t_Vec3fC::C_0_0_0());

                        oResult(i) = lightContributionMask(i).iif(
                            oResult(i) + clampedContributionC,
                            oResult(i));

                        RTFACT_PACKET_FOR_ALL_ITEMS_IN_CONTAINER(k, taSize)
                        {
                            if(lightContributionMask(i)[k])
                            {
                                frameLuminanceContribution += clampedContributionC[k].luminance();
                            }
                        }
                    }
                    aContext.particleFilter.stats.incrementTotalFrameLuminance(frameLuminanceContribution);
                }
            }

            if(weightAlpha)
            {
                if(mDiffuseTexture.hasTransparency())
                {
                    RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
                    {
                        oResult(i) = aRayMask(i).iif(
                            oResult(i) * opacity(i) +
                                transparentColor(i) * (t_FPacketC::C_1 - opacity(i)),
                            oResult(i));
                        if (
                            taStoreAlpha)
                        {
                            // 1 - resA = (1 - opac) * (1 - trA) <=> resA = opac + trA - opac*trA
                            (*oResultAlpha)(i) = aRayMask(i).iif(
                                opacity(i) + transparentAlpha(i) - opacity(i) * transparentAlpha(i),
                                (*oResultAlpha)(i));
                        }
                    }
                }
                else
                {
                    const t_FPacketC trans = t_FPacketC::replicate(mTransparency);
                    const t_FPacketC alpha = t_FPacketC::replicate(1.f - mTransparency);

                    RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
                    {
                        oResult(i) = aRayMask(i).iif(
                            oResult(i) * alpha + diffuseColor(i) *
                                transparentColor(i) * trans,
                            oResult(i));
                        if (
                            taStoreAlpha)
                        {
                            // 1 - resA = (1 - a) * (1 - trA) <=> resA = a + trA - a*trA
                            (*oResultAlpha)(i) = aRayMask(i).iif(
                                alpha + transparentAlpha(i) - alpha * transparentAlpha(i),
                                (*oResultAlpha)(i));
                        }
                    }
                }
            }
            else if (
                taStoreAlpha)
            {
                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
                {
                    (*oResultAlpha)(i) = aRayMask(i).iif(
                        t_FPacketC::C_1,
                        (*oResultAlpha)(i));
                }
            }
        } // normal ray branch of if-else (importon || particle || normal ray)

#ifdef RTFACT_VISUALIZE_IMPORTONS

        tContext::tImporton importon;
        tContext::tImportonQuery iqHandle;
        aContext.particleFilter.importonStorage.initParticleQuery(iqHandle);
        t_Vec3fC importonPosC;
        t_MaskC iDistMaskC;
        while(aContext.particleFilter.importonStorage.getNextParticle(iqHandle, importon))
        {
            importonPosC = t_Vec3fC::replicate(importon.getPosition());
            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
            {
                const t_FPacketC importonDistance = (importonPosC - hitPoint(i)).length();

                iDistMaskC = (importonDistance < t_FPacketC::replicate(0.5f));
                iDistMaskC &= aRayMask(i);
                oResult(i) = iDistMaskC.iif(t_Vec3fC::replicate(0.f, 0.f, importon.getIntensity().luminance()), oResult(i));
            }
        }

#endif

#ifdef RTFACT_VISUALIZE_PARTICLES

        tContext::tParticle particle;
        tContext::tParticleQuery pqHandle;
        aContext.particleGenerator.particleStorage.initParticleQuery(pqHandle);
        t_Vec3fC particlePosC;
        t_MaskC distMaskC;
        while(aContext.particleGenerator.particleStorage.getNextParticle(pqHandle, particle))
        {
            particlePosC = t_Vec3fC::replicate(particle.getPosition());
            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
            {
                const t_FPacketC particleDistance = (particlePosC - hitPoint(i)).length();

                distMaskC = (particleDistance < t_FPacketC::replicate(0.5f));
                distMaskC &= aRayMask(i);
                oResult(i) = distMaskC.iif(t_Vec3fC::replicate(particle.getIntensity().luminance(), 0.f, 0.f), oResult(i));
            }
        }

#endif

    }

    template<
        uint taSize>
    RTFACT_APPEARANCE_INLINE void attenuateLight(
        const ActiveMask<taSize>& aMask,
        SurfaceIntersection<taSize>& aIntersection,
        Mask<taSize>& oMask,
        Vec3f<taSize>& oIntensity) const
    {
        typedef Vec3f<taSize>                    t_Vec3f;
        typedef Packet<taSize, float>            t_FPacket;
        typedef typename t_FPacket::Container    t_FPacketC;
        typedef typename Mask<taSize>::Container t_MaskC;
        typedef typename t_Vec3f::Container   t_Vec3fC;

        if(mDiffuseTexture.hasTransparency())
        {
            t_FPacket transparency;

            mDiffuseTexture.template sample<false, true>(
                    aIntersection, aMask, (t_Vec3f*) 0, &transparency);

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
            {
                oIntensity(i) = aMask(i).iif(oIntensity(i) * (1.f - transparency(i)), oIntensity(i));
            }
        }
        else if(mTransparency > 0.f)
        {
            const t_FPacketC transparency = t_FPacketC::replicate(mTransparency);
            const t_Vec3fC diffColC = t_Vec3fC::replicate(mDiffuseColor);

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
            {
                oIntensity(i) = aMask(i).iif(oIntensity(i) * transparency * diffColC, oIntensity(i));
            }
        }
        else
        {
            const t_MaskC falseC = t_MaskC::C_FALSE();

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
            {
                oMask(i) &= ~aMask(i);
            }
        }
    }

    #include RTFACT_SURFACE_SHADER_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__XML3DPHONGSHADERGI_HPP
