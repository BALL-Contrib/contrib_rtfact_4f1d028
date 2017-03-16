/* 
 *  KdTreeIntersector.hpp
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
 
 #ifndef RTFACT__KDTREEINTERSECTOR_HPP
#define RTFACT__KDTREEINTERSECTOR_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Primitive.hpp>
#include <RTfact/Model/Intersector/Detail/Mailbox.hpp>
#include <RTfact/Utils/RayPacket.hpp>

RTFACT_NAMESPACE_BEGIN

#define RTFACT__NEAR  0
#define RTFACT__FAR   1
#define RTFACT__LEFT  0
#define RTFACT__RIGHT 1
#define RTFACT__MIN   0
#define RTFACT__MAX   1

template<
    class tIntersector>
class KdTreeIntersector :
    public tIntersector
{

protected:

    template<
        class tIterator,
        int taSize>
    struct TraversalState
    {
        Packet<taSize, float> tMin;
        Packet<taSize, float> tMax;
        tIterator treeIterator;
    };

    #ifdef RTFACT_USE_MAILBOXING

        Detail::Mailbox mMailbox;

    #endif

public:

    template<
        umask taIsectData,
        bool taCommonOrg,
        uint taSize,
        class tKdTree>
    RTFACT_INTERSECTOR_INLINE void intersect(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        tKdTree& aTree,
        typename tIntersector::template Result<taSize>& oResult)
    {
        typedef Packet<taSize, float>        t_Packet;
        typedef Vec3f<taSize>                t_Vec3f;
        typedef BitMask<taSize>              t_BitMask;
        typedef ActiveMask<taSize>     t_ActiveMask;
        typedef typename t_Packet::Container t_PacketC;
        typedef typename t_Vec3f::Container  t_Vec3fC;

        oResult.initBase(aTree.getElementBase());

        t_BitMask rayDirSignMask[3];
        rayDirSignMask[0].setFalse();
        rayDirSignMask[1].setFalse();
        rayDirSignMask[2].setFalse();

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        {
            rayDirSignMask[0].setContainer(i, aRayPacket.dir(i).x.getSignIntMask());
            rayDirSignMask[1].setContainer(i, aRayPacket.dir(i).y.getSignIntMask());
            rayDirSignMask[2].setContainer(i, aRayPacket.dir(i).z.getSignIntMask());
        }

        t_Vec3f l1, l2, lMin, lMax;
        t_Packet tMin, tMax;
        t_ActiveMask rayActiveMask;

        //REMOVE
        rayActiveMask.setFalse();
        
        t_BitMask rayActiveBitMask;
        rayActiveBitMask.setFalse();

        t_Vec3fC boundsMin =
            t_Vec3fC::replicate(aTree.getBounds().min);

        t_Vec3fC boundsMax =
            t_Vec3fC::replicate(aTree.getBounds().max);

        t_Vec3fC l1part, l2part;

        if(taCommonOrg)
        {
            l1part = boundsMin - aRayPacket.org(0);
            l2part = boundsMax - aRayPacket.org(0);
        }

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        {
            if(!taCommonOrg)
            {
                l1part = boundsMin - aRayPacket.org(i);//I-more
                l2part = boundsMax - aRayPacket.org(i);
            }

            l1(i) = l1part * aRayPacket.invDir(i);
            l2(i) = l2part * aRayPacket.invDir(i);
            lMin(i) = t_Vec3fC::min(l1(i), l2(i));
            lMax(i) = t_Vec3fC::max(l1(i), l2(i));
            
            tMin(i) = Math::max(Math::max(Math::max(
                aRayPacket.tMin(i), lMin(i).x), lMin(i).y), lMin(i).z);

            tMax(i) = Math::min(Math::min(Math::min(
                oResult.dist(i), lMax(i).x), lMax(i).y), lMax(i).z);

            rayActiveMask(i) = aRayMask(i) & (tMin(i) <= tMax(i));

            rayActiveBitMask.setContainer(i, rayActiveMask(i).getIntMask());
        }

        if(rayActiveBitMask.isFalse())
        {
            return;
        }

        rayActiveMask.updateActiveContainers();

        TraversalState<typename tKdTree::Iterator, taSize>
            stack[RTFACT_TRAVERSER_MAX_STACK_DEPTH];
        int top = -1;

        t_BitMask terminationMask = ~rayActiveBitMask;

        #ifdef RTFACT_USE_MAILBOXING

            Mailbox mailbox;

        #endif

        typename tKdTree::Iterator iterator = aTree.getRoot();

        for(;;)
        {
            int splitDimension;

            while((splitDimension = aTree.getType(iterator)) != 3)
            {
                const t_PacketC splitValue =
                    t_Packet::Container::replicate(aTree.getSplitValue(iterator));

                t_BitMask farChildConditionMask, nearChildConditionMask;
                farChildConditionMask.setFalse();
                nearChildConditionMask.setFalse();
                t_Packet tSplit;

                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, rayActiveMask)
                {
                    if(taCommonOrg)
                    {
                        tSplit(i) = (splitValue - aRayPacket.org(0).get(splitDimension)) *
                                    aRayPacket.invDir(i).get(splitDimension);
                    }
                    else
                    {
                        tSplit(i) = (splitValue - aRayPacket.org(i).get(splitDimension)) *
                                    aRayPacket.invDir(i).get(splitDimension);
                    }

                    farChildConditionMask.setContainer(
                        i, (tMin(i) > tSplit(i)).getIntMask());

                    nearChildConditionMask.setContainer(
                        i, (tSplit(i) > tMax(i)).getIntMask());
                }

                const t_BitMask bothChildrenActiveMask =
                    ~(nearChildConditionMask | farChildConditionMask) & rayActiveBitMask;

                nearChildConditionMask &= rayActiveBitMask;
                farChildConditionMask &= rayActiveBitMask;

                const t_BitMask rightChildActiveMask =
                    (nearChildConditionMask & (rayDirSignMask[splitDimension])) |
                    (farChildConditionMask & ~(rayDirSignMask[splitDimension])) |
                    bothChildrenActiveMask;

                const t_BitMask leftChildActiveMask =
                    (nearChildConditionMask & ~(rayDirSignMask[splitDimension])) |
                    (farChildConditionMask & (rayDirSignMask[splitDimension])) |
                    bothChildrenActiveMask;

                if(leftChildActiveMask.isFalse())       // continue with the right child
                {
                    iterator = aTree.getRightChild(iterator);
                }
                else if(rightChildActiveMask.isFalse()) // continue with the left child
                {
                    iterator = aTree.getLeftChild(iterator);
                }
                else                                    // both children are traversed
                {
                    const uint leftChildFirst = 
                        rayDirSignMask[splitDimension].isFalse();

                    iterator = aTree.getLeftChild(iterator);

                    ++top;

                    stack[top].treeIterator = iterator + leftChildFirst;

                    iterator += !leftChildFirst;

                    RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, rayActiveMask)
                    {
                        stack[top].tMin(i) = Math::max(tSplit(i), tMin(i));
                        stack[top].tMax(i) = tMax(i);
                        tMax(i) = Math::min(tMax(i), tSplit(i));
                    }

                    rayActiveBitMask &= leftChildFirst ? leftChildActiveMask :
                                                         rightChildActiveMask;
                }
            }

            typename tKdTree::ElementIterator elementBegin, elementEnd;
            aTree.getElements(iterator, elementBegin, elementEnd);

            if(elementBegin != elementEnd)
            {
                do
                {
                    #ifdef RTFACT_USE_MAILBOXING

                        if(!mailbox.check(&(*elementBegin)))
                        {
                            tIntersector::intersect<taIsectData,
                                                    taCommonOrg>(
                                aRayPacket,
                                rayActiveMask,
                                *elementBegin,
                                oResult);

                            mailbox.mark(&(*elementBegin));
                        }

                    #else

                        tIntersector::template intersect<
                            taIsectData,
                            taCommonOrg>(
                                aRayPacket,
                                rayActiveMask,
                                *elementBegin,
                                oResult);

                    #endif

                    ++elementBegin;
                }
                while(elementBegin != elementEnd);

                terminationMask |= rayActiveBitMask &
                                   (oResult.dist <= tMax).getBitMask();

                if(terminationMask.isTrue())
                {
                    return;
                }
            }

            if(top < 0)
            {
                return;
            }
            else
            {
                tMin = stack[top].tMin;
                tMax = stack[top].tMax;

                //terminationMask |= (tMax >= aRayPacket.tMax).getBitMask();

                rayActiveBitMask = ~terminationMask & (tMin <= tMax).getBitMask();
                iterator         = stack[top].treeIterator;
                --top;
            }
        }
    }

    template<
        umask taIsectData,
        bool taCommonOrg,
        class tKdTree>
    RTFACT_INTERSECTOR_INLINE void intersect(
        const RayPacket<1>& aRay,
        const ActiveMask<1>& aRayMask,
        tKdTree& aTree,
        typename tIntersector::template Result<1>& oResult)
    {
        oResult.initBase(aTree.getElementBase());

        typename tKdTree::Iterator iterator;
        int splitDimension;

        const Vec3f<1> l1 = (aTree.getBounds().min - aRay.org) * aRay.invDir;
        const Vec3f<1> l2 = (aTree.getBounds().max - aRay.org) * aRay.invDir;
        const Vec3f<1> lMin = Vec3f<1>::min(l1, l2);
        const Vec3f<1> lMax = Vec3f<1>::max(l1, l2);

        float tMin = std::max(std::max(std::max(
            aRay.tMin, lMin.x), lMin.y), lMin.z);

        float tMax = std::min(std::min(std::min(
            aRay.tMax, lMax.x), lMax.y), lMax.z);

        if(tMin > tMax)
        {
            return;
        }

        TraversalState<typename tKdTree::Iterator, 1>
            stack[RTFACT_TRAVERSER_MAX_STACK_DEPTH];
        int top = -1;

        const uint childrenOffset[3][2] =
        {
            { (aRay.dir.xf < 0), (aRay.dir.xf >= 0) },
            { (aRay.dir.yf < 0), (aRay.dir.yf >= 0) },
            { (aRay.dir.zf < 0), (aRay.dir.zf >= 0) }
        };

        #ifdef RTFACT_USE_MAILBOXING

            Mailbox mailbox;

        #endif

        iterator = aTree.getRoot();

        for(;;)
        {
            while((splitDimension = aTree.getType(iterator)) != 3)
            {
                const float tSplit =
                    (aTree.getSplitValue(iterator) -
                     aRay.org.get(splitDimension)) * aRay.invDir.get(splitDimension);

                iterator = aTree.getLeftChild(iterator);

                if(tSplit > tMax)
                {
                    iterator += childrenOffset[splitDimension][RTFACT__NEAR];
                }
                else if(tSplit < tMin)
                {
                    iterator += childrenOffset[splitDimension][RTFACT__FAR];
                }
                else
                {
                    stack[++top].tMin = tSplit;
                    stack[top].tMax = tMax;
                    tMax = tSplit;
                    stack[top].treeIterator =
                        iterator + childrenOffset[splitDimension][RTFACT__FAR];

                    iterator += childrenOffset[splitDimension][RTFACT__NEAR];
                }
            }

            typename tKdTree::ElementIterator elementBegin, elementEnd;
            aTree.getElements(iterator, elementBegin, elementEnd);

            if(elementBegin != elementEnd)
            {
                do
                {
                    #ifdef RTFACT_USE_MAILBOXING

                        if(!mailbox.check(&(*elementBegin)))
                        {
                            tIntersector::intersect<taIsectData,
                                                    taCommonOrg>(
                                aRay,
                                aRayMask,
                                *elementBegin,
                                oResult);

                            mailbox.mark(&(*elementBegin));
                        }

                    #else

                    tIntersector::intersect<taIsectData,
                                                taCommonOrg>(
                            aRay,
                            aRayMask,
                            *elementBegin,
                            oResult);

                    #endif

                    ++elementBegin;
                }
                while(elementBegin != elementEnd);

                if(oResult.dist < tMax)
                {
                    return;
                }
            }

            if(top < 0)
            {
                return;
            }
            else
            {
                tMin        = stack[top].tMin;
                tMax        = stack[top].tMax;
                iterator    = stack[top].treeIterator;
                --top;
            }
        }
    }
};

#undef RTFACT__NEAR
#undef RTFACT__FAR
#undef RTFACT__LEFT
#undef RTFACT__RIGHT
#undef RTFACT__MIN
#undef RTFACT__MAX

RTFACT_NAMESPACE_END

#endif // RTFACT__KDTREEINTERSECTOR_HPP
