/* 
 *  XML3DPhongShader.hpp
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
 *  Created on: 2010-10-13 16:38:43 +0200
 *  Author(s): Felix Klein, Kristian Sons
 */
 
 #ifndef RTFACT__XML3DPHONGSHADER_HPP
#define RTFACT__XML3DPHONGSHADER_HPP

#include <RTfact/Config/Common.hpp>

#include <utility>

#include <RTfact/Concept/Appearance/SurfaceShader.hpp>
#include <RTfact/Utils/ShadingUtils.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tContext,
    class tDiffuseTexture>
class XML3DPhongShader :
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
    uint mMaxDepth;
    bool mUseVertexColor;
    bool mReceiveIllumination;

    bool mIsReflective;

public:

    XML3DPhongShader() :
        mDiffuseColor(0.5f, 0.5f, 0.5f),
        mSpecularColor(0.f, 0.f, 0.f),
        mEmissiveColor(0.f, 0.f, 0.f),
        mReflectanceColor(0.f, 0.f, 0.f),
        mAmbientIntensity(0.7f, 0.7f, 0.7f),
        mShininess(0.2f),
        mTransparency(0.f),
        mTransparencyTolerance(0.05f),
        mMaxDepth(10),
        mUseVertexColor(false),
        mReceiveIllumination(true),
        mIsReflective(false)
    {}

    RTFACT_DEFINE_PROPERTY(tDiffuseTexture&, DiffuseTexture);
    RTFACT_DEFINE_PROPERTY(Vec3f1&,          DiffuseColor);
    RTFACT_DEFINE_PROPERTY(Vec3f1&,          SpecularColor);
    RTFACT_DEFINE_PROPERTY(Vec3f1&,          EmissiveColor);
    RTFACT_DEFINE_PROPERTY(Vec3f1&,          AmbientIntensity);
    RTFACT_DEFINE_PROPERTY(float,            Shininess);
    RTFACT_DEFINE_PROPERTY(float,            Transparency);
    RTFACT_DEFINE_PROPERTY(uint,             MaxDepth);
    RTFACT_DEFINE_PROPERTY(bool,             UseVertexColor);
    RTFACT_DEFINE_PROPERTY(bool,             ReceiveIllumination);

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
        typedef typename t_Vec3f::Container   t_Vec3fC;
        typedef Mask<taSize>                  t_Mask;
        typedef typename t_Mask::Container    t_MaskC;
        typedef ActiveMask<taSize>            t_ActiveMask;

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
        const t_Vec3fC ambientIntC = t_Vec3fC::replicate(mAmbientIntensity);
        t_Vec3f hitPoint;

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

            if(mUseVertexColor)
            {
                diffuseColor(i) *= aIntersection.vertexColor(i);
            }
            else
            {
                diffuseColor(i) *= diffColC;
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

        bool weightAlpha = false;
        t_Vec3f transparentColor;
        t_FPacket transparentAlpha;

        if(aRayPacket.depth < mMaxDepth &&
           (mDiffuseTexture.hasTransparency() || mTransparency > mTransparencyTolerance))
        {
			// Has Tranparency from either texture or parameter
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

        if(aRayPacket.depth < mMaxDepth && mIsReflective)
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

            reflRay.depth = aRayPacket.depth + 1;
            reflRay.maxDepth = aRayPacket.maxDepth;

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

                if(!getLightIntensity(
                    hitPoint, aIntersection.normal, aRayMask,
                    *aContext.scene.lights[l], aContext, directionToLight, intensity,
                    dotLightNormal, lightContributionMask))
                {
                    continue;
                }

                const t_Vec3fC specColorC = t_Vec3fC::replicate(mSpecularColor);

                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, lightContributionMask)
                {
                    const t_Vec3fC reflView =
                        aRayPacket.dir(i) - 2.f * aIntersection.normal(i) * Math::dot(
                            aRayPacket.dir(i), aIntersection.normal(i));

                    const t_FPacketC dotReflNormal = Math::max(
                        Math::dot(directionToLight(i), reflView), t_FPacketC::C_0());

                    oResult(i) = lightContributionMask(i).iif(
                        oResult(i) + intensity(i) * (
									diffuseColor(i) * dotLightNormal(i) +
                                    specColorC * Math::pow(
                                        dotReflNormal, t_FPacketC::replicate(mShininess*128.f))),
                        oResult(i));
                }
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
				const t_Vec3fC diffColC = t_Vec3fC::replicate(mDiffuseColor);

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

#endif // RTFACT__XML3DPHONGSHADER_HPP
