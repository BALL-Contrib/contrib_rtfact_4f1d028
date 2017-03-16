/* 
 *  PlueckerTriangleIntersector.hpp
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
 *  Created on: 2008-06-09 17:30:26 +0200
 *  Author(s): Iliyan Georgiev, Petr Kmoch
 */
 
 #ifndef RTFACT__PLUECKERTRIANGLEINTERSECTOR_HPP
#define RTFACT__PLUECKERTRIANGLEINTERSECTOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Containers/Array.hpp>
#include <RTfact/Utils/Packets/Packet.hpp>
#include <RTfact/Utils/Packets/Vec3f.hpp>

#include <RTfact/Model/Intersector/TriangleIntersector.hpp>
#include <RTfact/Model/Primitive/Triangle.hpp>

RTFACT_NAMESPACE_BEGIN

class PlueckerTriangleIntersector :
    public TriangleIntersector
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
        typedef typename Packet<taSize, float>::Container t_FPacketC;
        typedef typename Packet<taSize, int>::Container   t_IPacketC;
        typedef typename Vec3f<taSize>::Container         t_Vec3fC;
        typedef typename Mask<taSize>::Container          t_MaskC;

        t_Vec3fC ao = aTriangle.vertices[0] - aRayPacket.frustum.org;
        t_Vec3fC bo = aTriangle.vertices[1] - aRayPacket.frustum.org;
        t_Vec3fC co = aTriangle.vertices[2] - aRayPacket.frustum.org;

        const t_Vec3fC normalC = t_Vec3fC::replicate(aTriangle.getNormal());
        t_FPacketC nominator = Math::dot(ao, normalC);
        uint mask =  nominator.getSignIntMask() ^ RTFACT_SIMD_MASK_TRUE;

        t_Vec3fC v0c4 = Math::cross(co, bo);
        const t_FPacketC v0d = Math::dot(v0c4, aRayPacket.frustum.dir);
        const uint v0ds = v0d.getSignIntMask();

        if(v0ds == mask)
        {
            //RTFACT_INTERSECTOR_ASSERT(!aRayPacket.frustum.bad);
            //return;
        }

        t_Vec3fC v1c4 = Math::cross(bo, ao);
        const t_FPacketC v1d = Math::dot(v1c4, aRayPacket.frustum.dir);
        const uint v1ds = v1d.getSignIntMask();

        if(v1ds == mask)
        {
            //RTFACT_INTERSECTOR_ASSERT(!aRayPacket.frustum.bad);
            //return; //know???
        }

        t_Vec3fC v2c4 = Math::cross(ao, co);
        const t_FPacketC v2d = Math::dot(v2c4, aRayPacket.frustum.dir);
        const uint v2ds = v2d.getSignIntMask();

        if(v2ds == mask)
        {
            //RTFACT_INTERSECTOR_ASSERT(!aRayPacket.frustum.bad);
            //return;
        }

        const ptrdiff_t triangleOffset = oResult.getOffset(aTriangle);

        t_IPacketC offset = t_IPacketC::replicate(RTFACT_DOWNSIZE_PACKET_CAST(int,triangleOffset));

        //RTFACT_PACKET_FOR_ALL_CONTAINERS_INIT(i, aI >> 2, taSize)
        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        {
            if(!taCommonOrg)
            {
                ao = aTriangle.vertices[0] - aRayPacket.org(i);
                bo = aTriangle.vertices[1] - aRayPacket.org(i);
                co = aTriangle.vertices[2] - aRayPacket.org(i);

                v0c4 = Math::cross(co, bo);
                v1c4 = Math::cross(bo, ao);
                v2c4 = Math::cross(ao, co);

                nominator = Math::dot(ao, normalC);

                mask = nominator.getSignIntMask() ^ RTFACT_SIMD_MASK_TRUE;
            }

            const t_FPacketC v0d = Math::dot(v0c4, aRayPacket.dir(i));
            const uint v0s = v0d.getSignIntMask();

            const t_FPacketC v1d = Math::dot(v1c4, aRayPacket.dir(i));
            const uint v1s = v1d.getSignIntMask();

            const t_FPacketC v2d = Math::dot(v2c4, aRayPacket.dir(i));
            const uint v2s = v2d.getSignIntMask();

            if((v0s != mask) & (v1s != mask) & (v2s != mask))
            {             
                uint distMask = ((v0s ^ mask) & (v1s ^ mask) & (v2s ^ mask));

                t_FPacketC dist;

                dist = nominator * Math::reciprocal(Math::dot(normalC, aRayPacket.dir(i)));

                distMask &= (aRayMask(i) &
                             dist < oResult.dist(i) &
                             dist > aRayPacket.tMin(i)).getIntMask();

                if(distMask)
                {
                    t_MaskC maskC(distMask);

                    oResult.mask(i) |= maskC;

                    if(!RTFACT_CHECK_UMASK(taIsectData, IsectData::NONE))
                    {
                        if(RTFACT_CHECK_UMASK(taIsectData, IsectData::INTERPOLATED_DATA))
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
    }

    template<
        umask taIsectData,
        bool taCommonOrg>
    RTFACT_INTERSECTOR_INLINE void intersect(
        const RayPacket<1>& aRay,
        const ActiveMask<1>& aRayMask,
        Triangle& aTriangle,
        Result<1>& oResult,
        uint aI = 0) const
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
                        oResult.primitiveOffset = RTFACT_DOWNSIZE_PACKET_CAST(int,oResult.getOffset(aTriangle));
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
};

RTFACT_NAMESPACE_END

#endif // RTFACT__PLUECKERTRIANGLEINTERSECTOR_HPP
