/* 
 *  SAHBVHBuilder.hpp
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
 *  Author(s): Iliyan Georgiev, Rossen Dimov, Felix Klein, Vaclav Krajicek
 */
 
 #ifndef RTFACT__SAHBVHBUILDER_HPP
#define RTFACT__SAHBVHBUILDER_HPP

#define RTFACT__CREATE_LEAF 4

#include <RTfact/Config/Common.hpp>

#include <algorithm>
#include <boost/lambda/bind.hpp>

#include <RTfact/Utils/BBox.hpp>
#include <RTfact/Utils/Containers/IntMap.hpp>

#include <RTfact/Concept/Builder.hpp>
#include <RTfact/Model/Builder/Detail/SAHParameters.hpp>

RTFACT_NAMESPACE_BEGIN

class SAHBVHBuilder :
    public Builder
{
    struct Params
    {
        static const int MAX_FACES = 8;
        static const int MAX_DEPTH = 32;
    };

    template<
        class tElement>
    struct PrimitiveInfo
    {
        const tElement* element;
        BBox bounds;
        Vec3f<1> centroid;

        PrimitiveInfo()
        {}

        PrimitiveInfo(
            const tElement& aElement,
            const BBox& aBounds
        ) :
            element(&aElement),
            bounds(aBounds),
            centroid(aBounds.getCentroid())
        {}

        const tElement& getElement() const
        {
            return *element;
        }
    };

    class ControidComparer
    {
        int mAxis;

    public:

        ControidComparer(
            const int aAxis
        ) :
            mAxis(aAxis)
        {}

        template<
            class tPrimitiveInfo>
        bool operator()(
            const tPrimitiveInfo& aPrim1,
            const tPrimitiveInfo& aPrim2) const
        {
            return (float) aPrim1.centroid.get(mAxis) <
                   (float) aPrim2.centroid.get(mAxis);
        }
    };

    SAHParameters m_SAHParams;

public:

    SAHBVHBuilder()
    {
        m_SAHParams.intersectionCost = 1.f;
        m_SAHParams.traversalCost = 1.f;
    }

    template<
        class tBVH,
        class tIterator,
        class tBBoxExtractor>
    void build(
        tBVH& aBVH,
        tIterator aBegin,
        tIterator aEnd,
        tBBoxExtractor aExtractor)
    {
        BBox bounds = BBox::empty();
        Vector<PrimitiveInfo<typename tBVH::Element> > buildNodes;

        //buildNodes.reserve(aElemEnd - aElemBegin);

        size_t index = 0;

        for(tIterator it = aBegin; it != aEnd; ++it)
        {
            buildNodes.push_back(
                PrimitiveInfo<typename tBVH::Element>(*it, aExtractor(*it)));
            bounds.extend(buildNodes[index++].bounds);
        }

        aBVH.setElementBase(*aBegin);
        aBVH.init(buildNodes.size());

        if(buildNodes.size() == 0)
            return;

        aBVH.setBounds(aBVH.getRoot(), bounds);

        buildRecursive(
            aBVH, buildNodes.begin(), buildNodes.end(),
            bounds, aBVH.getRoot(), 0);

        aBVH.finalizeBuild();
    }

	template<
        class tBVH,
		class tElement,
        class tIterator,
        class tBBoxExtractor>
    void build(
        tBVH& aBVH,
		const tElement& aElementBase,
        tIterator aBegin,
        tIterator aEnd,
        tBBoxExtractor aExtractor)
    {
        BBox bounds = BBox::empty();
        Vector<PrimitiveInfo< typename tBVH::Element > > buildNodes;

        //buildNodes.reserve(aElemEnd - aElemBegin);

        size_t index = 0;

        for(tIterator it = aBegin; it != aEnd; ++it)
        {
            buildNodes.push_back(
                PrimitiveInfo<typename tBVH::Element>(*it, aExtractor(*it)));
            bounds.extend(buildNodes[index++].bounds);
        }

        aBVH.setElementBase(aElementBase);
        aBVH.init(buildNodes.size());

        if(buildNodes.size() == 0)
            return;

        aBVH.setBounds(aBVH.getRoot(), bounds);

        buildRecursive(
            aBVH, buildNodes.begin(), buildNodes.end(),
            bounds, aBVH.getRoot(), 0);

        aBVH.finalizeBuild();
    }

    template<
        class tBVH,
        class tElement,
        class tItIterator,
        class tBBoxExtractor>
    void build(
        tBVH& aBVH,
        tItIterator aKeyBegin,
        tItIterator aKeyEnd,
        const IntMap<tElement> &aMap,
        tBBoxExtractor aExtractor)
    {
        BBox bounds = BBox::empty();
        Vector<PrimitiveInfo<typename tBVH::Element> > buildNodes;

        //buildNodes.reserve(aElemEnd - aElemBegin);

        size_t index = 0;

        for(tItIterator it = aKeyBegin; it != aKeyEnd; ++it)
        {
            buildNodes.push_back(
                PrimitiveInfo<typename tBVH::Element>(aMap[*it], aExtractor(aMap[*it])));
            bounds.extend(buildNodes[index++].bounds);
        }

        aBVH.setElementBase(aMap.getElementBase());
        aBVH.init(buildNodes.size());

        if(buildNodes.size() == 0)
            return;

        aBVH.setBounds(aBVH.getRoot(), bounds);

        buildRecursive(
            aBVH, buildNodes.begin(), buildNodes.end(),
            bounds, aBVH.getRoot(), 0);

        aBVH.finalizeBuild();
    }


private:

    template<
        class tBVH,
        class tIterator>
    void buildRecursive(
        tBVH& aBVH,
        tIterator aBegin,
        tIterator aEnd,
        const BBox& aBounds,
        typename tBVH::Iterator aIterator,
        uint aDepth)
    {
        int splitAxis;
        float splitValue;

        calcCost(aBegin, aEnd, aBounds, splitAxis, splitValue);


        if(splitAxis == RTFACT__CREATE_LEAF ||
           aEnd - aBegin <= Params::MAX_FACES)
        {
            aBVH.createLeafNode(
                aIterator, aBegin, aEnd,
                boost::lambda::bind(
                    &PrimitiveInfo<typename tBVH::Element>::getElement,
                    boost::lambda::_1));

            return;
        }

        tIterator leftIt = aBegin;
        tIterator rightIt = aEnd - 1;
        BBox leftBounds = BBox::empty();
        BBox rightBounds = BBox::empty();

        for(;;)
        {
            while(leftIt->centroid.get(splitAxis) < splitValue &&
                  leftIt != aEnd - 1 || (leftIt == rightIt && leftIt == aBegin))
            {
                leftBounds.extend(leftIt->bounds);
                ++leftIt;
            }

            while(rightIt->centroid.get(splitAxis) >= splitValue &&
                  rightIt != aBegin || (leftIt == rightIt && rightIt == aEnd - 1))
            {
                rightBounds.extend(rightIt->bounds);
                --rightIt;
            }

            RTFACT_ASSERT(rightIt != leftIt || (leftIt == rightIt &&
                        (rightIt == aBegin || leftIt == aEnd - 1)));

            if(rightIt < leftIt)
            {
                break;
            }

            std::swap(*leftIt, *rightIt);
        }

        typename tBVH::ChildIteratorPair childIters =
            aBVH.createInnerNode(aIterator);
        aBVH.setBounds(childIters.first, leftBounds);
        aBVH.setBounds(childIters.second, rightBounds);

        RTFACT_ASSERT(aBegin < leftIt);
        RTFACT_ASSERT(leftIt < aEnd);

        buildRecursive(aBVH, aBegin, leftIt, leftBounds,
                       childIters.first, aDepth + 1);
        buildRecursive(aBVH, leftIt, aEnd, rightBounds,
                       childIters.second, aDepth + 1);
    }

    template<
        class tIterator>
    void calcCost(
        tIterator aBegin,
        tIterator aEnd,
        const BBox& aBounds,
        int& oSplitAxis,
        float& oSplitValue)
    {
        oSplitValue = RTFACT_FLOAT_MAX;
        oSplitAxis = -1;
        float bestCost = RTFACT_FLOAT_MAX;

        const float parentArea = aBounds.getArea();

        for(int axis = 0; axis < 3; ++axis)
        {
            std::sort(aBegin, aEnd, ControidComparer(axis));

            BBox leftBounds = BBox::empty();
            BBox rightBounds = BBox::empty();
            int leftCount = 0;
            int rightCount = 0;

            Vector<float> accumCost;
            accumCost.resize(aEnd - aBegin - 1);

            typename std::iterator_traits<tIterator>::difference_type i = 0;

            for(tIterator it = aBegin; it != aEnd - 1; ++it, ++i)
            {
                leftBounds.extend(it->bounds);

                ++leftCount;

                accumCost[i] = leftCount * leftBounds.getArea() / parentArea;
            }

            i = aEnd - aBegin - 2;

            for(tIterator it = aEnd - 1; it != aBegin; --it, --i)
            {
                rightBounds.extend(it->bounds);

                ++rightCount;

                float curCost = rightCount * rightBounds.getArea() /parentArea +
                                accumCost[i];
                curCost = m_SAHParams.traversalCost +
                          m_SAHParams.intersectionCost * curCost;

                if(curCost < bestCost)
                {
                    bestCost = curCost;
                    oSplitValue = it->centroid.get(axis);
                    oSplitAxis = axis;
                }
            }
        }

        float curCost = m_SAHParams.traversalCost +
                        m_SAHParams.intersectionCost * (aEnd - aBegin);

        if(curCost <= bestCost)
        {
            oSplitAxis = RTFACT__CREATE_LEAF;
        }
    }
};

RTFACT_NAMESPACE_END

#undef RTFACT__CREATE_LEAF

#endif // RTFACT__SAHBVHBUILDER_HPP
