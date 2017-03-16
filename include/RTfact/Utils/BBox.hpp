/* 
 *  BBox.hpp
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
 *  Created on: 2008-11-02 21:38:37 +0100
 *  Author(s): Iliyan Georgiev, Felix Klein
 */
 
 #ifndef RTFACT__BBOX_HPP
#define RTFACT__BBOX_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Packets/Vec3f.hpp>
#include <RTfact/Utils/Packets/BitMask.hpp>
#include <RTfact/Utils/BoundingSphere.hpp>

#define RTFACT__BBOX_INLINE RTFACT_FORCE_INLINE

RTFACT_NAMESPACE_BEGIN

struct BBox
{
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    Vec3f<1> min;
    Vec3f<1> max;

    RTFACT__BBOX_INLINE BBox()
    {}

    RTFACT__BBOX_INLINE BBox(
        const Vec3f<1>& aMin,
        const Vec3f<1>& aMax
    ) :
        min(aMin),
        max(aMax)
    {}

    //Use this to allow better optimization of SSE code on certain compilers
    RTFACT__BBOX_INLINE void assign(
        const BBox &aOther)
    {
        min = aOther.min;
        max = aOther.max;
    }

    RTFACT__BBOX_INLINE const float operator[](
        const size_t aIndex) const
    {
        RTFACT_ASSERT(aIndex < 7 && aIndex != 3);

        return *(&min.xf + aIndex);
    }

    RTFACT__BBOX_INLINE float& operator[](
        const size_t aIndex)
    {
        RTFACT_ASSERT(aIndex < 7 && aIndex != 3);

        return *(&min.xf + aIndex);
    }

    RTFACT__BBOX_INLINE const Vec3f<1> getExtent() const
    {
        return max - min;
    }

    RTFACT__BBOX_INLINE int getMaximumExtent() const
    {
        return getExtent().maxDim();
    }

    RTFACT__BBOX_INLINE const Vec3f<1> getCentroid() const
    {
        return (max + min) * 0.5f;
    }

    RTFACT__BBOX_INLINE BBox& fixCorners()
    {
        Vec3f<1> mn = Vec3f<1>::min(min, max);
        Vec3f<1> mx = Vec3f<1>::max(min, max);
        min = mn;
        max = mx;

        return *this;
    }

    RTFACT__BBOX_INLINE float surfaceAreaX8() const
    {
        const float4 sizeTmp = max.data - min.data;

        return float4::dot3(sizeTmp, sizeTmp.shuffle<1, 2, 0, 3>());
    }

    RTFACT__BBOX_INLINE BBox& extend(
        const Vec3f<1>& aPoint)
    {
        min = Vec3f<1>::min(min, aPoint);
        max = Vec3f<1>::max(max, aPoint);

        return *this;
    }

    RTFACT__BBOX_INLINE BBox& extend(
        const BBox& aOther)
    {
        min = Vec3f<1>::min(min, aOther.min);
        max = Vec3f<1>::max(max, aOther.max);
        return *this;
    }

    RTFACT__BBOX_INLINE bool isEmpty() const
    {
        return ((min.data > max.data).getI32Mask() & 7) == 7;
    }

    RTFACT__BBOX_INLINE bool contains(
        const BBox& aOther) const
    {
        return ((min.data <= aOther.min.data &
                 aOther.max.data <= max.data).getI32Mask() & 7) == 7;
    }

    RTFACT__BBOX_INLINE bool intersects(
        const BBox& aOther) const
    {
        int4 cmp1 = min.data <= aOther.min.data;
        cmp1 &= aOther.min.data < max.data;

        int4 cmp2 = min.data < aOther.max.data;
        cmp2 &= aOther.max.data <= max.data;

        int4 cmp3 = aOther.min.data <= min.data;
        cmp3 &= min.data < aOther.max.data;

        int4 cmp = cmp1 | cmp2 | cmp3;

        return (cmp.getI32Mask() & 7) == 7;
    }

    RTFACT__BBOX_INLINE float getArea() const
    {
        const Vec3f<1> bboxExt = getExtent();

        return 2 * bboxExt.xf * bboxExt.yf +
               2 * bboxExt.xf * bboxExt.zf +
               2 * bboxExt.yf * bboxExt.zf;
    }
    
    RTFACT__BBOX_INLINE const BoundingSphere getBoundingSphere() const
    {
        if(isEmpty())
        {
            return BoundingSphere::empty();
        }
        else
        {
            BoundingSphere ret;
            
            ret.center = getCentroid();
            ret.radius = (ret.center - min).length();

            return ret;
        }
    }

    /*template<
        uint taSize>
    RTFACT__BBOX_INLINE const BitMask<taSize> clip(
        RayPacket<taSize>& aRayPacket) const
    {
        typedef typename Vec3f<taSize>::Container         t_Vec3fC;
        typedef typename Packet<taSize, float>::Container t_PacketC;
        typedef BitMask<taSize>                           t_BitMask;

        t_BitMask mask;
        mask.setFalse();

        t_Vec3fC minC = t_Vec3fC::replicate(min);
        t_Vec3fC maxC = t_Vec3fC::replicate(max);

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            const t_Vec3fC l1 = (minC - aRayPacket.org(i)) * aRayPacket.invDir(i);
            const t_Vec3fC l2 = (maxC - aRayPacket.org(i)) * aRayPacket.invDir(i);
            const t_Vec3fC lMin = t_Vec3fC::min(l1, l2);
            const t_Vec3fC lMax = t_Vec3fC::max(l1, l2);

            aRayPacket.tMin(i) = Math::max(aRayPacket.tMin(i), lMin.max());
            aRayPacket.tMax(i) = Math::min(aRayPacket.tMax(i), lMax.min());

            mask.setContainer(i, (aRayPacket.tMin(i) < aRayPacket.tMax(i)).getIntMask());
        }

        return mask;
    }*/

    static RTFACT__BBOX_INLINE const BBox merge(
        const BBox& aBBox1,
        const BBox& aBBox2)
    {
        BBox ret = aBBox1;
        ret.extend(aBBox2);

        return ret;
    }

    static RTFACT__BBOX_INLINE const BBox empty()
    {
        return BBox(Vec3f<1>(Vec3f<1>::ContainerData::C_MAX_MAX_MAX_MAX),
                    Vec3f<1>(Vec3f<1>::ContainerData::C_MIN_MIN_MIN_MIN));
    }
};

//template<>
//RTFACT__BBOX_INLINE const BitMask<1> BBox::clip<1>(
//    RayPacket<1>& aRayPacket) const
//{
//    typedef Vec3f<1>::Container            t_Vec3fC;
//    typedef Packet<1, float>::Container    t_PacketC;
//    typedef BitMask<1>                    t_BitMask;
//
//    const t_Vec3fC l1 = (min - aRayPacket.org) * aRayPacket.invDir;
//    const t_Vec3fC l2 = (max - aRayPacket.org) * aRayPacket.invDir;
//    const t_Vec3fC lMin = t_Vec3fC::min(l1, l2);
//    const t_Vec3fC lMax = t_Vec3fC::max(l1, l2);
//
//    aRayPacket.tMin = Math::max(aRayPacket.tMin, lMin.max());
//    aRayPacket.tMax = Math::min(aRayPacket.tMax, lMax.min());
//
//    return (aRayPacket.tMin < aRayPacket.tMax).getBitMask();
//}

#undef RTFACT__BBOX_INLINE

RTFACT_NAMESPACE_END

#endif // RTFACT__BBOX_HPP
