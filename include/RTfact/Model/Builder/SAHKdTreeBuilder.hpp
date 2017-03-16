/* 
 *  SAHKdTreeBuilder.hpp
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
 *  Created on: 2009-01-16 03:55:31 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT_SAHKDTREEBUILDER_HPP_INCLUDED
#define RTFACT_SAHKDTREEBUILDER_HPP_INCLUDED

#include <RTfact/Config/Common.hpp>

#include <cstdlib>
#include <boost/lambda/bind.hpp>

#include <RTfact/Utils/BBox.hpp>
#include <RTfact/Utils/Containers/Vector.hpp>
#include <RTfact/Model/Primitive/Triangle.hpp>
#include <RTfact/Model/Builder/Detail/SAHParameters.hpp>

RTFACT_NAMESPACE_BEGIN

template <class tKdTree>
class SAHKdTreeBuilder :
    public Builder
{
    typedef typename tKdTree::Element t_Element;

    struct ElementInfo
    {
        const t_Element* element;
        BBox bounds;

        ElementInfo() :
            element(RTFACT_NULL),
            bounds(BBox::empty())
        {}

        ElementInfo(const t_Element* aElement, const BBox& aBounds) :
            element(aElement),
            bounds(aBounds)
        {}

        const t_Element& getElement() const
        {
            return *(t_Element*)element;
        }
    };

    struct EventInfo
    {
        const void* element;
        bool start;
        Vec3f<1> position;

        EventInfo()
        {}

        EventInfo(const void* aElement, bool aStart, const Vec3f<1>& aPosition) :
            element(aElement),
            start(aStart),
            position(aPosition)
        {}
    };

    class EventComparer
    {
        int mAxis;

    public:

        EventComparer(const int aAxis)
            : mAxis(aAxis)
        {}

        bool operator()(const EventInfo& aEv1, const EventInfo& aEv2) const
        {
            return (aEv1.position.get(mAxis) < aEv2.position.get(mAxis)) ||
                (aEv1.position.get(mAxis) == aEv2.position.get(mAxis)) &&
                aEv1.element < aEv2.element;
        }
    };

    
    struct SplitPlane
    {
        int SplitAxis;
        float SplitValue;
        bool PutBothOnLeft;

        int BothCount;
        int LeftCount;
        int RightCount;
    };
    
    Vector<ElementInfo> m_BuildElements;
    Vector<EventInfo> m_Events;
    SAHParameters m_SAHParams;

    int m_UnsuccSteps;

public:

    SAHKdTreeBuilder()
    {
        m_SAHParams.intersectionCost = 1.5f;
        m_SAHParams.traversalCost = 1.f;
        m_SAHParams.ATCDepthOffset = 1.f;
        m_SAHParams.ATCDepthCoef = 0.5f;
        m_SAHParams.ATCSuccessfulStepMinQuality = 0.75f;
        m_SAHParams.terminateQuality = 0.99f;

        m_UnsuccSteps = 0;
    }

    template<
        class tIterator,
        class tBBoxExtractor>
    void build(
        tKdTree& aTree,
        tIterator aElemBegin,
        tIterator aElemEnd,
        tBBoxExtractor aBBoxExtractor)
    {
        BBox bounds = BBox::empty();

        int elCount = 0;
        for(tIterator it = aElemBegin; it != aElemEnd; ++it)
        {
            elCount++;
        }

        m_BuildElements.reserve(elCount * 2);

        for(tIterator it = aElemBegin; it != aElemEnd; ++it)
        {
            bounds.extend(it->getBounds());
            const ElementInfo primInfo(&*it, it->getBounds());

            m_BuildElements.push_back(primInfo);
        }

        aTree.setElementBase(*aElemBegin);
        aTree.init(m_BuildElements.size(), bounds);

        m_Events.reserve(elCount * 2);

        float dMax = m_SAHParams.ATCDepthOffset + m_SAHParams.ATCDepthCoef * 
            (Math::log2((float)elCount));
        m_SAHParams.maxUnsuccsessfulSteps = (unsigned int)(m_SAHParams.ATCDepthOffset + 
            m_SAHParams.ATCDepthCoef * dMax);

        buildRecursive(aTree, 0, 
            m_BuildElements.end() - m_BuildElements.begin(), 
            bounds, aTree.getRoot(), 0, 0);
    }                         

private:

    void buildRecursive(
        tKdTree& aTree,
        int aStartOffset,
        int aEndOffset,
        const BBox& aBounds,
        typename tKdTree::Iterator aIterator,
        uint aDepth,
        int aUnSuccessfulSteps)
    {
        SplitPlane sp;

        typedef typename Vector<ElementInfo>::iterator tIterator;
        tIterator begin = m_BuildElements.begin() + aStartOffset;
        tIterator end = m_BuildElements.begin() + aEndOffset;

        float quality = calcCost(begin, end, aBounds, sp);
        
        if(shouldTerminate(quality, aEndOffset - aStartOffset, 
            aUnSuccessfulSteps, aBounds.getArea()))
        {
            aTree.createLeafNode(
                aIterator, begin, end,
                boost::lambda::bind(
                &ElementInfo::getElement,
                boost::lambda::_1));

            return;
        }
        else if (quality > m_SAHParams.ATCSuccessfulStepMinQuality)
            aUnSuccessfulSteps++;

        RTFACT_ASSERT(sp.LeftCount >= 0);
        RTFACT_ASSERT(sp.RightCount >= 0);
        RTFACT_ASSERT(sp.BothCount >= 0);
        RTFACT_ASSERT(sp.LeftCount + sp.RightCount + sp.BothCount == aEndOffset - aStartOffset);

        BBox leftBounds = aBounds;
        BBox rightBounds = aBounds;

        RTFACT_ASSERT(!(aBounds.min.x == aBounds.max.x &&
            aBounds.min.y == aBounds.max.y &&
            aBounds.min.z == aBounds.max.z));

        leftBounds.max.get(sp.SplitAxis) = sp.SplitValue;
        rightBounds.min.get(sp.SplitAxis) = sp.SplitValue;

        RTFACT_ASSERT(!(leftBounds.min.x == leftBounds.max.x &&
            leftBounds.min.y == leftBounds.max.y &&
            leftBounds.min.z == leftBounds.max.z));
        RTFACT_ASSERT(!(rightBounds.min.x == rightBounds.max.x &&
            rightBounds.min.y == rightBounds.max.y &&
            rightBounds.min.z == rightBounds.max.z));
        
        aTree.createInnerNode(aIterator, sp.SplitAxis, sp.SplitValue);

        m_BuildElements.resize(aEndOffset + sp.BothCount);

        begin = m_BuildElements.begin() + aStartOffset;
        end = m_BuildElements.begin() + aEndOffset;

        tIterator b = begin;
        tIterator l = b + sp.BothCount;
        tIterator le, r;
        le = r = l + sp.LeftCount;
        tIterator re = end;

#define IS_FLAT(element, sp) \
    (element->bounds.max.get(sp.SplitAxis) == sp.SplitValue && \
     element->bounds.min.get(sp.SplitAxis) == sp.SplitValue)
#define IS_LEFT(element, sp) \
    ((element->bounds.max.get(sp.SplitAxis) <= sp.SplitValue && \
     element->bounds.min.get(sp.SplitAxis) < sp.SplitValue) || \
     (IS_FLAT(element, sp) && sp.PutBothOnLeft))
#define IS_RIGHT(element, sp) \
    ((element->bounds.min.get(sp.SplitAxis) >= sp.SplitValue && \
     element->bounds.max.get(sp.SplitAxis) > sp.SplitValue) || \
     (IS_FLAT(element, sp) && !sp.PutBothOnLeft))
#define IS_BOTH(element, sp) \
    (element->bounds.max.get(sp.SplitAxis) > sp.SplitValue && \
     element->bounds.min.get(sp.SplitAxis) < sp.SplitValue)

        for(;;)
        {
            while(l != le && !IS_RIGHT(l, sp))
            {
                if(IS_LEFT(l, sp)) ++l;
                else
                {
                    while(IS_BOTH(b, sp)) ++b;

                    std::swap(*l, *b);
                }
            }
            
            while(r != re && !IS_LEFT(r, sp))
            {
                if(IS_RIGHT(r, sp)) ++r;
                else
                {
                    while(IS_BOTH(b, sp)) ++b;

                    std::swap(*r, *b);
                }
            }

            if(l != le || r != re)
            {
                RTFACT_ASSERT(r != re);
                RTFACT_ASSERT(l != le);

                std::swap(*l , *r);
            }
            else break;
        }

        int degenOffset = 0;
        
        for(tIterator lit = begin, rit = re; 
            lit != begin + sp.BothCount;)
        {
            RTFACT_ASSERT(lit->bounds.min.x <= lit->bounds.max.x);
            RTFACT_ASSERT(lit->bounds.min.y <= lit->bounds.max.y);
            RTFACT_ASSERT(lit->bounds.min.z <= lit->bounds.max.z);
            BBox newLBBox = BBox::empty();
            BBox newRBBox = BBox::empty();

            RTFACT_ASSERT(lit->bounds.min.get(sp.SplitAxis) < sp.SplitValue &&
                lit->bounds.max.get(sp.SplitAxis) > sp.SplitValue);
            
            clipAndReduceBBox(lit->bounds, lit->getElement(), 
                sp.SplitAxis, sp.SplitValue, newLBBox, newRBBox);

            RTFACT_ASSERT(newLBBox.min.x <= newLBBox.max.x);
            RTFACT_ASSERT(newLBBox.min.y <= newLBBox.max.y);
            RTFACT_ASSERT(newLBBox.min.z <= newLBBox.max.z);
            RTFACT_ASSERT(newRBBox.min.x <= newRBBox.max.x);
            RTFACT_ASSERT(newRBBox.min.y <= newRBBox.max.y);
            RTFACT_ASSERT(newRBBox.min.z <= newRBBox.max.z);

            RTFACT_ASSERT(!(newLBBox.min.x == newLBBox.max.x &&
                newLBBox.min.y == newLBBox.max.y &&
                newLBBox.min.z == newLBBox.max.z &&
                newRBBox.min.x == newRBBox.max.x &&
                newRBBox.min.y == newRBBox.max.y &&
                newRBBox.min.z == newRBBox.max.z));

            lit->bounds = newLBBox;
            rit->element = lit->element;
            rit->bounds = newRBBox;

            RTFACT_ASSERT(lit->bounds.max.get(sp.SplitAxis) <= sp.SplitValue);
            RTFACT_ASSERT(rit->bounds.min.get(sp.SplitAxis) >= sp.SplitValue);

            ++lit;
            ++rit;

            RTFACT_ASSERT(!(newLBBox.min.x == newLBBox.max.x &&
                newLBBox.min.y == newLBBox.max.y &&
                newLBBox.min.z == newLBBox.max.z) ||
                !(newRBBox.min.x == newRBBox.max.x &&
                newRBBox.min.y == newRBBox.max.y &&
                newRBBox.min.z == newRBBox.max.z));
        }

        buildRecursive(aTree, aStartOffset + sp.LeftCount + sp.BothCount, 
            aEndOffset + sp.BothCount,  rightBounds, aTree.getRightChild(aIterator), 
            aDepth + 1, aUnSuccessfulSteps);
        buildRecursive(aTree, aStartOffset + degenOffset, 
            aStartOffset + sp.LeftCount + sp.BothCount, leftBounds, 
            aTree.getLeftChild(aIterator), aDepth + 1, aUnSuccessfulSteps);
    }

    bool shouldTerminate(
        float aQuality, size_t aObjCount, int aUnsuccSteps, float aBBoxArea)
    {
        return aObjCount == 0 || aQuality > m_SAHParams.terminateQuality || 
            (aQuality > m_SAHParams.ATCSuccessfulStepMinQuality && 
            aUnsuccSteps >= m_SAHParams.maxUnsuccsessfulSteps) || aBBoxArea < 0.000001f;
    }

    template<class tIterator>
    float calcCost(
        tIterator aBegin,
        tIterator aEnd,
        const BBox& aBounds,
        SplitPlane& oSP)
    {
        int elemCount = aEnd - aBegin;
        
        m_Events.resize(elemCount * 2);

        oSP.SplitValue = RTFACT_FLOAT_MAX;
        oSP.SplitAxis = -1;
        oSP.PutBothOnLeft = false;
        oSP.LeftCount = -1;
        oSP.RightCount = -1;
        oSP.BothCount = -1;
        float bestCost = RTFACT_FLOAT_MAX;

        const float parentArea = aBounds.getArea();

        typedef typename Vector<EventInfo>::iterator EventsIt;

        EventsIt ev = m_Events.begin();
        for(tIterator it = aBegin; it != aEnd; ++it)
        {
            const EventInfo eventInfoB((void*)(it->element), true, it->bounds.min);
            *ev++ = eventInfoB;
            const EventInfo eventInfoE((void*)(it->element), false, it->bounds.max);
            *ev++ = eventInfoE;
        }

        for(int axis = 0; axis < 3; ++axis)
        {
            int nL = 0;
            int nR = elemCount;

            std::sort(m_Events.begin(), m_Events.end(), EventComparer(axis));

            for(EventsIt it = m_Events.begin(); it != m_Events.end();)
            {
                float val = it->position.get(axis);
                int nStart = 0;
                int nEnd = 0;
                int nFlat = 0;
                void* lastId = RTFACT_NULL;

                while(it != m_Events.end() && it->position.get(axis) == val)
                {
                    if(it->start) nStart++;
                    else nEnd++;
                    
                    if(it->element == lastId)
                    {
                        nStart--;
                        nEnd--;
                        nFlat++;
                    }

                    lastId = (void*)it->element;

                    it++;
                }

                nR -= nEnd;

                BBox left = aBounds;
                BBox right = aBounds;
                left.max.get(axis) = val;
                right.min.get(axis) = val;
                float lArea = left.getArea();
                float rArea = right.getArea();
                
                if(lArea < rArea)
                {
                    nL += nFlat;
                    nR -= nFlat;
                }
                
                float cost = nL * lArea + nR * rArea;
                if(cost < bestCost)
                {
                    oSP.SplitAxis = axis;
                    oSP.SplitValue = val;
                    oSP.PutBothOnLeft = lArea < rArea;
                    oSP.BothCount = nL + nR - elemCount;
                    oSP.LeftCount = nL - oSP.BothCount;
                    oSP.RightCount = nR - oSP.BothCount;
                    bestCost = cost;
                }

                nL += nStart;
                if(lArea >= rArea)                                               
                {
                    nL += nFlat;
                    nR -= nFlat;
                }
            }
        }

        bestCost = m_SAHParams.traversalCost + 
            m_SAHParams.intersectionCost * bestCost / parentArea;

        float leafCost = m_SAHParams.intersectionCost * elemCount;

        return bestCost / leafCost;
    }
        
    void clipAndReduceBBox(const BBox& aOriginalBBox, const Triangle& aTriangle, 
        int aPlaneAxis, float aPlaneOffset, BBox& aBBoxOut1, BBox& aBBoxOut2)
    {
        static const int mod3Table[] = {0, 1, 2, 0, 1, 2};

        aBBoxOut1 = BBox::empty();
        aBBoxOut2 = BBox::empty();

        for(int curPt = 0; curPt < 3; curPt++)
        {
            int nextPt = mod3Table[curPt + 1];

            float pVal = aTriangle.vertices[curPt].get(aPlaneAxis);

            if(pVal <= aPlaneOffset)
                aBBoxOut1.extend(aTriangle.vertices[curPt]);

            if(pVal >= aPlaneOffset)
                aBBoxOut2.extend(aTriangle.vertices[curPt]);

            float p1Val = pVal;
            float p2Val = aTriangle.vertices[nextPt].get(aPlaneAxis);

            if(p1Val > p2Val)
                std::swap(p1Val, p2Val);

            if(p1Val < aPlaneOffset && aPlaneOffset < p2Val) 
            {
                Vec3f<1> dir = aTriangle.vertices[curPt] - aTriangle.vertices[nextPt];
                float lambda = (aPlaneOffset - pVal) / dir.get(aPlaneAxis);
                Vec3f<1> intPt = aTriangle.vertices[curPt] + (aPlaneOffset - pVal)  * dir / dir.get(aPlaneAxis);

                intPt = Vec3f<1>::max(Vec3f<1>::min(intPt, aOriginalBBox.max), aOriginalBBox.min);

                aBBoxOut1.extend(intPt);
                aBBoxOut2.extend(intPt);
            }

        }

        aBBoxOut1.max = Vec3f<1>::min(aBBoxOut1.max, aOriginalBBox.max);
        aBBoxOut2.max = Vec3f<1>::min(aBBoxOut2.max, aOriginalBBox.max);

        aBBoxOut1.min = Vec3f<1>::max(aBBoxOut1.min, aOriginalBBox.min);
        aBBoxOut2.min = Vec3f<1>::max(aBBoxOut2.min, aOriginalBBox.min);

        aBBoxOut1.max.get(aPlaneAxis) = aPlaneOffset;
        aBBoxOut2.min.get(aPlaneAxis) = aPlaneOffset;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT_SAHKDTREEBUILDER_HPP_INCLUDED
