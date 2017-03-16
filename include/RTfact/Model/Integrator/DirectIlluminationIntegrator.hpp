/*
 *  DirectIlluminationIntegrator.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2009  Saarland University
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
 *  Created on: 2008-06-09 17:30:26 +0200
 *  Author(s): Iliyan Georgiev
 */

 #ifndef RTFACT__DIRECTILLUMINATIONINTEGRATOR_HPP
#define RTFACT__DIRECTILLUMINATIONINTEGRATOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Containers/Array.hpp>

#include <RTfact/Concept/Integrator.hpp>

RTFACT_NAMESPACE_BEGIN

class DirectIlluminationIntegrator :
    public Integrator
{
    enum
    {
        INTERSECTION_DATA =
            IsectData::DISTANCE |
            IsectData::NORMAL |
            IsectData::TEX_COORDS |
            IsectData::APPEARANCE_ID
    };

public:

    float epsilon;
    bool errors, finished;

    template<
        bool taCommonOrg,
        uint taSize,
        class tShadingInfoScene,
        class tPrimitive,
        class tIntersector>
    RTFACT_INTEGRATOR_INLINE void evaluate(
        const PixelSampler::Sample<taSize>&,
        RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        const tShadingInfoScene& aScene,
        tPrimitive& aPrimitive,
        tIntersector& aIntersector,
        FBData<taSize>& oResult)
    {
        typedef RayPacket<taSize>             t_RayPacket;
        typedef Vec3f<taSize>                 t_Vec3f;
        typedef typename t_Vec3f::Container   t_Vec3fC;
        typedef Packet<taSize, float>         t_FPacket;
        typedef typename t_FPacket::Container t_FPacketC;
        typedef Packet<taSize, int>           t_IPacket;
        typedef typename t_IPacket::Container t_IPacketC;
        typedef Mask<taSize>                  t_Mask;
        typedef ActiveMask<taSize>            t_ActiveMask;
        typedef BitMask<taSize>               t_BitMask;

        RTFACT_INTEGRATOR_ASSERT(
            RTFACT_CHECK_UMASK(oResult.supportedData, FBSupportedData::COLOR));

        typename tIntersector::template Result<taSize> intersection;

        Intersector::intersect<INTERSECTION_DATA, taCommonOrg>(
            aIntersector, aRayPacket, aRayMask, aPrimitive, intersection);

        if(!intersection.mask.isFalse())
        {
            finished = false;
        }

        oResult.color = t_Vec3f::C_0_0_0();

        if(!intersection.mask.isFalse())
        {
            const t_Vec3f hitPoint = aRayPacket.evaluate<taCommonOrg>(
                intersection.dist, intersection.mask);;

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, intersection.mask)
            {
                intersection.normal(i) =
                    (Math::dot(intersection.normal(i), aRayPacket.dir(i)) >
                     t_FPacketC::C_0()).iif(
                        -intersection.normal(i),
                         intersection.normal(i));

            }

            const t_Vec3f rayOppDir = -aRayPacket.dir;

            Array<taSize, int> uniqueMats;
            typename Array<taSize, int>::iterator uniqueMatEnd =
                intersection.appearanceID.getUniqueValues(
                    uniqueMats.begin(), uniqueMats.end(), intersection.mask);

            for(uint l = 0; l < aScene.lights.size(); ++l)
            {
                t_Vec3f reflection, emission;

                for(typename Array<taSize, int>::iterator materialId = uniqueMats.begin();
                    materialId != uniqueMatEnd; ++materialId)
                {
                    t_IPacketC materialIdC = t_IPacketC::replicate(*materialId);

                    t_ActiveMask materialMask;
                    t_Vec3f reflDir, reflColor;
                    t_RayPacket reflRay;

                    /*if(*materialId == 13)
                    {
                        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
                        {
                            materialMask(i) = intersection.mask(i) &
                                (intersection.appearanceID(i) == materialIdC);

                            reflRay.dir(i) = aRayPacket.dir(i) - 2.f * Math::dot(aRayPacket.dir(i), intersection.normal(i)) * intersection.normal(i);
                            reflRay.dir(i).setNormalized();
                            reflRay.invDir(i) = reflRay.dir(i).reciprocal();
                            reflRay.org(i) = hitPoint(i);
                            reflRay.tMin(i) = t_FPacketC::C_RAY_EPS;
                            reflRay.tMax(i) = t_FPacketC::C_INFINITY;
                        }

                        materialMask.updateActiveContainers();

                        typename tIntersector::template Result<taSize> reflInters;

                        Intersector::intersect<INTERSECTION_DATA, false>(
                            aIntersector, reflRay, materialMask, aPrimitive, reflInters);

                        Array<taSize, int> uniqueReflMats;
                        typename Array<taSize, int>::iterator uniqueReflMatEnd =
                            reflInters.appearanceID.getUniqueValues(
                            uniqueReflMats.begin(), uniqueReflMats.end(), reflInters.mask);

                        t_Vec3f reflReflection = t_Vec3f::C_0_0_0();

                        for(typename Array<taSize, int>::iterator reflMaterialId = uniqueReflMats.begin();
                            reflMaterialId != uniqueReflMatEnd; ++reflMaterialId)
                        {
                            t_IPacketC reflMaterialIdC = t_IPacketC::replicate(*reflMaterialId);

                            t_ActiveMask reflMaterialMask;

                            RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
                            {
                                reflMaterialMask(i) = materialMask(i) & reflInters.mask(i) &
                                    (reflInters.appearanceID(i) == reflMaterialIdC);
                            }

                            reflMaterialMask.updateActiveContainers();

                            aScene.appearances[*reflMaterialId]->evaluate<taSize>(
                                rayOppDir, rayOppDir, reflInters, reflMaterialMask, reflReflection);
                        }

                        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
                        {
                            oResult.color(i) = reflInters.mask(i).iif(reflReflection(i) + oResult.color(i), oResult.color(i));
                        }

                        return;
                    }
                    else*/
                    {
                        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
                        {
                            materialMask(i) = intersection.mask(i) &
                                (intersection.appearanceID(i) == materialIdC);
                        }

                        materialMask.updateActiveContainers();
                    }

                    aScene.appearances[*materialId]->template evaluate<taSize>(
                        rayOppDir, rayOppDir, intersection, materialMask, reflection);

                    aScene.appearances[*materialId]->template emittance<taSize>(
                        rayOppDir, intersection, materialMask, emission);
                }

                t_RayPacket shadowRayPacket;
                t_ActiveMask shadowRayMask;
                t_Vec3f directionToLight;
                t_FPacket dotLightNormal;
                t_Vec3f intensity;

                typename tIntersector::template Result<taSize> shadowInters;

                if(aScene.lights[l]->getHasCommonSamplePosition())
                {
                    Vec3f1 shadowRayOrg;

                    aScene.lights[l]->illuminatePoint(
                        Math::uniformFloat2D<taSize>(),
                        hitPoint, aRayMask, directionToLight,
                        shadowRayPacket.tMax, intensity, &shadowRayOrg);

                    shadowRayMask.clearActiveContainerEndIndex();

                    if(aScene.lights[l]->getCastsShadows())
                    {
                        shadowRayPacket.org = t_Vec3f::replicate(shadowRayOrg);

                        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
                        {
                            shadowRayPacket.tMax(i) -= t_FPacketC::C_RAY_EPS;
                            shadowRayPacket.tMin(i) = t_FPacketC::C_RAY_EPS;
                            shadowRayPacket.dir(i) = -directionToLight(i);
                            shadowRayPacket.invDir(i) = shadowRayPacket.dir(i).reciprocal();

                            dotLightNormal(i) = Math::dot(directionToLight(i), intersection.normal(i));

                            shadowRayMask(i) =
                                aRayMask(i) &
                                (intensity(i) != t_Vec3fC::C_0_0_0()) &
                                (dotLightNormal(i) > t_FPacketC::C_0());

                            shadowRayMask.updateActiveContainerEndIndex(i);
                        }

                        if(shadowRayMask.isFalse())
                        {
                            continue;
                        }

                        Intersector::intersect<IsectData::NONE, true>(
                            aIntersector, shadowRayPacket, shadowRayMask,
                            aPrimitive, shadowInters);
                    }
                    else
                    {
                        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
                        {
                            dotLightNormal(i) = Math::dot(directionToLight(i), intersection.normal(i));

                            shadowRayMask(i) =
                                aRayMask(i) &
                                (intensity(i) != t_Vec3fC::C_0_0_0()) &
                                (dotLightNormal(i) > t_FPacketC::C_0());

                            shadowRayMask.updateActiveContainerEndIndex(i);
                        }

                        shadowInters.mask.setFalseAndUpdateActiveContainers();
                    }
                }
                else
                {
                    aScene.lights[l]->illuminatePoint(
                        Math::uniformFloat2D<taSize>(),
                        hitPoint, aRayMask, shadowRayPacket.dir,
                        shadowRayPacket.tMax, intensity);

                    shadowRayMask.clearActiveContainerEndIndex();

                    if(aScene.lights[l]->getCastsShadows())
                    {
                        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
                        {
                            shadowRayPacket.org(i) = hitPoint(i);
                            shadowRayPacket.tMax(i) -= t_FPacketC::C_RAY_EPS;
                            shadowRayPacket.tMin(i) = t_FPacketC::C_RAY_EPS;
                            shadowRayPacket.invDir(i) = shadowRayPacket.dir(i).reciprocal();

                            dotLightNormal(i) = Math::dot(shadowRayPacket.dir(i), intersection.normal(i));

                            shadowRayMask(i) =
                                aRayMask(i) &
                                (intensity(i) != t_Vec3fC::C_0_0_0()) &
                                (dotLightNormal(i) > t_FPacketC::C_0());

                            shadowRayMask.updateActiveContainerEndIndex(i);
                        }

                        if(shadowRayMask.isFalse())
                        {
                            continue;
                        }

                        Intersector::intersect<IsectData::NONE, false>(
                            aIntersector, shadowRayPacket, shadowRayMask,
                            aPrimitive, shadowInters);
                    }
                    else
                    {
                        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
                        {
                            dotLightNormal(i) = Math::dot(directionToLight(i), intersection.normal(i));

                            shadowRayMask(i) =
                                aRayMask(i) &
                                (intensity(i) != t_Vec3fC::C_0_0_0()) &
                                (dotLightNormal(i) > t_FPacketC::C_0());

                            shadowRayMask.updateActiveContainerEndIndex(i);
                        }

                        shadowInters.mask.setFalseAndUpdateActiveContainers();
                    }
                }

                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, intersection.mask)
                {
                    shadowRayMask(i) &= ~shadowInters.mask(i);

                    oResult.color(i) = shadowRayMask(i).iif(
                        oResult.color(i) + dotLightNormal(i) * intensity(i) * reflection(i),
                        oResult.color(i));

                    /*if(shadowRayPacket.frustum.bad)
                    {
                        oResult.color(i) *= t_Vec3fC::replicate(0.4, 0.4, 0.4);
                    }*/
                }
            }
        }

        intersection.mask = (~intersection.mask) & aRayMask;
        intersection.mask.updateActiveContainers();

        aScene.environmentTexture.template sample<false, taCommonOrg>(
            aRayPacket, intersection.mask, oResult.color);

        if(RTFACT_CHECK_UMASK(oResult.supportedData, FBSupportedData::DISTANCE))
        {
            oResult.distance = intersection.dist;
        }
    }

    /*template<class tCamera, class tPrimitive, class tIntersector>
    RTFACT_INTEGRATOR_INLINE Vec3f<1> evaluate(PixelSampler::Sample<1>& aSample, RayPacket<1>& aRayPacket, tCamera& aCamera,
        Scene& aScene, tPrimitive& aPrimitive, const tIntersector& aIntersector)
    {
        typedef Vec3f<1>         t_Vec3f;
        typedef Packet<1, float> t_FPacket;
        typedef Mask<1>          t_Mask;
        typedef RayPacket<1>     t_RayPacket;

        SurfaceIntersection<1> intersection = aIntersector.intersect(aRayPacket, aPrimitive, aScene);

        if(!intersection.mask.isFalse())
        {
            t_Vec3f color = t_Vec3f::C_0_0_0();

            intersection.normal = (Math::dot(intersection.normal, aRayPacket.dir) > 0.f).iif(
                    -intersection.normal,
                    intersection.normal);

            for(uint l = 0; l < aScene.lights.size(); ++l)
            {
                t_RayPacket shadowRayPacket;
                t_Vec3f directionToLight;

                const t_Vec3f intensity = aScene.lights[l]->sample(aRayPacket.evaluate(intersection.dist), shadowRayPacket, directionToLight);

                const t_FPacket dotNormal = Math::dot(directionToLight, intersection.normal);

                t_Mask mask = intersection.mask & (intensity != t_Vec3f::C_0_0_0()) & (dotNormal > 0.f);

                if(mask.isFalse())
                {
                    continue;
                }

                const int materialID = aScene.primitives[intersection.appearanceID].appearanceID;
                const t_Vec3f rayOppDir = -aRayPacket.dir;

                const t_Vec3f reflection = aScene.appearances[materialID]->evaluate<1>(rayOppDir, directionToLight, intersection);
                const t_Vec3f emission = aScene.appearances[materialID]->emittance<1>(rayOppDir, intersection);

                mask &= ((reflection + emission) != t_Vec3f::C_0_0_0());

                if(mask.isTrue())
                {
                    color += emission + aIntersector.intersects(shadowRayPacket, aPrimitive, aScene).iif(
                        t_Vec3f::C_0_0_0(),
                        dotNormal * intensity * reflection);
                }
            }

            return color;
        }
        else
        {
            return t_Vec3f::C_0_0_0();
        }
    }*/
};

RTFACT_NAMESPACE_END

#endif // RTFACT__DIRECTILLUMINATIONINTEGRATOR_HPP
