/*
 *  BVHPartitionIntersector.hpp
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
 *  Created on: 2008-11-21 17:06:44 +0100
 *  Author(s): Iliyan Georgiev, Felix Klein
 */

 #ifndef RTFACT__BVHPARTITIONINTERSECTOR_HPP
#define RTFACT__BVHPARTITIONINTERSECTOR_HPP

#include <RTfact/Config/Common.hpp>

#include <utility>

RTFACT_NAMESPACE_BEGIN

template<
    class tIntersector>
class BVHPartitionIntersector :
    public tIntersector
{
protected:

    template<
        class tIterator>
    struct TraversalState
    {
        uint lastActiveContainerId;
        tIterator iterator;
    };

    template<
        class tIterator>
    struct TraversalStateSingleRay
    {
        tIterator iterator;
    };

    template<
        bool taCommonOrg,
        uint taSize>
    RTFACT_INTERSECTOR_INLINE uint intersectBox(
        const BBox& aBounds,
        const RayPacket<taSize>& aRayPacket,
        const Packet<taSize, float>& aTMin,
        const Packet<taSize, float>& aTMax,
        ActiveMask<taSize>& aMask) const
    {
        typedef typename Vec3f<taSize>::Container t_Vec3fC;
        typedef Packet<taSize, float>             t_Packet;
        typedef typename t_Packet::Container      t_PacketC;
        typedef typename Mask<taSize>::Container  t_MaskC;

        t_Vec3fC boundsMinC = t_Vec3fC::replicate(aBounds.min);
        t_Vec3fC boundsMaxC = t_Vec3fC::replicate(aBounds.max);

        t_Vec3fC l1part, l2part;

        if(taCommonOrg)
        {
            l1part = boundsMinC - aRayPacket.org(0);
            l2part = boundsMaxC - aRayPacket.org(0);
        }

        ////test if the whole frustum intersects the box
        //const t_Vec3fC l1 = l1part * aRayPacket.frustum.cornerInvDir;
        //const t_Vec3fC l2 = l2part * aRayPacket.frustum.cornerInvDir;
        //const t_Vec3fC lMin = t_Vec3fC::min(l1, l2);
        //const t_Vec3fC lMax = t_Vec3fC::max(l1, l2);

        ////HACK: doesn't take into account tmin/tmax
        //const t_PacketC tMinTmp = lMin.max();
        //const t_PacketC tMaxTmp = lMax.min();

        //if((tMinTmp <= tMaxTmp).isTrue())
        //{
        //    //DOESNT WORK PROPERLY!
        //    return aMask.getActiveContainerEndIndex();
        //}

        //// the bounding frustum misses -> the whole packet misses
        //if(aRayPacket.frustum.misses(aBounds))
        //{
        //    return 0;
        //}

        uint newActiveContainerEnd = 0;

        for(uint aI = 0; aI < aMask.getActiveContainerEndIndex(); ++aI)
        {
            const uint i = aMask.getActiveContainerAt(aI);

            if(!taCommonOrg)
            {
                l1part = boundsMinC - aRayPacket.org(i);
                l2part = boundsMaxC - aRayPacket.org(i);
            }

            const t_Vec3fC l1 = l1part * aRayPacket.invDir(i);
            const t_Vec3fC l2 = l2part * aRayPacket.invDir(i);
            const t_Vec3fC lMin = t_Vec3fC::min(l1, l2);
            const t_Vec3fC lMax = t_Vec3fC::max(l1, l2);

            const t_PacketC tMinTmp = Math::max(aTMin(i), lMin.max());
            const t_PacketC tMaxTmp = Math::min(aTMax(i), lMax.min());

            const t_MaskC isectMask = aMask(i) & (tMinTmp <= tMaxTmp);

            if(isectMask.getIntMask())
            {
                std::swap(aMask.getActiveContainerAt(newActiveContainerEnd++),
                          aMask.getActiveContainerAt(aI));
            }
        }

        return newActiveContainerEnd;
    }

public:

    BVHPartitionIntersector()
    {}

    template<
        umask taIsectData,
        bool taCommonOrg,
        uint taSize,
        class tBVH>
    RTFACT_INTERSECTOR_INLINE void intersect(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        tBVH& aTree,
        typename tIntersector::template Result<taSize>& oResult) const
    {
        typedef Packet<taSize, float>        t_Packet;
        typedef Vec3f<taSize>                t_Vec3f;
        typedef BitMask<taSize>              t_BitMask;
        typedef typename t_Packet::Container t_PacketC;
        typedef typename t_Vec3f::Container  t_Vec3fC;
        typedef SurfaceIntersection<taSize>  t_Intersection;

        typedef typename tBVH::Iterator      t_Iterator;

        oResult.initBase(aTree.getElementBase());

        ActiveMask<taSize> mask = aRayMask;

        TraversalState<t_Iterator>
            traversalStack[RTFACT_TRAVERSER_MAX_STACK_DEPTH];
        int stackTop = -1;

        if(&aTree.getElementBase() == RTFACT_NULL)
            return;

        t_Iterator iterator = aTree.getRoot();

        for(;;)
        {
            mask.activeContainerEndIndex = intersectBox<taCommonOrg>(
                aTree.getBounds(iterator), aRayPacket,
                aRayPacket.tMin, oResult.dist, mask);

            if(mask.activeContainerEndIndex > 0)
            {
                if(!aTree.isLeaf(iterator))
                {
                    t_Iterator secondChildIterator;

                    aTree.getChildren(
                        iterator, aRayPacket.frustum.dominantAxis,
                        aRayPacket.frustum.dominantAxisSign,
                        aRayPacket.frustum.dir[0],
                        iterator, secondChildIterator);

                    traversalStack[++stackTop].iterator = secondChildIterator;
                    traversalStack[stackTop].lastActiveContainerId =
                        mask.activeContainerEndIndex;

                    continue;
                }
                else
                {
                    typename tBVH::ElementIterator elementBegin, elementEnd;
                    aTree.getElements(iterator, elementBegin, elementEnd);

                    //START_TIMER(1);

                    if(elementBegin != elementEnd)
                    {
                        do
                        {
                            tIntersector::template intersect<
                                taIsectData, taCommonOrg>(
                                    aRayPacket,
                                    mask,
                                    *elementBegin,
                                    oResult);

                            ++elementBegin;
                        }
                        while(elementBegin != elementEnd);
                    }

                    //STOP_TIMER(1);
                }
            }

            if(stackTop < 0)
            {
                break;
            }

            mask.activeContainerEndIndex = traversalStack[stackTop].lastActiveContainerId;
            iterator = traversalStack[stackTop--].iterator;
        }
    }

    template<
        umask taIsectData,
        bool taCommonOrg,
        class tBVH>
    RTFACT_INTERSECTOR_INLINE void intersect(
        const RayPacket<1>& aRay,
        const ActiveMask<1>& aRayMask,
        tBVH& aTree,
        typename tIntersector::template Result<1>& oResult) const
    {
        oResult.initBase(aTree.getElementBase());

        typedef typename tBVH::Iterator t_Iterator;

        TraversalStateSingleRay<t_Iterator>
            traversalStack[RTFACT_TRAVERSER_MAX_STACK_DEPTH];
        int stackTop = -1;

        const uint dominantAxis = RTFACT_MAX_DIMENSION(
            Math::abs(aRay.dir.x), Math::abs(aRay.dir.y), Math::abs(aRay.dir.z));

        const uint dominantAxisSign = (aRay.dir.get(dominantAxis) < 0.f) ? 1 : 0;

        if(&aTree.getElementBase() == RTFACT_NULL)
            return;

        t_Iterator iterator = aTree.getRoot();

        for(;;)
        {
            const BBox& nodeBounds = aTree.getBounds(iterator);

            const Vec3f1 l1 = (nodeBounds.min - aRay.org) * aRay.invDir;
            const Vec3f1 l2 = (nodeBounds.max - aRay.org) * aRay.invDir;
            const Vec3f1 lMin = Vec3f1::min(l1, l2);
            const Vec3f1 lMax = Vec3f1::max(l1, l2);

            float tMin = Math::max(aRay.tMin,    lMin.max());
            float tMax = Math::min(oResult.dist, lMax.min());

            if(tMin <= tMax)
            {
                if(!aTree.isLeaf(iterator))
                {
                    aTree.getChildren(
                        iterator, dominantAxis, dominantAxisSign, aRay.dir,
                        iterator, traversalStack[++stackTop].iterator);

                    continue;
                }
                else
                {
                    typename tBVH::ElementIterator elementBegin, elementEnd;

                    aTree.getElements(iterator, elementBegin, elementEnd);

                    if(elementBegin != elementEnd)
                    {
                        do
                        {
                            tIntersector::template intersect<
                                taIsectData, taCommonOrg>(
                                aRay,
                                aRayMask,
                                *elementBegin,
                                oResult,
                                0);

                            ++elementBegin;
                        }
                        while(elementBegin != elementEnd);
                    }
                }
            }

            if(stackTop < 0)
            {
                break;
            }

            iterator = traversalStack[stackTop--].iterator;
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__BVHPARTITIONINTERSECTOR_HPP
