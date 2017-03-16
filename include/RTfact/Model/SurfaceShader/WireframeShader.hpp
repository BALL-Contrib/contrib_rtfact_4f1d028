/* 
 *  WireframeShader.hpp
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
 *  Created on: 2009-01-29 19:31:16 +0100
 *  Author(s): Iliyan Georgiev, Petr Kmoch
 */
 
 #ifndef RTFACT__WIREFRAMESHADER_HPP
#define RTFACT__WIREFRAMESHADER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Appearance/SurfaceShader.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tContext>
class WireframeShader :
    public SurfaceShader<tContext>
{
    Vec3f1 mDiffuseColor;
    Vec3f1 mEmissiveColor;
    Vec3f1 mAmbientIntensity;
    float mEdgeThreshold;
    uint mMaxDepth;
    bool mUseVertexColor;

public:

    WireframeShader() :
        mDiffuseColor(0.f, 0.f, 0.f),
        mEmissiveColor(0.f, 0.f, 0.f),
        mAmbientIntensity(0.f, 0.f, 0.f),
        mEdgeThreshold(0.01f),
        mMaxDepth(10),
        mUseVertexColor(false)
    {}

    RTFACT_DEFINE_PROPERTY(Vec3f1&, DiffuseColor);
    RTFACT_DEFINE_PROPERTY(Vec3f1&, EmissiveColor);
    RTFACT_DEFINE_PROPERTY(Vec3f1&, AmbientIntensity);
    RTFACT_DEFINE_PROPERTY(float,   EdgeThreshold);
    RTFACT_DEFINE_PROPERTY(uint,    MaxDepth);
    RTFACT_DEFINE_PROPERTY(bool,    UseVertexColor);

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
        typedef ActiveMask<taSize>            t_ActiveMask;
        typedef typename t_Mask::Container    t_MaskC;
        typedef ActiveMask<taSize>            t_ActiveMask;

        const t_FPacketC edgeThresholdC = t_FPacketC::replicate(mEdgeThreshold);

        t_ActiveMask activeMask, inactiveMask;
        activeMask = inactiveMask = aRayMask;

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        {
            const t_MaskC thresholdMaskU = (aIntersection.u(i) <= edgeThresholdC);
            const t_MaskC thresholdMaskV = (aIntersection.v(i) <= edgeThresholdC);
            const t_MaskC thresholdMaskW = ((aIntersection.u(i) + aIntersection.v(i)) >= (t_FPacketC::C_1 - edgeThresholdC));

            const t_MaskC thresholdMask = thresholdMaskU | thresholdMaskV | thresholdMaskW;

            activeMask(i) = aRayMask(i) & thresholdMask;
            inactiveMask(i) = aRayMask(i) & (~thresholdMask);

            activeMask.updateActiveContainerEndIndex(i);
            inactiveMask.updateActiveContainerEndIndex(i);
        }

        if(aRayPacket.depth < mMaxDepth && !inactiveMask.isFalse())
        {
            RayPacket<taSize> newRays = aRayPacket;
            ++newRays.depth;

            newRays.tMin = aIntersection.dist + t_FPacket::C_RAY_EPS;

            aContext.template trace<taCommonOrg, taStoreAlpha>(newRays, inactiveMask, oResult, oResultAlpha);
        }

        if(!activeMask.isFalse())
        {
            t_Vec3f diffuseColor;

            const t_Vec3fC diffColC = t_Vec3fC::replicate(mDiffuseColor);
            const t_Vec3fC emissiveColC = t_Vec3fC::replicate(mEmissiveColor);
            const t_Vec3fC ambientIntC = t_Vec3fC::replicate(mAmbientIntensity);
            t_Vec3f hitPoint;

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, activeMask)
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
                    diffuseColor(i) = aIntersection.vertexColor(i);
                }
                else
                {
                    diffuseColor(i) = diffColC;
                }

                oResult(i) = activeMask(i).iif(
                    emissiveColC + ambientIntC * diffuseColor(i),
                    oResult(i));
                
                if (
                    taStoreAlpha)
                {
                    (*oResultAlpha)(i) = activeMask(i).iff(
                        t_FPacketC::C_1,
                        (*oResultAlpha)(i));
                }

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

            for(uint l = 0; l < aContext.scene.lights.size(); ++l)
            {
                RayPacket<taSize> shadowRayPacket;
                typename tContext::Intersector::template Result<taSize> shadowInters;

                t_Vec3f directionToLight;
                t_Vec3f &directionToLightRef = directionToLight;
                t_FPacket invProb;
                t_Vec3f intensity;
                t_FPacket dotLightNormal;
                t_ActiveMask shadowRayMask = t_Mask::C_FALSE();

                if(aContext.scene.lights[l]->getHasCommonSamplePosition())
                {
                    aContext.scene.lights[l]->sample(
                        hitPoint, directionToLight,
                        shadowRayPacket.tMax, intensity, activeMask, &shadowRayPacket.org(0));

                    RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, activeMask)
                    {
                        shadowRayPacket.tMax(i) -= t_FPacketC::C_RAY_EPS;
                        shadowRayPacket.tMin(i) = t_FPacketC::C_RAY_EPS;
                        shadowRayPacket.dir(i) = -directionToLight(i);
                        shadowRayPacket.invDir(i) = shadowRayPacket.dir(i).reciprocal();

                        dotLightNormal(i) = Math::dot(directionToLight(i), aIntersection.normal(i));

                        shadowRayMask(i) =
                            activeMask(i) &
                            (intensity(i) != t_Vec3fC::C_0_0_0()) &
                            (dotLightNormal(i) > t_FPacketC::C_0());
                    }

                    shadowRayMask.updateActiveContainers();

                    if(!aContext.scene.lights[l]->getCastsShadows())
                    {
                        shadowInters.mask.setFalseAndUpdateActiveContainers();
                    }
                    else
                    {
                        aContext.template intersect<IsectData::NONE, true>(
                            shadowRayPacket, shadowRayMask, shadowInters);
                    }
                }
                else
                {
                    aContext.scene.lights[l]->sample(
                        hitPoint, shadowRayPacket.dir,
                        shadowRayPacket.tMax, intensity, activeMask);

                    directionToLightRef = shadowRayPacket.dir;

                    RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, activeMask)
                    {
                        shadowRayPacket.org(i) = hitPoint(i);
                        shadowRayPacket.tMax(i) -= t_FPacketC::C_RAY_EPS;
                        shadowRayPacket.tMin(i) = t_FPacketC::C_RAY_EPS;
                        shadowRayPacket.invDir(i) = shadowRayPacket.dir(i).reciprocal();

                        dotLightNormal(i) = Math::dot(directionToLightRef(i), aIntersection.normal(i));

                        shadowRayMask(i) =
                            activeMask(i) &
                            (intensity(i) != t_Vec3fC::C_0_0_0()) &
                            (dotLightNormal(i) > t_FPacketC::C_0());
                    }

                    shadowRayMask.updateActiveContainers();

                    aContext.template intersect<IsectData::NONE, false>(
                        shadowRayPacket, shadowRayMask, shadowInters);
                }

                shadowInters.mask.clearActiveContainerEndIndex();

                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, shadowRayMask)
                {
                    shadowInters.mask(i) = shadowRayMask(i) & ~shadowInters.mask(i);
                    shadowInters.mask.updateActiveContainerEndIndex(i);
                }

                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, shadowInters.mask)
                {
                    oResult(i) = shadowInters.mask(i).iif(
                        oResult(i) + diffuseColor(i) * intensity(i) * dotLightNormal(i),
                        oResult(i));
                }
            }
        }
    }

    #include RTFACT_SURFACE_SHADER_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__WIREFRAMESHADER_HPP
