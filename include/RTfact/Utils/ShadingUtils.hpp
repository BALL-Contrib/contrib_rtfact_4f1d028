/* 
 *  ShadingUtils.hpp
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
 *  Created on: 2009-12-23 17:35:48 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__SHADINGUTILS_HPP
#define RTFACT__SHADINGUTILS_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Light.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    uint taSize,
    class tContext>
bool getLightIntensity(
    const Vec3f<taSize>& aPoint,
    const Vec3f<taSize>& aNormal,
    const ActiveMask<taSize>& aRayMask,
    const Light& aLight,
    tContext& aContext,
    Vec3f<taSize>& oDirToLight,
    Vec3f<taSize>& oIntensity,
    Packet<taSize, float>& oDotLightNormal,
    ActiveMask<taSize>& oLightMask)
{
    typedef Vec3f<taSize>                 t_Vec3f;
    typedef Packet<taSize, float>         t_FPacket;
    typedef typename t_FPacket::Container t_FPacketC;
    typedef typename t_Vec3f::Container   t_Vec3fC;
    typedef Mask<taSize>                  t_Mask;
    typedef typename t_Mask::Container    t_MaskC;
    typedef ActiveMask<taSize>            t_ActiveMask;

    RayPacket<taSize> shadowRayPacket;
    typename tContext::Intersector::template Result<taSize> shadowInters;

    shadowRayPacket.rayType = RayType::ShadowRay;

    oLightMask.setFalseAndUpdateActiveContainers();

    if(aLight.getHasCommonSamplePosition())
    {
        Vec3f1 shadowRayOrg;

        aLight.illuminatePoint(
            Math::uniformFloat2D<taSize>(),
            aPoint, aRayMask, oDirToLight,
            shadowRayPacket.tMax, oIntensity, &shadowRayOrg);

        if(aLight.getCastsShadows())
        {
            t_ActiveMask shadowRayMask;
            shadowRayMask.setFalseAndUpdateActiveContainers();

            shadowRayPacket.org(0) = t_Vec3fC::replicate(shadowRayOrg);

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
            {
                shadowRayPacket.tMax(i) -= t_FPacketC::C_RAY_EPS;
                shadowRayPacket.tMin(i) = t_FPacketC::C_RAY_EPS;
                shadowRayPacket.dir(i) = -oDirToLight(i);
                shadowRayPacket.invDir(i) = shadowRayPacket.dir(i).reciprocal();

                oDotLightNormal(i) = Math::dot(oDirToLight(i), aNormal(i));

                shadowRayMask(i) =
                    aRayMask(i) &
                    (oIntensity(i) != t_Vec3fC::C_0_0_0()) &
                    (oDotLightNormal(i) > t_FPacketC::C_0());

                shadowRayMask.updateActiveContainerEndIndex(i);
            }

            if(shadowRayMask.isFalse())
            {
                return false;
            }

            aContext.template getLightContribution<true>(
                shadowRayPacket, shadowRayMask, oLightMask, oIntensity);
        }
        else
        {
            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
            {
                oDotLightNormal(i) = Math::dot(oDirToLight(i), aNormal(i));

                oLightMask(i) =
                    aRayMask(i) &
                    (oIntensity(i) != t_Vec3fC::C_0_0_0()) &
                    (oDotLightNormal(i) > t_FPacketC::C_0());

                oLightMask.updateActiveContainerEndIndex(i);
            }
        }
    }
    else
    {
        aLight.illuminatePoint(
            Math::uniformFloat2D<taSize>(),
            aPoint, aRayMask, shadowRayPacket.dir,
            shadowRayPacket.tMax, oIntensity);

        oDirToLight = shadowRayPacket.dir;

        if(aLight.getCastsShadows())
        {
            t_ActiveMask shadowRayMask;
            shadowRayMask.setFalseAndUpdateActiveContainers();

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
            {
                shadowRayPacket.org(i) = aPoint(i);
                shadowRayPacket.tMax(i) -= t_FPacketC::C_RAY_EPS;
                shadowRayPacket.tMin(i) = t_FPacketC::C_RAY_EPS;
                shadowRayPacket.invDir(i) = shadowRayPacket.dir(i).reciprocal();

                oDotLightNormal(i) = Math::dot(oDirToLight(i), aNormal(i));

                shadowRayMask(i) =
                    aRayMask(i) &
                    (oIntensity(i) != t_Vec3fC::C_0_0_0()) &
                    (oDotLightNormal(i) > t_FPacketC::C_0());

                shadowRayMask.updateActiveContainerEndIndex(i);
            }

            if(shadowRayMask.isFalse())
            {
                return false;
            }

            aContext.template getLightContribution<false>(
                shadowRayPacket, shadowRayMask, oLightMask, oIntensity);
        }
        else
        {
            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
            {
                oDotLightNormal(i) =
                    Math::dot(oDirToLight(i), aNormal(i));

                oLightMask(i) =
                    aRayMask(i) &
                    (oIntensity(i) != t_Vec3fC::C_0_0_0()) &
                    (oDotLightNormal(i) > t_FPacketC::C_0());

                oLightMask.updateActiveContainerEndIndex(i);
            }
        }
    }

    return !oLightMask.isFalse();
}

RTFACT_FORCE_INLINE const Vec3f1 faceForward(
    const Vec3f1& aNormal,
    const Vec3f1& aRayDir)
{
    return (Math::dot(aNormal, aRayDir) > 0.f) ? -aNormal : aNormal;
}

RTFACT_FORCE_INLINE const Vec3fC faceForward(
    const Vec3fC& aNormal,
    const Vec3fC& aRayDir)
{
    return (Math::dot(aNormal, aRayDir) > FPacketC::C_0()).iif(
        -aNormal, aNormal);
}

RTFACT_NAMESPACE_END

#endif // RTFACT__SHADINGUTILS_HPP
