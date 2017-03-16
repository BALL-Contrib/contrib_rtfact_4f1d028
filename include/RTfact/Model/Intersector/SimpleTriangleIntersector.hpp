/* 
 *  SimpleTriangleIntersector.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2008  Saarland University
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
 
 #ifndef RTFACT__SIMPLETRIANGLEINTERSECTOR_HPP
#define RTFACT__SIMPLETRIANGLEINTERSECTOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Intersector.hpp>
#include <RTfact/Utils/Packets/Packet.hpp>
#include <RTfact/Utils/Packets/Vec3f.hpp>

RTFACT_NAMESPACE_BEGIN

class SimpleTriangleIntersector :
    public Intersector
{
public:

    template<
        umask taIsectData,
        uint taSize>
    RTFACT_INTERSECTOR_INLINE void intersect(
        const RayPacket<taSize>& aRayPacket,
        const int aTriangleID,
        const Scene& aScene,
        SurfaceIntersection<taSize>& oResult) const
    {
        typedef Packet<taSize, float>         t_FPacket;
        typedef typename t_FPacket::Container t_FPacketC;
        typedef Packet<taSize, int>           t_IPacket;
        typedef typename t_IPacket::Container t_IPacketC;
        typedef Mask<taSize>                  t_Mask;
        typedef BitMask<taSize>               t_BitMask;
        typedef Vec3f<taSize>                 t_Vec3f;
        typedef typename t_Vec3f::Container   t_Vec3fC;

        const Triangle& triangle = aScene.primitives[aTriangleID];

        const t_Vec3fC vertices[0] = t_Vec3fC::replicate(triangle.vertices[0]);

        const t_Vec3fC edge1 =
            t_Vec3fC::replicate(triangle.vertices[1] - vertices[0]);

        const t_Vec3fC edge2 =
            t_Vec3fC::replicate(triangle.vertices[2] - vertices[0]);

        t_Vec3f pvec;
        t_FPacket det;
        t_Mask mask;
        t_BitMask intersectionBitMask;
        intersectionBitMask.setFalse();

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            pvec(i) = Math::cross(aRayPacket.dir(i), edge2);
            det(i) = Math::dot(edge1, pvec(i));

            mask(i) = (Math::abs(det(i)) >= t_FPacketC::C_RAY_EPS);
            intersectionBitMask.setContainer(i, mask(i).getIntMask());
        }
    
        if(intersectionBitMask.isFalse())
        {
            return;
        }

        t_FPacket inv_det;
        t_Vec3f tvec;
        t_FPacket lambda;
        t_BitMask lambdaBitMask;
        lambdaBitMask.setFalse();

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            inv_det(i) = Math::reciprocal(det(i));
            tvec(i) = aRayPacket.org(i) - vertices[0];
            lambda(i) = inv_det(i) * Math::dot(tvec(i), pvec(i));

            mask(i) &= (lambda(i) >= t_FPacketC::C_0() &
                        lambda(i) <= t_FPacketC::C_1);

            lambdaBitMask.setContainer(i, mask(i).getIntMask());
        }

        if(lambdaBitMask.isFalse())
        {
            return;
        }

        t_Vec3f qvec;
        t_FPacket mue;
        t_BitMask mueBitMask;
        mueBitMask.setFalse();

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            qvec(i) = Math::cross(tvec(i), edge1);
            mue(i) = inv_det(i) * Math::dot(aRayPacket.dir(i), qvec(i));

            mask(i) &= (mue(i) >= t_FPacketC::C_0() &
                        (mue(i) + lambda(i)) <= t_FPacketC::C_1);

            mueBitMask.setContainer(i, mask(i).getIntMask());
        }
        
        if(mueBitMask.isFalse())
        {
            return;
        }

        t_FPacket f;
        t_BitMask finalBitMask;
        finalBitMask.setFalse();

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            f(i) = inv_det(i) * Math::dot(edge2, qvec(i));

            mask(i) &= (f(i) >= t_FPacketC::C_RAY_EPS &
                        f(i) < oResult.dist(i));

            finalBitMask.setContainer(i, mask(i).getIntMask());
        }

        if(finalBitMask.isFalse())
        {
            return;
        }

        if(taIsectData)
        {
            const t_Vec3fC normal =
                Math::cross(edge1, edge2).normalize();

            const t_IPacketC triangleID =
                t_IPacketC::replicate(aTriangleID);

            RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
            {
                oResult.mask(i)       |= mask(i);

                oResult.dist(i)        = mask(i).iif(f(i),
                                                           oResult.dist(i));
                oResult.u(i)           = mask(i).iif(lambda(i),
                                                           oResult.u(i));
                oResult.v(i)           = mask(i).iif(mue(i),
                                                           oResult.v(i));
                oResult.appearanceID(i) = mask(i).iif(triangleID,
                                                           oResult.appearanceID(i));
                oResult.normal(i)      = mask(i).iif(normal,
                                                           oResult.normal(i));
            }
        }
        else
        {
            RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
            {
                oResult.mask(i) |= mask(i) & (f(i) < aRayPacket.tMax(i));
            }
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__SIMPLETRIANGLEINTERSECTOR_HPP
