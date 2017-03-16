/* 
 *  ReshetovTriangleIntersector.hpp
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
 *  Created on: 2008-11-21 17:06:44 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__RESHETOVTRIANGLEINTERSECTOR_HPP
#define RTFACT__RESHETOVTRIANGLEINTERSECTOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Containers/Array.hpp>
#include <RTfact/Utils/Packets/Packet.hpp>
#include <RTfact/Utils/Packets/Vec3f.hpp>

#include <RTfact/Concept/Intersector/BasicIntersector.hpp>
#include <RTfact/Model/Primitive/Triangle.hpp>

RTFACT_NAMESPACE_BEGIN

class ReshetovTriangleIntersector :
    public BasicIntersector<Triangle>
{
public:

    template<
        umask taIsectData,
        bool taCommonOrg,
        uint taSize>
    RTFACT_INTERSECTOR_INLINE void intersect(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        Triangle& aTriangle,
        Result<taSize>& oResult,
        uint aI) const
    {
        typedef typename Packet<taSize, float>::Container t_FPacketC;
        typedef typename Packet<taSize, int>::Container   t_IPacketC;
        typedef typename Vec3f<taSize>::Container         t_Vec3fC;
        typedef typename Mask<taSize>::Container          t_MaskC;

        const Vec3f1 edge20 = aTriangle.getVertex(2) - aTriangle.getVertex(0);
        const Vec3f1 edge10 = aTriangle.getVertex(1) - aTriangle.getVertex(0);

        const Vec3f1 normalN = Math::cross(edge10, edge20);
        const t_Vec3fC normalNC = t_Vec3fC::replicate(normalN);

        const t_FPacketC denom = Math::dot(normalNC, aRayPacket.frustum.dir);

        if((denom >= t_FPacketC::C_0()).isTrue())
        {
            return;
        }

        const t_FPacketC denomRecip = Math::reciprocal(denom);

        const t_Vec3fC edge02C = t_Vec3fC::replicate(-edge20);
        const t_Vec3fC edge10C = t_Vec3fC::replicate(edge10);
        const t_Vec3fC org0 = aTriangle.getVertex(0) - aRayPacket.frustum.org;

        const t_Vec3fC crosDirOrg0 = Math::cross(aRayPacket.frustum.dir, org0);

        const t_FPacketC uFrustum = Math::dot(edge02C, crosDirOrg0) * denomRecip;
        const t_MaskC uFrustumCond0 = (uFrustum <= t_FPacketC::C_0());
        const t_MaskC uFrustumCond1 = (uFrustum >= t_FPacketC::C_1);

        if(uFrustumCond0.isTrue() | uFrustumCond1.isTrue())
        {
            return;
        }

        const t_FPacketC vFrustum = Math::dot(edge10C, crosDirOrg0) * denomRecip;
        const t_MaskC vFrustumCond0 = (vFrustum <= t_FPacketC::C_0());
        const t_MaskC vFrustumCond1 = ((uFrustum + vFrustum) >= t_FPacketC::C_1);

        if(vFrustumCond0.isTrue() | vFrustumCond1.isTrue())
        {
            return;
        }

        const t_FPacketC distTerm = Math::dot(normalNC, org0);

        t_Vec3fC normalCNormalized;
        t_IPacketC offset;

        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::GEOM_NORMAL))
        {
            normalCNormalized = normalNC.normalize();
        }

        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::U_V))
        {
            offset = t_IPacketC::replicate(oResult.getOffset(aTriangle));
        }

        for(uint i = aI >> 2; i < Packet<taSize, float>::CONTAINER_COUNT; ++i)
        {
            const t_FPacketC denom1 = Math::dot(normalNC, aRayPacket.dir(i));

            if((denom1 >= t_FPacketC::C_0()).isTrue())
            {
                continue;
            }

            const t_FPacketC denom1Recip = Math::reciprocal(denom1);
            const t_Vec3fC dirCrossOrg0 = Math::cross(aRayPacket.dir(i), org0);

            const t_FPacketC u = Math::dot(edge02C, dirCrossOrg0) * denom1Recip;
            const t_MaskC uCond = (u >= t_FPacketC::C_0()) & (u <= t_FPacketC::C_1);

            if(uCond.isFalse())
            {
                continue;
            }

            const t_FPacketC v = Math::dot(edge10C, dirCrossOrg0) * denom1Recip;
            const t_MaskC vCond = (v >= t_FPacketC::C_0()) & ((u + v) <= t_FPacketC::C_1);

            t_MaskC mask = uCond & vCond;

            if(mask.isFalse())
            {
                continue;
            }

            const t_FPacketC dist = distTerm * denom1Recip;

            mask &= (dist < Math::min(aRayPacket.tMax(i), oResult.dist(i)) &
                     dist > aRayPacket.tMin(i));

            if(mask.isFalse())
            {
                continue;
            }
            
            oResult.mask(i) |= mask;

            if(!RTFACT_CHECK_UMASK(taIsectData, IsectData::NONE))
            {
                if(RTFACT_CHECK_UMASK(taIsectData, IsectData::DISTANCE))
                {
                    oResult.dist(i) = mask.iif(dist, oResult.dist(i));
                }

                if(RTFACT_CHECK_UMASK(taIsectData, IsectData::U_V))
                {
                    oResult.primitiveOffset(i) =
                        mask.iif(offset, oResult.primitiveOffset(i));
                }

                if(RTFACT_CHECK_UMASK(taIsectData, IsectData::GEOM_NORMAL))
                {
                    oResult.geomNormal(i) = mask.iif(normalCNormalized, oResult.geomNormal(i));

                    if(!RTFACT_CHECK_UMASK(taIsectData, IsectData::NORMAL))
                    {
                        oResult.normal(i) = oResult.geomNormal(i);
                    }
                }

                if(RTFACT_CHECK_UMASK(taIsectData, IsectData::U_V))
                {
                    oResult.u(i) = mask.iif(u, oResult.u(i));
                    oResult.v(i) = mask.iif(v, oResult.v(i));
                }

                if(RTFACT_CHECK_UMASK(taIsectData, IsectData::APPEARANCE_ID))
                {
                    oResult.appearanceID(i) = mask.iif(
                        t_IPacketC::replicate(aTriangle.appearanceID),
                        oResult.appearanceID(i));
                }
            }
        }
    }

    template<
        umask taIsectData,
        bool taCommonOrg>
    RTFACT_INTERSECTOR_INLINE void intersect(
        const RayPacket<1>& aRay,
        const ActiveMask<1>& aRayMask,
        Triangle& aTriangle,
        Result<1>& oResult,
        uint aI) const
    {
        const Vec3f1 edge20 = aTriangle.getVertex(2) - aTriangle.getVertex(0);
        const Vec3f1 edge10 = aTriangle.getVertex(1) - aTriangle.getVertex(0);

        const Vec3f1 normal = Math::cross(edge10, edge20);

        const float den = Math::dot(normal, aRay.dir);

        if(den >= 0.f)
        {
            return;
        }

        const float deni = 1.f / den;

        const Vec3f1 org0 = aTriangle.getVertex(0) - aRay.org;

        const Vec3f1 cros = Math::cross(aRay.dir, org0);

        const float u = -Math::dot(edge20, cros) * deni;

        if(u < 0.f || u > 1.f)
        {
            return;
        }

        const float v = Math::dot(edge10, cros) * deni;

        if(v < 0.f || (u + v) > 1.f)
        {
            return;
        }

        const float dist = Math::dot(normal, org0) * deni;
        
        if((dist >= 0.f) && (dist <= oResult.dist) && (dist <= aRay.tMax))
        {
            oResult.mask = RTFACT_MASK_TRUE;

            if(!RTFACT_CHECK_UMASK(taIsectData, IsectData::NONE))
            {
                if(RTFACT_CHECK_UMASK(taIsectData, IsectData::DISTANCE))
                {
                    oResult.dist = dist;
                }

                if(RTFACT_CHECK_UMASK(taIsectData, IsectData::GEOM_NORMAL))
                {
                    oResult.geomNormal = aTriangle.getNormal();

                    if(!RTFACT_CHECK_UMASK(taIsectData, IsectData::NORMAL))
                    {
                        oResult.normal = aTriangle.getNormal();
                    }
                }

                if(RTFACT_CHECK_UMASK(taIsectData, IsectData::U_V))
                {
                    oResult.u = u;
                    oResult.v = v;
                }

                if(RTFACT_CHECK_UMASK(taIsectData, IsectData::APPEARANCE_ID))
                {
                    oResult.appearanceID = aTriangle.getAppearanceID();
                }
            }
        }
    }

    template<
        umask taIsectData,
        uint taSize>
    RTFACT_FORCE_INLINE void finalizeResult(
        const RayPacket<taSize>& aRayPacket,
        Result<taSize>& oResult)
    {
        typedef Packet<taSize, float>         t_Packet;
        typedef typename t_Packet::Container  t_PacketC;
        typedef Packet<taSize, int>           t_IPacket;
        typedef typename t_IPacket::Container t_IPacketC;
        typedef Vec3f<taSize>                 t_Vec3f;
        typedef typename t_Vec3f::Container   t_Vec3fC;
        typedef ActiveMask<taSize>      t_ActiveMask;

        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::NORMAL) ||
           RTFACT_CHECK_UMASK(taIsectData, IsectData::TEX_COORDS))
        {
            Array<taSize, int> uniquePrims;
            typename Array<taSize, int>::iterator uniquePrimEnd =
                oResult.primitiveOffset.getUniqueValues(
                    uniquePrims.begin(), uniquePrims.end(), oResult.mask);

            for(typename Array<taSize, int>::iterator primOffset = uniquePrims.begin();
                primOffset != uniquePrimEnd; ++primOffset)
            {
                const Triangle& triangle = oResult.getPrimitive(*primOffset);

                t_IPacketC primOffsetC = t_IPacketC::replicate(*primOffset);

                t_Vec3fC normal1C, normal2C, normal3C;
                t_PacketC texCoord11C, texCoord12C,
                          texCoord21C, texCoord22C,
                          texCoord31C, texCoord32C;

                if(RTFACT_CHECK_UMASK(taIsectData, IsectData::NORMAL))
                {
                    normal1C = t_Vec3fC::replicate(triangle.normals[0]);
                    normal2C = t_Vec3fC::replicate(triangle.normals[1]);
                    normal3C = t_Vec3fC::replicate(triangle.normals[2]);
                }

                if(RTFACT_CHECK_UMASK(taIsectData, IsectData::TEX_COORDS))
                {
                    texCoord11C = t_PacketC::replicate(triangle.texCoords[0].first);
                    texCoord12C = t_PacketC::replicate(triangle.texCoords[0].second);
                    texCoord21C = t_PacketC::replicate(triangle.texCoords[1].first);
                    texCoord22C = t_PacketC::replicate(triangle.texCoords[1].second);
                    texCoord31C = t_PacketC::replicate(triangle.texCoords[2].first);
                    texCoord32C = t_PacketC::replicate(triangle.texCoords[2].second);
                }

                t_ActiveMask primitiveMask;

                RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
                {
                    primitiveMask(i) = oResult.mask(i) & (oResult.primitiveOffset(i) == primOffsetC);
                }

                primitiveMask.updateActiveContainers();

                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, primitiveMask)
                {
                    const t_PacketC w = (t_PacketC::C_1 - oResult.u(i) - oResult.v(i));

                    if(RTFACT_CHECK_UMASK(taIsectData, IsectData::NORMAL))
                    {
                        const t_Vec3fC normal =
                            normal1C * w +
                            normal2C * oResult.u(i) +
                            normal3C * oResult.v(i);

                        oResult.normal(i) = primitiveMask(i).iif(
                            normal, oResult.normal(i));

                        oResult.normal(i).setNormalized();
                    }

                    if(RTFACT_CHECK_UMASK(taIsectData, IsectData::TEX_COORDS))
                    {
                        const t_PacketC texCoord1 =
                            texCoord11C * w +
                            texCoord21C * oResult.u(i) +
                            texCoord31C * oResult.v(i);

                        const t_PacketC texCoord2 =
                            texCoord12C * w +
                            texCoord22C * oResult.u(i) +
                            texCoord32C * oResult.v(i);

                        oResult.texCoord1(i) = primitiveMask(i).iif(
                            texCoord1, oResult.texCoord1(i));

                        oResult.texCoord2(i) = primitiveMask(i).iif(
                            texCoord2, oResult.texCoord2(i));
                    }
                }
            }
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__RESHETOVTRIANGLEINTERSECTOR_HPP
