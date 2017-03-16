/* 
 *  OrderedBVHIntersector.hpp
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
 *  Created on: 2008-06-09 17:30:26 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__BVHINTERSECTOR2_HPP
#define RTFACT__BVHINTERSECTOR2_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/RayPacket.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tIntersector>
class OrderedBVHIntersector :
    public tIntersector
{
protected:

    template<
        class tIterator>
    struct TraversalState
    {
        uint firstActiveRayId;
        tIterator iterator;
    };

    template<
        class tIterator>
    struct TraversalStateSingleRay
    {
        float     tMin;
        bool      isLeaf;
        tIterator firstChildIterator;
        tIterator secondChildIterator;
    };

    struct ActiveRay
    {
        Vec3f<1> org;
        Vec3f<1> invDir;
        float boxTMin;
    };

    int mActiveRayIndices[16];

    template<
        bool taCommonOrg,
        uint taSize>
    RTFACT_INTERSECTOR_INLINE uint findFirstActiveRay(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        const Packet<taSize, float>& aTMin,
        const Packet<taSize, float>& aTMax,
        uint aFirstActiveRayId,
        ActiveRay& oFirstActiveRay,
        const BBox& aBounds) const
    {
        // ray/box test for the active ray
        const Vec3f<1> l1 = (aBounds.min - oFirstActiveRay.org) * oFirstActiveRay.invDir;
        const Vec3f<1> l2 = (aBounds.max - oFirstActiveRay.org) * oFirstActiveRay.invDir;
        const Vec3f<1> lMin = Vec3f<1>::min(l1, l2);
        const Vec3f<1> lMax = Vec3f<1>::max(l1, l2);

        oFirstActiveRay.boxTMin = Math::max(aTMin[aFirstActiveRayId], lMin.max());
        const float activeRayTMax = Math::min(aTMax[aFirstActiveRayId], lMax.min());

        // the first active ray hits -> packet hits
        if(oFirstActiveRay.boxTMin <= activeRayTMax ||
           ++aFirstActiveRayId == taSize)
        {
            return aFirstActiveRayId;
        }

        // the bounding frustum misses -> the whole packet misses
        if(aRayPacket.frustum.misses(aBounds))
        {
            //return taSize;
        }

        typedef typename Vec3f<taSize>::Container t_Vec3fC;
        typedef Packet<taSize, float>             t_Packet;
        typedef typename t_Packet::Container      t_PacketC;

        t_Vec3fC minC = t_Vec3fC::replicate(aBounds.min);
        t_Vec3fC maxC = t_Vec3fC::replicate(aBounds.max);

        t_Vec3fC l1part, l2part;

        if(taCommonOrg)
        {
            l1part = minC - aRayPacket.org(0);
            l2part = maxC - aRayPacket.org(0);
        }

        //clip rays from the packet with the bounding box until hit
        RTFACT_PACKET_FOR_ALL_CONTAINERS_INIT(i, aFirstActiveRayId >> 2, taSize) if(aRayMask(i).getIntMask())
        {
            if(!taCommonOrg)
            {
                l1part = minC - aRayPacket.org(i);
                l2part = maxC - aRayPacket.org(i);
            }

            const t_Vec3fC l1 = l1part * aRayPacket.invDir(i);
            const t_Vec3fC l2 = l2part * aRayPacket.invDir(i);
            const t_Vec3fC lMin = t_Vec3fC::min(l1, l2);
            const t_Vec3fC lMax = t_Vec3fC::max(l1, l2);

            const t_PacketC tMinTmp = Math::max(aTMin(i), lMin.max());
            const t_PacketC tMaxTmp = Math::min(aTMax(i), lMax.min());

            const int mask = (aRayMask(i) & (tMinTmp <= tMaxTmp)).getIntMask();

            if(mask != 0)
            {
                const uint rayIndex = mActiveRayIndices[mask];
                const uint firstRayActiveIndex = 4 * i + rayIndex;

                oFirstActiveRay.invDir = aRayPacket.invDir[firstRayActiveIndex];
                oFirstActiveRay.boxTMin = tMinTmp[rayIndex];

                if(!taCommonOrg)
                {
                    oFirstActiveRay.org = aRayPacket.org[firstRayActiveIndex];
                }

                return firstRayActiveIndex;
            }
        }

        //all rays in the packet miss the box
        return taSize;
    }

public:

    OrderedBVHIntersector()
    {
        const uint offset[16] = { 0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 };
        memcpy(mActiveRayIndices, offset, 16 * sizeof(int));
    }

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

        oResult.initBase(aTree.getElementBase());

        TraversalState<typename tBVH::Iterator>
            traversalStack[RTFACT_TRAVERSER_MAX_STACK_DEPTH];
        int stackTop = -1;

        typename tBVH::Iterator iterator = aTree.getRoot();

        const uint rayIndex = mActiveRayIndices[
            aRayMask(aRayMask.getActiveContainerAt(0)).getIntMask()];

        uint firstActiveRayId = 4 * aRayMask.getActiveContainerAt(0) + rayIndex;

        ActiveRay firstActiveRay;
        firstActiveRay.org = aRayPacket.org[firstActiveRayId];
        firstActiveRay.invDir = aRayPacket.invDir[firstActiveRayId];

        for(;;)
        {
            firstActiveRayId = findFirstActiveRay<taCommonOrg>(
                aRayPacket, aRayMask, aRayPacket.tMin, oResult.dist,
                firstActiveRayId, firstActiveRay,
                aTree.getBounds(iterator));

            //INCREMENT_COUNTER(0);
          
            if(firstActiveRayId < taSize)
            {
                if(!aTree.isLeaf(iterator))
                {
                    typename tBVH::Iterator secondChildIterator;

                    aTree.getChildren(
                        iterator, aRayPacket.frustum.dominantAxis,
                        aRayPacket.frustum.dominantAxisSign,
                        firstActiveRay.invDir,
                        iterator, secondChildIterator);

                    traversalStack[++stackTop].iterator = secondChildIterator;
                    traversalStack[stackTop].firstActiveRayId = firstActiveRayId;

                    continue;
                }
                else
                {
                    //INCREMENT_COUNTER(1);
                    
                    //START_TIMER(1);

                    typename tBVH::ElementIterator elementBegin, elementEnd;
                    aTree.getElements(iterator, elementBegin, elementEnd);

                    if(elementBegin != elementEnd)
                    {
                        do 
                        {
                            //INCREMENT_COUNTER(2);

                            tIntersector::template intersect<
                                taIsectData, taCommonOrg>(
                                    aRayPacket,
                                    aRayMask,
                                    *elementBegin,
                                    oResult,
                                    firstActiveRayId);

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

            firstActiveRayId = traversalStack[stackTop].firstActiveRayId;
            iterator = traversalStack[stackTop--].iterator;

            firstActiveRay.invDir = aRayPacket.invDir[firstActiveRayId];

            if(!taCommonOrg)
            {
                firstActiveRay.org = aRayPacket.org[firstActiveRayId];
            }
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
        typedef typename tBVH::Iterator t_Iterator;

        TraversalStateSingleRay<t_Iterator>
            traversalStack[RTFACT_TRAVERSER_MAX_STACK_DEPTH];
        int stackTop = -1;

        t_Iterator iterator = aTree.getRoot();

        const BBox& nodeBounds = aTree.getBounds(iterator);

        // ray/box test
        const Vec3f1 l1 = (nodeBounds.min - aRay.org) * aRay.invDir;
        const Vec3f1 l2 = (nodeBounds.max - aRay.org) * aRay.invDir;
        const Vec3f1 lMin = Vec3f1::min(l1, l2);
        const Vec3f1 lMax = Vec3f1::max(l1, l2);

        float tMin = Math::max(aRay.tMin, lMin.max());
        float tMax = Math::min(aRay.tMax, lMax.min());

        if(tMin > tMax)
        {
            return;
        }

        bool isLeaf = aTree.isLeaf(iterator);

        t_Iterator firstChildIterator, secondChildIterator;

        aTree.getChildren(iterator, firstChildIterator, secondChildIterator);

        for(;;)
        {
            while(!isLeaf)
            {
                const BBox& firstBounds = aTree.getBounds(firstChildIterator);

                // ray/box test
                const Vec3f1 firstL1 = (firstBounds.min - aRay.org) * aRay.invDir;
                const Vec3f1 firstL2 = (firstBounds.max - aRay.org) * aRay.invDir;
                const Vec3f1 firstLMin = Vec3f1::min(firstL1, firstL2);
                const Vec3f1 firstLMax = Vec3f1::max(firstL1, firstL2);

                const float firstTMin = Math::max(tMin, firstLMin.max());
                const float firstTMax = Math::min(tMax, firstLMax.min());

                const BBox& secondBounds = aTree.getBounds(secondChildIterator);

                // ray/box test
                const Vec3f1 secondL1 = (secondBounds.min - aRay.org) * aRay.invDir;
                const Vec3f1 secondL2 = (secondBounds.max - aRay.org) * aRay.invDir;
                const Vec3f1 secondLMin = Vec3f1::min(secondL1, secondL2);
                const Vec3f1 secondLMax = Vec3f1::max(secondL1, secondL2);

                const float secondTMin = Math::max(tMin, secondLMin.max());
                const float secondTMax = Math::min(tMax, secondLMax.min());

                const bool enterFirstChild = (firstTMin <= firstTMax);
                const bool enterSecondChild = (secondTMin <= secondTMax);

                if(enterFirstChild && enterSecondChild)
                {
                    if(firstTMin < secondTMin)
                    {
                        traversalStack[++stackTop].tMin = secondTMin;
                        traversalStack[stackTop].isLeaf = aTree.isLeaf(secondChildIterator);

                        aTree.getChildren(
                            secondChildIterator,
                            traversalStack[stackTop].firstChildIterator,
                            traversalStack[stackTop].secondChildIterator);

                        isLeaf = aTree.isLeaf(firstChildIterator);

                        aTree.getChildren(
                            firstChildIterator,
                            firstChildIterator, secondChildIterator);
                    }
                    else
                    {
                        traversalStack[++stackTop].tMin = firstTMin;
                        traversalStack[stackTop].isLeaf = aTree.isLeaf(firstChildIterator);

                        aTree.getChildren(
                            firstChildIterator,
                            traversalStack[stackTop].firstChildIterator,
                            traversalStack[stackTop].secondChildIterator);

                        isLeaf = aTree.isLeaf(secondChildIterator);

                        aTree.getChildren(
                            secondChildIterator,
                            firstChildIterator, secondChildIterator);
                    }
                }
                else if(enterFirstChild | enterSecondChild)
                {
                    t_Iterator nextIterator = enterFirstChild ?
                        firstChildIterator :
                        secondChildIterator;

                    isLeaf = aTree.isLeaf(nextIterator);

                    aTree.getChildren(nextIterator,
                        firstChildIterator, secondChildIterator);
                }
                else
                {
                    break;
                }
            }

            if(isLeaf)
            {
                typename tBVH::ElementIterator elementBegin, elementEnd;

                aTree.getElements(
                    firstChildIterator, secondChildIterator,
                    elementBegin, elementEnd);

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

            while(stackTop >= 0 && oResult.dist < traversalStack[stackTop].tMin)
            {
                --stackTop;
            }

            if(stackTop < 0)
            {
                break;
            }

            isLeaf              = traversalStack[stackTop].isLeaf;
            firstChildIterator  = traversalStack[stackTop].firstChildIterator;
            secondChildIterator = traversalStack[stackTop--].secondChildIterator;
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__BVHINTERSECTOR2_HPP
