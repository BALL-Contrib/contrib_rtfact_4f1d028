/* 
 *  OrderedBVH.hpp
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
 *  Author(s): Iliyan Georgiev, Felix Klein, Georgiev Iliyan
 */
 
 #ifndef RTFACT__ORDEREDBVH_HPP
#define RTFACT__ORDEREDBVH_HPP

#include <RTfact/Config/Common.hpp>

#include <utility>
#include <limits>

#include <RTfact/Utils/BBox.hpp>
#include <RTfact/Utils/IO/SerializationUtils.hpp>

#include <RTfact/Concept/Scene.hpp>
#include <RTfact/Concept/Primitive.hpp>
#include <RTfact/Model/Primitive/Detail/OffsetElementIterator.hpp>

RTFACT_NAMESPACE_BEGIN

// if defined, nodes will be ordered depending on centroid distance,
// otherwise will be ordered in each dimension according to their bounds
#define RTFACT__CENTROID_ORDER

template<
    class tElement>
class OrderedBVH :
    public Primitive
{
    struct Node
    {
        union
        {
            float4 min;

            struct 
            {
                float minX, minY, minZ;
                uint32 firstChildId;
            };
        };

        union
        {
            float4 max;

            struct
            {
                float maxX, maxY, maxZ;
                uint32 nodeTypeAndObjEnd;
            };
        };

        enum
        {
            FULL_SHIFT = 32,

            #ifdef RTFACT__CENTROID_ORDER

                SPLIT_AXIS_SHIFT         = FULL_SHIFT - 3,
                FIRST_CHILD_OFFSET_SHIFT = FULL_SHIFT - 1,
                ELEMENT_END_GET_MASK     = 0x1FFFFFFF,
                ELEMENT_END_ASSERT_MASK  = 0xE0000000

            #else

                AXES_SHIFT           = FULL_SHIFT - 8,
                NODE_TYPE_SHIFT      = FULL_SHIFT - 1,
                DATA_ASSERT_MASK     = 0x3FFFFFFF,
                FIRST_SET_MASK       = 0x40000000,
                FIRST_GET_MASK       = 0x3FFFFFFF,
                AXES_SET_MASK        = 0x40000000,
                AXES_GET_MASK        = 0x3FFFFFFF,
                ELEMENT_END_GET_MASK = 0x3FFFFFFF,
                ELEMENT_END_SET_MASK = 0xC0000000

            #endif
        };

        RTFACT_PRIMITIVE_INLINE const BBox& getBounds() const
        {
            return *reinterpret_cast<const BBox*>(this);
        }

        RTFACT_PRIMITIVE_INLINE uint32 getElementBegin() const
        {
            return getFirstChild();
        }

        RTFACT_PRIMITIVE_INLINE uint32 getElementEnd() const
        {
            return nodeTypeAndObjEnd & ELEMENT_END_GET_MASK;
        }

        RTFACT_PRIMITIVE_INLINE void setBounds(
            const BBox& aBounds)
        {
            minX = aBounds.min.x;
            minY = aBounds.min.y;
            minZ = aBounds.min.z;

            maxX = aBounds.max.x;
            maxY = aBounds.max.y;
            maxZ = aBounds.max.z;
        }

        #ifdef RTFACT__CENTROID_ORDER

            RTFACT_PRIMITIVE_INLINE bool isLeaf() const
            {
                return (nodeTypeAndObjEnd & ELEMENT_END_GET_MASK) != 0;
            }

            RTFACT_PRIMITIVE_INLINE uint32 getFirstChild() const
            {
                return firstChildId;
            }

            RTFACT_PRIMITIVE_INLINE uint32 getSecondChild() const
            {
                return firstChildId + 1;
            }

            RTFACT_PRIMITIVE_INLINE uint32 getFirstChildOffset() const
            {
                return nodeTypeAndObjEnd >> FIRST_CHILD_OFFSET_SHIFT;
            }

            RTFACT_PRIMITIVE_INLINE uint32 getSplitAxis() const
            {
                return (nodeTypeAndObjEnd >> SPLIT_AXIS_SHIFT) & 3;
            }

            RTFACT_PRIMITIVE_INLINE void initInner()
            {
                nodeTypeAndObjEnd = 0;
            }

            void initLeaf()
            {}

            RTFACT_PRIMITIVE_INLINE void setSplitInfo(
                const uint32 aAxis,
                const uint32 aFirstChildOffset)
            {
                RTFACT_PRIMITIVE_ASSERT(aAxis < 3);
                RTFACT_PRIMITIVE_ASSERT(aFirstChildOffset < 2);

                nodeTypeAndObjEnd = ((aFirstChildOffset << 2) | aAxis) << SPLIT_AXIS_SHIFT;
            }

            RTFACT_PRIMITIVE_INLINE void setFirstChild(
                const uint32 aFirstChildId)
            {
                firstChildId = aFirstChildId;
            }

            RTFACT_PRIMITIVE_INLINE void setElementBegin(
                const uint32 aElementBegin)
            {
                setFirstChild(aElementBegin);
            }

            RTFACT_PRIMITIVE_INLINE void setElementEnd(
                const uint32 aElementEnd)
            {
                RTFACT_PRIMITIVE_ASSERT((aElementEnd & ELEMENT_END_ASSERT_MASK) == 0);

                nodeTypeAndObjEnd = aElementEnd;
            }

        #else

            RTFACT_PRIMITIVE_INLINE bool isLeaf() const
            {
                return (nodeTypeAndObjEnd & ELEMENT_END_GET_MASK) != 0;

                return (nodeTypeAndObjEnd >> NODE_TYPE_SHIFT != 0);
            }

            RTFACT_PRIMITIVE_INLINE uint32 getFirstChild() const
            {
                return firstChildId & FIRST_GET_MASK;
            }

            RTFACT_PRIMITIVE_INLINE uint32 getSecondChild() const
            {
                return getFirstChild() + 1;
            }

            RTFACT_PRIMITIVE_INLINE uint32 getFirstChildOffsets() const
            {
                return (nodeTypeAndObjEnd & AXES_GET_MASK) >> AXES_SHIFT;
            }

            RTFACT_PRIMITIVE_INLINE void initInner()
            {
                nodeTypeAndObjEnd = AXES_SET_MASK;
            }

            RTFACT_PRIMITIVE_INLINE void initLeaf()
            {}

            RTFACT_PRIMITIVE_INLINE void setFirstChildOffsets(
                const uint32 aOffsets)
            {
                RTFACT_PRIMITIVE_ASSERT(aOffsets < 64);

                nodeTypeAndObjEnd = (aOffsets << AXES_SHIFT) | AXES_SET_MASK;
            }

            RTFACT_PRIMITIVE_INLINE void setFirstChild(
                const uint32 aFirstChildId)
            {
                RTFACT_PRIMITIVE_ASSERT((aFirstChildId & DATA_ASSERT_MASK) == 0);

                firstChildId = aFirstChildId | FIRST_SET_MASK;
            }

            RTFACT_PRIMITIVE_INLINE void setElementBegin(
                const uint32 aElementBegin)
            {
                RTFACT_PRIMITIVE_ASSERT((aElementBegin & DATA_ASSERT_MASK) == 0);

                setFirstChild(aElementBegin);
            }

            RTFACT_PRIMITIVE_INLINE void setElementEnd(
                const uint32 aElementEnd)
            {
                RTFACT_PRIMITIVE_ASSERT((aElementEnd & DATA_ASSERT_MASK) == 0);

                nodeTypeAndObjEnd = aElementEnd | ELEMENT_END_SET_MASK;
            }

        #endif
    };

public:

    typedef tElement                       Element;
    typedef uint32                         Iterator;
    typedef std::pair<Iterator, Iterator>  ChildIteratorPair;
    typedef Detail::OffsetElementIterator<
         Element, Vector<int>::iterator>   ElementIterator;

protected:

    Vector<Node> mNodes;
    Vector<int> mElementOffsets;
    uint32 mNextNodeIndex, mNextLeafIndex;
    Element* mElementBase;

public:

    void getStats(
        int& oNumNodes,
        int& oNumLeaves,
        int& oNumTri)
    {
        oNumLeaves = oNumTri = 0;

        oNumNodes = mNextNodeIndex;
        oNumTri = mElementOffsets.size();

        for(uint i = 0; i < mNextNodeIndex; ++i)
        {
            if(mNodes[i].isLeaf())
            {
                ++oNumLeaves;
            }
        }
    }

    OrderedBVH() :
        mElementBase(RTFACT_NULL)
    {}

    size_t getNodeCount() const
    {
        return mNodes.size();
    }

    size_t getLeafCount() const
    {
        return mElementOffsets.size();
    }

#pragma region Building-related methods

    void setElementBase(
        const Element& aElementBase)
    {
        mElementBase = const_cast<Element*>(&aElementBase);
    }

    Element& getElementBase() const
    {
        return *mElementBase;
    }

    void init(
        const size_t aElementCount)
    {
        mNodes.resize(aElementCount * 2 + 2);
        mElementOffsets.resize(aElementCount);
        // initialize tree like this so that we can call
        // order() on an empty tree without getting into an infinite loop
        mNodes[0].initLeaf();
        mNodes[0].setElementBegin(1);
        mNodes[0].setElementEnd(1);

        mNextNodeIndex = 1;
        mNextLeafIndex = 0;
    }

    RTFACT_PRIMITIVE_INLINE ChildIteratorPair createInnerNode(
        const Iterator aWhere)
    {
        const uint32 firstChildId = mNextNodeIndex;
        const uint32 secondChildId = mNextNodeIndex + 1;
        mNextNodeIndex += 2;

        mNodes[aWhere].initInner();
        mNodes[aWhere].setFirstChild(firstChildId);

        return std::make_pair(firstChildId, secondChildId);
    }

    template<
        class tIterator>
    RTFACT_PRIMITIVE_INLINE void createLeafNode(
        const Iterator aWhere,
        tIterator aElemBegin,
        tIterator aElemEnd)
    {
        RTFACT_PRIMITIVE_ASSERT(mElementBase != RTFACT_NULL);

        mNodes[aWhere].initLeaf();
        if(aElemBegin == aElemEnd)
        {
            mNodes[aWhere].setElementBegin(1);
            mNodes[aWhere].setElementEnd(1);
            return;
        }
        mNodes[aWhere].setElementBegin(mNextLeafIndex);

        for(tIterator it = aElemBegin; it != aElemEnd; ++it)
        {
            RTFACT_PRIMITIVE_ASSERT(
                (&(*it) - mElementBase) <
                static_cast<ptrdiff_t>(std::numeric_limits<int>::max()));

            mElementOffsets[mNextLeafIndex++] =
                static_cast<int>(&(*it) - mElementBase);
        }

        mNodes[aWhere].setElementEnd(mNextLeafIndex);
    }

    template<
        class tIterator,
        class tElemExtractor>
    RTFACT_PRIMITIVE_INLINE void createLeafNode(
        const Iterator aWhere,
        tIterator aBegin,
        tIterator aEnd,
        tElemExtractor aElemExtractor)
    {
        RTFACT_PRIMITIVE_ASSERT(mElementBase != RTFACT_NULL);

        mNodes[aWhere].initLeaf();
        if(aBegin == aEnd)
        {
            mNodes[aWhere].setElementBegin(1);
            mNodes[aWhere].setElementEnd(1);
            return;
        }
        mNodes[aWhere].setElementBegin(mNextLeafIndex);

        for(tIterator it = aBegin; it != aEnd; ++it)
        {
            RTFACT_PRIMITIVE_ASSERT(
                (&(aElemExtractor(*it)) - mElementBase) <
                static_cast<ptrdiff_t>(std::numeric_limits<int>::max()));

            mElementOffsets[mNextLeafIndex++] =
                static_cast<int>(&(aElemExtractor(*it)) - mElementBase);
        }

        mNodes[aWhere].setElementEnd(mNextLeafIndex);
    }

    RTFACT_PRIMITIVE_INLINE void setBounds(
        const Iterator aWhere,
        const BBox& aBounds)
    {
        mNodes[aWhere].setBounds(aBounds);
    }

protected:

    void order(
        const Iterator aWhere)
    {
        if(mNodes[aWhere].isLeaf())
        {
            return;
        }

        const Iterator leftChild = mNodes[aWhere].getFirstChild();
        const Iterator rightChild = leftChild + 1;

        #ifdef RTFACT__CENTROID_ORDER

            const Vec3f1 centroidDiff  = mNodes[rightChild].getBounds().getCentroid() -
                mNodes[leftChild].getBounds().getCentroid();

            const uint maxDim = RTFACT_MAX_DIMENSION(
                Math::abs(centroidDiff.x),
                Math::abs(centroidDiff.y),
                Math::abs(centroidDiff.z));

            mNodes[aWhere].setSplitInfo(maxDim, centroidDiff.get(maxDim) < 0.f);

        #else

            const BBox& leftBounds  = mNodes[leftChild].getBounds();
            const BBox& rightBounds = mNodes[rightChild].getBounds();

            const uint xPosOffset = (leftBounds.min.xf > rightBounds.min.xf);
            const uint xNegOffset = (leftBounds.max.xf < rightBounds.max.xf);
            const uint yPosOffset = (leftBounds.min.yf > rightBounds.min.yf);
            const uint yNegOffset = (leftBounds.max.yf < rightBounds.max.yf);
            const uint zPosOffset = (leftBounds.min.zf > rightBounds.min.zf);
            const uint zNegOffset = (leftBounds.max.zf < rightBounds.max.zf);

            const uint32 firstChildOffsets =
                xPosOffset       |
                (xNegOffset << 1) |
                (yPosOffset << 2) |
                (yNegOffset << 3) |
                (zPosOffset << 4) |
                (zNegOffset << 5);

            mNodes[aWhere].setFirstChildOffsets(firstChildOffsets);

        #endif

        order(leftChild);
        order(rightChild);
    }

public:

    void finalizeBuild()
    {
        order(getRoot());
    }

    const BBox testTree(
        const Iterator aWhere)
    {
        if(mNodes[aWhere].isLeaf())
        {
            BBox bounds = BBox::empty();

            ElementIterator elemBegin, elemEnd;

            getElements(aWhere, elemBegin, elemEnd);

            do 
            {
                bounds.extend((*elemBegin).getBounds());
            }
            while(++elemBegin != elemEnd);

            if(!getBounds(aWhere).contains(bounds))
            {
                RTFACT_LOG(".");
            }

            return bounds;
        }
        else
        {
            Iterator left, right;

            getChildren(aWhere, left, right);

            BBox bounds = testTree(left);
            bounds.extend(testTree(right));

            if(!getBounds(aWhere).contains(bounds))
            {
                RTFACT_LOG(".");
            }

            return bounds;
        }
    }

    void testTree()
    {
        testTree(getRoot());
    }

#pragma endregion // Building-related methods

#pragma region Traversal-related methods

    RTFACT_PRIMITIVE_INLINE const BBox& getBounds() const
    {
        return mNodes[0].getBounds();
    }

    RTFACT_PRIMITIVE_INLINE Iterator getRoot() const
    {
        return 0;
    }

    RTFACT_PRIMITIVE_INLINE bool isLeaf(
        const Iterator aWhere) const
    {
        return mNodes[aWhere].isLeaf();
    }

    RTFACT_PRIMITIVE_INLINE const BBox& getBounds(
        const Iterator aWhere) const
    {
        return mNodes[aWhere].getBounds();
    }

    RTFACT_PRIMITIVE_INLINE void getChildren(
        const Iterator aWhere,
        Iterator& oFirst,
        Iterator& oSecond) const
    {
        oFirst  = mNodes[aWhere].getFirstChild();
        oSecond = mNodes[aWhere].getSecondChild();
    }

    RTFACT_PRIMITIVE_INLINE void getChildren(
        const Iterator aWhere, 
        const uint aDominantAxis,
        const uint aDirectionSign,
        const Vec3f1& aRayDir,
        Iterator& oFirst,
        Iterator& oSecond) const
    {
        RTFACT_PRIMITIVE_ASSERT(!mNodes[aWhere].isLeaf());
        RTFACT_PRIMITIVE_ASSERT(aDominantAxis < 3);
        RTFACT_PRIMITIVE_ASSERT(aDirectionSign < 2);

        #ifdef RTFACT__CENTROID_ORDER

            const uint firstChildOffset =
                ((float) aRayDir.get(mNodes[aWhere].getSplitAxis()) < 0.f) ^
                    mNodes[aWhere].getFirstChildOffset();

        #else

            const uint firstChildOffset =
                (mNodes[aWhere].getFirstChildOffsets() >>
                    (2 * aDominantAxis + aDirectionSign)) & 1;

        #endif

        const uint32 firstChild = mNodes[aWhere].getFirstChild();

        oFirst  = firstChild + firstChildOffset;
        oSecond = firstChild + 1 - firstChildOffset;
    }

    RTFACT_PRIMITIVE_INLINE void getElements(
        const Iterator aLeaf,
        ElementIterator& oBegin,
        ElementIterator& oEnd)
    {
        RTFACT_PRIMITIVE_ASSERT(mElementBase != RTFACT_NULL);
        RTFACT_PRIMITIVE_ASSERT(isLeaf(aLeaf));

        oBegin = ElementIterator(mElementBase, mElementOffsets.begin() +
                                               mNodes[aLeaf].getElementBegin());

        oEnd   = ElementIterator(mElementBase, mElementOffsets.begin() +
                                               mNodes[aLeaf].getElementEnd());
    }

    RTFACT_PRIMITIVE_INLINE void getElements(
        const Iterator aFirst,
        const Iterator aSecond, 
        ElementIterator& oBegin,
        ElementIterator& oEnd)
    {
        RTFACT_PRIMITIVE_ASSERT(mElementBase != RTFACT_NULL);

        oBegin = ElementIterator(mElementBase, mElementOffsets.begin() +
                                                aFirst);

        oEnd = ElementIterator(mElementBase, mElementOffsets.begin() +
                                              aSecond);
    }

#pragma endregion // Traversal-related methods

    void serialize(
        IO::Serializer& aSerializer)
    {
        aSerializer & mNodes
                    & mElementOffsets;
    }
};

RTFACT_NAMESPACE_END

RTFACT_SERIALIZATION_REGISTER_SERIALIZE_METHOD_TEMPLATE_CLASS(RTfact::OrderedBVH, (class))

#endif // RTFACT__ORDEREDBVH_HPP
