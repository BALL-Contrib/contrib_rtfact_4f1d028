/* 
 *  KdTree.hpp
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
 
 #ifndef RTFACT__KDTREE_HPP
#define RTFACT__KDTREE_HPP

#include <RTfact/Config/Common.hpp>

#include <limits>

#include <RTfact/Utils/Containers/Vector.hpp>
#include <RTfact/Utils/IO/SerializationUtils.hpp>

#include <RTfact/Concept/Scene.hpp>
#include <RTfact/Concept/Primitive.hpp>
#include <RTfact/Model/Primitive/Detail/OffsetElementIterator.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tElement>
class KdTree :
    public Primitive
{
    struct Node
    {
        uint32 leftChildAndSplitType;

        union
        {
            float  splitVal;
            uint32 firstLeaf;
        };

        enum
        {
            FULL_SHIFT       = 32,
            NODE_TYPE_SHIFT  = FULL_SHIFT - 2,
            LEAF_NODE_MASK   = 0xC0000000,
            DATA_ASSERT_MASK = 0xC0000000,
            LEFT_CHILD_MASK  = 0x3FFFFFFF
        };

        RTFACT_PRIMITIVE_INLINE void initInner(
            const uint aType,
            const float aSplitVal,
            const uint32 aLeftChildId)
        {
            RTFACT_PRIMITIVE_ASSERT((aLeftChildId & DATA_ASSERT_MASK) == 0);
            RTFACT_PRIMITIVE_ASSERT(aType <= 3);

            splitVal = aSplitVal;
            leftChildAndSplitType = (aType << NODE_TYPE_SHIFT) | aLeftChildId;
        }

        RTFACT_PRIMITIVE_INLINE void initLeaf(
            const uint32 aElementBegin,
            const uint32 aElementEnd)
        {
            leftChildAndSplitType = LEAF_NODE_MASK | aElementEnd;
            firstLeaf = aElementBegin;
        }

        RTFACT_PRIMITIVE_INLINE uint getType() const
        {
            return static_cast<uint>(
                leftChildAndSplitType >> NODE_TYPE_SHIFT);
        }

        RTFACT_PRIMITIVE_INLINE float getSplitValue() const
        {
            return splitVal;
        }

        RTFACT_PRIMITIVE_INLINE uint32 getLeftChild() const
        {
            return leftChildAndSplitType & LEFT_CHILD_MASK;
        }

        RTFACT_PRIMITIVE_INLINE uint32 getElementBegin() const
        {
            return firstLeaf;
        }

        RTFACT_PRIMITIVE_INLINE uint32 getElementEnd() const
        {
            return leftChildAndSplitType & LEFT_CHILD_MASK;
        }
    };

public:

    typedef tElement                       Element;
    typedef uint32                        Iterator;
    typedef std::pair<Iterator, Iterator>  ChildIteratorPair;
    typedef Detail::OffsetElementIterator<
        Element, Vector<int>::iterator>    ElementIterator;

protected:

    BBox mBounds;
    Vector<Node> mNodes;
    Vector<int> mElementOffsets;
    Element* mElementBase;

public:

    KdTree() :
        mElementBase(RTFACT_NULL)
    {}

#pragma region Building-related methods

    void setElementBase(
        const Element& aElementBase)
    {
        mElementBase = const_cast<Element*>(&aElementBase);
    }

    void init(
        const size_t aElementCount,
        const BBox& aBounds)
    {
        mBounds = aBounds;
        mNodes.reserve(aElementCount * 3 / 2);
        mNodes.resize(1);
        mElementOffsets.reserve(aElementCount * 3 / 2);
    }

    RTFACT_PRIMITIVE_INLINE void createInnerNode(
        const Iterator aWhere,
        const uint aSplitAxis,
        const float aSplitVal)
    {
        const uint32 leftChild = mNodes.size();

        mNodes[aWhere].initInner(aSplitAxis, aSplitVal, leftChild);
        mNodes.resize(leftChild + 2);
    }

    template<
        class tIterator>
    RTFACT_PRIMITIVE_INLINE void createLeafNode(
        const Iterator aWhere,
        tIterator aElemBegin,
        tIterator aElemEnd)
    {
        RTFACT_PRIMITIVE_ASSERT(mElementBase != RTFACT_NULL);

        const size_t elementBeginIndex = mElementOffsets.size();

        for(tIterator it = aElemBegin; it != aElemEnd; ++it)
        {
            RTFACT_PRIMITIVE_ASSERT(
                (&(*it) - mElementBase) <
                static_cast<ptrdiff_t>(std::numeric_limits<int>::max()));

            mElementOffsets.push_back(static_cast<int>(&(*it) - mElementBase));
        }
        
        mNodes[aWhere].initLeaf(elementBeginIndex, mElementOffsets.size());
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

        const size_t elementBeginIndex = mElementOffsets.size();

        for(tIterator it = aBegin; it != aEnd; ++it)
        {
            RTFACT_PRIMITIVE_ASSERT(
                (&(aElemExtractor(*it)) - mElementBase) <
                static_cast<ptrdiff_t>(std::numeric_limits<int>::max()));

            mElementOffsets.push_back(
                static_cast<int>(&(aElemExtractor(*it)) - mElementBase));
        }

        mNodes[aWhere].initLeaf(elementBeginIndex, mElementOffsets.size());
    }

    #pragma endregion

    #pragma region Traversal-related methods

    Element& getElementBase() const
    {
        return *mElementBase;
    }

    RTFACT_PRIMITIVE_INLINE const BBox& getBounds() const
    {
        return mBounds;
    }

    RTFACT_PRIMITIVE_INLINE Iterator getRoot() const
    {
        return 0;
    }

    RTFACT_PRIMITIVE_INLINE uint getType(
        const Iterator aWhere) const
    {
        return mNodes[aWhere].getType();
    }

    RTFACT_PRIMITIVE_INLINE float getSplitValue(
        const Iterator aWhere) const
    {
        RTFACT_PRIMITIVE_ASSERT(getType(aWhere) != 3);

        return mNodes[aWhere].getSplitValue();
    }

    RTFACT_PRIMITIVE_INLINE Iterator getChild(
        const Iterator aWhere,
        const int aRightOrLeft) const
    {
        RTFACT_PRIMITIVE_ASSERT(getType(aWhere) != 3);

        return mNodes[aWhere].getLeftChild() + aRightOrLeft;
    }

    RTFACT_PRIMITIVE_INLINE Iterator getLeftChild(
        const Iterator aWhere) const
    {
        RTFACT_PRIMITIVE_ASSERT(getType(aWhere) != 3);

        return mNodes[aWhere].getLeftChild();
    }

    RTFACT_PRIMITIVE_INLINE Iterator getRightChild(
        const Iterator aWhere) const
    {
        RTFACT_PRIMITIVE_ASSERT(getType(aWhere) != 3);

        return mNodes[aWhere].getLeftChild() + 1;
    }

    RTFACT_PRIMITIVE_INLINE void getElements(
        const Iterator aLeaf,
        ElementIterator& oBegin,
        ElementIterator& oEnd)
    {
        RTFACT_PRIMITIVE_ASSERT(mElementBase != RTFACT_NULL);
        RTFACT_PRIMITIVE_ASSERT(getType(aLeaf) == 3);

        oBegin = ElementIterator(mElementBase, mElementOffsets.begin() +
                                               mNodes[aLeaf].getElementBegin());

        oEnd   = ElementIterator(mElementBase, mElementOffsets.begin() +
                                               mNodes[aLeaf].getElementEnd());
    }

    #pragma endregion

    void serialize(
        IO::Serializer& aSerializer)
    {
        aSerializer & mBounds
                    & mNodes
                    & mElementOffsets;
    }
};

#undef RTFACT__ASSERT_RENDER_STRUCT_NON_NULL
#undef RTFACT__ASSERT_RENDER_STRUCT_VALID

RTFACT_NAMESPACE_END

RTFACT_SERIALIZATION_REGISTER_SERIALIZE_METHOD_TEMPLATE_CLASS(RTfact::KdTree, (class))

#endif // RTFACT__KDTREE_HPP
