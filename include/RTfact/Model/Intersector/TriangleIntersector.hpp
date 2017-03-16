/*
 *  TriangleIntersector.hpp
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
 *  Created on: 2009-01-22 23:53:27 +0100
 *  Author(s): Iliyan Georgiev
 */

 #ifndef RTFACT__TRIANGLEINTERSECTOR_HPP
#define RTFACT__TRIANGLEINTERSECTOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Intersector/BasicIntersector.hpp>

RTFACT_NAMESPACE_BEGIN

class TriangleIntersector :
    public BasicIntersector<Triangle>
{
public:

    template<
        umask taIsectData,
        uint taSize>
    RTFACT_FORCE_INLINE void finalizeResult(
        const RayPacket<taSize>& aRayPacket,
        Result<taSize>& oResult) const
    {
        typedef Packet<taSize, float>         t_Packet;
        typedef typename t_Packet::Container  t_PacketC;
        typedef Packet<taSize, int>           t_IPacket;
        typedef typename t_IPacket::Container t_IPacketC;
        typedef Vec3f<taSize>                 t_Vec3f;
        typedef typename t_Vec3f::Container   t_Vec3fC;
        typedef ActiveMask<taSize>            t_ActiveMask;

        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::GEOM_NORMAL) &&
           !RTFACT_CHECK_UMASK(taIsectData, IsectData::NORMAL))
        {
            oResult.normal = oResult.geomNormal;
        }

        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::GEOM_NORMAL))
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
                t_ActiveMask primitiveMask;

                RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
                {
                    primitiveMask(i) = oResult.mask(i) & (oResult.primitiveOffset(i) == primOffsetC);
                    primitiveMask.updateActiveContainerEndIndex(i);
                }

                oResult.geomNormal = primitiveMask.iif(
                    t_Vec3f::replicate(triangle.normal), oResult.geomNormal);
            }
        }

        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::INTERPOLATED_DATA))
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

                t_Vec3fC normal1C, normal2C, normal3C,
                         vertexColor1C, vertexColor2C, vertexColor3C;
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

                if(RTFACT_CHECK_UMASK(taIsectData, IsectData::VERTEX_COLOR))
                {
                    vertexColor1C = t_Vec3fC::replicate(triangle.getVertexColor(0));
                    vertexColor2C = t_Vec3fC::replicate(triangle.getVertexColor(1));
                    vertexColor3C = t_Vec3fC::replicate(triangle.getVertexColor(2));
                }

                t_ActiveMask primitiveMask;

                RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
                {
                    primitiveMask(i) = oResult.mask(i) & (oResult.primitiveOffset(i) == primOffsetC);
                    primitiveMask.updateActiveContainerEndIndex(i);
                }

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

                    if(RTFACT_CHECK_UMASK(taIsectData, IsectData::DERIVATIVES))
                    {
                        const t_PacketC invDotDirNormal =
                            Math::reciprocal(Math::dot(aRayPacket.dir(i), oResult.normal(i)));

                        const t_Vec3fC tdDdx = oResult.dist(i) * aRayPacket.dDdx(i);
                        const t_Vec3fC dpdx = tdDdx -
                            (Math::dot(tdDdx, oResult.normal(i)) * invDotDirNormal) * aRayPacket.dir(i);

                        const t_Vec3fC tdDdy = oResult.dist(i) * aRayPacket.dDdy(i);
                        const t_Vec3fC dpdy = tdDdy -
                            (Math::dot(tdDdy, oResult.normal(i)) * invDotDirNormal) * aRayPacket.dir(i);

                        oResult.dpdu(i) = primitiveMask(i).iif(dpdx, oResult.dpdu(i));
                        oResult.dpdv(i) = primitiveMask(i).iif(dpdy, oResult.dpdv(i));

                        oResult.du(i) = primitiveMask(i).iif(aRayPacket.deltaX(i), oResult.du(i));
                        oResult.dv(i) = primitiveMask(i).iif(aRayPacket.deltaY(i), oResult.dv(i));

                        oResult.point(i) = primitiveMask(i).iif(
                            aRayPacket.org(i) + oResult.dist(i) * aRayPacket.dir(i), oResult.point(i));

                        const t_Vec3fC v0 = t_Vec3fC::replicate(triangle.vertices[0]);
                        const t_Vec3fC v1 = t_Vec3fC::replicate(triangle.vertices[1]);
                        const t_Vec3fC v2 = t_Vec3fC::replicate(triangle.vertices[2]);

                        const t_Vec3fC a  = v2 - v1;
                        const t_Vec3fC b  = v2 - v0;
                        const t_Vec3fC c  = v1 - v0;
                        const t_Vec3fC an = a.normalize();
                        const t_Vec3fC bn = b.normalize();
                        const t_Vec3fC cn = c.normalize();

                        const t_Vec3fC wDirUnnorm = Math::dot(an, b) * an - b;
                        const t_Vec3fC uDirUnnorm = Math::dot(bn, a) * bn - a;
                        const t_Vec3fC vDirUnnorm = a - Math::dot(cn, a) * cn;

                        const t_PacketC wLength = wDirUnnorm.length();
                        const t_PacketC uLength = uDirUnnorm.length();
                        const t_PacketC vLength = vDirUnnorm.length();

                        const t_Vec3fC wDir = wDirUnnorm / (wLength * wLength);
                        const t_Vec3fC uDir = uDirUnnorm / (uLength * uLength);
                        const t_Vec3fC vDir = vDirUnnorm / (vLength * vLength);

                        const t_PacketC dotwdpdx = Math::dot(wDir, dpdx);
                        const t_PacketC dotudpdx = Math::dot(uDir, dpdx);
                        const t_PacketC dotvdpdx = Math::dot(vDir, dpdx);

                        const t_PacketC dt1du = dotwdpdx * texCoord11C + dotudpdx * texCoord21C + dotvdpdx * texCoord31C;
                        const t_PacketC dt2du = dotwdpdx * texCoord12C + dotudpdx * texCoord22C + dotvdpdx * texCoord32C;

                        const t_PacketC dotwdpdy = Math::dot(wDir, dpdy);
                        const t_PacketC dotudpdy = Math::dot(uDir, dpdy);
                        const t_PacketC dotvdpdy = Math::dot(vDir, dpdy);

                        const t_PacketC dt1dv = dotwdpdy * texCoord11C + dotudpdy * texCoord21C + dotvdpdy * texCoord31C;
                        const t_PacketC dt2dv = dotwdpdy * texCoord12C + dotudpdy * texCoord22C + dotvdpdy * texCoord32C;

                        oResult.dt1du(i) = primitiveMask(i).iif(dt1du, oResult.dt1du(i));
                        oResult.dt2du(i) = primitiveMask(i).iif(dt2du, oResult.dt2du(i));
                        oResult.dt1dv(i) = primitiveMask(i).iif(dt1dv, oResult.dt1dv(i));
                        oResult.dt2dv(i) = primitiveMask(i).iif(dt2dv, oResult.dt2dv(i));
                    }

                    if(RTFACT_CHECK_UMASK(taIsectData, IsectData::VERTEX_COLOR))
                    {
                        const t_Vec3fC vertexColor =
                            vertexColor1C * w +
                            vertexColor2C * oResult.u(i) +
                            vertexColor3C * oResult.v(i);

                        oResult.vertexColor(i) = primitiveMask(i).iif(
                            vertexColor, oResult.vertexColor(i));
                    }

                    if (RTFACT_CHECK_UMASK(taIsectData, IsectData::TANGENT_SPACE))
                    {
                        oResult.tangent(i)  = primitiveMask(i).iif(t_Vec3fC::replicate(triangle.tangent), oResult.tangent(i));
                        oResult.binormal(i) = primitiveMask(i).iif(t_Vec3fC::replicate(triangle.binormal), oResult.binormal(i));
                    }
                }
            }
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__TRIANGLEINTERSECTOR_HPP
