/* 
 *  SimpleKdTreeBuilder.hpp
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
 *  Created on: 2008-06-18 01:29:56 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__SIMPLEKDTREEBUILDER_HPP
#define RTFACT__SIMPLEKDTREEBUILDER_HPP

#define RTFACT__MAX_DEPTH 25
#define RTFACT__MAX_FACES 5

#include <RTfact/Config/Common.hpp>

#include <cstdlib>
#include <boost/lambda/bind.hpp>

#include <RTfact/Utils/BBox.hpp>
#include <RTfact/Concept/Builder.hpp>

RTFACT_NAMESPACE_BEGIN

class SimpleKdTreeBuilder :
    public Builder
{
    template<
        class tElement>
    struct PrimitiveInfo
    {
        BBox bounds;
        const tElement* element;

        PrimitiveInfo()
        {}

        PrimitiveInfo(
            const tElement& aElement,
            const BBox& aBounds
        ) :
            bounds(aBounds),
            element(&aElement)
        {}

        const tElement& getElement() const
        {
            return *element;
        }
    };

    /*class PointComparer
    {
        const Scene::PointVector& mPoints;
        int mAxis;

    public:

        PointComparer(const Scene& aScene, const int aAxis)
            : mPoints(aScene.primitives), mAxis(aAxis)
        {}

        bool operator()(const int aIndex1, const int aIndex2)
        {
            return mPoints[aIndex1].pos.data[mAxis] < mPoints[aIndex2].pos.data[mAxis];
        }
    };*/

    /*template<class tIterator>
    void _recursiveBuild(uint aDepth, tIterator aBegin, tIterator aEnd,
        const BBox& aBounds, IndexKdTree::Iterator aIterator, IndexKdTree& aTree, Scene& aScene)
    {
        if(aDepth > RTFACT__MAX_DEPTH || (aEnd - aBegin) <= MIN_POINTS)
        {
            aTree.createLeafNode(aIterator, aBegin, aEnd);

            return;
        }

        const int splitAxis = aBounds.getMaximumExtent();
        tIterator splitPosition = aBegin + (aEnd - aBegin) / 2;

        std::nth_element(aBegin, splitPosition, aEnd, PointComparer(aScene, splitAxis));

        const float splitValue = aScene.primitives[*splitPosition].pos.data[splitAxis];

        BBox leftBounds = aBounds;
        BBox rightBounds = aBounds;

        leftBounds.max.data[splitAxis] = rightBounds.min.data[splitAxis] = splitValue;

        aTree.createInnerNode(aIterator, splitAxis, splitValue);

        _recursiveBuild(aDepth + 1, aBegin, splitPosition, leftBounds, aTree.getLeftChild(aIterator), aTree, aScene);
        _recursiveBuild(aDepth + 1, splitPosition, aEnd, rightBounds, aTree.getRightChild(aIterator), aTree, aScene);
    }*/

public:

    template<
        class tKdTree,
        class tIterator,
        class tBBoxExtractor>
    void build(
        tKdTree& aTree,
        tIterator aElemBegin,
        tIterator aElemEnd,
        tBBoxExtractor aExtractor)
    {
        typedef PrimitiveInfo<typename tKdTree::Element> t_PrimitiveInfo;

        BBox bounds = BBox::empty();
        size_t elementCount = 0;
        Vector<t_PrimitiveInfo> buildElements;
        //buildElements.reserve(aElemEnd - aElemBegin);

        for(tIterator it = aElemBegin; it != aElemEnd; ++it)
        {
            const t_PrimitiveInfo primInfo(*it, it->getBounds());

            bounds.extend(primInfo.bounds);
            buildElements.push_back(primInfo);
            ++elementCount;
        }

        aTree.setElementBase(*aElemBegin);
        aTree.init(elementCount, bounds);

        buildRecursive(aTree, buildElements.begin(), buildElements.end(),
                       bounds, aTree.getRoot(), 0);
    }

private:

    template<
        class tKdTree,
        class tIterator>
    void buildRecursive(
        tKdTree& aTree,
        tIterator aBegin,
        tIterator aEnd,
        const BBox& aBounds,
        typename tKdTree::Iterator aIterator,
        uint aDepth)
    {
        typedef PrimitiveInfo<typename tKdTree::Element> t_PrimitiveInfo;

        if(aDepth > RTFACT__MAX_DEPTH ||
           (aEnd - aBegin) <= RTFACT__MAX_FACES)
        {
            aTree.createLeafNode(
                aIterator, aBegin, aEnd,
                boost::lambda::bind(
                    &PrimitiveInfo<typename tKdTree::Element>::getElement,
                    boost::lambda::_1));

            return;
        }

        int splitAxis = aBounds.getMaximumExtent();

        BBox leftBounds = aBounds;
        BBox rightBounds = aBounds;

        float splitValue = 0.5f * (aBounds.min.get(splitAxis) +
                                   aBounds.max.get(splitAxis));
        leftBounds.max.get(splitAxis) = splitValue;
        rightBounds.min.get(splitAxis) = splitValue;

        aTree.createInnerNode(aIterator, splitAxis, splitValue);

        Vector<t_PrimitiveInfo> leftElements, rightElements;

        for(tIterator it = aBegin; it != aEnd; ++it)
        {
            if(it->bounds.min.get(splitAxis) < splitValue)
            {
                leftElements.push_back(*it);
            }

            if(it->bounds.max.get(splitAxis) >= splitValue)
            {
                rightElements.push_back(*it);
            }
        }

        buildRecursive(aTree, leftElements.begin(), leftElements.end(),
                       leftBounds, aTree.getLeftChild(aIterator), aDepth + 1);
        buildRecursive(aTree, rightElements.begin(), rightElements.end(),
                       rightBounds, aTree.getRightChild(aIterator), aDepth + 1);
    }
};

RTFACT_NAMESPACE_END

#undef RTFACT__MAX_DEPTH
#undef RTFACT__MAX_FACES

#endif // RTFACT__SIMPLEKDTREEBUILDER_HPP
