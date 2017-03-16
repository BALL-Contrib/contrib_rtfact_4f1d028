/* 
 *  XML3DGlassShader.hpp
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
 *  Author(s): Felix Klein
 */
 
 #ifndef RTFACT__XML3DGLASSSHADER_HPP
#define RTFACT__XML3DGLASSSHADER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Appearance/SurfaceShader.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tContext>
class XML3DGlassShader :
    public SurfaceShader<tContext>
{
    float mNormalReflectance;
    uint mMaxDepth;

public:

    XML3DGlassShader() :
        mNormalReflectance(0.1f),
        mMaxDepth(3)
    {}

    RTFACT_DEFINE_PROPERTY(float, NormalReflectance);
    RTFACT_DEFINE_PROPERTY(uint,  MaxDepth);

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

        const t_Vec3fC zero = t_Vec3fC::C_0_0_0();

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        {
            oResult(i) = aRayMask(i).iif(zero, oResult(i));
        }

        if(aRayPacket.depth < mMaxDepth)
        {
            t_FPacket reflCoeff;
            t_FPacket dotNormal;

            const t_FPacketC one = t_FPacketC::replicate(1.f);
            const t_FPacketC normalReflectanceC =
                t_FPacketC::replicate(mNormalReflectance);

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
            {
                dotNormal(i) = Math::dot(
                    aRayPacket.dir(i), aIntersection.normal(i));

                if(RTFACT_CHECK_UMASK(tContext::Scene::PROPERTIES, SceneProperties::INVERT_NORMAL))
                {
                    aIntersection.normal(i) = (dotNormal(i) > t_FPacketC::C_0()).iif(
                        -aIntersection.normal(i),
                        aIntersection.normal(i));
                }

				dotNormal(i) = -Math::dot(
                    aRayPacket.dir(i), aIntersection.normal(i));

                const t_FPacketC oneMinusCosTheta = one - dotNormal(i);
                const t_FPacketC oneMinusCosTheta2 = oneMinusCosTheta * oneMinusCosTheta;
                const t_FPacketC oneMinusCosTheta5 =
                    oneMinusCosTheta2 * oneMinusCosTheta2 * oneMinusCosTheta;

                reflCoeff(i) =
                    normalReflectanceC +
                    (one - normalReflectanceC) * oneMinusCosTheta5;
            }

            if(!aRayPacket.inside)
            {
                t_Vec3f hitPoint;
                RayPacket<taSize> reflRay;

                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
                {
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

                    reflRay.dir(i) = aRayPacket.dir(i) + 2.f * dotNormal(i) * aIntersection.normal(i);
                    reflRay.dir(i).setNormalized();
                    reflRay.invDir(i) = reflRay.dir(i).reciprocal();
                    reflRay.org(i) = hitPoint(i);
                    reflRay.tMin(i) = t_FPacketC::C_RAY_EPS;
                    reflRay.tMax(i) = t_FPacketC::C_INFINITY;
                }

                reflRay.depth = aRayPacket.depth + 1;

                t_Vec3f reflectedColor = t_Vec3f::C_0_0_0();

                aContext.template trace<false, false>(reflRay, aRayMask, reflectedColor);

                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
                {
                    oResult(i) = aRayMask(i).iif(
                        oResult(i) + reflCoeff(i) *  reflectedColor(i),
                        oResult(i));
                }
            }
			else
            {
                reflCoeff = t_FPacket::C_0();
            }

            t_Vec3f transparentColor;
            t_FPacket transparentAlpha;

            RayPacket<taSize> newRays = aRayPacket;
            newRays.inside ^= true;
            ++newRays.depth;

            newRays.tMin = aIntersection.dist + t_FPacket::C_RAY_EPS;

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

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
            {
                oResult(i) = aRayMask(i).iif(
                    oResult(i) + (one - reflCoeff(i)) * transparentColor(i),
                    oResult(i));
                if (
                    taStoreAlpha)
                {
                    // 1 - resA = (1 - rC) * (1 - trA) <=> resA = rC + trA - rC*trA
                    (*oResultAlpha)(i) = aRayMask(i).iif(
                        reflCoeff(i) + transparentAlpha(i) - reflCoeff(i) * transparentAlpha(i),
                        (*oResultAlpha)(i));
                }
            }
        }
        else
        {
            if (
                taStoreAlpha)
            {
                const t_FPacketC zeroAlpha = t_FPacketC::C_0();
                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
                {
                    (*oResultAlpha)(i) = aRayMask(i).iif(zeroAlpha, (*oResultAlpha)(i));
                }
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
        typedef Packet<taSize, float>            t_FPacket;
        typedef typename t_FPacket::Container    t_FPacketC;

        const t_FPacketC alpha = t_FPacketC::replicate(1.f - mNormalReflectance);

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            oIntensity(i) = aMask(i).iif(alpha * oIntensity(i), oIntensity(i));
        }
    }

    #include RTFACT_SURFACE_SHADER_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__XML3DGLASSSHADER_HPP
