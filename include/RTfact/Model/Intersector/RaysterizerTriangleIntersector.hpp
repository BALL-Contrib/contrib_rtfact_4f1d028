/* 
 *  RaysterizerTriangleIntersector.hpp
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
 *  Created on: 2008-11-27 16:51:10 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__RAYSTERIZERTRIANGLEINTERSECTOR_HPP
#define RTFACT__RAYSTERIZERTRIANGLEINTERSECTOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Containers/Array.hpp>
#include <RTfact/Utils/Packets/Packet.hpp>
#include <RTfact/Utils/Packets/Vec3f.hpp>

#include <RTfact/Concept/Intersector/BasicIntersector.hpp>
#include <RTfact/Model/Primitive/Triangle.hpp>


#include <iostream>
using namespace std;

RTFACT_NAMESPACE_BEGIN

class RaysterizerTriangleIntersector :
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
        uint aI = 0) const
    {
        if(taCommonOrg)
        {
            intersectCommonOrigin<taIsectData, taSize>(
                aRayPacket, aRayMask, aTriangle, oResult, aI);
        }
        else
        {
            intersectNonCommonOrigin<taIsectData, taSize>(
                aRayPacket, aRayMask, aTriangle, oResult, aI);
        }
    }

template<
        umask taIsectData,
        uint taSize>
    RTFACT_INTERSECTOR_INLINE void intersectCommonOrigin(
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

        const t_Vec3fC ao = aTriangle.vertices[0] - aRayPacket.org(0);
        const t_Vec3fC bo = aTriangle.vertices[1] - aRayPacket.org(0);
        const t_Vec3fC co = aTriangle.vertices[2] - aRayPacket.org(0);

        const t_Vec3fC normalC = t_Vec3fC::replicate(aTriangle.getNormal());
        const t_FPacketC nominator = Math::dot(ao, normalC);

        const uint mask = nominator.getSignIntMask() ^ RTFACT_SIMD_MASK_TRUE;

        const t_Vec3fC aStarDer = Math::cross(co, bo);
        const t_FPacketC aStarDerDotFDir = Math::dot(aStarDer, aRayPacket.frustum.dir);

        if(aStarDerDotFDir.getSignIntMask() == mask)
        {
            return;
        }

        const t_Vec3fC bStarDer = Math::cross(ao, co);
        const t_FPacketC bStarDerDotFDir = Math::dot(bStarDer, aRayPacket.frustum.dir);

        if(bStarDerDotFDir.getSignIntMask() == mask)
        {
            return;
        }

        const t_Vec3fC gStarDer = Math::cross(bo, ao);
        const t_FPacketC v1d = Math::dot(gStarDer, aRayPacket.frustum.dir);

        if(v1d.getSignIntMask() == mask)
        {
            return;
        }

        const t_IPacketC offset = t_IPacketC::replicate(oResult.getOffset(aTriangle));

        const t_Vec3fC totStarDer(
            aStarDer.x + bStarDer.x + gStarDer.x,
            aStarDer.y + bStarDer.y + gStarDer.y,
            aStarDer.z + bStarDer.z + gStarDer.z);

        for(uint i = aI >> 2; i < Packet<taSize, float>::CONTAINER_COUNT; ++i)
        {
            //volumes of the tetrahedrons
            const t_FPacketC aStar = Math::dot(aRayPacket.dir(i), aStarDer);
            const t_FPacketC bStar = Math::dot(aRayPacket.dir(i), bStarDer);
            const t_FPacketC gStar = Math::dot(aRayPacket.dir(i), gStarDer);

            const t_FPacketC totStar = Math::dot(aRayPacket.dir(i), totStarDer);
            const t_FPacketC totStarRecip = Math::reciprocal(totStar);

            const t_FPacketC alpha = aStar * totStarRecip;
            const t_FPacketC beta  = bStar * totStarRecip;
            const t_FPacketC gamma = gStar * totStarRecip;

            const t_FPacketC zeroC = t_FPacketC::C_0();

            const t_MaskC hitMask =
                (alpha >= zeroC) & (beta >= zeroC) & (gamma >= zeroC);
        
            if(!hitMask.isFalse())
            {
                const t_FPacketC dist = nominator * Math::reciprocal(
                    Math::dot(normalC, aRayPacket.dir(i)));

                const t_MaskC distMask = hitMask &
                    (dist < aRayPacket.tMax(i)) &
                    (dist > aRayPacket.tMin(i)) &
                    (dist < oResult.dist(i));

                if(!distMask.isFalse())
                {
                    oResult.mask(i) |= distMask;
         
                    if(!RTFACT_CHECK_UMASK(taIsectData, IsectData::NONE))
                    {
                        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::U_V))
                        {
                            oResult.primitiveOffset(i) =
                                distMask.iif(offset, oResult.primitiveOffset(i));
                        }

                        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::DISTANCE))
                        {
                            oResult.dist(i) = distMask.iif(dist(i), oResult.dist(i));
                        }

                        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::GEOM_NORMAL))
                        {
                            oResult.geomNormal(i) = distMask.iif(
                                normalC, oResult.geomNormal(i));

                            if(!RTFACT_CHECK_UMASK(taIsectData, IsectData::NORMAL))
                            {
                                oResult.normal(i) = oResult.geomNormal(i);
                            }
                        }

                        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::U_V))
                        {
                            //const t_FPacketC v0l = Math::reciprocal(v0d + v1d + v2d);
                            oResult.u(i) = distMask.iif(beta(i), oResult.u(i));
                            oResult.v(i) = distMask.iif(gamma(i), oResult.v(i));
                        }

                        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::APPEARANCE_ID))
                        {
                            oResult.appearanceID(i) = distMask.iif(
                                t_IPacketC::replicate(aTriangle.appearanceID),
                                oResult.appearanceID(i));
                        }
                    }
                    
                }
            }
        }
    }

template<
        umask taIsectData,
        uint taSize>
    RTFACT_INTERSECTOR_INLINE void intersectNonCommonOrigin(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        Triangle& aTriangle,
        Result<taSize>& oResult,
        uint aI) const
    {
     
        typedef typename Packet<taSize, float>::Container t_FPacketC;
        typedef typename Packet<taSize, int>::Container   t_IPacketC;
        typedef typename Vec3f<taSize>::Container         t_Vec3fC;
        typedef typename Mask<taSize>::Container    t_MaskC;

        //RTFACT_BREAK();

        t_Vec3fC ao = aTriangle.vertices[0] - aRayPacket.org(0);
        t_Vec3fC bo = aTriangle.vertices[1] - aRayPacket.org(0);
        t_Vec3fC co = aTriangle.vertices[2] - aRayPacket.org(0);

        const t_Vec3fC normalC = t_Vec3fC::replicate(aTriangle.getNormal());
        t_FPacketC nominator = Math::dot(ao, normalC);
        uint mask = nominator.getSignIntMask() ^ RTFACT_SIMD_MASK_TRUE;

        t_Vec3fC v0c4 = Math::cross(co, bo);
        const t_FPacketC v0d = Math::dot(v0c4, aRayPacket.frustum.dir);
        const uint v0ds = v0d.getSignIntMask();

        if(v0ds == mask)
        {
            RTFACT_INTERSECTOR_ASSERT(!aRayPacket.frustum.bad);

            return;
        }

        t_Vec3fC v1c4 = Math::cross(bo, ao);
        const t_FPacketC v1d = Math::dot(v1c4, aRayPacket.frustum.dir);
        const uint v1ds = v1d.getSignIntMask();

        if(v1ds == mask)
        {
            RTFACT_INTERSECTOR_ASSERT(!aRayPacket.frustum.bad);

            return; //know???
        }

        t_Vec3fC v2c4 = Math::cross(ao, co);
        const t_FPacketC v2d = Math::dot(v2c4, aRayPacket.frustum.dir);
        const uint v2ds = v2d.getSignIntMask();

        if(v2ds == mask)
        {
            RTFACT_INTERSECTOR_ASSERT(!aRayPacket.frustum.bad);

            return;
        }

        //START_TIMER(1);

        t_IPacketC offset = t_IPacketC::replicate(oResult.getOffset(aTriangle));

        //RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        //RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        //RTFACT_BREAK();

        for(uint i = aI >> 2; i < Packet<taSize, float>::CONTAINER_COUNT; ++i)
        { 
            ao = aTriangle.vertices[0] - aRayPacket.org(i);
            bo = aTriangle.vertices[1] - aRayPacket.org(i);
            co = aTriangle.vertices[2] - aRayPacket.org(i);

            v0c4 = Math::cross(co, bo);
            v1c4 = Math::cross(bo, ao);
            v2c4 = Math::cross(ao, co);

            nominator = Math::dot(ao, normalC);

            mask = nominator.getSignIntMask() ^ RTFACT_SIMD_MASK_TRUE;

            const t_FPacketC v0d = Math::dot(v0c4, aRayPacket.dir(i));
            const uint v0s = v0d.getSignIntMask();

            const t_FPacketC v1d = Math::dot(v1c4, aRayPacket.dir(i));
            const uint v1s = v1d.getSignIntMask();

            const t_FPacketC v2d = Math::dot(v2c4, aRayPacket.dir(i));
            const uint v2s = v2d.getSignIntMask();

            //RTFACT_BREAK();

            if((v0s != mask) & (v1s != mask) & (v2s != mask))
            {
                uint distMask = ((v0s ^ mask) & (v1s ^ mask) & (v2s ^ mask));

                t_FPacketC dist;

                dist = nominator * Math::reciprocal(Math::dot(normalC, aRayPacket.dir(i)));

                distMask &= (dist < aRayPacket.tMax(i) &
                             dist < oResult.dist(i) &
                             dist > aRayPacket.tMin(i)).getIntMask();

                //RTFACT_BREAK();

                if(distMask)
                {
                    t_MaskC maskC(distMask);

                    oResult.mask(i)  |= maskC;

                    if(!RTFACT_CHECK_UMASK(taIsectData, IsectData::NONE))
                    {
                        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::U_V))
                        {
                            oResult.primitiveOffset(i) =
                                maskC.iif(offset, oResult.primitiveOffset(i));
                        }

                        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::DISTANCE))
                        {
                            oResult.dist(i) = maskC.iif(dist, oResult.dist(i));
                        }

                        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::GEOM_NORMAL))
                        {
                            oResult.geomNormal(i) = maskC.iif(normalC, oResult.geomNormal(i));

                            if(!RTFACT_CHECK_UMASK(taIsectData, IsectData::NORMAL))
                            {
                                oResult.normal(i) = oResult.geomNormal(i);
                            }
                        }

                        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::U_V))
                        {
                            const t_FPacketC v0l = Math::reciprocal(v0d + v1d + v2d);
                            oResult.u(i) = maskC.iif(v2d * v0l, oResult.u(i));
                            oResult.v(i) = maskC.iif(v1d * v0l, oResult.v(i));
                        }

                        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::APPEARANCE_ID))
                        {
                            oResult.appearanceID(i) = maskC.iif(
                                t_IPacketC::replicate(aTriangle.appearanceID),
                                oResult.appearanceID(i));
                        }
                    }
                    
                }
            }
        }

        //RTFACT_BREAK();

        //STOP_TIMER(1);
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
        const Vec3f1 ao = aTriangle.vertices[0] - aRay.org;
        const Vec3f1 bo = aTriangle.vertices[1] - aRay.org;
        const Vec3f1 co = aTriangle.vertices[2] - aRay.org;

        const Vec3f1 v0 = Math::cross(co, bo);
        const Vec3f1 v1 = Math::cross(bo, ao);
        const Vec3f1 v2 = Math::cross(ao, co);

        const float v0d = Math::dot(v0, aRay.dir);
        const float v1d = Math::dot(v1, aRay.dir);
        const float v2d = Math::dot(v2, aRay.dir);

        if(((v0d < 0) & (v1d < 0) & (v2d < 0)) | ((v0d >=0) & (v1d >= 0) & (v2d >= 0)))
        {
            const Vec3f1 normal = aTriangle.getNormal();

            const float dist =
                Math::dot(normal, ao) /
                Math::dot(normal, aRay.dir);

            if((dist > 0.f) && (dist < oResult.dist))
            {
                oResult.mask = RTFACT_MASK_TRUE;

                if(!RTFACT_CHECK_UMASK(taIsectData, IsectData::NONE))
                {
                    if(RTFACT_CHECK_UMASK(taIsectData, IsectData::U_V))
                    {
                        oResult.primitiveOffset = oResult.getOffset(aTriangle);
                    }

                    if(RTFACT_CHECK_UMASK(taIsectData, IsectData::DISTANCE))
                    {
                        oResult.dist = dist;
                    }

                    if(RTFACT_CHECK_UMASK(taIsectData, IsectData::GEOM_NORMAL))
                    {
                        oResult.geomNormal = normal;

                        if(!RTFACT_CHECK_UMASK(taIsectData, IsectData::NORMAL))
                        {
                            oResult.normal = normal;
                        }
                    }

                    if(RTFACT_CHECK_UMASK(taIsectData, IsectData::U_V))
                    {
                        const float v0l = Math::reciprocal(v0d + v1d + v2d);
                        oResult.u = v2d * v0l;
                        oResult.v = v1d * v0l;
                    }

                    if(RTFACT_CHECK_UMASK(taIsectData, IsectData::APPEARANCE_ID))
                    {
                        oResult.appearanceID = aTriangle.appearanceID;
                    }
                }
            }
        }
    }

    template<
        umask taIsectData,
        uint taSize>
    RTFACT_INTERSECTOR_INLINE void finalizeResult(
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

#endif // RTFACT__RAYSTERIZERTRIANGLEINTERSECTOR_HPP
