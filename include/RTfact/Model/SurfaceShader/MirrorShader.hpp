/* 
 *  MirrorShader.hpp
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
 *  Created on: 2008-08-07 18:24:48 +0200
 *  Author(s): Iliyan Georgiev, Petr Kmoch
 */
 
 #ifndef RTFACT__MIRRORSHADER_HPP
#define RTFACT__MIRRORSHADER_HPP

#include <RTfact/Config/Common.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tContext>
class MirrorShader :
    public SurfaceShader<tContext>
{
public:

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

        if(aRayPacket.depth == 5)
        {
            return;
        }

        RayPacket<taSize> reflRay;

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        {
            aIntersection.normal(i) = 
                (Math::dot(aIntersection.normal(i), aRayPacket.dir(i)) >
                t_FPacketC::C_0()).iif(
                    aIntersection.normal(i),
                    -aIntersection.normal(i));

            reflRay.dir(i) = aRayPacket.dir(i) - 2.f * Math::dot(aRayPacket.dir(i), aIntersection.normal(i)) * aIntersection.normal(i);
            reflRay.dir(i).setNormalized();
            reflRay.invDir(i) = reflRay.dir(i).reciprocal();

            if(taCommonOrg)
            {
                reflRay.org(i) = aRayPacket.org(0) + (aIntersection.dist(i) - t_FPacketC::C_RAY_EPS) * aRayPacket.dir(i);
            }
            else
            {
                reflRay.org(i) = aRayPacket.org(i) + (aIntersection.dist(i) - t_FPacketC::C_RAY_EPS) * aRayPacket.dir(i);
            }

            reflRay.tMin(i) = t_FPacketC::C_RAY_EPS;
            reflRay.tMax(i) = t_FPacketC::C_INFINITY;
        }

        reflRay.depth = aRayPacket.depth + 1;

        aContext.trace<false, false>(reflRay, aRayMask, oResult);
        if (
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
        bool taCommonOrg,
        bool taStoreAlpha>
    RTFACT_APPEARANCE_INLINE void shade(
        const RayPacket<1>& aRayPacket,
        const ActiveMask<1>& aRayMask,
        SurfaceIntersection<1>& aIntersection,
        tContext& aContext,
        Vec3f<1>& oResult,
        Packet<1, float>* oResultAlpha = RTFACT_NULL) const
    {}

    #include RTFACT_SURFACE_SHADER_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__MIRRORSHADER_HPP
